#include "mainwindow.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. Connexion Oracle via ODBC (DSN cree dans odbcad32 64 bits)
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("Source_Oracle");
    db.setUserName("SYSTEM");
    db.setPassword("aziz2006");  // <-- 7ot el-password mte3 Oracle XE

    if (!db.open()) {
        qDebug() << "Erreur de connexion :" << db.lastError().text();
        QMessageBox::critical(nullptr, "Connexion échouée",
                              "Impossible de se connecter à la base de données.\n\n"
                                  + db.lastError().text());
        return 1;
    }
    qDebug() << "Connexion à la base réussie !";

    // 2. Creation de la table (ORA-00955 = table deja existante, c'est normal)
    {
        QSqlQuery query;
        bool ok = query.exec(
            "CREATE TABLE STATION ("
            "   ID_STATION VARCHAR2(50) PRIMARY KEY,"
            "   NOM_STATION VARCHAR2(100),"
            "   TYPE_STATION VARCHAR2(50),"
            "   GOUVERNORAT VARCHAR2(50),"
            "   ALTITUDE NUMBER,"
            "   STATUT VARCHAR2(50),"
            "   DATE_INSTALLATION DATE"
            ")");
        if (ok)
            qDebug() << "Table STATION créée avec succès !";
        else
            qDebug() << "Note création table :" << query.lastError().text();
    }

    MainWindow w;
    w.show();

    return a.exec();
}