#ifndef STATIONWINDOW_H
#define STATIONWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QModelIndex>
#include "station.h"

QT_BEGIN_NAMESPACE
namespace Ui { class StationWindow; }
QT_END_NAMESPACE

class StationWindow : public QMainWindow
{
    Q_OBJECT

public:
    StationWindow(QWidget *parent = nullptr);
    ~StationWindow();
    void refreshData() { refreshTable(); }   // appelee par la fenetre principale

private slots:
    void on_btnAjouter_clicked();
    void on_btnModifier_clicked();
    void on_btnSupprimer_clicked();
    void on_btnAfficher_clicked();                       // bouton "Actualiser"
    void on_searchLineEdit_textChanged(const QString &arg1);
    void on_sortComboBox_currentTextChanged(const QString &arg1);
    void on_tableViewStation_clicked(const QModelIndex &index);
    void on_btnExportCsv_clicked();                      // bouton "Exporter CSV"
    void on_btnStats_clicked();
    void on_btnExportPdf_clicked();
    void on_btnSuggestion_clicked();
    void on_btnHeartbeat_clicked();

private:
    Ui::StationWindow *ui;
    Station S;

    void refreshTable();                                 // recherche + tri courants
    void afficherModele(QSqlQueryModel *model);          // remplace le modele sans fuite memoire
    bool lireFormulaire(Station &st);                    // lit + controle de saisie
    void viderFormulaire();
    void selectionnerCombo(QComboBox *combo, const QString &texte);
};

#endif // STATIONWINDOW_H