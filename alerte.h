#ifndef ALERTE_H
#define ALERTE_H

#include <QString>
#include <QDateTime>
#include <QSqlQueryModel>
#include <QMap>

class Alerte
{
public:
    // Constructeurs
    Alerte();
    Alerte(const QString &id, const QString &type, const QString &severite,
           const QDateTime &emission, const QDateTime &fin,
           const QString &station, const QString &gouv,
           const QString &statut = "En cours", const QString &desc = "");

    // Getters
    QString getIdAlerte() const { return idAlerte; }
    QString getTypeAlerte() const { return typeAlerte; }
    QString getNiveauSeverite() const { return niveauSeverite; }
    QDateTime getDateEmission() const { return dateEmission; }
    QDateTime getDateFin() const { return dateFin; }
    QString getIdStation() const { return idStation; }
    QString getGouvernorat() const { return gouvernorat; }
    QString getStatut() const { return statut; }
    QString getDescription() const { return description; }

    // Setters
    void setIdAlerte(const QString &id) { idAlerte = id; }
    void setTypeAlerte(const QString &type) { typeAlerte = type; }
    void setNiveauSeverite(const QString &sev) { niveauSeverite = sev; }
    void setDateEmission(const QDateTime &dt) { dateEmission = dt; }
    void setDateFin(const QDateTime &dt) { dateFin = dt; }
    void setIdStation(const QString &st) { idStation = st; }
    void setGouvernorat(const QString &g) { gouvernorat = g; }
    void setStatut(const QString &st) { statut = st; }
    void setDescription(const QString &d) { description = d; }

    // Fonctionnalités CRUD
    bool ajouter();
    bool modifier();
    static bool mettreAJourSeverite(const QString &id, const QString &nouveauNiveau);
    static bool supprimer(const QString &id);
    static bool archiver(const QString &id);
    static bool existe(const QString &id);

    // Fonctionnalités de recherche, affichage et tri
    static QSqlQueryModel* afficher();
    static QSqlQueryModel* rechercher(const QString &terme, 
                                      const QString &typeFiltre = "Tous", 
                                      const QString &severiteFiltre = "Tous", 
                                      const QString &statutFiltre = "Tous");
    static QSqlQueryModel* trier(const QString &critere, const QString &ordre = "ASC");

    // Fonctionnalités d'exportation
    static bool exporterTXT(const QString &cheminFichier);
    static bool exporterPDF(const QString &cheminFichier, const QString &utilisateur = "Responsable");

    // Statistiques pour graphiques
    static QMap<QString, int> statistiquesParGouvernorat(int annee = 2026);
    static QMap<QString, int> statistiquesParSeverite();
    static QMap<QString, int> statistiquesParType();
    static int compterAlertesEnCours();
    static int compterAlertesCritiques();

private:
    QString idAlerte;
    QString typeAlerte;      // Gel, Sirocco/Canicule, Tempête, Vent fort
    QString niveauSeverite;  // Jaune, Orange, Rouge
    QDateTime dateEmission;
    QDateTime dateFin;
    QString idStation;
    QString gouvernorat;
    QString statut;          // En cours, Expirée, Archivée
    QString description;
};

#endif // ALERTE_H
