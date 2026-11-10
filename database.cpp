#include "database.h"

#include <QCoreApplication>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>

bool Database::connect(QString *error)
{
    QSettings cfg(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);

    // Valeurs par defaut (Oracle XE local). Modifiez config.ini pour changer.
    const QString host    = cfg.value("oracle/host", "localhost").toString();
    const int     port    = cfg.value("oracle/port", 1521).toInt();
    const QString service = cfg.value("oracle/service", "XE").toString();
    const QString user    = cfg.value("oracle/user", "system").toString();
    const QString pass    = cfg.value("oracle/password", "oracle").toString();

    // Ecrit le fichier la premiere fois pour que vous puissiez l'editer
    if (!cfg.contains("oracle/host")) {
        cfg.setValue("oracle/host", host);
        cfg.setValue("oracle/port", port);
        cfg.setValue("oracle/service", service);
        cfg.setValue("oracle/user", user);
        cfg.setValue("oracle/password", pass);
        cfg.sync();
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QOCI");
    db.setHostName(host);
    db.setPort(port);
    db.setDatabaseName(service);
    db.setUserName(user);
    db.setPassword(pass);

    if (!db.open()) {
        if (error) *error = db.lastError().text();
        return false;
    }
    return true;
}
