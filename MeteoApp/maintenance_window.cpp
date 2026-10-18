#include "maintenance_window.h"
#include "ui_maintenance_window.h"
#include "intervention_dialog.h"
#include "predictive_dialog.h"
#include "maintenance_db.h"

#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QCheckBox>
#include <QColor>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QInputDialog>
#include <QSettings>
#include <QCoreApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QPageLayout>
#include <QPainter>
#include <QPieSeries>
#include <QPrinter>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStatusBar>
#include <QTextDocument>
#include <QTextStream>
#include <QTimer>
#include <QMap>
#include <QPointer>
#include <QPushButton>
#include <QValueAxis>

// Qt5 : classes dans le namespace QtCharts | Qt6 : plus de namespace
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QT_CHARTS_USE_NAMESPACE
#endif

// Colonnes de la requete principale
enum Col { C_ID, C_STATION, C_GOUV, C_TECH, C_TYPE, C_PRIO, C_CREA, C_PLAN, C_CLOT,
           C_STATUT, C_COUT, C_AERO, C_ALERT };

// ------------------------------------------------------------------
//  Modele SQL + coloration des interventions en retard SLA
// ------------------------------------------------------------------
class SlaModel : public QSqlQueryModel
{
public:
    using QSqlQueryModel::QSqlQueryModel;

    // Delai SLA (heures) selon la criticite.
    // Station aeronautique OU alerte rouge en cours => delai court (divise par 2).
    static int slaHours(const QString &prio, bool shortDelay)
    {
        int h = 168;                          // Basse
        if (prio == "Moyenne") h = 72;
        else if (prio == "Haute") h = 24;
        else if (prio == "Urgente") h = 4;
        if (shortDelay) h = qMax(1, h / 2);
        return h;
    }

    bool isAero(int row) const  { return QSqlQueryModel::data(index(row, C_AERO)).toInt() == 1; }
    bool isAlert(int row) const { return QSqlQueryModel::data(index(row, C_ALERT)).toInt() == 1; }
    bool isShortSla(int row) const { return isAero(row) || isAlert(row); }

    QDateTime deadline(int row) const
    {
        const QDateTime created = QSqlQueryModel::data(index(row, C_CREA)).toDateTime();
        const QString prio = QSqlQueryModel::data(index(row, C_PRIO)).toString();
        return created.addSecs(qint64(slaHours(prio, isShortSla(row))) * 3600);
    }

    bool isLate(int row) const
    {
        const QString st = QSqlQueryModel::data(index(row, C_STATUT)).toString();
        if (st != "Planifiée" && st != "En cours") return false;
        return deadline(row) < QDateTime::currentDateTime();
    }

    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::BackgroundRole && isLate(idx.row()))
            return QColor("#fde2e2");
        if (role == Qt::ForegroundRole && idx.column() == C_PRIO) {
            const QString p = QSqlQueryModel::data(idx).toString();
            if (p == "Urgente") return QColor("#c62828");
            if (p == "Haute")   return QColor("#ef6c00");
        }
        if (idx.column() == C_STATUT) {
            const QString st = QSqlQueryModel::data(idx).toString();
            if (role == Qt::ForegroundRole) {
                if (st == "Terminée") return QColor("#1b8a5a");
                if (st == "En cours") return QColor("#1565c0");
                if (st == "Annulée")  return QColor("#8a96a8");
                return QColor("#6a3fd1");                       // Planifiée
            }
            if (role == Qt::FontRole) { QFont f; f.setBold(true); return f; }
        }
        if (role == Qt::DisplayRole && idx.column() == C_PLAN) {
            const QDateTime d = QSqlQueryModel::data(idx).toDateTime();
            return d.isValid() ? d.toString("dd/MM/yyyy") : QString();
        }
        if (role == Qt::DisplayRole && idx.column() >= C_CREA && idx.column() <= C_CLOT) {
            const QDateTime d = QSqlQueryModel::data(idx).toDateTime();
            return d.isValid() ? d.toString("dd/MM/yyyy HH:mm") : QString();
        }
        if (role == Qt::DisplayRole && idx.column() == C_COUT)
            return QString::number(QSqlQueryModel::data(idx).toDouble(), 'f', 2) + " DT";
        if (role == Qt::TextAlignmentRole && idx.column() == C_COUT)
            return int(Qt::AlignRight | Qt::AlignVCenter);
        return QSqlQueryModel::data(idx, role);
    }
};

