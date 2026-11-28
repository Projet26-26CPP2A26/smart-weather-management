#include "equipementwindow.h"
#include "ui_equipementwindow.h"
#include "equipement.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QSqlQuery>
#include <QFileDialog>
#include <QDialog>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QPainter>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>

EquipementWindow::EquipementWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::EquipementWindow)
{
    ui->setupUi(this);


    ui->cbType->addItems({"Capteur", "Anémomètre", "Baromètre", "Pluviomètre", "Thermomètre", "Autre"});
    ui->cbStatut->addItems({"Opérationnel", "En panne", "En maintenance"});
    ui->cbTri->addItems({"Trier par...", "Nom", "Type", "Statut", "Date d'installation", "Station"});
    ui->deInstallation->setDate(QDate::currentDate());
    ui->deInstallation->setMaximumDate(QDate::currentDate());

    ui->tblEquipement->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblEquipement->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblEquipement->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblEquipement->setAlternatingRowColors(true);

    chargerStations();
    actualiser();
}

EquipementWindow::~EquipementWindow() { delete ui; }

void EquipementWindow::chargerStations()
{
    ui->cbStation->clear();
    QSqlQuery q("SELECT ID_STATION, NOM_STATION FROM STATION ORDER BY NOM_STATION");
    while (q.next())
        ui->cbStation->addItem(q.value(1).toString(), q.value(0).toString());
}

void EquipementWindow::actualiser()
{
    const QString tri = ui->cbTri->currentIndex() > 0 ? ui->cbTri->currentText() : "";
    QAbstractItemModel *ancien = ui->tblEquipement->model();
    ui->tblEquipement->setModel(Equipement::afficher(ui->leRecherche->text(), tri));
    delete ancien;
    ui->tblEquipement->setColumnHidden(8, true);   // ID_STATION
    ui->tblEquipement->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void EquipementWindow::viderFormulaire()
{
    ui->leId->clear();
    ui->leNom->clear();
    ui->leMarque->clear();
    ui->leSerie->clear();
    ui->cbType->setCurrentIndex(0);
    ui->cbStatut->setCurrentIndex(0);
    ui->deInstallation->setDate(QDate::currentDate());
    ui->tblEquipement->clearSelection();
}

// Construit un objet Equipement a partir du formulaire (l'ID est 0 pour un ajout)
static Equipement depuisFormulaire(Ui::EquipementWindow *ui, int id)
{
    return Equipement(id, ui->leNom->text(), ui->cbType->currentText(), ui->leMarque->text(),
                      ui->leSerie->text(), ui->deInstallation->date(), ui->cbStatut->currentText(),
                      ui->cbStation->currentData().toString());
}

void EquipementWindow::on_btnAjouter_clicked()
{
    Equipement e = depuisFormulaire(ui, 0);
    const QString erreur = e.verifier();
    if (!erreur.isEmpty()) {
        QMessageBox::warning(this, "Saisie invalide", erreur);
        return;
    }
    if (e.ajouter()) {
        QMessageBox::information(this, "Ajout", "Équipement ajouté.");
        viderFormulaire();
        actualiser();
    } else {
        QMessageBox::critical(this, "Erreur Oracle", Equipement::derniereErreur());
    }
}

void EquipementWindow::on_btnModifier_clicked()
{
    if (ui->leId->text().isEmpty()) {
        QMessageBox::information(this, "Modifier", "Cliquez d'abord sur une ligne du tableau.");
        return;
    }
    Equipement e = depuisFormulaire(ui, ui->leId->text().toInt());
    const QString erreur = e.verifier();
    if (!erreur.isEmpty()) {
        QMessageBox::warning(this, "Saisie invalide", erreur);
        return;
    }
    if (e.modifier()) {
        QMessageBox::information(this, "Modification", "Équipement modifié.");
        actualiser();
    } else {
        QMessageBox::critical(this, "Erreur", "Modification impossible.\n" + Equipement::derniereErreur());
    }
}

void EquipementWindow::on_btnSupprimer_clicked()
{
    if (ui->leId->text().isEmpty()) {
        QMessageBox::information(this, "Supprimer", "Cliquez d'abord sur une ligne du tableau.");
        return;
    }
    if (QMessageBox::question(this, "Suppression",
            "Supprimer l'équipement « " + ui->leNom->text() + " » ?") != QMessageBox::Yes)
        return;
    if (Equipement::supprimer(ui->leId->text().toInt())) {
        viderFormulaire();
        actualiser();
    } else {
        QMessageBox::critical(this, "Erreur", "Suppression impossible.\n" + Equipement::derniereErreur());
    }
}

void EquipementWindow::on_btnVider_clicked() { viderFormulaire(); }

void EquipementWindow::on_btnExportPdf_clicked()
{
    const QString fichier = QFileDialog::getSaveFileName(this, "Exporter en PDF",
                                "equipements.pdf", "PDF (*.pdf)");
    if (fichier.isEmpty()) return;
    const QString tri = ui->cbTri->currentIndex() > 0 ? ui->cbTri->currentText() : "";
    if (Equipement::exporterPdf(fichier, ui->leRecherche->text(), tri))
        QMessageBox::information(this, "Export PDF", "Fichier créé :\n" + fichier);
    else
        QMessageBox::critical(this, "Export PDF", "Impossible de créer le fichier.");
}

void EquipementWindow::on_btnStats_clicked()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Statistiques des équipements");
    dlg.resize(560, 460);
    auto *choix = new QComboBox;
    choix->addItems({"Statut", "Type"});
    auto *vue = new QChartView;
    vue->setRenderHint(QPainter::Antialiasing);
    auto *lay = new QVBoxLayout(&dlg);
    lay->addWidget(choix);
    lay->addWidget(vue);

    // Redessine le camembert selon le critere choisi (donnees = Equipement::repartition)
    auto dessiner = [&]() {
        auto *serie = new QPieSeries;
        const auto donnees = Equipement::repartition(choix->currentText());
        for (auto it = donnees.cbegin(); it != donnees.cend(); ++it)
            serie->append(it.key() + " (" + QString::number(it.value()) + ")", it.value());
        serie->setLabelsVisible(true);
        auto *graphique = new QChart;
        graphique->addSeries(serie);
        graphique->setTitle("Équipements par " + choix->currentText().toLower());
        delete vue->chart();
        vue->setChart(graphique);
    };
    connect(choix, &QComboBox::currentIndexChanged, &dlg, dessiner);
    dessiner();
    dlg.exec();
}

