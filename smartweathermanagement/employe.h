#ifndef EMPLOYE_H
#define EMPLOYE_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QImage>

class Employe {
private:
    QString id_employe;
    QString nom;
    QString prenom;
    QString email;
    QString poste;
    QString structure;
    QString gouvernorat;
    QDate date_recrutement;
    QString statut;
    QString type_horaire;

public:
    Employe();
    Employe(QString id, QString nom, QString prenom, QString email, QString poste,
            QString structure, QString gouvernorat, QDate dateRecrutement, QString statut, QString horaire);

    // Getters
    QString getId() const { return id_employe; }
    QString getNom() const { return nom; }
    QString getPrenom() const { return prenom; }
    QString getEmail() const { return email; }
    QString getPoste() const { return poste; }
    QString getStructure() const { return structure; }
    QString getGouvernorat() const { return gouvernorat; }
    QDate getDateRecrutement() const { return date_recrutement; }
    QString getStatut() const { return statut; }
    QString getTypeHoraire() const { return type_horaire; }

    // Setters
    void setId(QString id) { id_employe = id; }
    void setNom(QString n) { nom = n; }
    void setPrenom(QString p) { prenom = p; }
    void setEmail(QString e) { email = e; }
    void setPoste(QString po) { poste = po; }
    void setStructure(QString s) { structure = s; }
    void setGouvernorat(QString g) { gouvernorat = g; }
    void setDateRecrutement(QDate d) { date_recrutement = d; }
    void setStatut(QString st) { statut = st; }
    void setTypeHoraire(QString th) { type_horaire = th; }

    // Méthodes CRUD
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(QString id);
    bool modifier();
    static bool archiver(QString id);

    // Méthodes Métier (Exports & Rapports)
    static bool exporterListePDF(const QString &cheminFichier, const QString &editeur = "Responsable RH");
    static bool exporterFichePDF(const QString &id, const QString &cheminFichier, const QString &editeur = "Responsable RH");
    static bool exporterAttestationPDF(const QString &id, const QString &cheminFichier, const QString &editeur = "Direction des Ressources Humaines");
    static bool exporterStatsPDF(const QString &cheminFichier, const QString &editeur = "Direction des Ressources Humaines");

    // Méthodes Métier (Badge Sécurisé & QR Code)
    static QImage genererQrCode(const QString &texte, int taille = 240);
    static QImage genererBadgeImage(const QString &id);
    static bool exporterBadgePDF(const QString &id, const QString &cheminFichier);

    // Méthodes Métier (Recherche & Tri Avancés)
    static QSqlQueryModel* trier(const QString &critere, const QString &ordre = "ASC");
    static QSqlQueryModel* rechercher(const QString &texte, const QString &critere = "Nom", const QString &tri = "Nom", const QString &ordre = "ASC");
    static bool existe(const QString &id);

    // Méthodes Métier (Journal d'audit & Traçabilité RH)
    static bool logAction(const QString &action, const QString &details, const QString &utilisateur = "admin");
    static QSqlQueryModel* afficherLogs();
};

#endif // EMPLOYE_H