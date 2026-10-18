/********************************************************************************
** Form generated from reading UI file 'intervention_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INTERVENTION_DIALOG_H
#define UI_INTERVENTION_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_InterventionDialog
{
public:
    QVBoxLayout *mainLayout;
    QLabel *dlgTitle;
    QLabel *dlgSub;
    QFrame *formCard;
    QGridLayout *formGrid;
    QLabel *lblStation;
    QComboBox *cbStation;
    QLabel *lblEmploye;
    QComboBox *cbEmploye;
    QLabel *lblType;
    QComboBox *cbType;
    QLabel *lblPriorite;
    QComboBox *cbPriorite;
    QLabel *lblPlan;
    QDateEdit *dePlan;
    QLabel *lblStatut;
    QComboBox *cbStatut;
    QLabel *lblCout;
    QDoubleSpinBox *spCout;
    QLabel *lblRapport;
    QTextEdit *teRapport;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *InterventionDialog)
    {
        if (InterventionDialog->objectName().isEmpty())
            InterventionDialog->setObjectName("InterventionDialog");
        InterventionDialog->resize(620, 640);
        InterventionDialog->setStyleSheet(QString::fromUtf8("/* ===========================================================\n"
"   SMART WEATHER \342\200\223 Module 5 : Maintenance & Interventions\n"
"   Theme clair, cartes degradees, boutons en relief (3D)\n"
"   =========================================================== */\n"
"* { font-family: \"Segoe UI\", \"Roboto\", sans-serif; font-size: 13px; color: #1f2d3d; }\n"
"QMainWindow, QDialog, QWidget#central, QWidget#content { background: #eef2f9; }\n"
"QStackedWidget, QWidget#pageList, QWidget#pageStats { background: transparent; }\n"
"\n"
"/* ---------- Sidebar ---------- */\n"
"QFrame#sidebar {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffffff, stop:1 #e9f0fb);\n"
"    border-right: 1px solid #d5e1f3;\n"
"}\n"
"QLabel#lblBrand  { font-size: 19px; font-weight: 800; color: #1565c0; background: transparent; }\n"
"QLabel#lblModule { color: #6b7c93; font-size: 11px; background: transparent; padding-bottom: 8px; }\n"
"QLabel#lblUserRole {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                        " stop:0 #ffffff, stop:1 #e3ecfa);\n"
"    border: 1px solid #c9d8ef; border-bottom: 3px solid #b3c8e8; border-radius: 10px;\n"
"    padding: 10px; color: #0d47a1; font-weight: 700; margin-bottom: 10px;\n"
"}\n"
"QLabel#lblClock { color: #1565c0; font-weight: 700; background: white; border: 1px solid #d5e1f3;\n"
"                  border-radius: 8px; padding: 6px; }\n"
"\n"
"QPushButton[nav=\"true\"] {\n"
"    text-align: left; padding: 11px 14px; border-radius: 10px; border: 1px solid transparent;\n"
"    background: transparent; color: #33475b; font-weight: 600;\n"
"}\n"
"QPushButton[nav=\"true\"]:hover { background: #dfeafb; }\n"
"QPushButton[nav=\"true\"]:checked {\n"
"    color: white; font-weight: 700; border: 1px solid #0d47a1; border-bottom: 3px solid #0b3a82;\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #42a5f5, stop:1 #1565c0);\n"
"}\n"
"\n"
"/* ---------- Banniere de role ---------- */\n"
"QLabel#lblRole {\n"
"    background: white; color: #1b8a5a; font-weight: 700; padding: 10p"
                        "x 16px;\n"
"    border: 2px solid #2bb673; border-radius: 10px;\n"
"}\n"
"\n"
"/* ---------- Cartes KPI (degrade + relief) ---------- */\n"
"QFrame[cardColor] { border-radius: 14px; border: none; }\n"
"QFrame[cardColor=\"blue\"]   { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2196f3, stop:1 #0d47a1);\n"
"                             border-bottom: 5px solid #0a3577; }\n"
"QFrame[cardColor=\"green\"]  { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #11c98f, stop:1 #00796b);\n"
"                             border-bottom: 5px solid #00564c; }\n"
"QFrame[cardColor=\"red\"]    { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ff5252, stop:1 #c62828);\n"
"                             border-bottom: 5px solid #8e1b1b; }\n"
"QFrame[cardColor=\"purple\"] { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #9575ff, stop:1 #5e35b1);\n"
"                             border-bottom: 5px solid #432780; }\n"
"QLabel[kpiValue=\"true\"] { font-size: 30px; font-weight: 800; colo"
                        "r: white; background: transparent; border: none; }\n"
