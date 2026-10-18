/********************************************************************************
** Form generated from reading UI file 'releve_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RELEVE_DIALOG_H
#define UI_RELEVE_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ReleveDialog
{
public:
    QVBoxLayout *dialogLayout;
    QLabel *titleLabel;
    QFormLayout *formLayout;
    QLabel *l1;
    QSpinBox *station;
    QLabel *l2;
    QDateTimeEdit *horo;
    QLabel *l3;
    QDoubleSpinBox *temp;
    QLabel *l4;
    QDoubleSpinBox *hum;
    QLabel *l5;
    QDoubleSpinBox *pres;
    QLabel *l6;
    QDoubleSpinBox *vent;
    QLabel *l7;
    QDoubleSpinBox *precip;
    QLabel *l8;
    QComboBox *mode;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ReleveDialog)
    {
        if (ReleveDialog->objectName().isEmpty())
            ReleveDialog->setObjectName("ReleveDialog");
        ReleveDialog->setMinimumSize(QSize(400, 0));
        ReleveDialog->setStyleSheet(QString::fromUtf8("/* Th\303\250me clair moderne - appliqu\303\251 dans main.cpp */\n"
"* { font-family: \"Segoe UI\", \"Noto Sans\", \"Helvetica Neue\", sans-serif; font-size: 10pt; }\n"
"\n"
"QMainWindow, QDialog { background: #f4f6fa; }\n"
"\n"
"QLabel#titleLabel {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1f4e8c, stop:1 #3a86c8);\n"
"    color: white; font-size: 15pt; font-weight: 600;\n"
"    padding: 14px 18px; border-radius: 8px;\n"
"}\n"
"\n"
"/* Champs de saisie */\n"
"QLineEdit, QDateEdit, QDateTimeEdit, QComboBox, QSpinBox, QDoubleSpinBox {\n"
"    background: white; border: 1px solid #c5ccd8; border-radius: 6px;\n"
"    padding: 5px 8px; min-height: 22px;\n"
"}\n"
"QLineEdit:focus, QDateEdit:focus, QDateTimeEdit:focus, QComboBox:focus,\n"
"QSpinBox:focus, QDoubleSpinBox:focus { border: 1px solid #2f6fb3; }\n"
"QComboBox::drop-down, QDateEdit::drop-down, QDateTimeEdit::drop-down { border: none; width: 22px; }\n"
"\n"
"/* Boutons */\n"
"QPushButton {\n"
"    background: white; color: #1f2d3d; "
                        "border: 1px solid #c5ccd8;\n"
"    border-radius: 6px; padding: 7px 14px;\n"
"}\n"
"QPushButton:hover   { background: #eaf1fb; border-color: #2f6fb3; }\n"
"QPushButton:pressed { background: #d6e5f7; }\n"
"\n"
"QPushButton#searchButton, QPushButton#addButton, QPushButton[primary=\"true\"] {\n"
"    background: #2f6fb3; color: white; border: none; font-weight: 600;\n"
"}\n"
"QPushButton#searchButton:hover, QPushButton#addButton:hover,\n"
"QPushButton[primary=\"true\"]:hover { background: #265b94; }\n"
"\n"
"QPushButton#editButton   { color: #2f6fb3; font-weight: 600; }\n"
"QPushButton#deleteButton { background: #d64545; color: white; border: none; font-weight: 600; }\n"
"QPushButton#deleteButton:hover { background: #b83636; }\n"
"QPushButton#attestButton { background: #2e9b6f; color: white; border: none; font-weight: 600; }\n"
"QPushButton#attestButton:hover { background: #247c59; }\n"
"QPushButton#anomalyButton { background: #e08a1e; color: white; border: none; font-weight: 600; }\n"
"QPushButton#anomalyButton:"
                        "hover { background: #b96f14; }\n"
