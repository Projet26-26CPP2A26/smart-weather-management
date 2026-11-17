#ifndef EQUIPEMENTWINDOW_H
#define EQUIPEMENTWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class EquipementWindow; }
QT_END_NAMESPACE

class EquipementWindow : public QMainWindow
{
    Q_OBJECT

public:
    EquipementWindow(QWidget *parent = nullptr);
    ~EquipementWindow();
    void refreshData() { chargerStations(); actualiser(); }   // appelee par la fenetre principale

private slots:
    void on_btnAjouter_clicked();
    void on_btnModifier_clicked();
    void on_btnSupprimer_clicked();
    void on_btnVider_clicked();
    void on_btnExportPdf_clicked();
    void on_btnStats_clicked();
    void on_tblEquipement_clicked(const QModelIndex &index);
    void on_leRecherche_textChanged(const QString &texte);
    void on_cbTri_currentIndexChanged(int index);

private:
    Ui::EquipementWindow *ui;

    void actualiser();            // recharge le tableau (recherche + tri courants)
    void viderFormulaire();
    void chargerStations();
};

#endif // EQUIPEMENTWINDOW_H