"QLabel[kpiLabel=\"true\"] { font-size: 12px; color: rgba(255,255,255,0.92); background: transparent; border: none; }\n"
"\n"
"/* ---------- Panneaux ---------- */\n"
"QFrame[panel=\"true\"] { background: white; border: 1px solid #e0e8f4; border-bottom: 3px solid #d0dbec; border-radius: 14px; }\n"
"QFrame[panel=\"true\"] QLabel, QFrame[panel=\"true\"] QCheckBox { background: transparent; border: none; }\n"
"QFrame[panel=\"true\"] QLabel[panelTitle=\"true\"] { font-size: 14px; font-weight: 800; color: #1565c0;\n"
"                            border-bottom: 2px solid #1976d2; padding-bottom: 4px; }\n"
"\n"
"/* ---------- Boutons en relief ---------- */\n"
"QPushButton {\n"
"    color: #1f2d3d; font-weight: 600; padding: 8px 14px 6px 14px; border-radius: 9px;\n"
"    border: 1px solid #c3cfe2; border-bottom: 3px solid #a9b9d3;\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffffff, stop:1 #e6edf8);\n"
"}\n"
"QPushButton:hover   { background"
                        ": qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffffff, stop:1 #d6e6fd); }\n"
"QPushButton:pressed { border-bottom: 1px solid #a9b9d3; padding: 10px 14px 4px 14px; }\n"
"QPushButton:disabled { color: #9aa7b8; background: #eef1f6; border: 1px solid #dde3ee; border-bottom: 3px solid #cfd7e4; }\n"
"\n"
"QPushButton[role=\"primary\"], QPushButton[role=\"edit\"], QPushButton[role=\"success\"], QPushButton[role=\"danger\"],\n"
"QPushButton[role=\"warning\"], QPushButton[role=\"purple\"], QPushButton[role=\"accent\"], QPushButton[role=\"navy\"] {\n"
"    color: white; font-weight: 700;\n"
"}\n"
"QPushButton[role=\"primary\"]  { border: 1px solid #0d47a1; border-bottom: 3px solid #0a3577;\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #42a5f5, stop:1 #1565c0); }\n"
"QPushButton[role=\"edit\"]     { border: 1px solid #3b6fb6; border-bottom: 3px solid #2c548c;\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #6f9bd6, stop:1 #3b6fb6); }\n"
"QPushButton[role=\"success\"]  { border: "
                        "1px solid #1b8a5a; border-bottom: 3px solid #12633f;\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #3ddc97, stop:1 #1b9e66); }\n"
"QPushButton[role=\"danger\"]   { border: 1px solid #c62828; border-bottom: 3px solid #8e1b1b;\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ff6b6b, stop:1 #d32f2f); }\n"
"QPushButton[role=\"warning\"]  { border: 1px solid #e67e00; border-bottom: 3px solid #b05f00;\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffb74d, stop:1 #f57c00); }\n"
"QPushButton[role=\"purple\"]   { border: 1px solid #6a3fd1; border-bottom: 3px solid #4b2b99;\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9d7bff, stop:1 #6a3fd1); }\n"
"QPushButton[role=\"accent\"]   { border: 1px solid #00796b; border-bottom: 3px solid #00564c;\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #26c6b0, stop:1 #00897b); }\n"
"QPushButton[role=\"navy\"]     { border: 1px solid #1a2f55; border-bottom: 3px solid #101e38;\n"
"   "
                        " background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #3d5a96, stop:1 #1f3a6e); }\n"