static QString esc(QString s) { return s.replace("'", "''"); }

// ------------------------------------------------------------------
// Ombre portee douce => effet 3D / carte flottante
static void addShadow(QWidget *w, int blur = 26, int dy = 7, int alpha = 70)
{
    auto *fx = new QGraphicsDropShadowEffect(w);
    fx->setBlurRadius(blur);
    fx->setOffset(0, dy);
    fx->setColor(QColor(13, 40, 90, alpha));
    w->setGraphicsEffect(fx);
}

static const QStringList &allRoles()
{
    static const QStringList r = {"Chef de maintenance", "Technicien de maintenance",
                                  "Administrateur réseau", "Responsable régional"};
    return r;
}

static QString roleDescription(const QString &role)
{
    if (role == "Chef de maintenance")
        return "Chef de maintenance : tous les droits (CRUD complet, planification, supervision SLA).";
    if (role == "Technicien de maintenance")
        return "Technicien de maintenance : création / mise à jour des interventions et ordres de mission.";
    if (role == "Administrateur réseau")
        return "Administrateur réseau : planification prédictive, suivi des retards SLA et exports.";
    return "Responsable régional : validation, archivage, suivi SLA et escalades de la région.";
}

// ------------------------------------------------------------------
//  Roles / permissions (Chef de maintenance, Technicien de maintenance,
//  Administrateur reseau, Responsable regional)
// ------------------------------------------------------------------
static bool roleCan(const QString &role, const QString &action)
{
    // actions : add edit archive delete predict mission sla export
    if (role == "Chef de maintenance")
        return true;
    if (role == "Technicien de maintenance")
        return action == "add" || action == "edit" || action == "mission" || action == "export";
    if (role == "Administrateur réseau")
        return action == "predict" || action == "sla" || action == "export";
    if (role == "Responsable régional")
        return action == "edit" || action == "archive" || action == "mission"
            || action == "sla" || action == "export";
    return false;
}

bool MaintenanceWindow::allowed(const QString &action, const QString &title)
{
    if (roleCan(m_role, action)) return true;
    QMessageBox::warning(this, title, "Action non autorisée pour le rôle « " + m_role + " ».");
    return false;
}

void MaintenanceWindow::applyRole()
{
    struct B { QPushButton *btn; const char *action; };
    const B list[] = { {ui->btnAdd, "add"}, {ui->btnEdit, "edit"}, {ui->btnArchive, "archive"},
                       {ui->btnDelete, "delete"}, {ui->btnMission, "mission"}, {ui->btnSla, "sla"},
                       {ui->btnPredict, "predict"}, {ui->btnNavPredict, "predict"},
                       {ui->btnCsv, "export"}, {ui->btnPdf, "export"} };
    for (const B &b : list) {
        const bool ok = roleCan(m_role, b.action);
        b.btn->setEnabled(ok);
        b.btn->setToolTip(ok ? QString() : "Non autorisé pour le rôle « " + m_role + " »");
    }
    ui->lblRole->setText("👤  " + roleDescription(m_role));
    ui->lblUserRole->setText("👤 " + m_role);
}

void MaintenanceWindow::changeRole()
{
    bool ok = false;
    const QString r = QInputDialog::getItem(this, "Changer de rôle", "Nouveau rôle :", allRoles(),
                                            qMax(0, allRoles().indexOf(m_role)), false, &ok);
    if (!ok || r == m_role) return;
    m_role = r;
    QSettings cfg("SmartWeather", "Module5");
    cfg.setValue("app/role", m_role);
    cfg.sync();
    applyRole();
    statusBar()->showMessage("Rôle actif : " + m_role, 4000);
}

