#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QModelIndex>
#include "station.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnAjouter_clicked();
    void on_btnModifier_clicked();
    void on_btnSupprimer_clicked();
    void on_btnAfficher_clicked();                       // bouton "Actualiser"
    void on_searchLineEdit_textChanged(const QString &arg1);
    void on_sortComboBox_currentTextChanged(const QString &arg1);
    void on_tableViewStation_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    Station S;

    void refreshTable();                                 // recherche + tri courants
    void afficherModele(QSqlQueryModel *model);          // remplace le modele sans fuite memoire
    bool lireFormulaire(Station &st);                    // lit + controle de saisie
    void viderFormulaire();
    void selectionnerCombo(QComboBox *combo, const QString &texte);
};

#endif // MAINWINDOW_H