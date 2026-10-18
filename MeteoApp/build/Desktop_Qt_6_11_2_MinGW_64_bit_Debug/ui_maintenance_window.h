/********************************************************************************
** Form generated from reading UI file 'maintenance_window.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINTENANCE_WINDOW_H
#define UI_MAINTENANCE_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MaintenanceWindow
{
public:
    QWidget *central;
    QHBoxLayout *rootLayout;
    QFrame *sidebar;
    QVBoxLayout *sideLayout;
    QLabel *lblBrand;
    QLabel *lblModule;
    QLabel *lblUserRole;
    QPushButton *btnNavList;
    QPushButton *btnNavStats;
    QPushButton *btnNavPredict;
    QSpacerItem *spacerSide;
    QLabel *lblClock;
    QPushButton *btnChangeRole;
    QWidget *content;
    QVBoxLayout *contentLayout;
    QLabel *lblRole;
    QStackedWidget *stack;
    QWidget *pageList;
    QVBoxLayout *listLayout;
    QHBoxLayout *kpiLayout;
    QFrame *cardTotal;
    QVBoxLayout *cardTotalLayout;
    QLabel *kpiTotal;
    QLabel *lblKpiTotal;
    QFrame *cardAvg;
    QVBoxLayout *cardAvgLayout;
    QLabel *kpiAvg;
    QLabel *lblKpiAvg;
    QFrame *cardLate;
    QVBoxLayout *cardLateLayout;
    QLabel *kpiLate;
    QLabel *lblKpiLate;
    QFrame *cardCost;
    QVBoxLayout *cardCostLayout;
    QLabel *kpiCost;
    QLabel *lblKpiCost;
    QFrame *actionsCard;
    QVBoxLayout *actionsLayout;
    QLabel *lblActionsTitle;
    QHBoxLayout *btnLayout;
    QPushButton *btnAdd;
    QPushButton *btnEdit;
    QPushButton *btnArchive;
    QPushButton *btnDelete;
    QSpacerItem *spacerBtns;
    QPushButton *btnCsv;
    QPushButton *btnPdf;
    QHBoxLayout *btnLayout2;
    QPushButton *btnMission;
    QPushButton *btnSla;
    QPushButton *btnPredict;
    QSpacerItem *spacerBtns2;
    QFrame *registryCard;
    QVBoxLayout *registryLayout;
    QLabel *lblRegistryTitle;
    QHBoxLayout *searchLayout;
    QLineEdit *lineSearch;
    QCheckBox *chkArchive;
    QComboBox *comboStatus;
    QComboBox *comboSort;
    QTableView *tableView;
    QWidget *pageStats;
    QGridLayout *chartsLayout;

    void setupUi(QMainWindow *MaintenanceWindow)
    {
        if (MaintenanceWindow->objectName().isEmpty())
            MaintenanceWindow->setObjectName("MaintenanceWindow");
        MaintenanceWindow->resize(1400, 820);
        MaintenanceWindow->setStyleSheet(QString::fromUtf8("/* ===========================================================\n"
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
        central = new QWidget(MaintenanceWindow);
        central->setObjectName("central");
        rootLayout = new QHBoxLayout(central);
        rootLayout->setSpacing(0);
        rootLayout->setObjectName("rootLayout");
        rootLayout->setContentsMargins(0, 0, 0, 0);
        sidebar = new QFrame(central);
        sidebar->setObjectName("sidebar");
        sidebar->setMinimumSize(QSize(235, 0));
        sidebar->setMaximumSize(QSize(235, 16777215));
        sideLayout = new QVBoxLayout(sidebar);
        sideLayout->setSpacing(8);
        sideLayout->setObjectName("sideLayout");
        sideLayout->setContentsMargins(16, 20, 16, 16);
        lblBrand = new QLabel(sidebar);
        lblBrand->setObjectName("lblBrand");

        sideLayout->addWidget(lblBrand);

        lblModule = new QLabel(sidebar);
        lblModule->setObjectName("lblModule");
        lblModule->setWordWrap(true);

        sideLayout->addWidget(lblModule);

        lblUserRole = new QLabel(sidebar);
        lblUserRole->setObjectName("lblUserRole");
        lblUserRole->setAlignment(Qt::AlignCenter);
        lblUserRole->setWordWrap(true);

        sideLayout->addWidget(lblUserRole);

        btnNavList = new QPushButton(sidebar);
        btnNavList->setObjectName("btnNavList");
        btnNavList->setProperty("nav", QVariant(true));
        btnNavList->setCheckable(true);
        btnNavList->setAutoExclusive(true);
        btnNavList->setChecked(true);
        btnNavList->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        sideLayout->addWidget(btnNavList);

        btnNavStats = new QPushButton(sidebar);
        btnNavStats->setObjectName("btnNavStats");
        btnNavStats->setProperty("nav", QVariant(true));
        btnNavStats->setCheckable(true);
        btnNavStats->setAutoExclusive(true);
        btnNavStats->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        sideLayout->addWidget(btnNavStats);

        btnNavPredict = new QPushButton(sidebar);
        btnNavPredict->setObjectName("btnNavPredict");
        btnNavPredict->setProperty("nav", QVariant(true));
        btnNavPredict->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        sideLayout->addWidget(btnNavPredict);

        spacerSide = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        sideLayout->addItem(spacerSide);

        lblClock = new QLabel(sidebar);
        lblClock->setObjectName("lblClock");
        lblClock->setAlignment(Qt::AlignCenter);

        sideLayout->addWidget(lblClock);

        btnChangeRole = new QPushButton(sidebar);
        btnChangeRole->setObjectName("btnChangeRole");
        btnChangeRole->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        sideLayout->addWidget(btnChangeRole);


        rootLayout->addWidget(sidebar);

        content = new QWidget(central);
        content->setObjectName("content");
        contentLayout = new QVBoxLayout(content);
        contentLayout->setSpacing(14);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(22, 18, 22, 10);
        lblRole = new QLabel(content);
        lblRole->setObjectName("lblRole");

        contentLayout->addWidget(lblRole);

        stack = new QStackedWidget(content);
        stack->setObjectName("stack");
        pageList = new QWidget();
        pageList->setObjectName("pageList");
        listLayout = new QVBoxLayout(pageList);
        listLayout->setSpacing(14);
        listLayout->setObjectName("listLayout");
        listLayout->setContentsMargins(0, 0, 0, 0);
        kpiLayout = new QHBoxLayout();
        kpiLayout->setSpacing(16);
        kpiLayout->setObjectName("kpiLayout");
        cardTotal = new QFrame(pageList);
        cardTotal->setObjectName("cardTotal");
        cardTotal->setMinimumSize(QSize(0, 104));
        cardTotalLayout = new QVBoxLayout(cardTotal);
        cardTotalLayout->setObjectName("cardTotalLayout");
        kpiTotal = new QLabel(cardTotal);
        kpiTotal->setObjectName("kpiTotal");
        kpiTotal->setProperty("kpiValue", QVariant(true));
        kpiTotal->setAlignment(Qt::AlignCenter);

        cardTotalLayout->addWidget(kpiTotal);

        lblKpiTotal = new QLabel(cardTotal);
        lblKpiTotal->setObjectName("lblKpiTotal");
        lblKpiTotal->setProperty("kpiLabel", QVariant(true));
        lblKpiTotal->setAlignment(Qt::AlignCenter);

        cardTotalLayout->addWidget(lblKpiTotal);


        kpiLayout->addWidget(cardTotal);

        cardAvg = new QFrame(pageList);
        cardAvg->setObjectName("cardAvg");
        cardAvg->setMinimumSize(QSize(0, 104));
        cardAvgLayout = new QVBoxLayout(cardAvg);
        cardAvgLayout->setObjectName("cardAvgLayout");
        kpiAvg = new QLabel(cardAvg);
        kpiAvg->setObjectName("kpiAvg");
        kpiAvg->setProperty("kpiValue", QVariant(true));
        kpiAvg->setAlignment(Qt::AlignCenter);

        cardAvgLayout->addWidget(kpiAvg);

        lblKpiAvg = new QLabel(cardAvg);
        lblKpiAvg->setObjectName("lblKpiAvg");
        lblKpiAvg->setProperty("kpiLabel", QVariant(true));
        lblKpiAvg->setAlignment(Qt::AlignCenter);

        cardAvgLayout->addWidget(lblKpiAvg);


        kpiLayout->addWidget(cardAvg);

        cardLate = new QFrame(pageList);
        cardLate->setObjectName("cardLate");
        cardLate->setMinimumSize(QSize(0, 104));
        cardLateLayout = new QVBoxLayout(cardLate);
        cardLateLayout->setObjectName("cardLateLayout");
        kpiLate = new QLabel(cardLate);
        kpiLate->setObjectName("kpiLate");
        kpiLate->setProperty("kpiValue", QVariant(true));
        kpiLate->setAlignment(Qt::AlignCenter);

        cardLateLayout->addWidget(kpiLate);

        lblKpiLate = new QLabel(cardLate);
        lblKpiLate->setObjectName("lblKpiLate");
        lblKpiLate->setProperty("kpiLabel", QVariant(true));
        lblKpiLate->setAlignment(Qt::AlignCenter);

        cardLateLayout->addWidget(lblKpiLate);


        kpiLayout->addWidget(cardLate);

        cardCost = new QFrame(pageList);
        cardCost->setObjectName("cardCost");
        cardCost->setMinimumSize(QSize(0, 104));
        cardCostLayout = new QVBoxLayout(cardCost);
        cardCostLayout->setObjectName("cardCostLayout");
        kpiCost = new QLabel(cardCost);
        kpiCost->setObjectName("kpiCost");
        kpiCost->setProperty("kpiValue", QVariant(true));
        kpiCost->setAlignment(Qt::AlignCenter);

        cardCostLayout->addWidget(kpiCost);

        lblKpiCost = new QLabel(cardCost);
        lblKpiCost->setObjectName("lblKpiCost");
        lblKpiCost->setProperty("kpiLabel", QVariant(true));
        lblKpiCost->setAlignment(Qt::AlignCenter);

        cardCostLayout->addWidget(lblKpiCost);


        kpiLayout->addWidget(cardCost);


        listLayout->addLayout(kpiLayout);

        actionsCard = new QFrame(pageList);
        actionsCard->setObjectName("actionsCard");
        actionsCard->setProperty("panel", QVariant(true));
        actionsLayout = new QVBoxLayout(actionsCard);
        actionsLayout->setObjectName("actionsLayout");
        lblActionsTitle = new QLabel(actionsCard);
        lblActionsTitle->setObjectName("lblActionsTitle");
        lblActionsTitle->setProperty("panelTitle", QVariant(true));

        actionsLayout->addWidget(lblActionsTitle);

        btnLayout = new QHBoxLayout();
        btnLayout->setObjectName("btnLayout");
        btnAdd = new QPushButton(actionsCard);
        btnAdd->setObjectName("btnAdd");
        btnAdd->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        btnLayout->addWidget(btnAdd);

        btnEdit = new QPushButton(actionsCard);
        btnEdit->setObjectName("btnEdit");
        btnEdit->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        btnLayout->addWidget(btnEdit);

        btnArchive = new QPushButton(actionsCard);
        btnArchive->setObjectName("btnArchive");
        btnArchive->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        btnLayout->addWidget(btnArchive);

        btnDelete = new QPushButton(actionsCard);
        btnDelete->setObjectName("btnDelete");
        btnDelete->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        btnLayout->addWidget(btnDelete);

        spacerBtns = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        btnLayout->addItem(spacerBtns);

        btnCsv = new QPushButton(actionsCard);
        btnCsv->setObjectName("btnCsv");
        btnCsv->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        btnLayout->addWidget(btnCsv);

        btnPdf = new QPushButton(actionsCard);
        btnPdf->setObjectName("btnPdf");
        btnPdf->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        btnLayout->addWidget(btnPdf);


        actionsLayout->addLayout(btnLayout);

        btnLayout2 = new QHBoxLayout();
        btnLayout2->setObjectName("btnLayout2");
        btnMission = new QPushButton(actionsCard);
        btnMission->setObjectName("btnMission");
        btnMission->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        btnLayout2->addWidget(btnMission);

        btnSla = new QPushButton(actionsCard);
        btnSla->setObjectName("btnSla");
        btnSla->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        btnLayout2->addWidget(btnSla);

        btnPredict = new QPushButton(actionsCard);
        btnPredict->setObjectName("btnPredict");
        btnPredict->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        btnLayout2->addWidget(btnPredict);

        spacerBtns2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        btnLayout2->addItem(spacerBtns2);


        actionsLayout->addLayout(btnLayout2);


        listLayout->addWidget(actionsCard);

        registryCard = new QFrame(pageList);
        registryCard->setObjectName("registryCard");
        registryCard->setProperty("panel", QVariant(true));
        registryLayout = new QVBoxLayout(registryCard);
        registryLayout->setObjectName("registryLayout");
        lblRegistryTitle = new QLabel(registryCard);
        lblRegistryTitle->setObjectName("lblRegistryTitle");
        lblRegistryTitle->setProperty("panelTitle", QVariant(true));

        registryLayout->addWidget(lblRegistryTitle);

        searchLayout = new QHBoxLayout();
        searchLayout->setObjectName("searchLayout");
        lineSearch = new QLineEdit(registryCard);
        lineSearch->setObjectName("lineSearch");

        searchLayout->addWidget(lineSearch);

        chkArchive = new QCheckBox(registryCard);
        chkArchive->setObjectName("chkArchive");

        searchLayout->addWidget(chkArchive);

        comboStatus = new QComboBox(registryCard);
        comboStatus->addItem(QString());
        comboStatus->addItem(QString());
        comboStatus->addItem(QString());
        comboStatus->addItem(QString());
        comboStatus->addItem(QString());
        comboStatus->setObjectName("comboStatus");

        searchLayout->addWidget(comboStatus);

        comboSort = new QComboBox(registryCard);
        comboSort->addItem(QString());
        comboSort->addItem(QString());
        comboSort->addItem(QString());
        comboSort->addItem(QString());
        comboSort->setObjectName("comboSort");

        searchLayout->addWidget(comboSort);

        searchLayout->setStretch(0, 1);

        registryLayout->addLayout(searchLayout);

        tableView = new QTableView(registryCard);
        tableView->setObjectName("tableView");

        registryLayout->addWidget(tableView);


        listLayout->addWidget(registryCard);

        stack->addWidget(pageList);
        pageStats = new QWidget();
        pageStats->setObjectName("pageStats");
        chartsLayout = new QGridLayout(pageStats);
        chartsLayout->setSpacing(16);
        chartsLayout->setObjectName("chartsLayout");
        chartsLayout->setContentsMargins(0, 0, 0, 0);
        stack->addWidget(pageStats);

        contentLayout->addWidget(stack);


        rootLayout->addWidget(content);

        MaintenanceWindow->setCentralWidget(central);

        retranslateUi(MaintenanceWindow);

        stack->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MaintenanceWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MaintenanceWindow)
    {
        MaintenanceWindow->setWindowTitle(QCoreApplication::translate("MaintenanceWindow", "Smart Weather \342\200\223 Module 5 : Maintenance & Interventions", nullptr));
        lblBrand->setText(QCoreApplication::translate("MaintenanceWindow", "\342\232\241 SMART WEATHER", nullptr));
        lblModule->setText(QCoreApplication::translate("MaintenanceWindow", "Module 5 : Maintenance & Interventions", nullptr));
        lblUserRole->setText(QCoreApplication::translate("MaintenanceWindow", "\360\237\221\244 \342\200\223", nullptr));
        btnNavList->setText(QCoreApplication::translate("MaintenanceWindow", "\360\237\223\213  Supervision globale", nullptr));
        btnNavStats->setText(QCoreApplication::translate("MaintenanceWindow", "\360\237\223\212  Statistiques", nullptr));
        btnNavPredict->setText(QCoreApplication::translate("MaintenanceWindow", "\360\237\224\256  Pr\303\251diction pr\303\251ventive", nullptr));
        lblClock->setText(QCoreApplication::translate("MaintenanceWindow", "\342\200\223", nullptr));
        btnChangeRole->setProperty("role", QVariant(QCoreApplication::translate("MaintenanceWindow", "primary", nullptr)));
        btnChangeRole->setText(QCoreApplication::translate("MaintenanceWindow", "\360\237\224\201  Changer de r\303\264le", nullptr));
        lblRole->setText(QCoreApplication::translate("MaintenanceWindow", "\342\200\223", nullptr));
        cardTotal->setProperty("cardColor", QVariant(QCoreApplication::translate("MaintenanceWindow", "blue", nullptr)));
        kpiTotal->setText(QCoreApplication::translate("MaintenanceWindow", "\342\200\223", nullptr));
        lblKpiTotal->setText(QCoreApplication::translate("MaintenanceWindow", "Interventions actives", nullptr));
        cardAvg->setProperty("cardColor", QVariant(QCoreApplication::translate("MaintenanceWindow", "green", nullptr)));
        kpiAvg->setText(QCoreApplication::translate("MaintenanceWindow", "\342\200\223", nullptr));
        lblKpiAvg->setText(QCoreApplication::translate("MaintenanceWindow", "Temps moyen de r\303\251solution", nullptr));
        cardLate->setProperty("cardColor", QVariant(QCoreApplication::translate("MaintenanceWindow", "red", nullptr)));
        kpiLate->setText(QCoreApplication::translate("MaintenanceWindow", "\342\200\223", nullptr));
        lblKpiLate->setText(QCoreApplication::translate("MaintenanceWindow", "En retard SLA", nullptr));
        cardCost->setProperty("cardColor", QVariant(QCoreApplication::translate("MaintenanceWindow", "purple", nullptr)));
        kpiCost->setText(QCoreApplication::translate("MaintenanceWindow", "\342\200\223", nullptr));
        lblKpiCost->setText(QCoreApplication::translate("MaintenanceWindow", "Co\303\273t total", nullptr));
        lblActionsTitle->setText(QCoreApplication::translate("MaintenanceWindow", "\342\232\241 Actions rapides", nullptr));
        btnAdd->setProperty("role", QVariant(QCoreApplication::translate("MaintenanceWindow", "primary", nullptr)));
        btnAdd->setText(QCoreApplication::translate("MaintenanceWindow", "\357\274\213  Ajouter", nullptr));
        btnEdit->setProperty("role", QVariant(QCoreApplication::translate("MaintenanceWindow", "edit", nullptr)));
        btnEdit->setText(QCoreApplication::translate("MaintenanceWindow", "\342\234\216  Modifier", nullptr));
        btnArchive->setProperty("role", QVariant(QCoreApplication::translate("MaintenanceWindow", "warning", nullptr)));
        btnArchive->setText(QCoreApplication::translate("MaintenanceWindow", "\360\237\227\204  Archiver", nullptr));
        btnDelete->setProperty("role", QVariant(QCoreApplication::translate("MaintenanceWindow", "danger", nullptr)));
        btnDelete->setText(QCoreApplication::translate("MaintenanceWindow", "\360\237\227\221  Supprimer", nullptr));
        btnCsv->setProperty("role", QVariant(QCoreApplication::translate("MaintenanceWindow", "navy", nullptr)));
        btnCsv->setText(QCoreApplication::translate("MaintenanceWindow", "\342\254\207  CSV", nullptr));
        btnPdf->setProperty("role", QVariant(QCoreApplication::translate("MaintenanceWindow", "navy", nullptr)));
        btnPdf->setText(QCoreApplication::translate("MaintenanceWindow", "\360\237\223\225  PDF", nullptr));
        btnMission->setProperty("role", QVariant(QCoreApplication::translate("MaintenanceWindow", "success", nullptr)));
        btnMission->setText(QCoreApplication::translate("MaintenanceWindow", "\360\237\223\204  Ordre de mission", nullptr));
        btnSla->setProperty("role", QVariant(QCoreApplication::translate("MaintenanceWindow", "purple", nullptr)));
        btnSla->setText(QCoreApplication::translate("MaintenanceWindow", "\342\232\240  Retards SLA", nullptr));
        btnPredict->setProperty("role", QVariant(QCoreApplication::translate("MaintenanceWindow", "accent", nullptr)));
        btnPredict->setText(QCoreApplication::translate("MaintenanceWindow", "\360\237\224\256  Planification pr\303\251dictive", nullptr));
        lblRegistryTitle->setText(QCoreApplication::translate("MaintenanceWindow", "\360\237\223\213 Registre des interventions", nullptr));
        lineSearch->setPlaceholderText(QCoreApplication::translate("MaintenanceWindow", "\360\237\224\216  Rechercher par station, technicien, type ou gouvernorat...", nullptr));
        chkArchive->setText(QCoreApplication::translate("MaintenanceWindow", "Afficher les archives", nullptr));
        comboStatus->setItemText(0, QCoreApplication::translate("MaintenanceWindow", "Tous les statuts", nullptr));
        comboStatus->setItemText(1, QCoreApplication::translate("MaintenanceWindow", "Planifi\303\251e", nullptr));
        comboStatus->setItemText(2, QCoreApplication::translate("MaintenanceWindow", "En cours", nullptr));
        comboStatus->setItemText(3, QCoreApplication::translate("MaintenanceWindow", "Termin\303\251e", nullptr));
        comboStatus->setItemText(4, QCoreApplication::translate("MaintenanceWindow", "Annul\303\251e", nullptr));

        comboSort->setItemText(0, QCoreApplication::translate("MaintenanceWindow", "Trier : Priorit\303\251", nullptr));
        comboSort->setItemText(1, QCoreApplication::translate("MaintenanceWindow", "Trier : Date planifi\303\251e", nullptr));
        comboSort->setItemText(2, QCoreApplication::translate("MaintenanceWindow", "Trier : Statut", nullptr));
        comboSort->setItemText(3, QCoreApplication::translate("MaintenanceWindow", "Trier : Co\303\273t", nullptr));

    } // retranslateUi

};

namespace Ui {
    class MaintenanceWindow: public Ui_MaintenanceWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINTENANCE_WINDOW_H