MaintenanceWindow::MaintenanceWindow(const QString &role, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MaintenanceWindow), m_model(new SlaModel(this)), m_role(role)
{
    ui->setupUi(this);

    // Table
    ui->tableView->setModel(m_model);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->verticalHeader()->setDefaultSectionSize(36);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setShowGrid(false);
    ui->tableView->setFrameShape(QFrame::NoFrame);

    // Recherche / tri / filtre
    connect(ui->lineSearch, &QLineEdit::textChanged, this, &MaintenanceWindow::reload);
    connect(ui->comboSort, qOverload<int>(&QComboBox::currentIndexChanged), this, &MaintenanceWindow::reload);
    connect(ui->comboStatus, qOverload<int>(&QComboBox::currentIndexChanged), this, &MaintenanceWindow::reload);
    connect(ui->chkArchive, &QCheckBox::toggled, this, [this](bool on) {
        ui->btnArchive->setText(on ? "♻  Restaurer" : "🗄  Archiver");
        reload();
    });

    // Boutons
    connect(ui->tableView, &QTableView::doubleClicked, this, &MaintenanceWindow::editIntervention);
    connect(ui->btnAdd, &QPushButton::clicked, this, &MaintenanceWindow::addIntervention);
    connect(ui->btnEdit, &QPushButton::clicked, this, &MaintenanceWindow::editIntervention);
    connect(ui->btnArchive, &QPushButton::clicked, this, &MaintenanceWindow::archiveIntervention);
    connect(ui->btnDelete, &QPushButton::clicked, this, &MaintenanceWindow::deleteIntervention);
    connect(ui->btnMission, &QPushButton::clicked, this, &MaintenanceWindow::generateMissionOrder);
    connect(ui->btnSla, &QPushButton::clicked, this, &MaintenanceWindow::showSlaDelays);
    connect(ui->btnPredict, &QPushButton::clicked, this, &MaintenanceWindow::predictivePlanning);
    connect(ui->btnCsv, &QPushButton::clicked, this, &MaintenanceWindow::exportCsv);
    connect(ui->btnPdf, &QPushButton::clicked, this, &MaintenanceWindow::exportHistoryPdf);

    // Navigation laterale
    connect(ui->btnNavList, &QPushButton::clicked, this, [this] { ui->stack->setCurrentIndex(0); });
    connect(ui->btnNavStats, &QPushButton::clicked, this, [this] {
        ui->stack->setCurrentIndex(1);
        refreshStats();
    });
    connect(ui->btnNavPredict, &QPushButton::clicked, this, &MaintenanceWindow::predictivePlanning);
    connect(ui->btnChangeRole, &QPushButton::clicked, this, &MaintenanceWindow::changeRole);

    // Effet 3D : ombres portees sur les cartes
    for (QWidget *w : {static_cast<QWidget *>(ui->cardTotal), static_cast<QWidget *>(ui->cardAvg),
                       static_cast<QWidget *>(ui->cardLate), static_cast<QWidget *>(ui->cardCost),
                       static_cast<QWidget *>(ui->actionsCard)})
        addShadow(w);

    // Horloge de la barre laterale
    auto *clock = new QTimer(this);
    auto tick = [this] { ui->lblClock->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd  HH:mm:ss")); };
    connect(clock, &QTimer::timeout, this, tick);
    tick();
    clock->start(1000);

    applyRole();
    reload();

    // Suivi SLA automatique : recalcul des retards toutes les 5 minutes
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MaintenanceWindow::reload);
    timer->start(5 * 60 * 1000);
}

