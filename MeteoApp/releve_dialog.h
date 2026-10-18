#pragma once
#include <QDialog>
#include <QPushButton>
#include <QDateTime>
#include "ui_releve_dialog.h"   // généré depuis releve_dialog.ui

// Boîte de saisie/modification d'un relevé (interface définie dans releve_dialog.ui)
class ReleveDialog : public QDialog
{
public:
    QSpinBox *station;
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
        ui.buttonBox->button(QDialogButtonBox::Ok)->setText("Valider");
        ui.buttonBox->button(QDialogButtonBox::Cancel)->setText("Annuler");
    }

private:
    Ui::ReleveDialog ui;
};
