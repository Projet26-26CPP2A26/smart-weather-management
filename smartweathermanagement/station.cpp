#include "station.h"
#include "stationdao.h"

// Constructeur par defaut
Station::Station()
{
    id_station = "";
    nom_station = "";
    type_station = "";
    gouvernorat = "";
    altitude = 0.0;
    statut = "Active";
    date_installation = QDate::currentDate();
}

// Constructeur parametre
Station::Station(QString id, QString nom, QString type, QString gouv, float alt, QString st, QDate date)
{
    this->id_station = id;
    this->nom_station = nom;
    this->type_station = type;
    this->gouvernorat = gouv;
    this->altitude = alt;
    this->statut = st;
    this->date_installation = date;
}

// ---- Toutes les requetes SQL sont dans StationDao : ici, simples appels ----

bool Station::creerTable()                      { return StationDao::creerTable(); }
bool Station::ajouter()                         { return StationDao::inserer(*this); }
QSqlQueryModel* Station::afficher()             { return StationDao::rechercher("", ""); }
bool Station::modifier(QString id)              { return StationDao::mettreAJour(*this, id); }
bool Station::supprimer(QString id)             { return StationDao::supprimer(id); }
QSqlQueryModel* Station::rechercher(QString val, QString critere)
                                                { return StationDao::rechercher(val, critere); }
QSqlQueryModel* Station::trier(QString critere) { return StationDao::rechercher("", critere); }
QMap<QString,int> Station::repartition(const QString &champ)
                                                { return StationDao::repartition(champ); }
QMap<QString,int> Station::stationsActivesParGouvernorat()
                                                { return StationDao::activesParGouvernorat(); }

// Heartbeat Score (logique metier, sans SQL)
int Station::calculerScore(const QString &statut, const QDate &dateInstall)
{
    int pts = 0;
    if (statut == "Active")              pts = 60;
    else if (statut == "En maintenance") pts = 30;
    else                                  pts = 0;

    double annees = dateInstall.daysTo(QDate::currentDate()) / 365.0;
    int anciennete = 40 - int(annees) * 10;
    if (anciennete < 0)  anciennete = 0;
    if (anciennete > 40) anciennete = 40;

    return pts + anciennete;
}

int Station::appliquerMaintenance(int seuil)
{
    int n = 0;
    const QList<StationRow> toutes = StationDao::toutesLesStations();
    for (const StationRow &r : toutes) {
        if (r.statut != "En maintenance" && calculerScore(r.statut, r.date) < seuil)
            if (StationDao::changerStatut(r.id, "En maintenance"))
                ++n;
    }
    return n;
}