// ------------------------------------------------------------------
//  Ouverture depuis le Module 3 (ou depuis une application hote)
//  Role : propriete d'application "role" si elle est definie par le login, sinon choix manuel.
// ------------------------------------------------------------------
void MaintenanceWindow::open(QWidget *parent)
{
    static QPointer<MaintenanceWindow> instance;
    if (instance) {                         // deja ouverte : on la remet au premier plan
        instance->show();
        instance->raise();
        instance->activateWindow();
        return;
    }

    QString role = qApp->property("role").toString();
    if (!allRoles().contains(role)) {
        QSettings cfg("SmartWeather", "Module5");
        const int last = qMax(0, allRoles().indexOf(cfg.value("role").toString()));
        bool ok = false;
        role = QInputDialog::getItem(parent, "Module 5 – Maintenance", "Votre rôle :", allRoles(), last, false, &ok);
        if (!ok) return;
        cfg.setValue("role", role);
    }

    auto *w = new MaintenanceWindow(role, parent);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setWindowFlag(Qt::Window);
    instance = w;
    w->show();
}

MaintenanceWindow::~MaintenanceWindow() { delete ui; }

int MaintenanceWindow::selectedId() const
{
    const QModelIndex i = ui->tableView->currentIndex();
    return i.isValid() ? m_model->data(m_model->index(i.row(), C_ID), Qt::DisplayRole).toInt() : 0;
}

// ------------------------------------------------------------------
//  Chargement : recherche + tri + filtre
// ------------------------------------------------------------------
void MaintenanceWindow::reload()
{
    QString where = QString("WHERE i.ARCHIVE = %1").arg(ui->chkArchive->isChecked() ? 1 : 0);

    const QString t = ui->lineSearch->text().trimmed().toUpper();
    if (!t.isEmpty()) {
        const QString like = "'%" + esc(t) + "%'";
        where += " AND (UPPER(s.NOM) LIKE " + like +
                 " OR UPPER(e.NOM || ' ' || e.PRENOM) LIKE " + like +
                 " OR UPPER(i.TYPE_INTERVENTION) LIKE " + like +
                 " OR UPPER(s.GOUVERNORAT) LIKE " + like + ")";
    }
    if (ui->comboStatus->currentIndex() > 0)
        where += " AND i.STATUT = '" + esc(ui->comboStatus->currentText()) + "'";

    QString order;
    switch (ui->comboSort->currentIndex()) {
    case 0: order = "CASE i.PRIORITE WHEN 'Urgente' THEN 1 WHEN 'Haute' THEN 2 "
                    "WHEN 'Moyenne' THEN 3 ELSE 4 END, i.DATE_PLANIFIEE"; break;
    case 1: order = "i.DATE_PLANIFIEE"; break;
    case 2: order = "i.STATUT, i.DATE_PLANIFIEE"; break;
    default: order = "i.COUT DESC"; break;
    }

    m_model->setQuery(dbSql(
        "SELECT i.ID_INTERVENTION, s.NOM, NVL(s.GOUVERNORAT, '-'), e.NOM || ' ' || e.PRENOM, "
        "i.TYPE_INTERVENTION, i.PRIORITE, i.DATE_CREATION, i.DATE_PLANIFIEE, i.DATE_CLOTURE, "
        "i.STATUT, i.COUT, CASE WHEN UPPER(s.TYPE_STATION) LIKE 'A_RONAUTIQUE%' THEN 1 ELSE 0 END, "
        // Alerte rouge en cours : Heartbeat Score < 40 (V_HEARTBEAT) ou anomalie capteur critique
        // detectee dans les releves du Module 3 (V_ANOMALIES_RECENTES)
        "CASE WHEN EXISTS (SELECT 1 FROM V_HEARTBEAT h WHERE h.ID_STATION = s.ID_STATION AND h.SCORE < 40) "
        "OR EXISTS (SELECT 1 FROM V_ANOMALIES_RECENTES a WHERE a.ID_STATION = s.ID_STATION AND a.NB_CRIT > 0) "
        "THEN 1 ELSE 0 END "
        "FROM INTERVENTION i "
        "JOIN STATION s ON s.ID_STATION = i.ID_STATION "
        "JOIN EMPLOYE e ON e.ID_EMPLOYE = i.ID_EMPLOYE " + where + " ORDER BY " + order));

    if (m_model->lastError().isValid()) {
        QMessageBox::critical(this, "Erreur Oracle", m_model->lastError().text());
        return;
    }

    const QStringList heads = {"ID", "Station", "Gouvernorat", "Technicien", "Type", "Priorité",
                               "Création", "Planifiée", "Clôture", "Statut", "Coût", "Aéro", "Alerte"};
    for (int i = 0; i < heads.size(); ++i)
        m_model->setHeaderData(i, Qt::Horizontal, heads[i]);
    ui->tableView->setColumnHidden(C_AERO, true);
    ui->tableView->setColumnHidden(C_ALERT, true);
    ui->tableView->setColumnWidth(C_ID, 50);
    ui->tableView->setColumnWidth(C_STATION, 160);
    ui->tableView->setColumnWidth(C_TECH, 160);
    ui->tableView->setColumnWidth(C_TYPE, 150);

    // KPI
    while (m_model->canFetchMore()) m_model->fetchMore();
    int late = 0; double cost = 0;
    for (int r = 0; r < m_model->rowCount(); ++r) {
        if (m_model->isLate(r)) ++late;
        cost += m_model->QSqlQueryModel::data(m_model->index(r, C_COUT)).toDouble();
    }
    ui->kpiTotal->setText(QString::number(m_model->rowCount()));
    ui->kpiLate->setText(QString::number(late));
    ui->kpiCost->setText(QString::number(cost, 'f', 2) + " DT");

    QSqlQuery q(dbIsSqlite()
        ? "SELECT AVG(julianday(DATE_CLOTURE) - julianday(DATE_CREATION)) FROM INTERVENTION WHERE DATE_CLOTURE IS NOT NULL"
        : "SELECT AVG(DATE_CLOTURE - DATE_CREATION) FROM INTERVENTION WHERE DATE_CLOTURE IS NOT NULL");
    ui->kpiAvg->setText(q.next() && !q.value(0).isNull()
                        ? QString::number(q.value(0).toDouble(), 'f', 1) + " jours" : "–");

    statusBar()->showMessage(QString("%1 intervention(s) · %2 en retard SLA")
                                 .arg(m_model->rowCount()).arg(late));
}

