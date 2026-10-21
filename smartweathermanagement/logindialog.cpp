#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>

/* ===================================================================
   BOÎTE DE DIALOGUE DE CONNEXION ET SÉLECTION DE RÔLE (DAYLIGHT)
   ===================================================================
   NOTE PÉDAGOGIQUE POUR ÉTUDIANTS EN INFORMATIQUE :
   - Cette boîte de dialogue permet d'authentifier l'utilisateur et de
     sélectionner son rôle métier (Contrôle d'accès RBAC).
   - Les boutons pré-remplissent les identifiants pour faciliter les tests.
   - Les styles appliqués utilisent le thème "Daylight Clear" :
     fonds blancs, bordures claires et mise en valeur active colorée.
   =================================================================== */

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    currentRole("Responsable de la vigilance")
{
    ui->setupUi(this);
    setFixedSize(540, 520);

    // Initialisation avec le rôle par défaut (Responsable de la vigilance)
    on_btnRoleVigilance_clicked();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QString LoginDialog::getSelectedRole() const
{
    return currentRole;
}

QString LoginDialog::getUsername() const
{
    return ui->txtUsername->text().trimmed();
}

void LoginDialog::on_btnRoleVigilance_clicked()
{
    currentRole = "Responsable de la vigilance";
    ui->txtUsername->setText("vigilance_admin");
    ui->txtPassword->setText("admin123");

    // Style actif en mode clair diurne (bleu ciel + bordure azur)
    ui->btnRoleVigilance->setStyleSheet(
        "background-color: #f0f9ff; color: #0284c7; border: 2px solid #0284c7; "
        "text-align: left; padding: 12px; font-weight: bold; border-radius: 8px;");
    ui->btnRoleProtection->setStyleSheet(
        "background-color: #ffffff; color: #ea580c; border: 1px solid #fed7aa; "
        "text-align: left; padding: 12px; border-radius: 8px;");
    ui->btnRoleDecideur->setStyleSheet(
        "background-color: #ffffff; color: #9333ea; border: 1px solid #e9d5ff; "
        "text-align: left; padding: 12px; border-radius: 8px;");
}

void LoginDialog::on_btnRoleProtection_clicked()
{
    currentRole = "Protection civile";
    ui->txtUsername->setText("protection_civile_agent");
    ui->txtPassword->setText("protect123");

    // Style actif en mode clair diurne (orange doux + bordure ambrée)
    ui->btnRoleVigilance->setStyleSheet(
        "background-color: #ffffff; color: #0284c7; border: 1px solid #bae6fd; "
        "text-align: left; padding: 12px; border-radius: 8px;");
    ui->btnRoleProtection->setStyleSheet(
        "background-color: #fff7ed; color: #ea580c; border: 2px solid #ea580c; "
        "text-align: left; padding: 12px; font-weight: bold; border-radius: 8px;");
    ui->btnRoleDecideur->setStyleSheet(
        "background-color: #ffffff; color: #9333ea; border: 1px solid #e9d5ff; "
        "text-align: left; padding: 12px; border-radius: 8px;");
}

void LoginDialog::on_btnRoleDecideur_clicked()
{
    currentRole = "Décideur institutionnel";
    ui->txtUsername->setText("ministere_decideur");
    ui->txtPassword->setText("decision123");

    // Style actif en mode clair diurne (violet doux + bordure pourpre)
    ui->btnRoleVigilance->setStyleSheet(
        "background-color: #ffffff; color: #0284c7; border: 1px solid #bae6fd; "
        "text-align: left; padding: 12px; border-radius: 8px;");
    ui->btnRoleProtection->setStyleSheet(
        "background-color: #ffffff; color: #ea580c; border: 1px solid #fed7aa; "
        "text-align: left; padding: 12px; border-radius: 8px;");
    ui->btnRoleDecideur->setStyleSheet(
        "background-color: #faf5ff; color: #9333ea; border: 2px solid #9333ea; "
        "text-align: left; padding: 12px; font-weight: bold; border-radius: 8px;");
}

void LoginDialog::on_btnLogin_clicked()
{
    // Contrôle de saisie élémentaire
    if (ui->txtUsername->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Connexion", "Veuillez renseigner un identifiant valide.");
        return;
    }
    accept(); // Ferme la boîte de dialogue en retournant QDialog::Accepted
}
