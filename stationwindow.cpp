#include "stationwindow.h"
#include "ui_stationwindow.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDialog>
#include <QVBoxLayout>
#include <QComboBox>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QPdfWriter>
#include <QTextDocument>
#include <QPageSize>
#include <QPageLayout>
#include <QDate>
#include <QListWidget>
#include <QLabel>
#include <QPair>
#include <algorithm>
#include <QTableWidget>
#include <QProgressBar>

StationWindow::StationWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StationWindow)
{
    ui->setupUi(this);

    // ---------- Style de l'interface ----------
    setWindowTitle("Smart Weather - Gestion des stations");
    setStyleSheet(R"(
QMainWindow { background: #EEF3F8; }
QGroupBox { font-weight: bold; color: #1F3B57; background: white;
            border: 1px solid #C5D3E0; border-radius: 8px;
            margin-top: 12px; padding-top: 10px; }
QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; }
QLabel { color: #1F3B57; }
QLineEdit, QComboBox, QDateEdit { border: 1px solid #B8C8D8; border-radius: 6px;
            padding: 5px 8px; background: white; min-height: 22px; }
QLineEdit:focus, QComboBox:focus, QDateEdit:focus { border: 1px solid #2E86DE; }
QPushButton { background: #2E86DE; color: white; border: none;
            border-radius: 6px; padding: 8px 14px; font-weight: bold; }
QPushButton:hover { background: #1B6EC2; }
QPushButton#btnModifier { background: #F39C12; }
QPushButton#btnModifier:hover { background: #D68910; }
QPushButton#btnSupprimer { background: #E74C3C; }
QPushButton#btnSupprimer:hover { background: #C0392B; }
QPushButton#btnAfficher { background: #27AE60; }
QPushButton#btnAfficher:hover { background: #1E8E4E; }
QPushButton#btnExportCsv { background: #8E44AD; }
QPushButton#btnExportCsv:hover { background: #732D91; }
QPushButton#btnExportPdf { background: #C0392B; }
QPushButton#btnExportPdf:hover { background: #A93226; }
QPushButton#btnSuggestion { background: #16A085; }
QPushButton#btnSuggestion:hover { background: #12806A; }
QPushButton#btnHeartbeat { background: #E67E22; }
QPushButton#btnHeartbeat:hover { background: #CA6C14; }
QTableView { background: white; alternate-background-color: #F4F8FC;
            gridline-color: #DDE6EE; border: 1px solid #C5D3E0;
            selection-background-color: #2E86DE; selection-color: white; }
QHeaderView::section { background: #1F3B57; color: white; padding: 6px;
            border: none; font-weight: bold; }
)");
    ui->tableViewStation->setAlternatingRowColors(true);

    // 1. Type (selon le cahier des charges)
    ui->typeComboBox->clear();
    ui->typeComboBox->addItems({"Synoptique", "Agro-meteo", "Climatologique",
                                "Pluviometrique", "Maritime", "Aeronautique"});

    // 2. Gouvernorat
    ui->gouvernoratComboBox->clear();
    ui->gouvernoratComboBox->addItems({
        "Tunis", "Ariana", "Ben Arous", "Manouba", "Nabeul", "Zaghouan", "Bizerte",
        "Beja", "Jendouba", "Le Kef", "Siliana", "Kairouan", "Kasserine", "Sidi Bouzid",
        "Sousse", "Monastir", "Mahdia", "Sfax", "Gafsa", "Tozeur", "Kebili",
        "Gabes", "Medenine", "Tataouine"
    });

    // 3. Statut (selon le cahier des charges)
    ui->statutComboBox->clear();
    ui->statutComboBox->addItems({"Active", "Inactive", "En maintenance"});

    // 4. Date
    ui->dateInstallationEdit->setCalendarPopup(true);
    ui->dateInstallationEdit->setDisplayFormat("dd/MM/yyyy");
    ui->dateInstallationEdit->setDate(QDate::currentDate());

    // 5. Recherche : champ vide + texte gris "Rechercher..."
    ui->searchLineEdit->clear();
    ui->searchLineEdit->setPlaceholderText("Rechercher...");
    ui->searchLineEdit->setClearButtonEnabled(true);

    // 6. Aspect du tableau
    ui->tableViewStation->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewStation->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewStation->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewStation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 7. Premier affichage
    refreshTable();
}

StationWindow::~StationWindow()
{
    delete ui;
}

// ---------- Fonctions utilitaires ----------

// Remplace le modele du tableau et supprime l'ancien (pas de fuite memoire)
void StationWindow::afficherModele(QSqlQueryModel *model)
{
    QAbstractItemModel *ancien = ui->tableViewStation->model();
    ui->tableViewStation->setModel(model);
    delete ancien;
}

// Recharge le tableau en gardant la recherche ET le tri choisis
void StationWindow::refreshTable()
{
    afficherModele(S.rechercher(ui->searchLineEdit->text(),
                                ui->sortComboBox->currentText()));
}

// Lit le formulaire + controle de saisie
bool StationWindow::lireFormulaire(Station &st)
{
    QString id  = ui->idLineEdit->text().trimmed();
    QString nom = ui->nomLineEdit->text().trimmed();

    // 1. Champs obligatoires
    if (id.isEmpty() || nom.isEmpty()) {
        QMessageBox::warning(this, "Champs vides", "Veuillez remplir l'ID et le Nom de la station.");
        return false;
    }

    // 2. ID numerique (la colonne ID_STATION est de type NUMBER)
    bool okId = false;
    id.toInt(&okId);
    if (!okId) {
        QMessageBox::warning(this, "ID invalide", "L'ID doit être un nombre entier.");
        return false;
    }

    // 3. Longueur du nom (VARCHAR2(100))
    if (nom.length() > 100) {
        QMessageBox::warning(this, "Nom invalide", "Le nom ne doit pas dépasser 100 caractères.");
        return false;
    }

    // 4. Altitude numerique et raisonnable
    float alt = 0;
    QString altTxt = ui->altitudeLineEdit->text().trimmed().replace(',', '.');
    if (!altTxt.isEmpty()) {
        bool ok = false;
        alt = altTxt.toFloat(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Altitude invalide", "L'altitude doit être un nombre.");
            return false;
        }
        if (alt < -500 || alt > 5000) {
            QMessageBox::warning(this, "Altitude invalide", "Altitude entre -500 et 5000 m.");
            return false;
        }
    }

    // 5. Date pas dans le futur
    if (ui->dateInstallationEdit->date() > QDate::currentDate()) {
        QMessageBox::warning(this, "Date invalide", "La date ne peut pas être dans le futur.");
        return false;
    }

    st = Station(id, nom,
                 ui->typeComboBox->currentText(),
                 ui->gouvernoratComboBox->currentText(),
                 alt,
                 ui->statutComboBox->currentText(),
                 ui->dateInstallationEdit->date());
    return true;
}

void StationWindow::viderFormulaire()
{
    ui->idLineEdit->clear();
    ui->nomLineEdit->clear();
    ui->altitudeLineEdit->clear();
    ui->typeComboBox->setCurrentIndex(0);
    ui->gouvernoratComboBox->setCurrentIndex(0);
    ui->statutComboBox->setCurrentIndex(0);
    ui->dateInstallationEdit->setDate(QDate::currentDate());
}

void StationWindow::selectionnerCombo(QComboBox *combo, const QString &texte)
{
    int i = combo->findText(texte);
    if (i >= 0)
        combo->setCurrentIndex(i);
}

// ---------- Slots ----------

// 1. AJOUTER
void StationWindow::on_btnAjouter_clicked()
{
    Station st;
    if (!lireFormulaire(st))
        return;

    if (st.ajouter()) {
        refreshTable();
        viderFormulaire();
        QMessageBox::information(this, "Succès", "Station ajoutée avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur",
                              "Échec de l'ajout.\nCet ID existe peut-être déjà.");
    }
}

// 2. MODIFIER
void StationWindow::on_btnModifier_clicked()
{
    Station st;
    if (!lireFormulaire(st))
        return;

    if (st.modifier(st.getId())) {
        refreshTable();
        QMessageBox::information(this, "Succès", "Station modifiée avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur",
                              "Échec de la modification.\nVérifiez que l'ID existe.");
    }
}

// 3. SUPPRIMER
void StationWindow::on_btnSupprimer_clicked()
{
    QString id = ui->idLineEdit->text().trimmed();
    if (id.isEmpty()) {
        QMessageBox::warning(this, "Attention",
                             "Sélectionnez une station dans le tableau ou saisissez son ID.");
        return;
    }

    if (QMessageBox::question(this, "Confirmation",
                              "Supprimer la station " + id + " ?") != QMessageBox::Yes)
        return;

    if (S.supprimer(id)) {
        refreshTable();
        viderFormulaire();
        QMessageBox::information(this, "Succès", "Station supprimée avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur",
                              "Échec de la suppression.\nVérifiez que l'ID existe.");
    }
}

// 4. ACTUALISER (le bouton s'appelle btnAfficher dans l'interface)
void StationWindow::on_btnAfficher_clicked()
{
    ui->searchLineEdit->clear();   // vide la recherche (appelle refreshTable via textChanged)
    refreshTable();
    viderFormulaire();
}

// 5. RECHERCHE en temps reel
void StationWindow::on_searchLineEdit_textChanged(const QString &)
{
    refreshTable();
}

// 6. TRI (Altitude / Type / Gouvernorat)
void StationWindow::on_sortComboBox_currentTextChanged(const QString &)
{
    refreshTable();
}

// 7. Clic sur une ligne => remplit le formulaire (pratique pour Modifier / Supprimer)
void StationWindow::on_tableViewStation_clicked(const QModelIndex &index)
{
    QAbstractItemModel *m = ui->tableViewStation->model();
    if (!m || !index.isValid())
        return;

    int r = index.row();
    auto val = [&](int col) { return m->data(m->index(r, col)); };

    ui->idLineEdit->setText(val(0).toString());
    ui->nomLineEdit->setText(val(1).toString());
    selectionnerCombo(ui->typeComboBox, val(2).toString());
    selectionnerCombo(ui->gouvernoratComboBox, val(3).toString());
    ui->altitudeLineEdit->setText(val(4).toString());
    selectionnerCombo(ui->statutComboBox, val(5).toString());

    QDate d = QDate::fromString(val(6).toString(), "dd/MM/yyyy");
    if (d.isValid())
        ui->dateInstallationEdit->setDate(d);
}

// 8. EXPORT CSV
void StationWindow::on_btnExportCsv_clicked()
{
    QAbstractItemModel *m = ui->tableViewStation->model();
    if (!m || m->rowCount() == 0) {
        QMessageBox::warning(this, "Export", "Aucune station à exporter.");
        return;
    }

    QString chemin = QFileDialog::getSaveFileName(this, "Exporter en CSV",
                                                  "stations.csv",
                                                  "Fichiers CSV (*.csv)");
    if (chemin.isEmpty())
        return;

    QFile fichier(chemin);
    if (!fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier.");
        return;
    }

    QTextStream out(&fichier);
    out.setGenerateByteOrderMark(true);   // pour que Excel lise bien les accents (UTF-8)

    // Protège les champs (guillemets)
    auto champ = [](QString s) {
        s.replace("\"", "\"\"");
        return "\"" + s + "\"";
    };

    // En-têtes
    QStringList entetes;
    for (int c = 0; c < m->columnCount(); ++c)
        entetes << champ(m->headerData(c, Qt::Horizontal).toString());
    out << entetes.join(";") << "\n";

    // Lignes
    for (int r = 0; r < m->rowCount(); ++r) {
        QStringList ligne;
        for (int c = 0; c < m->columnCount(); ++c)
            ligne << champ(m->data(m->index(r, c)).toString());
        out << ligne.join(";") << "\n";
    }

    fichier.close();
    QMessageBox::information(this, "Export", "Export CSV réussi !");
}

// 9. STATISTIQUES : graphique circulaire
void StationWindow::on_btnStats_clicked()
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle("Statistiques des stations");
    dlg->resize(650, 500);

    QVBoxLayout *lay = new QVBoxLayout(dlg);
    QComboBox *choix = new QComboBox(dlg);
    choix->addItems({"Type", "Statut"});
    QChartView *view = new QChartView(dlg);
    view->setRenderHint(QPainter::Antialiasing);
    lay->addWidget(choix);
    lay->addWidget(view);

    // Reconstruit le graphique selon le critere choisi
    auto dessiner = [this, view](const QString &critere) {
        QMap<QString,int> data = S.repartition(critere);

        int total = 0;
        for (int v : data) total += v;

        QPieSeries *series = new QPieSeries();
        for (auto it = data.begin(); it != data.end(); ++it)
            series->append(it.key() + " (" + QString::number(it.value()) + ")", it.value());

        series->setLabelsVisible(true);
        for (QPieSlice *s : series->slices())
            s->setLabel(QString("%1 - %2%")
                            .arg(s->label())
                            .arg(100.0 * s->value() / (total ? total : 1), 0, 'f', 1));

        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Répartition des stations par " + critere.toLower());
        chart->legend()->setAlignment(Qt::AlignRight);

        QChart *ancien = view->chart();
        view->setChart(chart);
        delete ancien;
    };

    connect(choix, &QComboBox::currentTextChanged, dlg, dessiner);
    dessiner(choix->currentText());
    dlg->exec();
}

// 10. EXPORT PDF
void StationWindow::on_btnExportPdf_clicked()
{
    QAbstractItemModel *m = ui->tableViewStation->model();
    if (!m || m->rowCount() == 0) {
        QMessageBox::warning(this, "Export", "Aucune station à exporter.");
        return;
    }

    QString chemin = QFileDialog::getSaveFileName(this, "Exporter en PDF",
                                                  "stations.pdf",
                                                  "Fichiers PDF (*.pdf)");
    if (chemin.isEmpty())
        return;

    // Construction du HTML
    QString html;
    html += "<h2 style='text-align:center; color:#1F3B57;'>Inventaire des stations météorologiques</h2>";
    html += "<p style='text-align:center;'>Généré le "
            + QDate::currentDate().toString("dd/MM/yyyy")
            + " - " + QString::number(m->rowCount()) + " station(s)</p>";
    html += "<table border='1' cellspacing='0' cellpadding='5' width='100%'>";

    // En-têtes
    html += "<tr style='background-color:#1F3B57; color:white;'>";
    for (int c = 0; c < m->columnCount(); ++c)
        html += "<th>" + m->headerData(c, Qt::Horizontal).toString().toHtmlEscaped() + "</th>";
    html += "</tr>";

    // Lignes
    for (int r = 0; r < m->rowCount(); ++r) {
        html += (r % 2 == 0) ? "<tr>" : "<tr style='background-color:#F4F8FC;'>";
        for (int c = 0; c < m->columnCount(); ++c)
            html += "<td>" + m->data(m->index(r, c)).toString().toHtmlEscaped() + "</td>";
        html += "</tr>";
    }
    html += "</table>";

    // Ecriture du PDF (A4 paysage)
    QPdfWriter writer(chemin);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageOrientation(QPageLayout::Landscape);
    writer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&writer);

    QMessageBox::information(this, "Export", "Export PDF réussi !");
}

// 11. SUGGESTION D'EMPLACEMENT
void StationWindow::on_btnSuggestion_clicked()
{
    QMap<QString,int> actives = S.stationsActivesParGouvernorat();

    // Tous les gouvernorats de la combo, avec leur nombre (0 si absent)
    QVector<QPair<int,QString>> liste;
    for (int i = 0; i < ui->gouvernoratComboBox->count(); ++i) {
        QString g = ui->gouvernoratComboBox->itemText(i);
        liste.append({actives.value(g, 0), g});
    }
    std::sort(liste.begin(), liste.end());   // du moins couvert au plus couvert

    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle("Suggestion d'emplacement");
    dlg->resize(480, 560);

    QVBoxLayout *lay = new QVBoxLayout(dlg);
    QLabel *titre = new QLabel(dlg);
    titre->setWordWrap(true);

    QListWidget *lw = new QListWidget(dlg);

    int nbZero = 0;
    for (const auto &p : liste) {
        QString txt = p.second + "  —  " + QString::number(p.first) + " station(s) active(s)";
        if (p.first == 0) { txt += "   ⚠ zone sous-couverte"; ++nbZero; }
        lw->addItem(txt);
    }

    titre->setText("<b>Meilleurs emplacements pour une nouvelle station :</b><br>"
                   "1. " + liste[0].second + " (" + QString::number(liste[0].first) + " active)<br>"
                                                                                "2. " + liste[1].second + " (" + QString::number(liste[1].first) + " active)<br>"
                                                                                "3. " + liste[2].second + " (" + QString::number(liste[2].first) + " active)<br><br>"
                   + QString::number(nbZero) + " gouvernorat(s) sans aucune station active.");

    lay->addWidget(titre);
    lay->addWidget(lw);
    dlg->exec();
}

// 12. HEARTBEAT SCORE
void StationWindow::on_btnHeartbeat_clicked()
{
    QAbstractItemModel *m = ui->tableViewStation->model();
    if (!m || m->rowCount() == 0) {
        QMessageBox::warning(this, "Heartbeat", "Aucune station.");
        return;
    }

    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle("Heartbeat Score des stations");
    dlg->resize(640, 420);
    QVBoxLayout *lay = new QVBoxLayout(dlg);

    QTableWidget *tw = new QTableWidget(m->rowCount(), 4, dlg);
    tw->setHorizontalHeaderLabels({"ID", "Nom", "Statut", "Score"});
    tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tw->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int r = 0; r < m->rowCount(); ++r) {
        QString statut = m->data(m->index(r, 5)).toString();
        QDate d = QDate::fromString(m->data(m->index(r, 6)).toString(), "dd/MM/yyyy");
        int score = Station::calculerScore(statut, d);

        tw->setItem(r, 0, new QTableWidgetItem(m->data(m->index(r, 0)).toString()));
        tw->setItem(r, 1, new QTableWidgetItem(m->data(m->index(r, 1)).toString()));
        tw->setItem(r, 2, new QTableWidgetItem(statut));

        QProgressBar *bar = new QProgressBar();
        bar->setRange(0, 100);
        bar->setValue(score);
        bar->setFormat("%v %");
        QString couleur = score >= 70 ? "#27AE60" : (score >= 50 ? "#F39C12" : "#E74C3C");
        bar->setStyleSheet("QProgressBar::chunk { background:" + couleur + "; }");
        tw->setCellWidget(r, 3, bar);
    }

    QPushButton *btn = new QPushButton("Mettre en maintenance les stations < 50 %", dlg);
    connect(btn, &QPushButton::clicked, dlg, [this, dlg]() {
        int n = S.appliquerMaintenance(50);
        QMessageBox::information(dlg, "Heartbeat",
                                 QString::number(n) + " station(s) passée(s) en maintenance.");
        refreshTable();
        dlg->accept();
    });

    lay->addWidget(tw);
    lay->addWidget(btn);
    dlg->exec();
}