// ------------------------------------------------------------------
//  CRUD
// ------------------------------------------------------------------
void MaintenanceWindow::addIntervention()
{
    if (!allowed("add", "Ajouter")) return;
    InterventionDialog d(0, this);
    if (d.exec() == QDialog::Accepted) reload();
}

void MaintenanceWindow::editIntervention()
{
    if (!allowed("edit", "Modifier")) return;
    const int id = selectedId();
    if (!id) { QMessageBox::information(this, "Modifier", "Sélectionnez une intervention."); return; }
    InterventionDialog d(id, this);
    if (d.exec() == QDialog::Accepted) reload();
}

// Statut de la ligne selectionnee
static QString statusOfRow(const QModelIndex &i, QAbstractItemModel *m)
{
    return m->data(m->index(i.row(), C_STATUT), Qt::DisplayRole).toString();
}

void MaintenanceWindow::deleteIntervention()
{
    if (!allowed("delete", "Supprimer")) return;
    const int id = selectedId();
    if (!id) { QMessageBox::information(this, "Supprimer", "Sélectionnez une intervention."); return; }
    const QString st = statusOfRow(ui->tableView->currentIndex(), m_model);
    if (st != "Terminée" && st != "Annulée") {
        QMessageBox::warning(this, "Suppression impossible",
            "Seule une intervention clôturée (Terminée ou Annulée) peut être supprimée.");
        return;
    }
    if (QMessageBox::question(this, "Confirmation",
            "Supprimer définitivement l'intervention n°" + QString::number(id) + " ?") != QMessageBox::Yes)
        return;
    QSqlQuery q;
    q.prepare("DELETE FROM INTERVENTION WHERE ID_INTERVENTION = ?");
    q.addBindValue(id);
    if (!q.exec()) QMessageBox::critical(this, "Erreur Oracle", q.lastError().text());
    reload();
}

