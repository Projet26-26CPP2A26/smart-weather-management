#include "relevewindow.h"
#include "releve_dialog.h"
#include "attestation_dialog.h"
#include "dbcompat.h"
#include "ui_relevewindow.h"   // généré depuis relevewindow.ui

#include <QtWidgets>
#include <QtSql>
#include <QtCharts>
#include <QPdfWriter>
#include <QHash>
#include <QVector>
#include <cmath>

static const char *COLS[]   = {"TEMPERATURE", "HUMIDITE", "PRESSION", "VITESSE_VENT"};
static const char *LABELS[] = {"Température (°C)", "Humidité (%)", "Pression (hPa)", "Vent (km/h)"};

// ---------------------------------------------------------------- Interface
// La disposition des widgets est définie dans relevewindow.ui (Qt Designer).
ReleveWindow::ReleveWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::ReleveWindow)
{
    ui->setupUi(this);

    model = new QSqlQueryModel(this);
    view = ui->view;
    view->setModel(model);

    stationEdit = ui->stationEdit;
    useDates = ui->useDates;
    fromEdit = ui->fromEdit;
    toEdit = ui->toEdit;
    sortBy = ui->sortBy;
    sortDir = ui->sortDir;
    fromEdit->setDate(QDate::currentDate().addMonths(-1));
    toEdit->setDate(QDate::currentDate());

    connect(ui->searchButton,  &QPushButton::clicked, this, &ReleveWindow::refresh);
    connect(ui->addButton,     &QPushButton::clicked, this, &ReleveWindow::addReleve);
    connect(ui->editButton,    &QPushButton::clicked, this, &ReleveWindow::editReleve);
    connect(ui->deleteButton,  &QPushButton::clicked, this, &ReleveWindow::deleteReleve);
    connect(ui->pdfButton,     &QPushButton::clicked, this, &ReleveWindow::exportPdf);
    connect(ui->histoButton,   &QPushButton::clicked, this, &ReleveWindow::showHistogram);
    connect(ui->attestButton,  &QPushButton::clicked, this, &ReleveWindow::showAttestation);
    connect(ui->anomalyButton, &QPushButton::clicked, this, &ReleveWindow::showAnomalies);

    if (QSqlDatabase::database().isOpen())
        refresh();
}

ReleveWindow::~ReleveWindow() { delete ui; }

// ------------------------------------------------------------------ Outils
void ReleveWindow::fail(const QSqlQuery &q)
{
    QMessageBox::critical(this, "Erreur Oracle", q.lastError().text());
}

void ReleveWindow::showText(const QString &title, const QString &text)
{
    QDialog d(this);
    d.setWindowTitle(title);
    d.resize(650, 450);
    auto *l = new QVBoxLayout(&d);
    auto *t = new QPlainTextEdit(text);
    t->setReadOnly(true);
    l->addWidget(t);
    d.exec();
}

int ReleveWindow::currentRow() const
{
    auto sel = view->selectionModel()->selectedRows();
    return sel.isEmpty() ? -1 : sel.first().row();
}

QString ReleveWindow::whereClause(QVariantList &b) const
{
    QStringList c;
    const QString st = stationEdit->text().trimmed();
    if (!st.isEmpty()) { c << "ID_STATION = ?"; b << st.toInt(); }
    if (useDates->isChecked()) {
        c << "HORODATAGE >= ? AND HORODATAGE < ?";
        b << QDateTime(fromEdit->date(), QTime(0, 0));
        b << QDateTime(toEdit->date().addDays(1), QTime(0, 0));
    }
    return c.isEmpty() ? QString() : " WHERE " + c.join(" AND ");
}

QSqlQuery ReleveWindow::run(const QString &sql, const QVariantList &binds)
{
    QSqlQuery q;
    q.prepare(sql);
    for (const auto &v : binds) q.addBindValue(v);
    if (!q.exec()) fail(q);
    return q;
}

