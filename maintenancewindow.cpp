#include "maintenancewindow.h"
#include "interventiondialog.h"
#include "station.h"
#include "style.h"
#include "dbcompat.h"

#include <QApplication>
#include <algorithm>
#include <QVector>
#include <QDate>
#include <QChart>
#include <QChartView>
#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QValueAxis>
#include <QPieSeries>
#include <QComboBox>
#include <QDateTime>
#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPageSize>
#include <QPrinter>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStatusBar>
#include <QTableView>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextDocument>
#include <QTextStream>
#include <QVBoxLayout>

#ifdef QT_CHARTS_USE_NAMESPACE
QT_CHARTS_USE_NAMESPACE   // Qt 5 / anciennes versions de Qt Charts
#endif

// Colonnes de la requete principale
enum Col { C_ID, C_STATION, C_GOUV, C_TECH, C_TYPE, C_PRIO, C_CREA, C_PLAN, C_CLOT,
           C_STATUT, C_COUT, C_AERO };

// ------------------------------------------------------------------
//  Modele SQL + coloration des interventions en retard SLA
// ------------------------------------------------------------------
class SlaModel : public QSqlQueryModel
{
public:
    using QSqlQueryModel::QSqlQueryModel;

    // Delai SLA (heures) selon la criticite. Station aeronautique => delai court.
    static int slaHours(const QString &prio, bool aero)
    {
        int h = 168;                          // Basse
        if (prio == "Moyenne") h = 72;
        else if (prio == "Haute") h = 24;
        else if (prio == "Urgente") h = 4;
        if (aero) h = qMax(1, h / 2);
        return h;
    }