void MaintenanceWindow::archiveIntervention()
{
    if (!allowed("archive", "Archiver")) return;
    const int id = selectedId();
    if (!id) { QMessageBox::information(this, "Archiver", "Sélectionnez une intervention."); return; }
    const bool restore = ui->chkArchive->isChecked();
    if (!restore) {
        const QString st = statusOfRow(ui->tableView->currentIndex(), m_model);
        if (st != "Terminée" && st != "Annulée") {
            QMessageBox::warning(this, "Archivage impossible",
                "Seule une intervention clôturée (Terminée ou Annulée) peut être archivée.");
            return;
        }
    }
    QSqlQuery q;
    q.prepare("UPDATE INTERVENTION SET ARCHIVE = ? WHERE ID_INTERVENTION = ?");
    q.addBindValue(restore ? 0 : 1);
    q.addBindValue(id);
    if (!q.exec()) QMessageBox::critical(this, "Erreur Oracle", q.lastError().text());
    reload();
}

// ------------------------------------------------------------------
//  Exports
// ------------------------------------------------------------------
void MaintenanceWindow::exportCsv()
{
    if (!allowed("export", "Export CSV")) return;
    const QString path = QFileDialog::getSaveFileName(this, "Exporter en CSV", "interventions.csv", "CSV (*.csv)");
    if (path.isEmpty()) return;
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&f);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    out << QChar(0xFEFF);                                   // BOM pour Excel
    QStringList h;
    for (int c = 0; c < C_AERO; ++c) h << m_model->headerData(c, Qt::Horizontal).toString();
    out << h.join(';') << "\n";
    for (int r = 0; r < m_model->rowCount(); ++r) {
        QStringList row;
        for (int c = 0; c < C_AERO; ++c)
            row << "\"" + m_model->data(m_model->index(r, c)).toString().replace("\"", "\"\"") + "\"";
        out << row.join(';') << "\n";
    }
    statusBar()->showMessage("CSV exporté : " + path, 5000);
}

void MaintenanceWindow::exportHistoryPdf()
{
    if (!allowed("export", "Export PDF")) return;
    const QString path = QFileDialog::getSaveFileName(this, "Exporter en PDF", "historique_interventions.pdf", "PDF (*.pdf)");
    if (path.isEmpty()) return;

    QString html = "<h2 style='color:#0d47a1'>Historique des interventions</h2>"
                   "<p>Édité le " + QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm") + "</p>"
                   "<table border='1' cellspacing='0' cellpadding='4' width='100%'><tr style='background:#e8f0fb'>";
    for (int c = 0; c < C_AERO; ++c)
        html += "<th>" + m_model->headerData(c, Qt::Horizontal).toString() + "</th>";
    html += "</tr>";
    for (int r = 0; r < m_model->rowCount(); ++r) {
        html += "<tr>";
        for (int c = 0; c < C_AERO; ++c)
            html += "<td>" + m_model->data(m_model->index(r, c)).toString().toHtmlEscaped() + "</td>";
        html += "</tr>";
    }
    html += "</table>";

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);
    printer.setPageOrientation(QPageLayout::Landscape);
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);
    statusBar()->showMessage("PDF exporté : " + path, 5000);
}

