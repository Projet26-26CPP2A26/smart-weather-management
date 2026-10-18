#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <functional>
#include "mainwindow.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

// ---------------------------------------------------------------- Outils Oracle
// Nom du driver ODBC Oracle installe (ex: "Oracle in OraDB21Home1")
static QString findOracleOdbcDriver()
{
    QString forced = qEnvironmentVariable("ORA_ODBC_DRIVER");
    if (!forced.isEmpty()) return forced;
#ifdef Q_OS_WIN
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\ODBC\\ODBCINST.INI\\ODBC Drivers",
                  QSettings::NativeFormat);
    for (const QString &k : reg.childKeys())
        if (k.contains("Oracle", Qt::CaseInsensitive)) return k;
#endif
    return QString();
}

// Dossier BIN d'Oracle (contient oci.dll, SQORA32.DLL, ...)
static QString findOracleBin(const QString &odbcDriver)
{
    QString bin = qEnvironmentVariable("ORA_BIN");
    if (!bin.isEmpty()) return QDir::toNativeSeparators(bin);
#ifdef Q_OS_WIN
    if (!odbcDriver.isEmpty()) {
        QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\ODBC\\ODBCINST.INI\\" + odbcDriver,
                      QSettings::NativeFormat);
        QString dll = reg.value("Driver").toString();
        if (!dll.isEmpty()) return QDir::toNativeSeparators(QFileInfo(dll).absolutePath());
    }
#endif
    QString home = qEnvironmentVariable("ORACLE_HOME");
    if (!home.isEmpty()) return QDir::toNativeSeparators(home + "/bin");
    return QString();
}

// Ajoute le dossier Oracle au PATH du processus pour que Windows trouve les DLL
static void addToDllPath(const QString &dir)
{
    if (dir.isEmpty()) return;
    qputenv("PATH", (dir + ";" + qEnvironmentVariable("PATH")).toLocal8Bit());
#ifdef Q_OS_WIN
    SetDllDirectoryW(reinterpret_cast<LPCWSTR>(dir.utf16()));
#endif
}

// Essaie d'ouvrir une connexion avec un driver donne
static bool tryConnect(const QString &driver,
                       const std::function<void(QSqlDatabase &)> &configure,
                       QString &errorText)
{
    {   // la portee garantit que 'db' est detruit avant removeDatabase
        QSqlDatabase db = QSqlDatabase::addDatabase(driver);
        configure(db);
        if (db.open()) return true;
        errorText += driver + " : " + db.lastError().text() + "\n\n";
    }
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    return false;
}

static bool connectOracle(QString &errorText)
{
    const QString host = qEnvironmentVariable("ORA_HOST", "localhost");
    const QString port = qEnvironmentVariable("ORA_PORT", "1521");
    const QString svc  = qEnvironmentVariable("ORA_SERVICE", "XEPDB1");   // essayer "XE" sinon
    const QString user = qEnvironmentVariable("ORA_USER", "meteo");
    const QString pass = qEnvironmentVariable("ORA_PASS", "meteo");

    const QString drv = findOracleOdbcDriver();
    const QString bin = findOracleBin(drv);
    addToDllPath(bin);

    // 1) Driver natif Qt (plugin QOCI) -> necessite oci.dll dans le PATH
    if (QSqlDatabase::isDriverAvailable("QOCI")) {
        if (tryConnect("QOCI", [&](QSqlDatabase &db) {
                // EZConnect : host:port/service_name (evite l'erreur ORA-12505 avec SID)
                db.setDatabaseName(QString("%1:%2/%3").arg(host, port, svc));
                db.setUserName(user);
                db.setPassword(pass);
            }, errorText))
            return true;
    }

    // 2) ODBC
    if (QSqlDatabase::isDriverAvailable("QODBC")) {
        const QString dsn = qEnvironmentVariable("ORA_DSN");
        if (dsn.isEmpty() && drv.isEmpty()) {
            errorText += "Aucun driver ODBC Oracle trouve.\n\n";
        } else if (tryConnect("QODBC", [&](QSqlDatabase &db) {
                       if (!dsn.isEmpty())
                           db.setDatabaseName(dsn);
                       else
                           db.setDatabaseName(QString("Driver={%1};Dbq=//%2:%3/%4;Uid=%5;Pwd=%6;")
                                                  .arg(drv, host, port, svc, user, pass));
                       db.setUserName(user);
                       db.setPassword(pass);
                   }, errorText))
            return true;
    }

    // Diagnostic
    errorText += "--- Diagnostic ---\n";
    errorText += "Dossier Oracle BIN : " + (bin.isEmpty() ? QString("(introuvable)") : bin) + "\n";
    if (!bin.isEmpty()) {
        for (const char *f : {"oci.dll", "SQORA32.DLL", "oraociei21.dll"})
            errorText += QString("  %1 : %2\n").arg(f, QFile::exists(bin + "\\" + f) ? "present" : "ABSENT");
    }
    errorText += "Drivers Qt : " + QSqlDatabase::drivers().join(", ");
    return false;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Connexion Oracle AVANT MainWindow (le constructeur appelle refresh())
    QString err;
    if (!connectOracle(err)) {
        QMessageBox::critical(nullptr, "Connexion Oracle", err);
        return 1;
    }

    MainWindow w;
    w.show();
    return app.exec();
}