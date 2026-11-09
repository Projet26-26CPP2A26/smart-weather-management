#ifndef SMARTWEATHER_H
#define SMARTWEATHER_H

#include <QList>
#include <QWidget>

class QLineEdit;
class QComboBox;
class QDateEdit;
class QTableView;
class QSqlQueryModel;
class QStackedWidget;
class QLabel;
class QPushButton;
class SwRing;
class SwLineChart;
class SwBarChart;
class ModuleAlertesWidget;

class smartweather : public QWidget
{
    Q_OBJECT
public:
    explicit smartweather(QWidget *parent = nullptr);

private slots:

    void on_btnAjouter_clicked();
    void on_btnModifier_clicked();
    void on_btnSupprimer_clicked();
    void seConnecter();
    void deconnecter();
    void allerPage(int index);
    void rechercheGlobale();
    void rafraichirDashboard();

    void ajouter();
    void modifier();
    void supprimer();
    void archiverEmploye();
    void charger();
    void exporterCsv();
    void exporterPdf();
    void afficherStats();
    void planifierVeille24h();
    void affecterTechnicien();
    void remplirFormulaire(const QModelIndex &index);
    void viderFormulaire();

private:
    bool initDb();
    void initUtilisateurs();
    void initEmployes();
    static QString hasher(const QString &motDePasse, const QString &sel);
    void appliquerStyle();

    QWidget *buildLoginPage();
    QWidget *buildShell();
    QWidget *buildSidebar();
    QWidget *buildHeader();
    QWidget *buildDashboardPage();
    QWidget *buildEmployesPage();
    QWidget *buildPlaceholder(const QString &nom);

    bool formulaireValide();
    void erreur(const QString &msg);

    QStackedWidget *m_root;
    QStackedWidget *m_pages;
    QLineEdit *m_loginUser, *m_loginPass;
    QLabel *m_loginErreur;
    QString m_user, m_role;

    QList<QPushButton *> m_nav;
    QPushButton *m_navEmployes;
    QLabel *m_titrePage, *m_avatar, *m_userNom, *m_userRole;
    QLineEdit *m_rechercheGlobale;

    QLabel *m_kpiTotal, *m_kpiTotalSous, *m_activites;
    SwRing *m_ringActifs, *m_ringGarde, *m_ringConge, *m_ringSiege, *m_ringRegionale;
    SwLineChart *m_courbe;
    SwBarChart *m_barres;

    QLineEdit *m_id, *m_nom, *m_prenom, *m_email, *m_gouvernorat;
    QComboBox *m_poste, *m_structure, *m_statut, *m_horaire;
    QDateEdit *m_date;
    QComboBox *m_critere, *m_tri;
    QLineEdit *m_recherche;
    QTableView *m_table;
    QSqlQueryModel *m_model;
    QString m_idCourant;
    ModuleAlertesWidget *m_moduleAlertes = nullptr;
};

#endif