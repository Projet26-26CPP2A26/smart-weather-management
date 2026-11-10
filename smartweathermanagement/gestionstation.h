#ifndef GESTIONSTATION_H
#define GESTIONSTATION_H

#include <QWidget>
#include <QComboBox>
#include <QModelIndex>
#include "station.h"

namespace Ui { class GestionStation; }

// Module 2 : Gestion des stations meteorologiques.
// Widget autonome : peut etre ouvert comme fenetre ou ajoute comme onglet.
class GestionStation : public QWidget
{
    Q_OBJECT

public:
    explicit GestionStation(QWidget *parent = nullptr);
    ~GestionStation();

private slots:
    // === MODULE STATION : CRUD ===
    void on_btnAjouter_clicked();
    void on_btnModifier_clicked();
    void on_btnSupprimer_clicked();
    void on_btnAfficher_clicked();                       // bouton "Actualiser"
    void on_searchLineEdit_textChanged(const QString &arg1);
    void on_sortComboBox_currentTextChanged(const QString &arg1);
    void on_tableViewStation_clicked(const QModelIndex &index);

    // === MODULE STATION : METIERS ===
    void on_btnExportCsv_clicked();
    void on_btnStats_clicked();
    void on_btnExportPdf_clicked();
    void on_btnSuggestion_clicked();
    void on_btnHeartbeat_clicked();

private:
    Ui::GestionStation *ui;
    Station S;

    void refreshTable();                                 // recherche + tri courants
    void afficherModele(QSqlQueryModel *model);          // remplace le modele sans fuite memoire
    bool lireFormulaire(Station &st);                    // lit + controle de saisie
    void viderFormulaire();
    void selectionnerCombo(QComboBox *combo, const QString &texte);
};

#endif // GESTIONSTATION_H