// ------------------------------------------------------------------
//  Innovant : Ordre de mission automatique + suivi SLA
// ------------------------------------------------------------------
void MaintenanceWindow::generateMissionOrder()
{
    if (!allowed("mission", "Ordre de mission")) return;
    const QModelIndex idx = ui->tableView->currentIndex();
    if (!idx.isValid()) { QMessageBox::information(this, "Ordre de mission", "Sélectionnez une intervention."); return; }
    const int r = idx.row();
    auto val = [&](int c) { return m_model->data(m_model->index(r, c)).toString().toHtmlEscaped(); };

    const int id = selectedId();
    const QString num = QString("OM-%1-%2").arg(QDate::currentDate().year()).arg(id, 5, 10, QChar('0'));
    const QDateTime dl = m_model->deadline(r);
    const bool aero  = m_model->isAero(r);
    const bool alert = m_model->isAlert(r);
    const int sla = SlaModel::slaHours(val(C_PRIO), aero || alert);

    const QString path = QFileDialog::getSaveFileName(this, "Ordre de mission", num + ".pdf", "PDF (*.pdf)");
    if (path.isEmpty()) return;

    const QString html =
        "<h1 style='color:#0d47a1'>ORDRE DE MISSION</h1>"
        "<h3>N° " + num + "</h3>"
        "<p>Date d'émission : " + QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm") + "</p><hr>"
        "<table cellpadding='6' width='100%'>"
        "<tr><td><b>Station</b></td><td>" + val(C_STATION) + " (" + val(C_GOUV) + ")</td></tr>"
        "<tr><td><b>Technicien assigné</b></td><td>" + val(C_TECH) + "</td></tr>"
        "<tr><td><b>Type d'intervention</b></td><td>" + val(C_TYPE) + "</td></tr>"
        "<tr><td><b>Priorité</b></td><td>" + val(C_PRIO) + "</td></tr>"
        "<tr><td><b>Date planifiée</b></td><td>" + val(C_PLAN) + "</td></tr>"
        "<tr><td><b>Délai SLA</b></td><td>" + QString::number(sla) + " h" +
            (aero ? " (station aéronautique : délai court)" : "") +
            (alert ? " (alerte rouge en cours : délai court)" : "") + "</td></tr>"
        "<tr><td><b>Échéance</b></td><td style='color:#c62828'><b>" + dl.toString("dd/MM/yyyy HH:mm") + "</b></td></tr>"
        "</table><br><br>"
        "<p>Signature du responsable : ______________________ &nbsp;&nbsp; Signature du technicien : ______________________</p>"
        "<p style='color:#6b7c93'>En cas de dépassement du délai, l'intervention sera escaladée vers le responsable régional (" + val(C_GOUV) + ").</p>";

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);
    statusBar()->showMessage("Ordre de mission généré : " + num, 6000);
}

void MaintenanceWindow::showSlaDelays()
{
    if (!allowed("sla", "Retards SLA")) return;
    // Retards regroupes par gouvernorat => escalade vers le responsable regional concerne
    QMap<QString, QString> byGov;
    int n = 0;
    for (int r = 0; r < m_model->rowCount(); ++r) {
        if (!m_model->isLate(r)) continue;
        ++n;
        const int h = int(m_model->deadline(r).secsTo(QDateTime::currentDateTime()) / 3600);
        const QString gov = m_model->data(m_model->index(r, C_GOUV)).toString();
        byGov[gov] += QString("   • #%1 — %2 (%3) — retard de %4 h\n")
                   .arg(m_model->data(m_model->index(r, C_ID)).toString(),
                        m_model->data(m_model->index(r, C_STATION)).toString(),
                        m_model->data(m_model->index(r, C_PRIO)).toString())
                   .arg(h);
    }
    if (!n) { QMessageBox::information(this, "Retards SLA", "Aucune intervention en retard."); return; }

    QString txt;
    for (auto it = byGov.constBegin(); it != byGov.constEnd(); ++it)
        txt += "Responsable régional — " + it.key() + " :\n" + it.value() + "\n";
    QMessageBox::warning(this, "Escalade vers le responsable régional",
                         QString("%1 intervention(s) hors délai SLA :\n\n%2").arg(n).arg(txt));
}

