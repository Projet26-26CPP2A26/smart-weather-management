#pragma once
#include <QtWidgets>
#include <QtSql>
#include <QPdfWriter>
#include <cmath>

// Métier innovant : Attestation Météorologique Automatique
// Demande (demandeur, lieu, période) -> station la plus proche disposant de
// relevés -> extraction des relevés -> attestation PDF numérotée -> suivi du statut.
class AttestationDialog : public QDialog
{
public:
    explicit AttestationDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowTitle("Attestations météorologiques");
        resize(950, 620);

        demandeur = new QLineEdit;
        lieu = new QLineEdit;
        lat = new QDoubleSpinBox; lat->setRange(-90, 90);   lat->setDecimals(4);
        lon = new QDoubleSpinBox; lon->setRange(-180, 180); lon->setDecimals(4);
        from = new QDateEdit(QDate::currentDate());
        to = new QDateEdit(QDate::currentDate());
        for (auto *d : {from, to}) { d->setCalendarPopup(true); d->setDisplayFormat("dd/MM/yyyy"); }
        statut = new QComboBox;
        statut->addItems({"EN_COURS", "DELIVREE", "REFUSEE", "ANNULEE"});

        auto *form = new QFormLayout;
        form->addRow("Demandeur", demandeur);
        form->addRow("Lieu", lieu);
        form->addRow("Latitude (°)", lat);
        form->addRow("Longitude (°)", lon);
        form->addRow("Du", from);
        form->addRow("Au", to);

        auto *gen = new QPushButton("Générer l'attestation PDF");
        gen->setProperty("primary", true);
        auto *upd = new QPushButton("Mettre à jour le statut →");

        model = new QSqlQueryModel(this);
        table = new QTableView;
        table->setModel(model);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setAlternatingRowColors(true);
        table->horizontalHeader()->setStretchLastSection(true);

        auto *bar = new QHBoxLayout;
        bar->addWidget(new QLabel("Suivi des demandes"));
        bar->addStretch();
        bar->addWidget(statut);
        bar->addWidget(upd);

        auto *lay = new QVBoxLayout(this);
        lay->addWidget(new QLabel("<b>Nouvelle demande</b>"));
        lay->addLayout(form);
        lay->addWidget(gen);
        lay->addLayout(bar);
        lay->addWidget(table);

        connect(gen, &QPushButton::clicked, this, [this] { generate(); });
        connect(upd, &QPushButton::clicked, this, [this] { updateStatus(); });
        reload();
    }