// -------------------------------------------------- CRUD + tri + recherche
void ReleveWindow::refresh()
{
    QVariantList b;
    QString sql = "SELECT ID_RELEVE, ID_STATION, HORODATAGE, TEMPERATURE, HUMIDITE, PRESSION, VITESSE_VENT, "
                  "PRECIPITATION, MODE_COLLECTE FROM RELEVE_METEO" + whereClause(b) +
                  " ORDER BY " + (sortBy->currentIndex() == 0 ? "HORODATAGE" : "TEMPERATURE") +
                  (sortDir->currentIndex() == 0 ? " ASC" : " DESC");
    model->setQuery(run(sql, b));
    const char *h[] = {"ID", "Station", "Horodatage", "Temp. (°C)", "Humidité (%)", "Pression (hPa)",
                       "Vent (km/h)", "Précip. (mm)", "Collecte"};
    for (int i = 0; i < 9; ++i) model->setHeaderData(i, Qt::Horizontal, h[i]);
    view->resizeColumnsToContents();
}

void ReleveWindow::addReleve()
{
    ReleveDialog d(this);
    if (d.exec() != QDialog::Accepted) return;

    // Détection d'anomalie sur le NOUVEAU relevé : comparaison à l'historique de la station
    const double vals[4] = {d.temp->value(), d.hum->value(), d.pres->value(), d.vent->value()};
    QStringList alerts;
    for (int i = 0; i < 4; ++i) {
        QSqlQuery h = run(QString("SELECT %1 FROM RELEVE_METEO WHERE ID_STATION = ? AND %1 IS NOT NULL")
                              .arg(COLS[i]), {d.stationId()});
        QVector<double> historique;
        while (h.next()) historique << h.value(0).toDouble();
        if (historique.size() < 10) continue;

        double somme = 0;
        for (double v : historique) somme += v;
        const double moyenne = somme / historique.size();
        double ecarts = 0;
        for (double v : historique) ecarts += (v - moyenne) * (v - moyenne);
        const double sigma = std::sqrt(ecarts / (historique.size() - 1));

        if (sigma > 0 && std::fabs(vals[i] - moyenne) > 3 * sigma)
            alerts << QString("%1 = %2 (moyenne historique %3)")
                          .arg(LABELS[i]).arg(vals[i]).arg(moyenne, 0, 'f', 1);
    }

    QSqlQuery q;
    q.prepare("INSERT INTO RELEVE_METEO (ID_RELEVE, ID_STATION, HORODATAGE, TEMPERATURE, HUMIDITE, PRESSION, "
              "VITESSE_VENT, PRECIPITATION, MODE_COLLECTE) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(DbCompat::nextId("RELEVE_METEO", "ID_RELEVE"));
    q.addBindValue(d.stationId());
    q.addBindValue(d.horo->dateTime());
    q.addBindValue(d.temp->value());
    q.addBindValue(d.hum->value());
    q.addBindValue(d.pres->value());
    q.addBindValue(d.vent->value());
    q.addBindValue(d.precip->value());
    q.addBindValue(d.mode->currentText());
    if (!q.exec()) { fail(q); return; }
    refresh();

    if (!alerts.isEmpty())
        QMessageBox::warning(this, "Valeur aberrante",
            QString("Capteur potentiellement défaillant (station %1) :\n\n%2")
                .arg(d.stationId()).arg(alerts.join("\n")));
}

void ReleveWindow::editReleve()
{
    int r = currentRow();
    if (r < 0) { QMessageBox::information(this, "Modifier", "Sélectionnez un relevé."); return; }
    auto val = [&](int c) { return model->index(r, c).data(); };
    const int id = val(0).toInt();

    ReleveDialog d(this);
    d.setStationId(val(1).toInt());
    d.horo->setDateTime(val(2).toDateTime());
    d.temp->setValue(val(3).toDouble());
    d.hum->setValue(val(4).toDouble());
    d.pres->setValue(val(5).toDouble());
    d.vent->setValue(val(6).toDouble());
    d.precip->setValue(val(7).toDouble());
    d.mode->setCurrentText(val(8).toString());
    if (d.exec() != QDialog::Accepted) return;

    QSqlQuery q;
    q.prepare("UPDATE RELEVE_METEO SET ID_STATION=?, HORODATAGE=?, TEMPERATURE=?, HUMIDITE=?, PRESSION=?, "
              "VITESSE_VENT=?, PRECIPITATION=?, MODE_COLLECTE=? WHERE ID_RELEVE=?");
    q.addBindValue(d.stationId());
    q.addBindValue(d.horo->dateTime());
    q.addBindValue(d.temp->value());
    q.addBindValue(d.hum->value());
    q.addBindValue(d.pres->value());
    q.addBindValue(d.vent->value());
    q.addBindValue(d.precip->value());
    q.addBindValue(d.mode->currentText());
    q.addBindValue(id);
    if (!q.exec()) { fail(q); return; }
    refresh();
}

void ReleveWindow::deleteReleve()
{
    int r = currentRow();
    if (r < 0) { QMessageBox::information(this, "Supprimer", "Sélectionnez un relevé."); return; }
    const int id = model->index(r, 0).data().toInt();
    if (QMessageBox::question(this, "Supprimer", QString("Supprimer le relevé n°%1 ?").arg(id)) != QMessageBox::Yes)
        return;
    run("DELETE FROM RELEVE_METEO WHERE ID_RELEVE = ?", {id});
    refresh();
}

// --------------------------------------------------- Statistiques mensuelles
QList<MonthStat> ReleveWindow::monthlyStats()
{
    QVariantList b;
    const QString mois = DbCompat::dateExpr("HORODATAGE", "YYYY-MM");
    QString sql = "SELECT " + mois + ", MIN(TEMPERATURE), AVG(TEMPERATURE), MAX(TEMPERATURE), "
                  "COALESCE(SUM(PRECIPITATION),0) FROM RELEVE_METEO" + whereClause(b) +
                  " GROUP BY " + mois + " ORDER BY 1";
    QList<MonthStat> out;
    QSqlQuery q = run(sql, b);
    while (q.next())
        out.append({q.value(0).toString(), q.value(1).toDouble(), q.value(2).toDouble(),
                    q.value(3).toDouble(), q.value(4).toDouble()});
    return out;
}

// Histogramme des températures (min/moy/max) ou des précipitations (cumul) par période
void ReleveWindow::showHistogram()
{
    bool ok;
    const QString metric = QInputDialog::getItem(this, "Histogramme", "Grandeur :",
                                                 QStringList{"Températures", "Précipitations"}, 0, false, &ok);
    if (!ok) return;
    const QString period = QInputDialog::getItem(this, "Histogramme", "Période :",
                                                 QStringList{"Jour", "Mois", "Année"}, 1, false, &ok);
    if (!ok) return;
    const QString fmt = period == "Jour" ? "YYYY-MM-DD" : (period == "Mois" ? "YYYY-MM" : "YYYY");
    const bool temps = metric == "Températures";

    QVariantList b;
    const QString periode = DbCompat::dateExpr("HORODATAGE", fmt);
    QString sql = QString("SELECT %1, %2 FROM RELEVE_METEO")
                      .arg(periode, QString(temps ? "MIN(TEMPERATURE), AVG(TEMPERATURE), MAX(TEMPERATURE)"
                                                  : "COALESCE(SUM(PRECIPITATION),0)"))
                  + whereClause(b)
                  + " GROUP BY " + periode + " ORDER BY 1";
    QSqlQuery q = run(sql, b);

    auto *s1 = new QBarSet(temps ? "Minimale" : "Précipitations (mm)");
    QBarSet *s2 = temps ? new QBarSet("Moyenne") : nullptr;
    QBarSet *s3 = temps ? new QBarSet("Maximale") : nullptr;
    QStringList cats;
    while (q.next()) {
        cats << q.value(0).toString();
        *s1 << q.value(1).toDouble();
        if (temps) { *s2 << q.value(2).toDouble(); *s3 << q.value(3).toDouble(); }
    }
    if (cats.isEmpty()) { QMessageBox::information(this, "Histogramme", "Aucune donnée."); return; }

    auto *series = new QBarSeries;
    series->append(s1);
    if (temps) { series->append(s2); series->append(s3); }

    auto *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle(temps ? QString("Températures minimales, moyennes et maximales — par %1").arg(period.toLower())
                          : QString("Précipitations cumulées — par %1").arg(period.toLower()));
    auto *ax = new QBarCategoryAxis; ax->append(cats);
    auto *ay = new QValueAxis; ay->setTitleText(temps ? "°C" : "mm");
    chart->addAxis(ax, Qt::AlignBottom); series->attachAxis(ax);
    chart->addAxis(ay, Qt::AlignLeft);   series->attachAxis(ay);

    QDialog d(this);
    d.setWindowTitle("Histogramme");
    d.resize(800, 500);
    auto *l = new QVBoxLayout(&d);
    auto *cv = new QChartView(chart);
    cv->setRenderHint(QPainter::Antialiasing);
    l->addWidget(cv);
    d.exec();
}

// ------------------------------------------------------- Export PDF ou CSV
void ReleveWindow::exportPdf()
{
    const QString path = QFileDialog::getSaveFileName(this, "Exporter le bilan climatique",
                                                      "bilan_climatique.pdf", "PDF (*.pdf);;CSV (*.csv)");
    if (path.isEmpty()) return;

    // ---- CSV (séparateur « ; » et BOM UTF-8 pour Excel)
    if (path.endsWith(".csv", Qt::CaseInsensitive)) {
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Export CSV", f.errorString());
            return;
        }
        QTextStream out(&f);
        out.setEncoding(QStringConverter::Utf8);
        out.setGenerateByteOrderMark(true);
        QStringList head;
        for (int c = 0; c < model->columnCount(); ++c) head << model->headerData(c, Qt::Horizontal).toString();
        out << head.join(';') << "\n";
        for (int r = 0; r < model->rowCount(); ++r) {
            QStringList row;
            for (int c = 0; c < model->columnCount(); ++c) {
                QVariant v = model->index(r, c).data();
                if (v.typeId() == QMetaType::QDateTime) row << v.toDateTime().toString("yyyy-MM-dd HH:mm:ss");
                else if (v.typeId() == QMetaType::Double) row << QString::number(v.toDouble(), 'f', 2).replace('.', ',');
                else row << v.toString();
            }
            out << row.join(';') << "\n";
        }
        QMessageBox::information(this, "Export CSV", "Fichier généré :\n" + path);
        return;
    }

    // ---- PDF
    QString html = "<h1 align='center'>Bilan climatique</h1>";
    html += QString("<p>Station : <b>%1</b> — Généré le %2</p>")
                .arg(stationEdit->text().trimmed().isEmpty() ? "toutes" : stationEdit->text().trimmed(),
                     QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"));
    if (useDates->isChecked())
        html += QString("<p>Période : %1 au %2</p>").arg(fromEdit->date().toString("dd/MM/yyyy"),
                                                         toEdit->date().toString("dd/MM/yyyy"));

    const QString th = "<tr bgcolor='#dfe6ee'>";
    html += "<h2>Synthèse mensuelle</h2><table border='1' cellpadding='4' cellspacing='0' width='100%'>"
            + th + "<th>Mois</th><th>T min</th><th>T moyenne</th><th>T max</th><th>Précip. (mm)</th></tr>";
    for (const auto &m : monthlyStats())
        html += QString("<tr align='center'><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td></tr>")
                    .arg(m.month).arg(m.mn, 0, 'f', 1).arg(m.avg, 0, 'f', 1).arg(m.mx, 0, 'f', 1)
                    .arg(m.precip, 0, 'f', 1);
    html += "</table><h2>Détail des relevés</h2>"
            "<table border='1' cellpadding='3' cellspacing='0' width='100%'>" + th;
    for (int c = 0; c < model->columnCount(); ++c)
        html += "<th>" + model->headerData(c, Qt::Horizontal).toString() + "</th>";
    html += "</tr>";
    for (int r = 0; r < model->rowCount(); ++r) {
        html += "<tr align='center'>";
        for (int c = 0; c < model->columnCount(); ++c) {
            QVariant v = model->index(r, c).data();
            html += "<td>" + (v.typeId() == QMetaType::QDateTime ? v.toDateTime().toString("dd/MM/yyyy HH:mm")
                                                                  : v.toString()) + "</td>";
        }
        html += "</tr>";
    }
    html += "</table>";

    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&writer);
    QMessageBox::information(this, "Export PDF", "Rapport généré :\n" + path);
}