"\n"
"/* Tableaux */\n"
"QTableView {\n"
"    background: white; alternate-background-color: #f1f5fb;\n"
"    border: 1px solid #d6dce6; border-radius: 6px; gridline-color: transparent;\n"
"    selection-background-color: #cfe2f8; selection-color: #10294a;\n"
"}\n"
"QHeaderView::section {\n"
"    background: #1f4e8c; color: white; padding: 8px; border: none; font-weight: 600;\n"
"}\n"
"QTableView QTableCornerButton::section { background: #1f4e8c; border: none; }\n"
"\n"
"/* Divers */\n"
"QCheckBox { spacing: 6px; }\n"
"QDialogButtonBox QPushButton { min-width: 80px; }\n"
"QScrollBar:vertical   { background: transparent; width: 10px; }\n"
"QScrollBar::handle:vertical   { background: #b8c2d3; border-radius: 5px; min-height: 30px; }\n"
"QScrollBar:horizontal { background: transparent; height: 10px; }\n"
"QScrollBar::handle:horizontal { background: #b8c2d3; border-radius: 5px; min-width: 30px; }\n"
"QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }\n"
"QPlainText"
                        "Edit { background: white; border: 1px solid #d6dce6; border-radius: 6px; padding: 6px; }\n"
"\n"
"/* Fl\303\250ches des listes d\303\251roulantes, dates et compteurs */\n"
"QComboBox::down-arrow, QDateEdit::down-arrow, QDateTimeEdit::down-arrow,\n"
"QAbstractSpinBox::down-arrow { image: url(:/arrow_down.png); width: 12px; height: 8px; }\n"
"QAbstractSpinBox::up-arrow   { image: url(:/arrow_up.png);   width: 12px; height: 8px; }\n"
"QAbstractSpinBox::up-button, QAbstractSpinBox::down-button {\n"
"    subcontrol-origin: border; width: 20px; background: #eef2f8;\n"
"    border-left: 1px solid #c5ccd8;\n"
"}\n"
"QAbstractSpinBox::up-button   { subcontrol-position: top right;    border-top-right-radius: 6px; }\n"
"QAbstractSpinBox::down-button { subcontrol-position: bottom right; border-bottom-right-radius: 6px; }\n"
"QDateEdit, QDateTimeEdit { min-width: 120px; }\n"
""));
        dialogLayout = new QVBoxLayout(ReleveDialog);
        dialogLayout->setObjectName("dialogLayout");
        titleLabel = new QLabel(ReleveDialog);
        titleLabel->setObjectName("titleLabel");

        dialogLayout->addWidget(titleLabel);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        l1 = new QLabel(ReleveDialog);
        l1->setObjectName("l1");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, l1);

        station = new QSpinBox(ReleveDialog);
        station->setObjectName("station");
        station->setMinimum(1);
        station->setMaximum(999999);

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, station);

        l2 = new QLabel(ReleveDialog);
        l2->setObjectName("l2");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, l2);

        horo = new QDateTimeEdit(ReleveDialog);
        horo->setObjectName("horo");
        horo->setCalendarPopup(true);

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, horo);

        l3 = new QLabel(ReleveDialog);
        l3->setObjectName("l3");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, l3);

        temp = new QDoubleSpinBox(ReleveDialog);
        temp->setObjectName("temp");
        temp->setMinimum(-60.000000000000000);
        temp->setMaximum(60.000000000000000);

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, temp);

        l4 = new QLabel(ReleveDialog);
        l4->setObjectName("l4");

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, l4);

        hum = new QDoubleSpinBox(ReleveDialog);
        hum->setObjectName("hum");
        hum->setMaximum(100.000000000000000);

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, hum);

        l5 = new QLabel(ReleveDialog);
        l5->setObjectName("l5");

        formLayout->setWidget(4, QFormLayout::ItemRole::LabelRole, l5);

        pres = new QDoubleSpinBox(ReleveDialog);
        pres->setObjectName("pres");
        pres->setMinimum(800.000000000000000);
        pres->setMaximum(1100.000000000000000);
        pres->setValue(1013.000000000000000);

        formLayout->setWidget(4, QFormLayout::ItemRole::FieldRole, pres);

        l6 = new QLabel(ReleveDialog);
        l6->setObjectName("l6");

        formLayout->setWidget(5, QFormLayout::ItemRole::LabelRole, l6);

        vent = new QDoubleSpinBox(ReleveDialog);
        vent->setObjectName("vent");
        vent->setMaximum(400.000000000000000);

        formLayout->setWidget(5, QFormLayout::ItemRole::FieldRole, vent);

        l7 = new QLabel(ReleveDialog);
        l7->setObjectName("l7");

        formLayout->setWidget(6, QFormLayout::ItemRole::LabelRole, l7);

        precip = new QDoubleSpinBox(ReleveDialog);
        precip->setObjectName("precip");
        precip->setMaximum(1000.000000000000000);

        formLayout->setWidget(6, QFormLayout::ItemRole::FieldRole, precip);

        l8 = new QLabel(ReleveDialog);
        l8->setObjectName("l8");

        formLayout->setWidget(7, QFormLayout::ItemRole::LabelRole, l8);

        mode = new QComboBox(ReleveDialog);
        mode->addItem(QString());
        mode->addItem(QString());
        mode->setObjectName("mode");

        formLayout->setWidget(7, QFormLayout::ItemRole::FieldRole, mode);


        dialogLayout->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(ReleveDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        dialogLayout->addWidget(buttonBox);


        retranslateUi(ReleveDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ReleveDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ReleveDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(ReleveDialog);
    } // setupUi

    void retranslateUi(QDialog *ReleveDialog)
    {
        ReleveDialog->setWindowTitle(QCoreApplication::translate("ReleveDialog", "Relev\303\251 m\303\251t\303\251o", nullptr));
        titleLabel->setText(QCoreApplication::translate("ReleveDialog", "Relev\303\251 m\303\251t\303\251o", nullptr));
        l1->setText(QCoreApplication::translate("ReleveDialog", "ID station", nullptr));
        l2->setText(QCoreApplication::translate("ReleveDialog", "Horodatage", nullptr));
        horo->setDisplayFormat(QCoreApplication::translate("ReleveDialog", "dd/MM/yyyy HH:mm", nullptr));
        l3->setText(QCoreApplication::translate("ReleveDialog", "Temp\303\251rature", nullptr));
        temp->setSuffix(QCoreApplication::translate("ReleveDialog", " \302\260C", nullptr));
        l4->setText(QCoreApplication::translate("ReleveDialog", "Humidit\303\251", nullptr));
        hum->setSuffix(QCoreApplication::translate("ReleveDialog", " %", nullptr));
        l5->setText(QCoreApplication::translate("ReleveDialog", "Pression", nullptr));
        pres->setSuffix(QCoreApplication::translate("ReleveDialog", " hPa", nullptr));
        l6->setText(QCoreApplication::translate("ReleveDialog", "Vitesse du vent", nullptr));
        vent->setSuffix(QCoreApplication::translate("ReleveDialog", " km/h", nullptr));
        l7->setText(QCoreApplication::translate("ReleveDialog", "Pr\303\251cipitation", nullptr));
        precip->setSuffix(QCoreApplication::translate("ReleveDialog", " mm", nullptr));
        l8->setText(QCoreApplication::translate("ReleveDialog", "Mode de collecte", nullptr));
        mode->setItemText(0, QCoreApplication::translate("ReleveDialog", "Automatique", nullptr));
        mode->setItemText(1, QCoreApplication::translate("ReleveDialog", "Manuel", nullptr));

    } // retranslateUi

};

namespace Ui {
    class ReleveDialog: public Ui_ReleveDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RELEVE_DIALOG_H
