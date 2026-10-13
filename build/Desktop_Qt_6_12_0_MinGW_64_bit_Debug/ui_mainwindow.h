/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *idLineEdit;
    QLabel *label_2;
    QLineEdit *nomLineEdit;
    QLabel *label_3;
    QComboBox *typeComboBox;
    QLabel *label_4;
    QComboBox *gouvernoratComboBox;
    QLabel *label_5;
    QLineEdit *altitudeLineEdit;
    QLabel *label_6;
    QComboBox *statutComboBox;
    QLabel *label_7;
    QDateEdit *dateInstallationEdit;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnAfficher;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *searchLineEdit;
    QComboBox *sortComboBox;
    QTableView *tableViewStation;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1338, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout_3 = new QHBoxLayout(centralwidget);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        formLayout = new QFormLayout(groupBox);
        formLayout->setObjectName("formLayout");
        label = new QLabel(groupBox);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, label);

        idLineEdit = new QLineEdit(groupBox);
        idLineEdit->setObjectName("idLineEdit");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, idLineEdit);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, label_2);

        nomLineEdit = new QLineEdit(groupBox);
        nomLineEdit->setObjectName("nomLineEdit");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, nomLineEdit);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, label_3);

        typeComboBox = new QComboBox(groupBox);
        typeComboBox->setObjectName("typeComboBox");

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, typeComboBox);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, label_4);

        gouvernoratComboBox = new QComboBox(groupBox);
        gouvernoratComboBox->setObjectName("gouvernoratComboBox");

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, gouvernoratComboBox);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");

        formLayout->setWidget(4, QFormLayout::ItemRole::LabelRole, label_5);

        altitudeLineEdit = new QLineEdit(groupBox);
        altitudeLineEdit->setObjectName("altitudeLineEdit");

        formLayout->setWidget(4, QFormLayout::ItemRole::FieldRole, altitudeLineEdit);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName("label_6");

        formLayout->setWidget(5, QFormLayout::ItemRole::LabelRole, label_6);

        statutComboBox = new QComboBox(groupBox);
        statutComboBox->setObjectName("statutComboBox");

        formLayout->setWidget(5, QFormLayout::ItemRole::FieldRole, statutComboBox);

        label_7 = new QLabel(groupBox);
        label_7->setObjectName("label_7");

        formLayout->setWidget(6, QFormLayout::ItemRole::LabelRole, label_7);

        dateInstallationEdit = new QDateEdit(groupBox);
        dateInstallationEdit->setObjectName("dateInstallationEdit");

        formLayout->setWidget(6, QFormLayout::ItemRole::FieldRole, dateInstallationEdit);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setMaximumSize(QSize(16777215, 90));
        horizontalLayout = new QHBoxLayout(groupBox_2);
        horizontalLayout->setObjectName("horizontalLayout");
        btnAjouter = new QPushButton(groupBox_2);
        btnAjouter->setObjectName("btnAjouter");

        horizontalLayout->addWidget(btnAjouter);

        btnModifier = new QPushButton(groupBox_2);
        btnModifier->setObjectName("btnModifier");

        horizontalLayout->addWidget(btnModifier);

        btnSupprimer = new QPushButton(groupBox_2);
        btnSupprimer->setObjectName("btnSupprimer");

        horizontalLayout->addWidget(btnSupprimer);

        btnAfficher = new QPushButton(groupBox_2);
        btnAfficher->setObjectName("btnAfficher");

        horizontalLayout->addWidget(btnAfficher);


        verticalLayout->addWidget(groupBox_2);


        horizontalLayout_3->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        searchLineEdit = new QLineEdit(centralwidget);
        searchLineEdit->setObjectName("searchLineEdit");

        horizontalLayout_2->addWidget(searchLineEdit);

        sortComboBox = new QComboBox(centralwidget);
        sortComboBox->addItem(QString());
        sortComboBox->setObjectName("sortComboBox");
        sortComboBox->setMaximumSize(QSize(150, 16777215));

        horizontalLayout_2->addWidget(sortComboBox);


        verticalLayout_2->addLayout(horizontalLayout_2);

        tableViewStation = new QTableView(centralwidget);
        tableViewStation->setObjectName("tableViewStation");

        verticalLayout_2->addWidget(tableViewStation);


        horizontalLayout_3->addLayout(verticalLayout_2);

        horizontalLayout_3->setStretch(1, 2);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1338, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Smart Weather: Gestion des stations", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Gestion Station", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "ID Station:", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Nom Station:", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Type:", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Gouvernorat:", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Altitude:", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Statut:", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Date Installation:", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "actions", nullptr));
        btnAjouter->setText(QCoreApplication::translate("MainWindow", "Ajouter", nullptr));
        btnModifier->setText(QCoreApplication::translate("MainWindow", "Modifier", nullptr));
        btnSupprimer->setText(QCoreApplication::translate("MainWindow", "Supprimer", nullptr));
        btnAfficher->setText(QCoreApplication::translate("MainWindow", "Actualiser", nullptr));
        searchLineEdit->setText(QCoreApplication::translate("MainWindow", "Rechercher...", nullptr));
        sortComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "Altitude", nullptr));

    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