// ---------- Métier innovant 2 : attestation météorologique automatique
// (logique complète dans attestation_dialog.h)
void ReleveWindow::showAttestation()
{
    AttestationDialog d(this);
    d.exec();
}

// --------------- Métier innovant 1 : détection d'anomalies de capteurs
// Un relevé est suspect s'il s'écarte de plus de 3 écarts-types de la moyenne
// historique de la même station (pour chaque grandeur mesurée).
void ReleveWindow::showAnomalies()
{
    QString txt;
    int total = 0;
    for (int i = 0; i < 4; ++i) {
        const QString c = COLS[i];
        struct Mesure { int id; int station; QDateTime horo; double valeur; };
        QVector<Mesure> mesures;
        QHash<int, QVector<double>> parStation;

        QSqlQuery q = run(QString("SELECT ID_RELEVE, ID_STATION, HORODATAGE, %1 FROM RELEVE_METEO "
                                  "WHERE %1 IS NOT NULL ORDER BY ID_STATION, HORODATAGE").arg(c));
        while (q.next()) {
            const Mesure m{q.value(0).toInt(), q.value(1).toInt(),
                           q.value(2).toDateTime(), q.value(3).toDouble()};
            mesures.append(m);
            parStation[m.station].append(m.valeur);
        }

        QHash<int, QPair<double, double>> stats;
        for (auto it = parStation.cbegin(); it != parStation.cend(); ++it) {
            const QVector<double> &v = it.value();
            if (v.size() < 2) continue;
            double somme = 0;
            for (double x : v) somme += x;
            const double moyenne = somme / v.size();
            double ecarts = 0;
            for (double x : v) ecarts += (x - moyenne) * (x - moyenne);
            stats.insert(it.key(), {moyenne, std::sqrt(ecarts / (v.size() - 1))});
        }

        for (const Mesure &m : mesures) {
            if (!stats.contains(m.station)) continue;
            const double moyenne = stats[m.station].first;
            const double sigma = stats[m.station].second;
            if (sigma > 0 && std::fabs(m.valeur - moyenne) > 3 * sigma) {
                txt += QString("Relevé %1 | Station %2 | %3 | %4 = %5 (moyenne %6, σ %7)\n")
                           .arg(m.id).arg(m.station)
                           .arg(m.horo.toString("dd/MM/yyyy HH:mm"))
                           .arg(LABELS[i]).arg(m.valeur, 0, 'f', 1)
                           .arg(moyenne, 0, 'f', 1).arg(sigma, 0, 'f', 1);
                ++total;
            }
        }
    }
    if (total == 0) txt = "Aucune anomalie détectée : tous les capteurs semblent cohérents.";
    else txt = QString("%1 relevé(s) suspect(s) — capteur potentiellement défaillant :\n\n").arg(total) + txt;
    showText("Détection d'anomalies de capteurs", txt);
}
