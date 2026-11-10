#include "localdb.h"

#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QtMath>

static bool executer(const QString &sql)
{
    QSqlQuery q;
    if (!q.exec(sql)) {
        qWarning() << "LocalDb:" << q.lastError().text() << "\nSQL:" << sql;
        return false;
    }
    return true;
}

static void creerTables()
{
    executer("CREATE TABLE IF NOT EXISTS utilisateur ("
             "login TEXT PRIMARY KEY, mot_de_passe TEXT NOT NULL, sel TEXT NOT NULL, role TEXT NOT NULL)");

    executer("CREATE TABLE IF NOT EXISTS employe ("
             "id_employe TEXT PRIMARY KEY, nom TEXT, prenom TEXT, email TEXT, poste TEXT, "
             "structure TEXT, gouvernorat TEXT, date_recrutement TEXT, statut TEXT, type_horaire TEXT)");

    // NOM et NOM_STATION coexistent : les modules historiques n'utilisent pas la meme colonne.
    executer("CREATE TABLE IF NOT EXISTS station ("
             "id_station TEXT PRIMARY KEY, nom_station TEXT, nom TEXT, type_station TEXT, "
             "gouvernorat TEXT, altitude REAL, statut TEXT, date_installation TEXT, "
             "latitude REAL, longitude REAL)");

    executer("CREATE TABLE IF NOT EXISTS releve_meteo ("
             "id_releve INTEGER PRIMARY KEY, id_station TEXT, horodatage TEXT, temperature REAL, "
             "humidite REAL, pression REAL, vitesse_vent REAL, precipitation REAL, mode_collecte TEXT)");

    executer("CREATE TABLE IF NOT EXISTS alertes ("
             "id_alerte TEXT PRIMARY KEY, type_alerte TEXT, niveau_severite TEXT, date_emission TEXT, "
             "date_fin TEXT, id_station TEXT, gouvernorat TEXT, statut TEXT, description TEXT)");

    executer("CREATE TABLE IF NOT EXISTS intervention ("
             "id_intervention INTEGER PRIMARY KEY, id_station TEXT, id_employe TEXT, "
             "type_intervention TEXT, priorite TEXT, date_creation TEXT, date_planifiee TEXT, "
             "date_cloture TEXT, statut TEXT, cout REAL, rapport_intervention TEXT, "
             "archive INTEGER NOT NULL DEFAULT 0)");

    executer("CREATE TABLE IF NOT EXISTS attestation ("
             "id_attestation INTEGER PRIMARY KEY, numero TEXT, demandeur TEXT, lieu TEXT, "
             "latitude REAL, longitude REAL, date_debut TEXT, date_fin TEXT, id_station TEXT, "
             "statut TEXT, date_demande TEXT, date_traitement TEXT)");
}

static void injecterEmployes()
{
    struct E { const char *id, *nom, *prenom, *poste, *structure, *gouv, *statut, *horaire; int annee, mois, jour; };
    const E employes[] = {
        {"101", "Ben Salah", "Ali",      "Ingénieur prévisionniste", "Siège",     "Tunis",     "Actif",     "Jour",      2015, 3, 12},
        {"102", "Trabelsi",  "Salma",    "Agent d'observation",      "Régionale", "Sousse",    "Actif",     "Garde 24h", 2018, 9, 3},
        {"103", "Khelifi",   "Mohamed",  "Technicien de maintenance","Régionale", "Sfax",      "Actif",     "Jour",      2016, 1, 20},
        {"104", "Bouazizi",  "Ines",     "Ingénieur prévisionniste", "Régionale", "Nabeul",    "Actif",     "Garde 24h", 2020, 6, 15},
        {"105", "Gharbi",    "Youssef",  "Technicien de maintenance","Siège",     "Tunis",     "Actif",     "Jour",      2019, 11, 5},
        {"106", "Jebali",    "Amira",    "Agent d'observation",      "Régionale", "Bizerte",   "En congé",  "Garde 24h", 2021, 2, 18},
        {"107", "Ayari",     "Karim",    "Agent d'observation",      "Régionale", "Gabès",     "Actif",     "Garde 24h", 2017, 7, 30},
        {"108", "Chaabane",  "Nadia",    "Administratif",            "Siège",     "Ariana",    "Actif",     "Jour",      2012, 4, 2},
        {"109", "Zouari",    "Hatem",    "Technicien de maintenance","Régionale", "Kairouan",  "Actif",     "Jour",      2022, 10, 10},
        {"110", "Mzoughi",   "Rania",    "Agent d'observation",      "Siège",     "Tunis",     "Stagiaire", "Jour",      2026, 1, 5},
        {"111", "Ben Amor",  "Sami",     "Agent de service",         "Siège",     "Ben Arous", "Actif",     "Jour",      2014, 8, 21},
        {"112", "Ferchichi", "Leila",    "Ingénieur prévisionniste", "Régionale", "Médenine",  "Actif",     "Garde 24h", 2023, 5, 9},
    };

    QSqlQuery q;
    q.prepare("INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, "
              "date_recrutement, statut, type_horaire) VALUES (?,?,?,?,?,?,?,?,?,?)");
    for (const E &e : employes) {
        q.addBindValue(QString::fromUtf8(e.id));
        q.addBindValue(QString::fromUtf8(e.nom));
        q.addBindValue(QString::fromUtf8(e.prenom));
        q.addBindValue(QString::fromUtf8(e.prenom) + "." + QString::fromUtf8(e.nom).toLower() + "@inm.tn");
        q.addBindValue(QString::fromUtf8(e.poste));
        q.addBindValue(QString::fromUtf8(e.structure));
        q.addBindValue(QString::fromUtf8(e.gouv));
        q.addBindValue(QDate(e.annee, e.mois, e.jour));
        q.addBindValue(QString::fromUtf8(e.statut));
        q.addBindValue(QString::fromUtf8(e.horaire));
        q.exec();
    }
}