// Metier innovant 1 : classement des equipements par score de sante (couleurs = niveau)
void EquipementWindow::on_btnSante_clicked()
{
    const auto liste = Equipement::classementSante();
    QDialog dlg(this);
    dlg.setWindowTitle("Équipements à remplacer");
    dlg.resize(820, 460);

    auto *tbl = new QTableWidget(liste.size(), 7);
    tbl->setHorizontalHeaderLabels({"Équipement", "Type", "Station", "Statut", "Âge (ans)", "Score", "Niveau"});
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tbl->verticalHeader()->hide();
    for (int r = 0; r < liste.size(); ++r) {
        const auto &h = liste[r];
        const QColor fond = h.niveau == "Critique" ? QColor("#FDECEA")
                          : (h.niveau == "Bon" ? QColor("#E8F5E9") : QColor("#FFF4E0"));
        const QStringList cellules = {h.nom, h.type, h.station, h.statut,
                                      QString::number(h.ageAns, 'f', 1), QString::number(h.score), h.niveau};
        for (int c = 0; c < cellules.size(); ++c) {
            auto *it = new QTableWidgetItem(cellules[c]);
            it->setBackground(fond);
            it->setForeground(QColor("#1F2D3D"));      // texte fonce : lisible meme en theme sombre
            tbl->setItem(r, c, it);
        }
    }

    auto *info = new QLabel("Score = 100 − usure (âge / durée de vie du type, jusqu'à 60) − statut "
                            "(panne 40, maintenance 20) − station en maintenance (10).");
    info->setWordWrap(true);
    auto *btnPdf = new QPushButton("Exporter PDF");
    auto *btnFermer = new QPushButton("Fermer");
    auto *boutons = new QHBoxLayout;
    boutons->addStretch();
    boutons->addWidget(btnPdf);
    boutons->addWidget(btnFermer);
    auto *lay = new QVBoxLayout(&dlg);
    lay->addWidget(info);
    lay->addWidget(tbl);
    lay->addLayout(boutons);

    connect(btnFermer, &QPushButton::clicked, &dlg, &QDialog::accept);
    connect(btnPdf, &QPushButton::clicked, &dlg, [&]() {
        const QString f = QFileDialog::getSaveFileName(&dlg, "Exporter en PDF", "equipements_a_remplacer.pdf", "PDF (*.pdf)");
        if (f.isEmpty()) return;
        if (Equipement::exporterSantePdf(f))
            QMessageBox::information(&dlg, "Export PDF", "Fichier créé :\n" + f);
        else
            QMessageBox::critical(&dlg, "Export PDF", "Impossible de créer le fichier.");
    });
    dlg.exec();
}

// Metier innovant 2 : equipement en panne -> intervention de maintenance
void EquipementWindow::on_btnIntervention_clicked()
{
    if (ui->leId->text().isEmpty()) {
        QMessageBox::information(this, "Intervention", "Cliquez d'abord sur un équipement « En panne » du tableau.");
        return;
    }
    if (QMessageBox::question(this, "Intervention",
            "Créer une intervention de maintenance pour « " + ui->leNom->text() + " » ?") != QMessageBox::Yes)
        return;
    QString message;
    const bool ok = Equipement::creerInterventionPanne(ui->leId->text().toInt(), message);
    if (ok) {
        QMessageBox::information(this, "Intervention créée", message);
        viderFormulaire();
        actualiser();
    } else {
        QMessageBox::warning(this, "Intervention", message);
    }
}

void EquipementWindow::on_tblEquipement_clicked(const QModelIndex &index)
{
    const QAbstractItemModel *m = ui->tblEquipement->model();
    const int r = index.row();
    auto val = [&](int c) { return m->data(m->index(r, c)).toString(); };

    ui->leId->setText(val(0));
    ui->leNom->setText(val(1));
    ui->cbType->setCurrentText(val(2));
    ui->leMarque->setText(val(3));
    ui->leSerie->setText(val(4));
    ui->deInstallation->setDate(QDate::fromString(val(5), "dd/MM/yyyy"));
    ui->cbStatut->setCurrentText(val(6));
    ui->cbStation->setCurrentIndex(ui->cbStation->findData(val(8)));
}

void EquipementWindow::on_leRecherche_textChanged(const QString &) { actualiser(); }
void EquipementWindow::on_cbTri_currentIndexChanged(int) { actualiser(); }
