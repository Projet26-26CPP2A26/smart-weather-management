#ifndef EQUIPEMENT_H
#define EQUIPEMENT_H

#include <QString>
#include <QDate>
#include <QSqlQueryModel>
#include <QMap>

// Couche metier : toute la logique SQL de la table EQUIPEMENT est ICI,
// jamais derriere un bouton (architecture Modele-Vue de l'atelier CRUD).
class Equipement
{
private:
    int     id_equipement;
    QString nom_equipement;
    QString type_equipement;
    QString marque;
    QString numero_serie;
    QDate   date_installation;
    QString statut;
    QString id_station;

public:
    Equipement();
    Equipement(int id, const QString &nom, const QString &type, const QString &marque,
               const QString &serie, const QDate &date, const QString &statut, const QString &idStation);

    // Controle de saisie : renvoie un message d'erreur, ou "" si tout est valide
    QString verifier() const;

    // CRUD
    bool ajouter();                 // Create (l'ID vient de SEQ_EQUIPEMENT)
    static QSqlQueryModel *afficher(const QString &recherche = "", const QString &tri = ""); // Read
    bool modifier();                // Update (utilise id_equipement)
    static bool supprimer(int id);  // Delete

    // Metiers
    // Statistiques : nombre d'equipements par "Statut" ou par "Type"
    static QMap<QString, int> repartition(const QString &champ);
    // Export PDF de la liste (avec la recherche et le tri courants)
    static bool exporterPdf(const QString &fichier, const QString &recherche = "", const QString &tri = "");

    // Dernier message d'erreur Oracle, pour l'affichage dans la fenetre
    static QString derniereErreur();
};

#endif // EQUIPEMENT_H
