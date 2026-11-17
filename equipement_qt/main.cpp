#include "mainwindow.h"
#include "connection.h"
#include <QApplication>
#include <QStyleHints>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Theme clair impose : sans ca, Qt suit le mode sombre de Windows (texte blanc)
    // et les couleurs claires de la feuille de style rendent le texte invisible.
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);

    // La connexion doit etre ouverte AVANT de creer la fenetre :
    // le constructeur de MainWindow appelle afficher() qui lance un SELECT.
    if (!Connection::createInstance().createConnection()) {
        QMessageBox::critical(nullptr, "Base de donnees",
                              "Connexion impossible.\nVerifiez la source ODBC MonOracleDSN.");
        return 1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