"QPushButton[role]:hover      { padding-bottom: 6px; }\n"
"QPushButton[role]:pressed    { border-bottom: 1px solid #000000; padding: 10px 14px 4px 14px; }\n"
"QPushButton[role]:disabled   { color: #9aa7b8; background: #eef1f6; border: 1px solid #dde3ee; border-bottom: 3px solid #cfd7e4; }\n"
"QPushButton[nav=\"true\"]:pressed { padding: 11px 14px; border-bottom: 1px solid #0b3a82; }\n"
"\n"
"/* ---------- Champs ---------- */\n"
"QLineEdit, QComboBox, QDateEdit, QDoubleSpinBox, QTextEdit {\n"
"    background: white; border: 1px solid #cfd8e6; border-bottom: 2px solid #b9c7dc; border-radius: 8px; padding: 6px 8px;\n"
"}\n"
"QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QDoubleSpinBox:focus, QTextEdit:focus {\n"
"    border: 1px solid #1976d2; border-bottom: 2px solid #1976d2;\n"
"}\n"
"QCheckBox { spacing: 6px; color: #33475b; }\n"
"\n"
"/* Fleches (arrow_down.png / arrow_up.png) */\n"
"QComboBox::drop-down, QDateEdi"
                        "t::drop-down {\n"
"    subcontrol-origin: padding; subcontrol-position: center right; width: 24px; border: none;\n"
"}\n"
"QComboBox::down-arrow, QDateEdit::down-arrow { image: url(:/maintenance_arrow_down.png); width: 12px; height: 8px; }\n"
"QComboBox QAbstractItemView {\n"
"    background: white; border: 1px solid #cfd8e6;\n"
"    selection-background-color: #bbdefb; selection-color: #0d1b2a;\n"
"}\n"
"QDoubleSpinBox::up-button   { subcontrol-origin: border; subcontrol-position: top right;    width: 22px; border: none; }\n"
"QDoubleSpinBox::down-button { subcontrol-origin: border; subcontrol-position: bottom right; width: 22px; border: none; }\n"
"QDoubleSpinBox::up-arrow    { image: url(:/maintenance_arrow_up.png);   width: 10px; height: 7px; }\n"
"QDoubleSpinBox::down-arrow  { image: url(:/maintenance_arrow_down.png); width: 10px; height: 7px; }\n"
"\n"
"/* ---------- Tableau ---------- */\n"
"QTableView {\n"
"    background: white; alternate-background-color: #f4f8fe;\n"
"    border: 1px solid #dbe5f3; b"
                        "order-radius: 10px; gridline-color: transparent;\n"
"    selection-background-color: #bbdefb; selection-color: #0d1b2a;\n"
"}\n"
"QHeaderView::section {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2f7fd6, stop:1 #1250a8);\n"
"    color: white; font-weight: 700; padding: 9px 8px; border: none; border-right: 1px solid #0d3f87;\n"
"}\n"
"QTableCornerButton::section { background: #1250a8; border: none; }\n"
"\n"
"/* ---------- Scrollbars ---------- */\n"
"QScrollBar:vertical { background: #eef2f9; width: 12px; margin: 0; border-radius: 6px; }\n"
"QScrollBar::handle:vertical { background: #b9cbe8; border-radius: 6px; min-height: 30px; }\n"
"QScrollBar::handle:vertical:hover { background: #90b4f0; }\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }\n"
"QScrollBar:horizontal { background: #eef2f9; height: 12px; margin: 0; border-radius: 6px; }\n"
"QScrollBar::handle:horizontal { background: #b9cbe8; border-radius: 6px; min-width: 30px; }\n"
"QScrollBar::add-line:h"
                        "orizontal, QScrollBar::sub-line:horizontal { width: 0; }\n"
"\n"
"QToolTip { background: #1f3a6e; color: white; border: none; padding: 5px 8px; border-radius: 6px; }\n"
"QStatusBar { background: white; color: #6b7c93; border-top: 1px solid #d5e1f3; }\n"
"\n"
"/* ---------- Dialogue Intervention ---------- */\n"
"QLabel#dlgTitle {\n"
"    color: white; font-size: 19px; font-weight: 800; padding: 16px 22px; border-radius: 14px;\n"
"    border-bottom: 5px solid #0a3577;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2196f3, stop:1 #0d47a1);\n"
"}\n"
"QLabel#dlgSub { color: #6b7c93; font-size: 12px; background: transparent; padding: 0 4px 2px 4px; }\n"
"QLabel[caption=\"true\"] { font-size: 11px; font-weight: 700; color: #4a6384; background: transparent; border: none;\n"
"                         padding-top: 4px; }\n"
"QFrame#formCard { background: white; border: 1px solid #e0e8f4; border-bottom: 3px solid #d0dbec; border-radius: 14px; }\n"
"QFrame#formCard QLabel { background: transparent; b"
                        "order: none; }\n"