static void injecterStations()
{
    struct S { const char *id, *nom, *type, *gouv, *statut; double altitude, lat, lon; int a, m, j; };
    const S stations[] = {
        {"1", "Tunis-Carthage", "Aéronautique",   "Tunis",    "Active",        4,  36.8510, 10.2270, 1998, 1, 15},
        {"2", "Sousse",         "Synoptique",     "Sousse",   "Active",       25,  35.8256, 10.6084, 2003, 5, 20},
        {"3", "Sfax",           "Synoptique",     "Sfax",     "Active",       22,  34.7406, 10.7603, 2005, 9, 12},
        {"4", "Bizerte",        "Climatologique", "Bizerte",  "En maintenance", 12, 37.2744, 9.8739, 2010, 3, 8},
        {"5", "Gabès",          "Climatologique", "Gabès",    "Active",        8,  33.8815, 10.0982, 2012, 11, 1},
        {"6", "Kairouan",       "Radar",          "Kairouan", "Active",       62,  35.6781, 10.0963, 2008, 6, 25},
    };

    QSqlQuery q;
    q.prepare("INSERT INTO station (id_station, nom_station, nom, type_station, gouvernorat, altitude, "
              "statut, date_installation, latitude, longitude) VALUES (?,?,?,?,?,?,?,?,?,?)");
    for (const S &s : stations) {
        const QString nom = QString::fromUtf8(s.nom);
        q.addBindValue(QString::fromUtf8(s.id));
        q.addBindValue(nom);
        q.addBindValue(nom);
        q.addBindValue(QString::fromUtf8(s.type));
        q.addBindValue(QString::fromUtf8(s.gouv));
        q.addBindValue(s.altitude);
        q.addBindValue(QString::fromUtf8(s.statut));
        q.addBindValue(QDate(s.a, s.m, s.j));
        q.addBindValue(s.lat);
        q.addBindValue(s.lon);
        q.exec();
    }
}

static void injecterReleves()
{
    QSqlQuery q;
    q.prepare("INSERT INTO releve_meteo (id_releve, id_station, horodatage, temperature, humidite, "
              "pression, vitesse_vent, precipitation, mode_collecte) VALUES (?,?,?,?,?,?,?,?,?)");
    const QDateTime maintenant = QDateTime::currentDateTime();
    int id = 1;
    for (int jour = 45; jour >= 0; --jour) {
        for (int h = 0; h < 24; h += 8) {
            for (int st = 1; st <= 3; ++st) {
                const QDateTime horo = maintenant.addDays(-jour).addSecs(h * 3600);
                const double base = 18.0 + st * 1.3 + 6.0 * qSin((45 - jour) / 7.0);
                const double temp = base + qCos((45 - jour) + h) * 2.5;
                const double humid = 55.0 + 12.0 * qSin((45 - jour) / 4.0 + st);
                const double pression = 1013.0 + 4.0 * qCos((45 - jour) / 5.0);
                const double vent = 8.0 + qAbs(qSin((45 - jour) + st)) * 14.0;
                const double precip = ((45 - jour) % 9 == 0 && h == 8) ? 3.4 : 0.0;

                q.addBindValue(id++);
                q.addBindValue(QString::number(st));
                q.addBindValue(horo);
                q.addBindValue(temp);
                q.addBindValue(humid);
                q.addBindValue(pression);
                q.addBindValue(vent);
                q.addBindValue(precip);
                q.addBindValue(QStringLiteral("Automatique"));
                q.exec();
            }
        }
    }
}

