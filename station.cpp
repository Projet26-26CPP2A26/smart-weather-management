#include "station.h"
#include "dbcompat.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// Requete de base : memes colonnes, meme ordre, partout (afficher / rechercher / trier)
// La date est formatee en texte DD/MM/YYYY pour un affichage propre dans le tableau.
static QString selectBase()
{
    return "SELECT ID_STATION, NOM_STATION, TYPE_STATION, GOUVERNORAT, ALTITUDE, STATUT, "
           + DbCompat::dateExpr("DATE_INSTALLATION", "DD/MM/YYYY") + " AS DATE_INSTALLATION FROM STATION";
}

// En-tetes des colonnes du QTableView
static void appliquerEntetes(QSqlQueryModel *model)
{
    model->setHeaderData(0, Qt::Horizontal, "ID Station");
    model->setHeaderData(1, Qt::Horizontal, "Nom Station");
    model->setHeaderData(2, Qt::Horizontal, "Type");
    model->setHeaderData(3, Qt::Horizontal, "Gouvernorat");
    model->setHeaderData(4, Qt::Horizontal, "Altitude (m)");
    model->setHeaderData(5, Qt::Horizontal, "Statut");
    model->setHeaderData(6, Qt::Horizontal, "Date Installation");
}

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

// 1. AJOUTER (CREATE)
bool Station::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO STATION (ID_STATION, NOM_STATION, TYPE_STATION, GOUVERNORAT, ALTITUDE, STATUT, DATE_INSTALLATION) "
                  "VALUES (:id, :nom, :type, :gouv, :alt, :statut, :date)");

    query.bindValue(":id", id_station);
    query.bindValue(":nom", nom_station);
    query.bindValue(":type", type_station);
    query.bindValue(":gouv", gouvernorat);
    query.bindValue(":alt", altitude);
    query.bindValue(":statut", statut);
    query.bindValue(":date", date_installation);

    if (!query.exec()) {
        qDebug() << "Erreur Ajout SQL :" << query.lastError().text();
        return false;
    }
    return true;
}

// 2. AFFICHER (READ)
QSqlQueryModel* Station::afficher()
{
    return rechercher("", "");
}

// 3. MODIFIER (UPDATE)
bool Station::modifier(QString id)
{
    QSqlQuery query;
    query.prepare("UPDATE STATION SET NOM_STATION = :nom, TYPE_STATION = :type, GOUVERNORAT = :gouv, "
                  "ALTITUDE = :alt, STATUT = :statut, DATE_INSTALLATION = :date WHERE ID_STATION = :id");

    query.bindValue(":nom", nom_station);
    query.bindValue(":type", type_station);
    query.bindValue(":gouv", gouvernorat);
    query.bindValue(":alt", altitude);
    query.bindValue(":statut", statut);
    query.bindValue(":date", date_installation);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur Modifier SQL :" << query.lastError().text();
        return false;
    }
    // Si aucune ligne touchee => l'ID n'existe pas
    return query.numRowsAffected() > 0;
}

// 4. SUPPRIMER (DELETE)
bool Station::supprimer(QString id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM STATION WHERE ID_STATION = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur Supprimer SQL :" << query.lastError().text();
        return false;
    }
    // Si aucune ligne touchee => l'ID n'existe pas
    return query.numRowsAffected() > 0;
}

// 5. RECHERCHER (par nom, type ou gouvernorat) + tri optionnel
QSqlQueryModel* Station::rechercher(QString val, QString critere)
{
    QSqlQueryModel* model = new QSqlQueryModel();

    QString sql = selectBase();
    bool filtre = !val.trimmed().isEmpty();

    if (filtre) {
        sql += " WHERE UPPER(NOM_STATION) LIKE UPPER(:v1)"
               " OR UPPER(TYPE_STATION) LIKE UPPER(:v2)"
               " OR UPPER(GOUVERNORAT) LIKE UPPER(:v3)";
    }

    // Liste blanche : pas d'injection SQL possible
    if (critere == "Altitude")
        sql += " ORDER BY ALTITUDE ASC";
    else if (critere == "Type")
        sql += " ORDER BY TYPE_STATION ASC";
    else if (critere == "Gouvernorat")
        sql += " ORDER BY GOUVERNORAT ASC";

    QSqlQuery query;
    query.prepare(sql);
    if (filtre) {
        QString p = "%" + val.trimmed() + "%";
        query.bindValue(":v1", p);
        query.bindValue(":v2", p);
        query.bindValue(":v3", p);
    }

    if (!query.exec())
        qDebug() << "Erreur Recherche SQL :" << query.lastError().text();

    model->setQuery(std::move(query));
    appliquerEntetes(model);
    return model;
}

// 6. TRIER (Altitude / Type / Gouvernorat)
QSqlQueryModel* Station::trier(QString critere)
{
    return rechercher("", critere);
}
// 7. STATISTIQUES (GROUP BY)
QMap<QString,int> Station::repartition(const QString &champ)
{
    QMap<QString,int> res;

    // Liste blanche : pas d'injection SQL
    QString col;
    if (champ == "Type")        col = "TYPE_STATION";
    else if (champ == "Statut") col = "STATUT";
    else                        return res;

    QSqlQuery query;
    query.prepare("SELECT " + col + ", COUNT(*) FROM STATION GROUP BY " + col);
    if (!query.exec()) {
        qDebug() << "Erreur Stats SQL :" << query.lastError().text();
        return res;
    }
    while (query.next())
        res[query.value(0).toString()] = query.value(1).toInt();
    return res;
}
// 8. SUGGESTION D'EMPLACEMENT
QMap<QString,int> Station::stationsActivesParGouvernorat()
{
    QMap<QString,int> res;
    QSqlQuery query;
    query.prepare("SELECT GOUVERNORAT, COUNT(*) FROM STATION "
                  "WHERE STATUT = 'Active' GROUP BY GOUVERNORAT");
    if (!query.exec()) {
        qDebug() << "Erreur Suggestion SQL :" << query.lastError().text();
        return res;
    }
    while (query.next())
        res[query.value(0).toString()] = query.value(1).toInt();
    return res;
}
// 9. HEARTBEAT SCORE
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
    QSqlQuery q;
    q.prepare("SELECT ID_STATION, STATUT, "
              + DbCompat::dateExpr("DATE_INSTALLATION", "DD/MM/YYYY") + " FROM STATION");
    if (!q.exec()) {
        qDebug() << "Erreur Heartbeat SQL :" << q.lastError().text();
        return 0;
    }
    QStringList aMettre;
    while (q.next()) {
        QString id = q.value(0).toString();
        QString st = q.value(1).toString();
        QDate d = QDate::fromString(q.value(2).toString(), "dd/MM/yyyy");
        if (st != "En maintenance" && calculerScore(st, d) < seuil)
            aMettre << id;
    }
    for (const QString &id : aMettre) {
        QSqlQuery u;
        u.prepare("UPDATE STATION SET STATUT = 'En maintenance' WHERE ID_STATION = :id");
        u.bindValue(":id", id);
        if (u.exec()) ++n;
    }
    return n;
}