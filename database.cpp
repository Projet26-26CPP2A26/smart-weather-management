#include "database.h"

#include <QCoreApplication>
#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

// ---------------------------------------------------------------------------
//  Base locale SQLite : schema + donnees de demonstration.
//  Utilisee quand Oracle (QOCI) est indisponible, pour que tous les boutons
//  de l'application restent fonctionnels hors du poste Oracle.
// ---------------------------------------------------------------------------
static void executer(const QString &sql)
{
    QSqlQuery q;
    if (!q.exec(sql))
        qWarning() << "Base locale :" << q.lastError().text() << "\nSQL:" << sql;
}

static void creerBaseLocale()
{
    executer("CREATE TABLE IF NOT EXISTS station ("
             "id_station INTEGER PRIMARY KEY, nom TEXT NOT NULL, gouvernorat TEXT, type_station TEXT)");
    executer("CREATE TABLE IF NOT EXISTS employe ("
             "id_employe INTEGER PRIMARY KEY, nom TEXT NOT NULL, prenom TEXT NOT NULL, fonction TEXT)");
    executer("CREATE TABLE IF NOT EXISTS heartbeat ("
             "id_station INTEGER, score REAL, date_mesure TEXT)");
    executer("CREATE TABLE IF NOT EXISTS intervention ("
             "id_intervention INTEGER PRIMARY KEY, id_station INTEGER, id_employe INTEGER, "
             "type_intervention TEXT, priorite TEXT, date_creation TEXT, date_planifiee TEXT, "
             "date_cloture TEXT, statut TEXT, cout REAL, rapport_intervention TEXT, "
             "archive INTEGER NOT NULL DEFAULT 0)");

    QSqlQuery compte("SELECT COUNT(*) FROM intervention");
    if (compte.next() && compte.value(0).toInt() > 0)
        return;

    QSqlQuery s;
    s.prepare("INSERT INTO station (id_station, nom, gouvernorat, type_station) VALUES (?,?,?,?)");
    const struct { int id; const char *nom, *gouv, *type; } stations[] = {
        {1, "Tunis-Carthage",  "Tunis",    "Aéronautique"},
        {2, "Sfax-Thyna",      "Sfax",     "Aéronautique"},
        {3, "Bizerte Port",    "Bizerte",  "Marine"},
        {4, "Kairouan Centre", "Kairouan", "Météo"},
    };
    for (const auto &st : stations) {
        s.addBindValue(st.id);
        s.addBindValue(QString::fromUtf8(st.nom));
        s.addBindValue(QString::fromUtf8(st.gouv));
        s.addBindValue(QString::fromUtf8(st.type));
        s.exec();
    }

    QSqlQuery e;
    e.prepare("INSERT INTO employe (id_employe, nom, prenom, fonction) VALUES (?,?,?,?)");
    const struct { int id; const char *nom, *prenom, *fn; } employes[] = {
        {1, "Ben Salah", "Ahmed", "Technicien de maintenance"},
        {2, "Trabelsi",  "Sana",  "Technicien de maintenance"},
    };
    for (const auto &em : employes) {
        e.addBindValue(em.id);
        e.addBindValue(QString::fromUtf8(em.nom));
        e.addBindValue(QString::fromUtf8(em.prenom));
        e.addBindValue(QString::fromUtf8(em.fn));
        e.exec();
    }

    QSqlQuery h;
    h.prepare("INSERT INTO heartbeat (id_station, score, date_mesure) VALUES (?,?,?)");
    const struct { int id; double score; } beats[] = {{1, 92}, {2, 38}, {3, 57}, {4, 68}};
    for (const auto &b : beats) {
        h.addBindValue(b.id);
        h.addBindValue(b.score);
        h.addBindValue(QDate::currentDate());
        h.exec();
    }

    QSqlQuery i;
    i.prepare("INSERT INTO intervention (id_intervention, id_station, id_employe, type_intervention, "
              "priorite, date_creation, date_planifiee, date_cloture, statut, cout, "
              "rapport_intervention, archive) VALUES (?,?,?,?,?,?,?,?,?,?,?,0)");
    const QDateTime maintenant = QDateTime::currentDateTime();
    struct I {
        int id, station, employe, creation, planifiee, cloture;
        const char *type, *prio, *statut, *rapport;
        double cout;
    };
    const I interventions[] = {
        {1, 1, 1,  4,  -2,  999, "Préventive",          "Haute",   "En cours",  nullptr, 450.00},
        {2, 3, 2,  1,  -1,  999, "Corrective",          "Urgente", "En cours",  nullptr, 1200.00},
        {3, 2, 1, 20, -18, -17, "Calibration",          "Moyenne", "Terminée", "Capteur de pression recalibré.", 300.00},
        {4, 4, 2, 30, -25, -24, "Remplacement capteur", "Basse",   "Terminée", "Anémomètre remplacé.", 780.00},
        {5, 4, 1,  2,   3,  999, "Préventive",          "Moyenne", "Planifiée", nullptr, 250.00},
        {6, 2, 2, 10,  10,  999, "Préventive",          "Basse",   "Planifiée", nullptr, 600.00},
    };
    for (const auto &iv : interventions) {
        i.addBindValue(iv.id);
        i.addBindValue(iv.station);
        i.addBindValue(iv.employe);
        i.addBindValue(QString::fromUtf8(iv.type));
        i.addBindValue(QString::fromUtf8(iv.prio));
        i.addBindValue(maintenant.addDays(-iv.creation));
        i.addBindValue(maintenant.addDays(iv.planifiee).date());
        if (iv.cloture == 999)
            i.addBindValue(QVariant(QMetaType::fromType<QDateTime>()));
        else
            i.addBindValue(maintenant.addDays(iv.cloture));
        i.addBindValue(QString::fromUtf8(iv.statut));
        i.addBindValue(iv.cout);
        if (iv.rapport)
            i.addBindValue(QString::fromUtf8(iv.rapport));
        else
            i.addBindValue(QVariant(QMetaType::fromType<QString>()));
        i.exec();
    }
}

bool Database::isSqlite()
{
    const QSqlDatabase db = QSqlDatabase::database();
    return db.isValid() && db.driverName().compare(QLatin1String("QSQLITE"), Qt::CaseInsensitive) == 0;
}

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

    // 1) Oracle via le client OCI (poste avec la base de l'ecole).
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QOCI");
        db.setHostName(host);
        db.setPort(port);
        db.setDatabaseName(service);
        db.setUserName(user);
        db.setPassword(pass);

        if (db.open())
            return true;

        qWarning() << "Connexion Oracle indisponible :" << db.lastError().text();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }

    // 2) Repli sur une base SQLite locale (a cote de l'executable) pour que
    //    toutes les fonctionnalites restent utilisables sans serveur Oracle.
    QSqlDatabase lite = QSqlDatabase::addDatabase("QSQLITE");
    const QString chemin = QDir(QCoreApplication::applicationDirPath()).filePath("maintenance.db");
    lite.setDatabaseName(chemin);
    if (!lite.open()) {
        if (error) *error = lite.lastError().text();
        return false;
    }
    creerBaseLocale();
    qInfo() << "Base locale utilisee :" << chemin;
    return true;
}