static void injecterAlertes()
{
    struct A { const char *id, *type, *sev, *gouv, *statut, *station, *desc; int debut, fin; };
    const A alertes[] = {
        {"A-2026-001", "Vents forts",        "Orange", "Tunis",   "En cours",  "1", "Rafales de 90 km/h attendues sur le littoral nord.", 1, 2},
        {"A-2026-002", "Pluies intenses",    "Rouge",  "Bizerte", "En cours",  "4", "Cumuls supérieurs à 60 mm en 24 h, risque d'inondation.", 0, 3},
        {"A-2026-003", "Brouillard dense",   "Jaune",  "Sousse",  "En cours",  "2", "Visibilité inférieure à 200 m entre 04 h et 09 h.", 2, 1},
        {"A-2026-004", "Chaleur",            "Orange", "Sfax",    "Archivée",  "3", "Températures maximales proches de 44 °C.", 12, 9},
        {"A-2026-005", "Orages",             "Jaune",  "Kairouan","Terminée",  "6", "Orages isolés avec fortes rafales.", 20, 19},
        {"A-2026-006", "Vents de sable",     "Orange", "Gabès",   "Archivée",  "5", "Réduction de visibilité liée au sable soulevé.", 30, 28},
    };

    QSqlQuery q;
    q.prepare("INSERT INTO alertes (id_alerte, type_alerte, niveau_severite, date_emission, date_fin, "
              "id_station, gouvernorat, statut, description) VALUES (?,?,?,?,?,?,?,?,?)");
    const QDateTime maintenant = QDateTime::currentDateTime();
    for (const A &a : alertes) {
        q.addBindValue(QString::fromUtf8(a.id));
        q.addBindValue(QString::fromUtf8(a.type));
        q.addBindValue(QString::fromUtf8(a.sev));
        q.addBindValue(maintenant.addDays(-a.debut));
        q.addBindValue(maintenant.addDays(a.fin));
        q.addBindValue(QString::fromUtf8(a.station));
        q.addBindValue(QString::fromUtf8(a.gouv));
        q.addBindValue(QString::fromUtf8(a.statut));
        q.addBindValue(QString::fromUtf8(a.desc));
        q.exec();
    }
}

static void injecterInterventions()
{
    struct I { int id; const char *station, *employe, *type, *prio, *statut, *rapport; int creation, planifiee, cloture; double cout; };
    const I interventions[] = {
        {1, "1", "103", "Préventive",            "Haute",   "En cours", "",         4,  -2,  999, 450.00},
        {2, "4", "105", "Corrective",            "Urgente", "En cours", "",         6,  -5,  999, 1200.00},
        {3, "2", "109", "Calibration",           "Moyenne", "Terminée", "Capteur de pression recalibré.", 20, -18, -17, 300.00},
        {4, "3", "103", "Remplacement capteur",  "Basse",   "Terminée", "Anémomètre remplacé.", 30, -25, -24, 780.00},
        {5, "5", "105", "Préventive",            "Moyenne", "Planifiée","",         2,   3,  999, 250.00},
    };

    QSqlQuery q;
    q.prepare("INSERT INTO intervention (id_intervention, id_station, id_employe, type_intervention, "
              "priorite, date_creation, date_planifiee, date_cloture, statut, cout, rapport_intervention, archive) "
              "VALUES (?,?,?,?,?,?,?,?,?,?,?,0)");
    const QDateTime maintenant = QDateTime::currentDateTime();
    for (const I &i : interventions) {
        q.addBindValue(i.id);
        q.addBindValue(QString::fromUtf8(i.station));
        q.addBindValue(QString::fromUtf8(i.employe));
        q.addBindValue(QString::fromUtf8(i.type));
        q.addBindValue(QString::fromUtf8(i.prio));
        q.addBindValue(maintenant.addDays(-i.creation));
        q.addBindValue(maintenant.addDays(i.planifiee));
        if (i.cloture == 999)
            q.addBindValue(QVariant(QMetaType::fromType<QDateTime>()));
        else
            q.addBindValue(maintenant.addDays(i.cloture));
        q.addBindValue(QString::fromUtf8(i.statut));
        q.addBindValue(i.cout);
        q.addBindValue(QString::fromUtf8(i.rapport));
        q.exec();
    }
}

bool LocalDb::prepare()
{
    creerTables();

    QSqlQuery compte("SELECT COUNT(*) FROM employe");
    if (compte.next() && compte.value(0).toInt() > 0)
        return true;

    injecterEmployes();
    injecterStations();
    injecterReleves();
    injecterAlertes();
    injecterInterventions();
    return true;
}
