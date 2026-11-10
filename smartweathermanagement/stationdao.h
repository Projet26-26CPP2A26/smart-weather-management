#ifndef STATIONDAO_H
#define STATIONDAO_H

#include <QString>
#include <QDate>
#include <QMap>
#include <QList>
#include <QSqlQueryModel>

class Station;

// Ligne minimale utilisee par le Heartbeat
struct StationRow {
    QString id;
    QString statut;
    QDate   date;
};

// Classe d'acces aux donnees : TOUTES les requetes SQL de la table STATION sont ici.
// Fonctionne avec SQLite (app commune) et Oracle (QODBC).
class StationDao
{
public:
    static bool creerTable();

    // CRUD
    static bool inserer(Station &s);
    static bool mettreAJour(Station &s, const QString &id);
    static bool supprimer(const QString &id);

    // Lecture / recherche / tri (critere : "Altitude" | "Type" | "Gouvernorat" | "")
    static QSqlQueryModel* rechercher(const QString &val, const QString &critere);

    // Statistiques et metiers
    static QMap<QString,int> repartition(const QString &champ);      // "Type" | "Statut"
    static QMap<QString,int> activesParGouvernorat();
    static QList<StationRow> toutesLesStations();
    static bool changerStatut(const QString &id, const QString &statut);
};

#endif // STATIONDAO_H
