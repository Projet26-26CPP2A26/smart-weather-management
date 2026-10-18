#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Apparence : style Fusion + feuille de style (style.qss)
    app.setStyle("Fusion");
    QFile qss(":/style.qss");
    if (qss.open(QIODevice::ReadOnly)) app.setStyleSheet(QString::fromUtf8(qss.readAll()));

    // Paramètres modifiables par variables d'environnement
    QSqlDatabase db = QSqlDatabase::addDatabase("QOCI");
    db.setDatabaseName(qEnvironmentVariable("ORA_DB", "//localhost:1521/XEPDB1"));
    db.setUserName(qEnvironmentVariable("ORA_USER", "meteo"));
    db.setPassword(qEnvironmentVariable("ORA_PASS", "meteo"));

    if (!db.open()) {
        QMessageBox::critical(nullptr, "Connexion Oracle", db.lastError().text());
        return 1;
    }
    MainWindow w;
    w.show();
    return app.exec();
}
