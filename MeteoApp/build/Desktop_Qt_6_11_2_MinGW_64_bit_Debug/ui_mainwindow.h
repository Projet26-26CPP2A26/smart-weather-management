/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QHBoxLayout *filterLayout;
    QLineEdit *stationEdit;
    QCheckBox *useDates;
    QDateEdit *fromEdit;
    QDateEdit *toEdit;
    QLabel *sortLabel;
    QComboBox *sortBy;
    QComboBox *sortDir;
    QPushButton *searchButton;
    QTableView *view;
    QHBoxLayout *actionLayout;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QSpacerItem *actionSpacer;
    QPushButton *pdfButton;
    QPushButton *histoButton;
    QPushButton *attestButton;
    QPushButton *anomalyButton;
    QPushButton *maintButton;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1100, 650);
        MainWindow->setStyleSheet(QString::fromUtf8("/* Th\303\250me clair moderne - appliqu\303\251 dans main.cpp */\n"
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
"QPushButton#maintButton { background: #6a3fd1; color: white; border: none; font-weight: 600; }\n"
"QPushButton#maintButton:hover { background: #5430a8; }\n"
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
"QScrollBar::handle:h"
                        "orizontal { background: #b8c2d3; border-radius: 5px; min-width: 30px; }\n"
"QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }\n"
"QPlainTextEdit { background: white; border: 1px solid #d6dce6; border-radius: 6px; padding: 6px; }\n"
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
"QDateEdit, QDateTimeEdit { min-width: 120p"
                        "x; }\n"
""));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QVBoxLayout(centralwidget);
        mainLayout->setObjectName("mainLayout");
        titleLabel = new QLabel(centralwidget);
        titleLabel->setObjectName("titleLabel");

        mainLayout->addWidget(titleLabel);

        filterLayout = new QHBoxLayout();
        filterLayout->setObjectName("filterLayout");
        stationEdit = new QLineEdit(centralwidget);
        stationEdit->setObjectName("stationEdit");

        filterLayout->addWidget(stationEdit);

        useDates = new QCheckBox(centralwidget);
        useDates->setObjectName("useDates");

        filterLayout->addWidget(useDates);

        fromEdit = new QDateEdit(centralwidget);
        fromEdit->setObjectName("fromEdit");
        fromEdit->setCalendarPopup(true);

        filterLayout->addWidget(fromEdit);

        toEdit = new QDateEdit(centralwidget);
        toEdit->setObjectName("toEdit");
        toEdit->setCalendarPopup(true);

        filterLayout->addWidget(toEdit);

        sortLabel = new QLabel(centralwidget);
        sortLabel->setObjectName("sortLabel");

        filterLayout->addWidget(sortLabel);

        sortBy = new QComboBox(centralwidget);
        sortBy->addItem(QString());
        sortBy->addItem(QString());
        sortBy->setObjectName("sortBy");

        filterLayout->addWidget(sortBy);

        sortDir = new QComboBox(centralwidget);
        sortDir->addItem(QString());
        sortDir->addItem(QString());
        sortDir->setObjectName("sortDir");

        filterLayout->addWidget(sortDir);

        searchButton = new QPushButton(centralwidget);
        searchButton->setObjectName("searchButton");

        filterLayout->addWidget(searchButton);


        mainLayout->addLayout(filterLayout);

        view = new QTableView(centralwidget);
        view->setObjectName("view");
        view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        view->setAlternatingRowColors(true);
        view->setSelectionMode(QAbstractItemView::SingleSelection);
        view->setSelectionBehavior(QAbstractItemView::SelectRows);
        view->horizontalHeader()->setStretchLastSection(true);

        mainLayout->addWidget(view);

        actionLayout = new QHBoxLayout();
        actionLayout->setObjectName("actionLayout");
        addButton = new QPushButton(centralwidget);
        addButton->setObjectName("addButton");

        actionLayout->addWidget(addButton);

        editButton = new QPushButton(centralwidget);
        editButton->setObjectName("editButton");

        actionLayout->addWidget(editButton);

        deleteButton = new QPushButton(centralwidget);
        deleteButton->setObjectName("deleteButton");

        actionLayout->addWidget(deleteButton);

        actionSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        actionLayout->addItem(actionSpacer);

        pdfButton = new QPushButton(centralwidget);
        pdfButton->setObjectName("pdfButton");

        actionLayout->addWidget(pdfButton);

        histoButton = new QPushButton(centralwidget);
        histoButton->setObjectName("histoButton");

        actionLayout->addWidget(histoButton);

        attestButton = new QPushButton(centralwidget);
        attestButton->setObjectName("attestButton");

        actionLayout->addWidget(attestButton);

        anomalyButton = new QPushButton(centralwidget);
        anomalyButton->setObjectName("anomalyButton");

        actionLayout->addWidget(anomalyButton);

        maintButton = new QPushButton(centralwidget);
        maintButton->setObjectName("maintButton");

        actionLayout->addWidget(maintButton);


        mainLayout->addLayout(actionLayout);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Gestion des Relev\303\251s M\303\251t\303\251o & Capteurs", nullptr));
        titleLabel->setText(QCoreApplication::translate("MainWindow", "Gestion des Relev\303\251s M\303\251t\303\251o & Capteurs", nullptr));
        stationEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "ID station (vide = toutes)", nullptr));
        useDates->setText(QCoreApplication::translate("MainWindow", "Plage de dates :", nullptr));
        fromEdit->setDisplayFormat(QCoreApplication::translate("MainWindow", "dd/MM/yyyy", nullptr));
        toEdit->setDisplayFormat(QCoreApplication::translate("MainWindow", "dd/MM/yyyy", nullptr));
        sortLabel->setText(QCoreApplication::translate("MainWindow", "Trier par :", nullptr));
        sortBy->setItemText(0, QCoreApplication::translate("MainWindow", "Horodatage", nullptr));
        sortBy->setItemText(1, QCoreApplication::translate("MainWindow", "Temp\303\251rature", nullptr));

        sortDir->setItemText(0, QCoreApplication::translate("MainWindow", "Croissant", nullptr));
        sortDir->setItemText(1, QCoreApplication::translate("MainWindow", "D\303\251croissant", nullptr));

        searchButton->setText(QCoreApplication::translate("MainWindow", "Rechercher", nullptr));
        addButton->setText(QCoreApplication::translate("MainWindow", "Ajouter", nullptr));
        editButton->setText(QCoreApplication::translate("MainWindow", "Modifier", nullptr));
        deleteButton->setText(QCoreApplication::translate("MainWindow", "Supprimer", nullptr));
        pdfButton->setText(QCoreApplication::translate("MainWindow", "Export PDF / CSV", nullptr));
        histoButton->setText(QCoreApplication::translate("MainWindow", "Histogramme", nullptr));
        attestButton->setText(QCoreApplication::translate("MainWindow", "\342\230\205 Attestation m\303\251t\303\251o", nullptr));
        anomalyButton->setText(QCoreApplication::translate("MainWindow", "\342\230\205 Anomalies capteurs", nullptr));
        maintButton->setText(QCoreApplication::translate("MainWindow", "\360\237\233\240 Maintenance (Module 5)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