// ------------------------------------------------------------------
//  Innovant : Planification preventive predictive (Heartbeat Score)
// ------------------------------------------------------------------
void MaintenanceWindow::predictivePlanning()
{
    if (!allowed("predict", "Prédictif")) return;
    PredictiveDialog dlg(this);
    if (dlg.isEmpty()) {
        QMessageBox::information(this, "Prédictif",
                                 "Aucune dégradation détectée : toutes les stations sont saines.");
        return;
    }
    connect(&dlg, &PredictiveDialog::planRequested, &dlg,
            [&](int idStation, const QString &prio, const QDate &date, const QString &note) {
        InterventionDialog d(0, &dlg);
        d.preset(idStation, "Préventive", prio, date, note);
        if (d.exec() == QDialog::Accepted) { reload(); dlg.accept(); }
    });
    dlg.exec();
}

// ------------------------------------------------------------------
//  Statistiques (graphiques)
// ------------------------------------------------------------------
static QChartView *pieChart(const QString &title, const QString &sql)
{
    auto *series = new QPieSeries;
    QSqlQuery q(dbSql(sql));
    while (q.next())
        series->append(QString("%1 (%2)").arg(q.value(0).toString()).arg(q.value(1).toInt()), q.value(1).toInt());
    series->setLabelsVisible(true);
    series->setHoleSize(0.35);
    auto *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle(title);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundRoundness(14);
    chart->setTitleFont(QFont("Segoe UI", 11, QFont::Bold));
    auto *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet("background: white; border-radius: 14px;");
    addShadow(view, 22, 6, 55);
    return view;
}

void MaintenanceWindow::refreshStats()
{
    while (auto *item = ui->chartsLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    ui->chartsLayout->addWidget(pieChart("Interventions par type",
        "SELECT TYPE_INTERVENTION, COUNT(*) FROM INTERVENTION WHERE ARCHIVE=0 GROUP BY TYPE_INTERVENTION"), 0, 0);
    ui->chartsLayout->addWidget(pieChart("Interventions par statut",
        "SELECT STATUT, COUNT(*) FROM INTERVENTION WHERE ARCHIVE=0 GROUP BY STATUT"), 0, 1);

    // Cout de maintenance par gouvernorat (histogramme)
    auto *set = new QBarSet("Coût (DT)");
    QStringList cats;
    QSqlQuery q(dbSql("SELECT NVL(s.GOUVERNORAT, 'Non renseigné'), NVL(SUM(i.COUT),0) FROM INTERVENTION i "
                "JOIN STATION s ON s.ID_STATION = i.ID_STATION WHERE i.ARCHIVE=0 "
                "GROUP BY NVL(s.GOUVERNORAT, 'Non renseigné') ORDER BY 2 DESC"));
    double maxv = 1;
    while (q.next()) {
        cats << q.value(0).toString();
        *set << q.value(1).toDouble();
        maxv = qMax(maxv, q.value(1).toDouble());
    }
    auto *bars = new QBarSeries;
    bars->append(set);
    bars->setLabelsVisible(true);
    auto *chart = new QChart;
    chart->addSeries(bars);
    chart->setTitle("Coût de maintenance par gouvernorat");
    auto *ax = new QBarCategoryAxis;
    ax->append(cats);
    chart->addAxis(ax, Qt::AlignBottom);
    bars->attachAxis(ax);
    auto *ay = new QValueAxis;
    ay->setRange(0, maxv * 1.15);
    ay->setLabelFormat("%.0f");
    chart->addAxis(ay, Qt::AlignLeft);
    bars->attachAxis(ay);
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundRoundness(14);
    chart->setTitleFont(QFont("Segoe UI", 11, QFont::Bold));
    set->setColor(QColor("#1976d2"));
    auto *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet("background: white; border-radius: 14px;");
    addShadow(view, 22, 6, 55);
    ui->chartsLayout->addWidget(view, 1, 0, 1, 2);
}
