#include "station.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// Requete de base : memes colonnes, meme ordre, partout (afficher / rechercher / trier)
// La date est formatee en texte DD/MM/YYYY pour un affichage propre dans le tableau.
static const QString SELECT_BASE =
    "SELECT ID_STATION, NOM_STATION, TYPE_STATION, GOUVERNORAT, ALTITUDE, STATUT, "
    "TO_CHAR(DATE_INSTALLATION, 'DD/MM/YYYY') AS DATE_INSTALLATION FROM STATION";

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

    QString sql = SELECT_BASE;
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