"QDialogButtonBox QPushButton { min-width: 120px; }\n"
""));
        mainLayout = new QVBoxLayout(InterventionDialog);
        mainLayout->setSpacing(12);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(22, 20, 22, 18);
        dlgTitle = new QLabel(InterventionDialog);
        dlgTitle->setObjectName("dlgTitle");

        mainLayout->addWidget(dlgTitle);

        dlgSub = new QLabel(InterventionDialog);
        dlgSub->setObjectName("dlgSub");

        mainLayout->addWidget(dlgSub);

        formCard = new QFrame(InterventionDialog);
        formCard->setObjectName("formCard");
        formGrid = new QGridLayout(formCard);
        formGrid->setObjectName("formGrid");
        formGrid->setHorizontalSpacing(18);
        formGrid->setVerticalSpacing(4);
        formGrid->setContentsMargins(20, 16, 20, 18);
        lblStation = new QLabel(formCard);
        lblStation->setObjectName("lblStation");
        lblStation->setProperty("caption", QVariant(true));

        formGrid->addWidget(lblStation, 0, 0, 1, 1);

        cbStation = new QComboBox(formCard);
        cbStation->setObjectName("cbStation");

        formGrid->addWidget(cbStation, 1, 0, 1, 1);

        lblEmploye = new QLabel(formCard);
        lblEmploye->setObjectName("lblEmploye");
        lblEmploye->setProperty("caption", QVariant(true));

        formGrid->addWidget(lblEmploye, 0, 1, 1, 1);

        cbEmploye = new QComboBox(formCard);
        cbEmploye->setObjectName("cbEmploye");

        formGrid->addWidget(cbEmploye, 1, 1, 1, 1);

        lblType = new QLabel(formCard);
        lblType->setObjectName("lblType");
        lblType->setProperty("caption", QVariant(true));

        formGrid->addWidget(lblType, 2, 0, 1, 1);

        cbType = new QComboBox(formCard);
        cbType->addItem(QString());
        cbType->addItem(QString());
        cbType->addItem(QString());
        cbType->addItem(QString());
        cbType->setObjectName("cbType");

        formGrid->addWidget(cbType, 3, 0, 1, 1);

        lblPriorite = new QLabel(formCard);
        lblPriorite->setObjectName("lblPriorite");
        lblPriorite->setProperty("caption", QVariant(true));

        formGrid->addWidget(lblPriorite, 2, 1, 1, 1);

        cbPriorite = new QComboBox(formCard);
        cbPriorite->addItem(QString());
        cbPriorite->addItem(QString());
        cbPriorite->addItem(QString());
        cbPriorite->addItem(QString());
        cbPriorite->setObjectName("cbPriorite");

        formGrid->addWidget(cbPriorite, 3, 1, 1, 1);

        lblPlan = new QLabel(formCard);
        lblPlan->setObjectName("lblPlan");
        lblPlan->setProperty("caption", QVariant(true));

        formGrid->addWidget(lblPlan, 4, 0, 1, 1);

        dePlan = new QDateEdit(formCard);
        dePlan->setObjectName("dePlan");
        dePlan->setCalendarPopup(true);

        formGrid->addWidget(dePlan, 5, 0, 1, 1);

        lblStatut = new QLabel(formCard);
        lblStatut->setObjectName("lblStatut");
        lblStatut->setProperty("caption", QVariant(true));

        formGrid->addWidget(lblStatut, 4, 1, 1, 1);

        cbStatut = new QComboBox(formCard);
        cbStatut->addItem(QString());
        cbStatut->addItem(QString());
        cbStatut->addItem(QString());
        cbStatut->addItem(QString());
        cbStatut->setObjectName("cbStatut");

        formGrid->addWidget(cbStatut, 5, 1, 1, 1);

        lblCout = new QLabel(formCard);
        lblCout->setObjectName("lblCout");
        lblCout->setProperty("caption", QVariant(true));

        formGrid->addWidget(lblCout, 6, 0, 1, 1);

        spCout = new QDoubleSpinBox(formCard);
        spCout->setObjectName("spCout");

        formGrid->addWidget(spCout, 7, 0, 1, 1);

        lblRapport = new QLabel(formCard);
        lblRapport->setObjectName("lblRapport");
        lblRapport->setProperty("caption", QVariant(true));

        formGrid->addWidget(lblRapport, 8, 0, 1, 2);

        teRapport = new QTextEdit(formCard);
        teRapport->setObjectName("teRapport");
        teRapport->setMinimumSize(QSize(0, 110));

        formGrid->addWidget(teRapport, 9, 0, 1, 2);


        mainLayout->addWidget(formCard);

        buttonBox = new QDialogButtonBox(InterventionDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Save);

        mainLayout->addWidget(buttonBox);


        retranslateUi(InterventionDialog);

        QMetaObject::connectSlotsByName(InterventionDialog);
    } // setupUi

    void retranslateUi(QDialog *InterventionDialog)
    {
        InterventionDialog->setWindowTitle(QCoreApplication::translate("InterventionDialog", "Intervention", nullptr));
        dlgTitle->setText(QCoreApplication::translate("InterventionDialog", "\360\237\233\240  Intervention de maintenance", nullptr));
        dlgSub->setText(QCoreApplication::translate("InterventionDialog", "Les champs marqu\303\251s d'une * sont obligatoires.", nullptr));
        lblStation->setText(QCoreApplication::translate("InterventionDialog", "\360\237\217\242  Station *", nullptr));
        lblEmploye->setText(QCoreApplication::translate("InterventionDialog", "\360\237\221\267  Technicien *", nullptr));
        lblType->setText(QCoreApplication::translate("InterventionDialog", "\360\237\224\247  Type *", nullptr));
        cbType->setItemText(0, QCoreApplication::translate("InterventionDialog", "Pr\303\251ventive", nullptr));
        cbType->setItemText(1, QCoreApplication::translate("InterventionDialog", "Corrective", nullptr));
        cbType->setItemText(2, QCoreApplication::translate("InterventionDialog", "Calibration", nullptr));
        cbType->setItemText(3, QCoreApplication::translate("InterventionDialog", "Remplacement capteur", nullptr));

        lblPriorite->setText(QCoreApplication::translate("InterventionDialog", "\342\232\241  Priorit\303\251 *", nullptr));
        cbPriorite->setItemText(0, QCoreApplication::translate("InterventionDialog", "Basse", nullptr));
        cbPriorite->setItemText(1, QCoreApplication::translate("InterventionDialog", "Moyenne", nullptr));
        cbPriorite->setItemText(2, QCoreApplication::translate("InterventionDialog", "Haute", nullptr));
        cbPriorite->setItemText(3, QCoreApplication::translate("InterventionDialog", "Urgente", nullptr));

        lblPlan->setText(QCoreApplication::translate("InterventionDialog", "\360\237\223\205  Date planifi\303\251e", nullptr));
        dePlan->setDisplayFormat(QCoreApplication::translate("InterventionDialog", "dd/MM/yyyy", nullptr));
        lblStatut->setText(QCoreApplication::translate("InterventionDialog", "\360\237\223\214  Statut", nullptr));
        cbStatut->setItemText(0, QCoreApplication::translate("InterventionDialog", "Planifi\303\251e", nullptr));
        cbStatut->setItemText(1, QCoreApplication::translate("InterventionDialog", "En cours", nullptr));
        cbStatut->setItemText(2, QCoreApplication::translate("InterventionDialog", "Termin\303\251e", nullptr));
        cbStatut->setItemText(3, QCoreApplication::translate("InterventionDialog", "Annul\303\251e", nullptr));

        lblCout->setText(QCoreApplication::translate("InterventionDialog", "\360\237\222\260  Co\303\273t", nullptr));
        lblRapport->setText(QCoreApplication::translate("InterventionDialog", "\360\237\223\235  Rapport d'intervention", nullptr));
        teRapport->setPlaceholderText(QCoreApplication::translate("InterventionDialog", "D\303\251crivez l'intervention r\303\251alis\303\251e, les pi\303\250ces chang\303\251es, les observations...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InterventionDialog: public Ui_InterventionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INTERVENTION_DIALOG_H