    QDateTime deadline(int row) const
    {
        const QDateTime created = QSqlQueryModel::data(index(row, C_CREA)).toDateTime();
        const QString prio = QSqlQueryModel::data(index(row, C_PRIO)).toString();
        const bool aero = QSqlQueryModel::data(index(row, C_AERO)).toInt() == 1;
        return created.addSecs(qint64(slaHours(prio, aero)) * 3600);
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
MaintenanceWindow::MaintenanceWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Maintenance & Interventions");
    setStyleSheet(APP_STYLE);
    resize(1280, 760);

    auto *central = new QWidget;
    central->setObjectName("central");
    auto *lay = new QVBoxLayout(central);
    lay->setContentsMargins(24, 18, 24, 12);

    auto *title = new QLabel("Gestion de la Maintenance & Interventions");
    title->setObjectName("title");
    auto *sub = new QLabel("Module 5 · suivi des interventions, SLA et statistiques");
    sub->setObjectName("subtitle");
    lay->addWidget(title);
    lay->addWidget(sub);
    lay->addSpacing(8);

    auto *tabs = new QTabWidget;
    tabs->addTab(buildListTab(), "Interventions");
    tabs->addTab(buildStatsTab(), "Statistiques");
    connect(tabs, &QTabWidget::currentChanged, this, [this](int i) { if (i == 1) refreshStats(); });
    lay->addWidget(tabs, 1);

    setCentralWidget(central);
    if (QSqlDatabase::database().isOpen())
        reload();
}

QWidget *MaintenanceWindow::makeKpiCard(const QString &label, QLabel **valueOut)
{
    auto *card = new QFrame;
    card->setObjectName("card");
    auto *l = new QVBoxLayout(card);
    *valueOut = new QLabel("–");
    (*valueOut)->setObjectName("kpiValue");
    auto *t = new QLabel(label);
    t->setObjectName("kpiLabel");
    l->addWidget(*valueOut);
    l->addWidget(t);
    return card;
}

QWidget *MaintenanceWindow::buildListTab()
{
    auto *w = new QWidget;
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(0, 8, 0, 0);

    // --- KPI ---
    auto *kpis = new QHBoxLayout;
    kpis->addWidget(makeKpiCard("Interventions actives", &m_kpiTotal));
    kpis->addWidget(makeKpiCard("En retard SLA", &m_kpiLate));
    kpis->addWidget(makeKpiCard("Temps moyen de résolution", &m_kpiAvg));
    kpis->addWidget(makeKpiCard("Coût total", &m_kpiCost));
    v->addLayout(kpis);

    // --- Recherche / tri ---
    m_search = new QLineEdit;
    m_search->setPlaceholderText("🔎  Rechercher par station, technicien, type ou gouvernorat...");
    m_sort = new QComboBox;
    m_sort->addItems({"Trier : Priorité", "Trier : Date planifiée", "Trier : Statut", "Trier : Coût"});
    m_statusFilter = new QComboBox;
    m_statusFilter->addItems({"Tous les statuts", "Planifiée", "En cours", "Terminée", "Annulée"});

    auto *bar = new QHBoxLayout;
    bar->addWidget(m_search, 1);
    bar->addWidget(m_statusFilter);
    bar->addWidget(m_sort);
    v->addLayout(bar);

    connect(m_search, &QLineEdit::textChanged, this, &MaintenanceWindow::reload);
    connect(m_sort, qOverload<int>(&QComboBox::currentIndexChanged), this, &MaintenanceWindow::reload);
    connect(m_statusFilter, qOverload<int>(&QComboBox::currentIndexChanged), this, &MaintenanceWindow::reload);

    // --- Table ---
    m_model = new SlaModel(this);
    m_table = new QTableView;
    m_table->setModel(m_model);
    m_table->setAlternatingRowColors(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->hide();
    m_table->verticalHeader()->setDefaultSectionSize(36);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setShowGrid(false);
    connect(m_table, &QTableView::doubleClicked, this, &MaintenanceWindow::editIntervention);
    v->addWidget(m_table, 1);

    // --- Boutons ---
    auto *btns = new QHBoxLayout;
    auto mk = [&](const QString &text, const char *obj = nullptr) {
        auto *b = new QPushButton(text);
        if (obj) b->setObjectName(obj);
        btns->addWidget(b);
        return b;
    };
    connect(mk("＋  Ajouter", "primary"), &QPushButton::clicked, this, &MaintenanceWindow::addIntervention);
    connect(mk("✎  Modifier"), &QPushButton::clicked, this, &MaintenanceWindow::editIntervention);
    connect(mk("🗄  Archiver"), &QPushButton::clicked, this, &MaintenanceWindow::archiveIntervention);
    connect(mk("🗑  Supprimer", "danger"), &QPushButton::clicked, this, &MaintenanceWindow::deleteIntervention);
    btns->addStretch();
    connect(mk("📄  Ordre de mission", "accent"), &QPushButton::clicked, this, &MaintenanceWindow::generateMissionOrder);
    connect(mk("⚠  Retards SLA"), &QPushButton::clicked, this, &MaintenanceWindow::showSlaDelays);
    connect(mk("🔮  Planification prédictive", "accent"), &QPushButton::clicked, this, &MaintenanceWindow::predictivePlanning);
    connect(mk("CSV"), &QPushButton::clicked, this, &MaintenanceWindow::exportCsv);
    connect(mk("PDF"), &QPushButton::clicked, this, &MaintenanceWindow::exportHistoryPdf);
    v->addLayout(btns);
    return w;
}

QWidget *MaintenanceWindow::buildStatsTab()
{
    auto *w = new QWidget;
    m_chartsLayout = new QGridLayout(w);
    m_chartsLayout->setContentsMargins(0, 8, 0, 0);
    m_chartsLayout->setSpacing(14);
    return w;
}

int MaintenanceWindow::selectedId() const
{
    const QModelIndex i = m_table->currentIndex();
    return i.isValid() ? m_model->data(m_model->index(i.row(), C_ID), Qt::DisplayRole).toInt() : 0;
}

// ------------------------------------------------------------------
//  Chargement : recherche + tri + filtre
// ------------------------------------------------------------------
void MaintenanceWindow::reload()
{
    QString where = "WHERE i.ARCHIVE = 0";

    const QString t = m_search->text().trimmed().toUpper();
    if (!t.isEmpty()) {
        const QString like = "'%" + esc(t) + "%'";
        where += " AND (UPPER(s.NOM_STATION) LIKE " + like +
                 " OR UPPER(e.NOM || ' ' || e.PRENOM) LIKE " + like +
                 " OR UPPER(i.TYPE_INTERVENTION) LIKE " + like +
                 " OR UPPER(s.GOUVERNORAT) LIKE " + like + ")";
    }
    if (m_statusFilter->currentIndex() > 0)
        where += " AND i.STATUT = '" + esc(m_statusFilter->currentText()) + "'";

    QString order;
    switch (m_sort->currentIndex()) {
    case 0: order = "CASE i.PRIORITE WHEN 'Urgente' THEN 1 WHEN 'Haute' THEN 2 "
                    "WHEN 'Moyenne' THEN 3 ELSE 4 END, i.DATE_PLANIFIEE"; break;
    case 1: order = "i.DATE_PLANIFIEE"; break;
    case 2: order = "i.STATUT, i.DATE_PLANIFIEE"; break;
    default: order = "i.COUT DESC"; break;
    }

    m_model->setQuery(
        "SELECT i.ID_INTERVENTION, s.NOM_STATION, s.GOUVERNORAT, e.NOM || ' ' || e.PRENOM, "
        "i.TYPE_INTERVENTION, i.PRIORITE, i.DATE_CREATION, i.DATE_PLANIFIEE, i.DATE_CLOTURE, "
        "i.STATUT, i.COUT, CASE WHEN s.TYPE_STATION LIKE 'A%ronautique' THEN 1 ELSE 0 END "
        "FROM INTERVENTION i "
        "JOIN STATION s ON s.ID_STATION = i.ID_STATION "
        "JOIN EMPLOYE e ON e.ID_EMPLOYE = i.ID_EMPLOYE " + where + " ORDER BY " + order);

    if (m_model->lastError().isValid()) {
        QMessageBox::critical(this, "Erreur Oracle", m_model->lastError().text());
        return;
    }

    const QStringList heads = {"ID", "Station", "Gouvernorat", "Technicien", "Type", "Priorité",
                               "Création", "Planifiée", "Clôture", "Statut", "Coût", "Aéro"};
    for (int i = 0; i < heads.size(); ++i)
        m_model->setHeaderData(i, Qt::Horizontal, heads[i]);
    m_table->setColumnHidden(C_AERO, true);
    m_table->setColumnWidth(C_ID, 50);
    m_table->setColumnWidth(C_STATION, 160);
    m_table->setColumnWidth(C_TECH, 160);
    m_table->setColumnWidth(C_TYPE, 150);

    // KPI
    while (m_model->canFetchMore()) m_model->fetchMore();
    int late = 0; double cost = 0;
    for (int r = 0; r < m_model->rowCount(); ++r) {
        if (m_model->isLate(r)) ++late;
        cost += m_model->QSqlQueryModel::data(m_model->index(r, C_COUT)).toDouble();
    }
    m_kpiTotal->setText(QString::number(m_model->rowCount()));
    m_kpiLate->setText(QString::number(late));
    m_kpiCost->setText(QString::number(cost, 'f', 2) + " DT");

    const QString sqlMoyenne = DbCompat::isSqlite()
        ? "SELECT AVG(julianday(DATE_CLOTURE) - julianday(DATE_CREATION)) FROM INTERVENTION "
          "WHERE DATE_CLOTURE IS NOT NULL AND ARCHIVE = 0"
        : "SELECT AVG(DATE_CLOTURE - DATE_CREATION) FROM INTERVENTION "
          "WHERE DATE_CLOTURE IS NOT NULL AND ARCHIVE = 0";
    QSqlQuery q(sqlMoyenne);
    m_kpiAvg->setText(q.next() && !q.value(0).isNull()
                      ? QString::number(q.value(0).toDouble(), 'f', 1) + " jours" : "–");

    statusBar()->showMessage(QString("%1 intervention(s) · %2 en retard SLA").arg(m_model->rowCount()).arg(late));
}

// ------------------------------------------------------------------
//  CRUD
// ------------------------------------------------------------------
void MaintenanceWindow::addIntervention()
{
    InterventionDialog d(0, this);
    if (d.exec() == QDialog::Accepted) reload();
}

void MaintenanceWindow::editIntervention()
{
    const int id = selectedId();
    if (!id) { QMessageBox::information(this, "Modifier", "Sélectionnez une intervention."); return; }
    InterventionDialog d(id, this);
    if (d.exec() == QDialog::Accepted) reload();
}

void MaintenanceWindow::deleteIntervention()
{
    const int id = selectedId();
    if (!id) { QMessageBox::information(this, "Supprimer", "Sélectionnez une intervention."); return; }
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
    const int id = selectedId();
    if (!id) { QMessageBox::information(this, "Archiver", "Sélectionnez une intervention."); return; }
    QSqlQuery q;
    q.prepare("UPDATE INTERVENTION SET ARCHIVE = 1 WHERE ID_INTERVENTION = ?");
    q.addBindValue(id);
    if (!q.exec()) QMessageBox::critical(this, "Erreur Oracle", q.lastError().text());
    reload();
}

// ------------------------------------------------------------------
//  Exports
// ------------------------------------------------------------------
void MaintenanceWindow::exportCsv()
{
    const QString path = QFileDialog::getSaveFileName(this, "Exporter en CSV", "interventions.csv", "CSV (*.csv)");
    if (path.isEmpty()) return;
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
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
//  Innovant 2 : Ordre de mission automatique + suivi SLA
// ------------------------------------------------------------------
void MaintenanceWindow::generateMissionOrder()
{
    const QModelIndex idx = m_table->currentIndex();
    if (!idx.isValid()) { QMessageBox::information(this, "Ordre de mission", "Sélectionnez une intervention."); return; }
    const int r = idx.row();
    auto val = [&](int c) { return m_model->data(m_model->index(r, c)).toString().toHtmlEscaped(); };

    const int id = selectedId();
    const QString num = QString("OM-%1-%2").arg(QDate::currentDate().year()).arg(id, 5, 10, QChar('0'));
    const QDateTime dl = m_model->deadline(r);
    const bool aero = m_model->QSqlQueryModel::data(m_model->index(r, C_AERO)).toInt() == 1;
    const int sla = SlaModel::slaHours(val(C_PRIO), aero);

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
            (aero ? " (station aéronautique : délai court)" : "") + "</td></tr>"
        "<tr><td><b>Échéance</b></td><td style='color:#c62828'><b>" + dl.toString("dd/MM/yyyy HH:mm") + "</b></td></tr>"
        "</table><br><br>"
        "<p>Signature du responsable : ______________________ &nbsp;&nbsp; Signature du technicien : ______________________</p>"
        "<p style='color:#6b7c93'>En cas de dépassement du délai, l'intervention sera escaladée vers le responsable régional.</p>";

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
    QString txt;
    int n = 0;
    for (int r = 0; r < m_model->rowCount(); ++r) {
        if (!m_model->isLate(r)) continue;
        ++n;
        const int h = int(m_model->deadline(r).secsTo(QDateTime::currentDateTime()) / 3600);
        txt += QString("• #%1 — %2 (%3) — retard de %4 h\n")
                   .arg(m_model->data(m_model->index(r, C_ID)).toString(),
                        m_model->data(m_model->index(r, C_STATION)).toString(),
                        m_model->data(m_model->index(r, C_PRIO)).toString())
                   .arg(h);
    }
    if (!n) { QMessageBox::information(this, "Retards SLA", "Aucune intervention en retard. 👍"); return; }
    QMessageBox::warning(this, "Escalade vers le responsable régional",
                         QString("%1 intervention(s) hors délai SLA :\n\n%2").arg(n).arg(txt));
}

// ------------------------------------------------------------------
//  Innovant 1 : Planification preventive predictive (Heartbeat Score)
// ------------------------------------------------------------------
void MaintenanceWindow::predictivePlanning()
{
    // Heartbeat Score calcule par le module Stations (Station::calculerScore)
    // a partir du statut et de l'anciennete ; seuil < 75 => maintenance preventive suggeree
    struct Row { int id; QString nom; int score; };
    QVector<Row> rows;
    {
        QSqlQuery q("SELECT ID_STATION, NOM_STATION, STATUT, "
                    + DbCompat::dateExpr("DATE_INSTALLATION", "DD/MM/YYYY") + " FROM STATION");
        while (q.next()) {
            const QDate inst = QDate::fromString(q.value(3).toString(), "dd/MM/yyyy");
            const int sc = Station::calculerScore(q.value(2).toString(), inst);
            if (sc < 75)
                rows.push_back({q.value(0).toInt(), q.value(1).toString(), sc});
        }
        std::sort(rows.begin(), rows.end(), [](const Row &a, const Row &b) { return a.score < b.score; });
    }

    QDialog dlg(this);
    dlg.setWindowTitle("Planification préventive prédictive");
    dlg.resize(620, 360);
    auto *v = new QVBoxLayout(&dlg);
    v->addWidget(new QLabel("Stations dont le Heartbeat Score se dégrade. Double-cliquez pour planifier :"));
    auto *tw = new QTableWidget(0, 4);
    tw->setHorizontalHeaderLabels({"Station", "Score", "Priorité suggérée", "Date suggérée"});
    tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tw->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw->horizontalHeader()->setStretchLastSection(true);
    tw->verticalHeader()->hide();
    v->addWidget(tw);

    struct Sug { int id; QString prio; QDate date; };
    QVector<Sug> sugs;
    for (const Row &r : rows) {
        const double score = r.score;
        QString prio; int days;
        if (score < 40)      { prio = "Urgente"; days = 1; }
        else if (score < 60) { prio = "Haute";   days = 3; }
        else                 { prio = "Moyenne"; days = 7; }
        const QDate d = QDate::currentDate().addDays(days);
        sugs.push_back({r.id, prio, d});

        const int row = tw->rowCount();
        tw->insertRow(row);
        tw->setItem(row, 0, new QTableWidgetItem(r.nom));
        tw->setItem(row, 1, new QTableWidgetItem(QString::number(score, 'f', 0)));
        tw->setItem(row, 2, new QTableWidgetItem(prio));
        tw->setItem(row, 3, new QTableWidgetItem(d.toString("dd/MM/yyyy")));
    }
    if (sugs.isEmpty()) {
        QMessageBox::information(this, "Prédictif", "Aucune dégradation détectée : toutes les stations sont saines.");
        return;
    }

    connect(tw, &QTableWidget::cellDoubleClicked, &dlg, [&](int row, int) {
        InterventionDialog d(0, &dlg);
        d.preset(sugs[row].id, "Préventive", sugs[row].prio, sugs[row].date);
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
    QSqlQuery q(sql);
    while (q.next())
        series->append(QString("%1 (%2)").arg(q.value(0).toString()).arg(q.value(1).toInt()), q.value(1).toInt());
    series->setLabelsVisible(true);
    series->setHoleSize(0.35);
    auto *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle(title);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    auto *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    return view;
}

void MaintenanceWindow::refreshStats()
{
    while (auto *item = m_chartsLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    m_chartsLayout->addWidget(pieChart("Interventions par type",
        "SELECT TYPE_INTERVENTION, COUNT(*) FROM INTERVENTION WHERE ARCHIVE=0 GROUP BY TYPE_INTERVENTION"), 0, 0);
    m_chartsLayout->addWidget(pieChart("Interventions par statut",
        "SELECT STATUT, COUNT(*) FROM INTERVENTION WHERE ARCHIVE=0 GROUP BY STATUT"), 0, 1);

    // Cout de maintenance par gouvernorat (histogramme)
    auto *set = new QBarSet("Coût (DT)");
    QStringList cats;
    QSqlQuery q("SELECT s.GOUVERNORAT, COALESCE(SUM(i.COUT),0) FROM INTERVENTION i "
                "JOIN STATION s ON s.ID_STATION = i.ID_STATION WHERE i.ARCHIVE=0 "
                "GROUP BY s.GOUVERNORAT ORDER BY 2 DESC");
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
    auto *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    m_chartsLayout->addWidget(view, 1, 0, 1, 2);
}

void MaintenanceWindow::refreshData()
{
    reload();
    refreshStats();
}