private:
    QLineEdit *demandeur, *lieu;
    QDoubleSpinBox *lat, *lon;
    QDateEdit *from, *to;
    QComboBox *statut;
    QTableView *table;
    QSqlQueryModel *model;

    void fail(const QSqlQuery &q) { QMessageBox::critical(this, "Erreur Oracle", q.lastError().text()); }

    void reload()
    {
        model->setQuery("SELECT ID_ATTESTATION, NUMERO, DEMANDEUR, LIEU, DATE_DEBUT, DATE_FIN, ID_STATION, "
                        "STATUT, DATE_DEMANDE FROM ATTESTATION ORDER BY ID_ATTESTATION DESC");
        const char *h[] = {"ID", "Numéro", "Demandeur", "Lieu", "Du", "Au", "Station", "Statut", "Demandée le"};
        for (int i = 0; i < 9; ++i) model->setHeaderData(i, Qt::Horizontal, h[i]);
        table->resizeColumnsToContents();
    }

    static double haversineKm(double la1, double lo1, double la2, double lo2)
    {
        const double R = 6371.0, r = M_PI / 180.0;
        const double dLa = (la2 - la1) * r, dLo = (lo2 - lo1) * r;
        const double a = std::sin(dLa / 2) * std::sin(dLa / 2) +
                         std::cos(la1 * r) * std::cos(la2 * r) * std::sin(dLo / 2) * std::sin(dLo / 2);
        return 2 * R * std::asin(std::sqrt(a));
    }

    void setStatus(int id, const QString &st, int station = -1)
    {
        QSqlQuery q;
        q.prepare("UPDATE ATTESTATION SET STATUT=?, ID_STATION=NVL(?, ID_STATION), "
                  "DATE_TRAITEMENT=SYSTIMESTAMP WHERE ID_ATTESTATION=?");
        q.addBindValue(st);
        q.addBindValue(station > 0 ? QVariant(station) : QVariant(QMetaType::fromType<int>()));
        q.addBindValue(id);
        if (!q.exec()) fail(q);
    }

    void updateStatus()
    {
        auto sel = table->selectionModel()->selectedRows();
        if (sel.isEmpty()) { QMessageBox::information(this, "Statut", "Sélectionnez une demande."); return; }
        setStatus(model->index(sel.first().row(), 0).data().toInt(), statut->currentText());
        reload();
    }

    void generate()
    {
        if (demandeur->text().trimmed().isEmpty() || lieu->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Demande", "Renseignez le demandeur et le lieu.");
            return;
        }
        if (to->date() < from->date()) {
            QMessageBox::warning(this, "Demande", "La date de fin précède la date de début.");
            return;
        }

        // 1) Numéro unique + enregistrement de la demande (statut EN_COURS)
        QSqlQuery q;
        if (!q.exec("SELECT SEQ_ATTESTATION.NEXTVAL FROM DUAL") || !q.next()) { fail(q); return; }
        const int id = q.value(0).toInt();
        const QString numero = QString("ATT-%1-%2").arg(QDate::currentDate().year())
                                   .arg(id, 5, 10, QChar('0'));
        q.prepare("INSERT INTO ATTESTATION (ID_ATTESTATION, NUMERO, DEMANDEUR, LIEU, LATITUDE, LONGITUDE, "
                  "DATE_DEBUT, DATE_FIN, STATUT) VALUES (?,?,?,?,?,?,?,?,'EN_COURS')");
        q.addBindValue(id);
        q.addBindValue(numero);
        q.addBindValue(demandeur->text().trimmed());
        q.addBindValue(lieu->text().trimmed());
        q.addBindValue(lat->value());
        q.addBindValue(lon->value());
        q.addBindValue(from->date());
        q.addBindValue(to->date());
        if (!q.exec()) { fail(q); return; }

        // 2) Station la plus proche disposant de relevés sur la période
        const QDateTime t0(from->date(), QTime(0, 0)), t1(to->date().addDays(1), QTime(0, 0));
        struct Cand { int id; QString nom; double km; };
        QList<Cand> cands;
        QSqlQuery s("SELECT ID_STATION, NOM, LATITUDE, LONGITUDE FROM STATION");
        while (s.next())
            cands.append({s.value(0).toInt(), s.value(1).toString(),
                          haversineKm(lat->value(), lon->value(), s.value(2).toDouble(), s.value(3).toDouble())});
        std::sort(cands.begin(), cands.end(), [](const Cand &a, const Cand &b) { return a.km < b.km; });

        int stId = -1; QString stNom; double stKm = 0;
        for (const auto &c : cands) {
            QSqlQuery n;
            n.prepare("SELECT COUNT(*) FROM RELEVE_METEO WHERE ID_STATION=? AND HORODATAGE>=? AND HORODATAGE<?");
            n.addBindValue(c.id); n.addBindValue(t0); n.addBindValue(t1);
            if (n.exec() && n.next() && n.value(0).toInt() > 0) { stId = c.id; stNom = c.nom; stKm = c.km; break; }
        }
        if (stId < 0) {
            setStatus(id, "REFUSEE");
            reload();
            QMessageBox::warning(this, "Attestation refusée",
                                 QString("Aucune station ne dispose de relevés sur cette période.\nDemande %1 : REFUSEE.")
                                     .arg(numero));
            return;
        }

        // 3) Synthèse et relevés officiels
        QSqlQuery st;
        st.prepare("SELECT COUNT(*), MIN(TEMPERATURE), AVG(TEMPERATURE), MAX(TEMPERATURE), AVG(HUMIDITE), "
                   "AVG(PRESSION), MAX(VITESSE_VENT), NVL(SUM(PRECIPITATION),0) FROM RELEVE_METEO "
                   "WHERE ID_STATION=? AND HORODATAGE>=? AND HORODATAGE<?");
        st.addBindValue(stId); st.addBindValue(t0); st.addBindValue(t1);
        if (!st.exec() || !st.next()) { fail(st); return; }

        QSqlQuery rl;
        rl.prepare("SELECT HORODATAGE, TEMPERATURE, HUMIDITE, PRESSION, VITESSE_VENT, PRECIPITATION, MODE_COLLECTE "
                   "FROM RELEVE_METEO WHERE ID_STATION=? AND HORODATAGE>=? AND HORODATAGE<? ORDER BY HORODATAGE");
        rl.addBindValue(stId); rl.addBindValue(t0); rl.addBindValue(t1);
        if (!rl.exec()) { fail(rl); return; }

        // 4) Document PDF
        const QString path = QFileDialog::getSaveFileName(this, "Enregistrer l'attestation",
                                                          numero + ".pdf", "PDF (*.pdf)");
        if (path.isEmpty()) { reload(); return; }   // demande conservée au statut EN_COURS

        auto f1 = [&](int i) { return QString::number(st.value(i).toDouble(), 'f', 1); };
        QString html = QString("<h1 align='center'>ATTESTATION MÉTÉOROLOGIQUE</h1>"
                               "<h3 align='center'>N° %1</h3>"
                               "<p>Demandeur : <b>%2</b><br>Lieu : <b>%3</b> (%4°, %5°)<br>"
                               "Période : du <b>%6</b> au <b>%7</b></p>"
                               "<p>Station de référence : <b>%8</b> (n°%9), située à %10 km du lieu demandé.</p>")
                           .arg(numero, demandeur->text().trimmed().toHtmlEscaped(), lieu->text().trimmed().toHtmlEscaped())
                           .arg(lat->value(), 0, 'f', 4).arg(lon->value(), 0, 'f', 4)
                           .arg(from->date().toString("dd/MM/yyyy"), to->date().toString("dd/MM/yyyy"),
                                stNom.toHtmlEscaped())
                           .arg(stId).arg(stKm, 0, 'f', 1);
        html += QString("<h2>Synthèse (%1 relevés)</h2>"
                        "<table border='1' cellpadding='4' cellspacing='0'>"
                        "<tr><td>Température min / moy / max</td><td>%2 / %3 / %4 °C</td></tr>"
                        "<tr><td>Humidité moyenne</td><td>%5 %</td></tr>"
                        "<tr><td>Pression moyenne</td><td>%6 hPa</td></tr>"
                        "<tr><td>Vent maximal</td><td>%7 km/h</td></tr>"
                        "<tr><td>Précipitations cumulées</td><td>%8 mm</td></tr></table>")
                    .arg(st.value(0).toInt()).arg(f1(1), f1(2), f1(3), f1(4), f1(5), f1(6), f1(7));
        html += "<h2>Relevés</h2><table border='1' cellpadding='3' cellspacing='0' width='100%'>"
                "<tr bgcolor='#dfe6ee'><th>Horodatage</th><th>T (°C)</th><th>Hum. (%)</th><th>P (hPa)</th>"
                "<th>Vent (km/h)</th><th>Précip. (mm)</th><th>Collecte</th></tr>";
        while (rl.next()) {
            html += "<tr align='center'><td>" + rl.value(0).toDateTime().toString("dd/MM/yyyy HH:mm") + "</td>";
            for (int i = 1; i <= 6; ++i) html += "<td>" + rl.value(i).toString() + "</td>";
            html += "</tr>";
        }
        html += QString("</table><p><i>Document généré automatiquement le %1 — "
                        "référence %2.</i></p>")
                    .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"), numero);

        QPdfWriter writer(path);
        writer.setPageSize(QPageSize(QPageSize::A4));
        writer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
        QTextDocument doc;
        doc.setHtml(html);
        doc.print(&writer);

        setStatus(id, "DELIVREE", stId);
        reload();
        QMessageBox::information(this, "Attestation délivrée",
                                 QString("Attestation %1 générée (station %2).\n%3").arg(numero, stNom, path));
    }
};
