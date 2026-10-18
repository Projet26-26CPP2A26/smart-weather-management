#ifndef STATION_H
#define STATION_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Station
{
private:
    QString id_station;
    QString nom_station;
    QString type_station;
    QString gouvernorat;
    float altitude;
    QString statut;
    QDate date_installation;

public:
    // Constructeurs
    Station();
    Station(QString id, QString nom, QString type, QString gouv, float alt, QString st, QDate date);

    // Getters
    QString getId() { return id_station; }
    QString getNom() { return nom_station; }
    QString getType() { return type_station; }
    QString getGouvernorat() { return gouvernorat; }
    float getAltitude() { return altitude; }
    QString getStatut() { return statut; }
    QDate getDateInstallation() { return date_installation; }

    // Setters
    void setId(QString id) { id_station = id; }
    void setNom(QString nom) { nom_station = nom; }
    void setType(QString type) { type_station = type; }
    void setGouvernorat(QString gouv) { gouvernorat = gouv; }
    void setAltitude(float alt) { altitude = alt; }
    void setStatut(QString st) { statut = st; }
    void setDateInstallation(QDate date) { date_installation = date; }

    // Cree la table STATION si elle n'existe pas (SQLite ou Oracle selon le pilote)
    static bool creerTable();

    // Fonctions CRUD
    bool ajouter();
    QSqlQueryModel* afficher();
    bool modifier(QString id);
    bool supprimer(QString id);

    // Fonctions avancees (Recherche + Tri)
    // val = texte a chercher (vide = tout afficher)
    // critere = "Altitude" | "Type" | "Gouvernorat" (vide = pas de tri)
    QSqlQueryModel* rechercher(QString val, QString critere = "");
    QSqlQueryModel* trier(QString critere);
};

#endif // STATION_H