#include "gestionstation.h"
#include "ui_gestionstation.h"
#include <QMessageBox>
#include <QHeaderView>

GestionStation::GestionStation(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GestionStation)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);   // permet au style de colorer le fond du widget

    // ---------- Style de l'interface ----------
    setWindowTitle("Smart Weather - Gestion des stations");
    setStyleSheet(R"(
GestionStation { background: #EEF3F8; }
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

    // 7. Table STATION (creee si besoin) + premier affichage
    Station::creerTable();
    refreshTable();
}

GestionStation::~GestionStation()
{
    delete ui;
}

// ---------- Fonctions utilitaires ----------

// Remplace le modele du tableau et supprime l'ancien (pas de fuite memoire)
void GestionStation::afficherModele(QSqlQueryModel *model)
{
    QAbstractItemModel *ancien = ui->tableViewStation->model();
    ui->tableViewStation->setModel(model);
    delete ancien;
}

// Recharge le tableau en gardant la recherche ET le tri choisis
void GestionStation::refreshTable()
{
    afficherModele(S.rechercher(ui->searchLineEdit->text(),
                                ui->sortComboBox->currentText()));
}

// Lit le formulaire + controle de saisie
bool GestionStation::lireFormulaire(Station &st)
{
    QString id  = ui->idLineEdit->text().trimmed();
    QString nom = ui->nomLineEdit->text().trimmed();

    if (id.isEmpty() || nom.isEmpty()) {
        QMessageBox::warning(this, "Champs vides", "Veuillez remplir l'ID et le Nom de la station.");
        return false;
    }

    float alt = 0;
    QString altTxt = ui->altitudeLineEdit->text().trimmed().replace(',', '.');
    if (!altTxt.isEmpty()) {
        bool ok = false;
        alt = altTxt.toFloat(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Altitude invalide", "L'altitude doit être un nombre.");
            return false;
        }
    }

    st = Station(id, nom,
                 ui->typeComboBox->currentText(),
                 ui->gouvernoratComboBox->currentText(),
                 alt,
                 ui->statutComboBox->currentText(),
                 ui->dateInstallationEdit->date());
    return true;
}

void GestionStation::viderFormulaire()
{
    ui->idLineEdit->clear();
    ui->nomLineEdit->clear();
    ui->altitudeLineEdit->clear();
    ui->typeComboBox->setCurrentIndex(0);
    ui->gouvernoratComboBox->setCurrentIndex(0);
    ui->statutComboBox->setCurrentIndex(0);
    ui->dateInstallationEdit->setDate(QDate::currentDate());
}

void GestionStation::selectionnerCombo(QComboBox *combo, const QString &texte)
{
    int i = combo->findText(texte);
    if (i >= 0)
        combo->setCurrentIndex(i);
}

// ---------- Slots ----------

// 1. AJOUTER
void GestionStation::on_btnAjouter_clicked()
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
void GestionStation::on_btnModifier_clicked()
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
void GestionStation::on_btnSupprimer_clicked()
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
void GestionStation::on_btnAfficher_clicked()
{
    ui->searchLineEdit->clear();   // vide la recherche (appelle refreshTable via textChanged)
    refreshTable();
    viderFormulaire();
}

// 5. RECHERCHE en temps reel
void GestionStation::on_searchLineEdit_textChanged(const QString &)
{
    refreshTable();
}

// 6. TRI (Altitude / Type / Gouvernorat)
void GestionStation::on_sortComboBox_currentTextChanged(const QString &)
{
    refreshTable();
}

// 7. Clic sur une ligne => remplit le formulaire (pratique pour Modifier / Supprimer)
void GestionStation::on_tableViewStation_clicked(const QModelIndex &index)
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