#pragma once
#include <QDialog>
#include <QPushButton>
#include <QDateTime>
#include <QSqlQuery>
#include <QMessageBox>
#include "ui_releve_dialog.h"   // généré depuis releve_dialog.ui

// Boîte de saisie/modification d'un relevé (interface définie dans releve_dialog.ui)
class ReleveDialog : public QDialog
{
public:
    QComboBox *station;   // liste des stations existantes (évite ORA-02291)
    QDateTimeEdit *horo;
    QDoubleSpinBox *temp, *hum, *pres, *vent, *precip;
    QComboBox *mode;

    explicit ReleveDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        ui.setupUi(this);
        station = ui.station; horo = ui.horo;
        temp = ui.temp; hum = ui.hum; pres = ui.pres; vent = ui.vent;
        precip = ui.precip; mode = ui.mode;
        horo->setDateTime(QDateTime::currentDateTime());

        // Remplir la liste avec les stations qui existent dans la table STATION
        QSqlQuery q("SELECT ID_STATION, NOM FROM STATION ORDER BY ID_STATION");
        while (q.next())
            station->addItem(QString("%1 - %2").arg(q.value(0).toInt()).arg(q.value(1).toString()),
                             q.value(0).toInt());
        ui.buttonBox->button(QDialogButtonBox::Ok)->setText("Valider");
        ui.buttonBox->button(QDialogButtonBox::Cancel)->setText("Annuler");
    }

    int stationId() const { return station->currentData().toInt(); }
    void setStationId(int id)
    {
        int i = station->findData(id);
        if (i >= 0) station->setCurrentIndex(i);
    }

    void accept() override
    {
        if (station->currentIndex() < 0) {
            QMessageBox::warning(this, "Station", "Aucune station disponible : ajoutez d'abord une station dans la table STATION.");
            return;
        }
        QDialog::accept();
    }

private:
    Ui::ReleveDialog ui;
};
