#include "modulealerteswidget.h"
#include "ui_modulealerteswidget.h"
#include "logindialog.h"
#include "predictiongel.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QRandomGenerator>
#include <QDesktopServices>
#include <QUrl>
#include <QHeaderView>
#include <QFile>
#include <QApplication>

ModuleAlertesWidget::ModuleAlertesWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModuleAlertesWidget)
    , currentUsername("vigilance_admin")
    , currentRole("Responsable de la vigilance")
    , statsViewWidget(nullptr)
    , clockTimer(nullptr)
    , watchdogTimer(nullptr)
    , bannerOpacityEffect(nullptr)
    , bannerAnimation(nullptr)
{
    ui->setupUi(this);

    // 1. Application de la feuille de style QSS (Inspiré du guide ESPRIT)
    QFile qssFile(":/style.qss");
    if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
        setStyleSheet(qssFile.readAll());
        qssFile.close();
    }

    // 2. Contrainte de dimensionnement fenêtre (Section 5 du guide ESPRIT)
    setMinimumSize(950, 620);

    // 3. Initialisation de l'animation de bannière (Section 12 du guide ESPRIT)
    bannerOpacityEffect = new QGraphicsOpacityEffect(ui->lblBannerInfo);
    ui->lblBannerInfo->setGraphicsEffect(bannerOpacityEffect);
    bannerAnimation = new QPropertyAnimation(bannerOpacityEffect, "opacity", this);
    bannerAnimation->setDuration(400);

    // 4. Initialisation des Timers (Section 6 du guide ESPRIT)
    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &ModuleAlertesWidget::updateClock);
    clockTimer->start(1000);
    updateClock();

    watchdogTimer = new QTimer(this);
    connect(watchdogTimer, &QTimer::timeout, this, &ModuleAlertesWidget::watchdogSensorCheck);
    watchdogTimer->start(30000); // Exécution de routine toutes les 30s

    // 5. Initialisation du Widget graphique comparatif personnalisé
    statsViewWidget = new StatsView(this);
    ui->layoutCustomStats->addWidget(statsViewWidget);

    // 6. Dates initiales du formulaire
    QDateTime now = QDateTime::currentDateTime();
    ui->dtEmission->setDateTime(now);
    ui->dtFin->setDateTime(now.addDays(1));

    // 7. Génération automatique d'un ID de départ
    on_btnGenererId_clicked();

    // 8. Configuration visuelle des tableaux
    ui->tableAlertes->horizontalHeader()->setStretchLastSection(true);
    ui->tableAlertes->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableAlertes->setAlternatingRowColors(true);
    ui->tableAlertes->verticalHeader()->setDefaultSectionSize(36);

    ui->tableDashAlertes->horizontalHeader()->setStretchLastSection(true);
    ui->tableDashAlertes->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableDashAlertes->setAlternatingRowColors(true);
    ui->tableDashAlertes->verticalHeader()->setDefaultSectionSize(36);

    // 9. Chargement initial des données & permissions
    rafraichirTableau();
    rafraichirIndicateurs();
    appliquerPermissionsRole();
    updateNavButtons(0);

    // 10. Calcul initial de la prédiction de gel avec sliders
    on_btnCalculerPrediction_clicked();
}

ModuleAlertesWidget::~ModuleAlertesWidget()
{
    if (clockTimer && clockTimer->isActive()) clockTimer->stop();
    if (watchdogTimer && watchdogTimer->isActive()) watchdogTimer->stop();
    delete ui;
}

// -------------------------------------------------------------
// GESTION DU TIMER HORLOGE TEMPS REEL (Section 6 Guide ESPRIT)
// -------------------------------------------------------------
void ModuleAlertesWidget::updateClock()
{
    QDateTime current = QDateTime::currentDateTime();
    ui->lblDigitalClock->setText(current.toString("yyyy-MM-dd  HH:mm:ss"));
}

void ModuleAlertesWidget::watchdogSensorCheck()
{
    if (!ui->chkWatchdog->isChecked()) return;

    // Vérifie le nombre d'alertes en cours
    int alertesCritiques = Alerte::compterAlertesCritiques();
    if (alertesCritiques > 0) {
        animateBanner(QString("⚠️ VEILLE ACTIVE : %1 alerte(s) critique(s) de niveau ROUGE en cours sur le territoire !").arg(alertesCritiques), "#ef4444");
    }
}

// -------------------------------------------------------------
// ANIMATION BANNIERE D'ALERTE (Section 12 Guide ESPRIT)
// -------------------------------------------------------------
void ModuleAlertesWidget::animateBanner(const QString &message, const QString &styleColor)
{
    ui->lblBannerInfo->setText(message);
    // Thème Daylight : Carte blanche avec bordure et texte dans la couleur thématique
    ui->lblBannerInfo->setStyleSheet(QString(
        "background-color: #ffffff; color: %1; border: 2px solid %1; "
        "border-radius: 8px; padding: 10px 16px; font-size: 13px; font-weight: bold;").arg(styleColor));

    bannerAnimation->stop();
    bannerAnimation->setStartValue(0.2);
    bannerAnimation->setEndValue(1.0);
    bannerAnimation->start();
}

void ModuleAlertesWidget::playAlertSound()
{
    // Avertissement sonore (Section 3 Guide ESPRIT)
    QApplication::beep();
}

// -------------------------------------------------------------
// NAVIGATION SIDEBAR & QStackedWidget (Section 10 Guide ESPRIT)
// -------------------------------------------------------------
void ModuleAlertesWidget::updateNavButtons(int activeIndex)
{
    ui->btnNavDashboard->setChecked(activeIndex == 0);
    ui->btnNavAlertes->setChecked(activeIndex == 1);
    ui->btnNavStats->setChecked(activeIndex == 2);
    ui->btnNavPrediction->setChecked(activeIndex == 3);

    auto setBtnStyle = [](QPushButton *btn, bool active) {
        if (active) {
            btn->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0284c7, stop:1 #0369a1); "
                               "color: #ffffff; font-weight: bold; border-left: 5px solid #0ea5e9; padding: 12px 18px; text-align: left; border-radius: 8px; font-size: 13px;");
        } else {
            btn->setStyleSheet("background-color: transparent; color: #0f172a; border: none; padding: 12px 18px; text-align: left; border-radius: 8px; font-weight: bold; font-size: 13px;");
        }
    };

    setBtnStyle(ui->btnNavDashboard, activeIndex == 0);
    setBtnStyle(ui->btnNavAlertes, activeIndex == 1);
    setBtnStyle(ui->btnNavStats, activeIndex == 2);
    setBtnStyle(ui->btnNavPrediction, activeIndex == 3);
}

void ModuleAlertesWidget::on_btnNavDashboard_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void ModuleAlertesWidget::on_btnNavAlertes_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void ModuleAlertesWidget::on_btnNavStats_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void ModuleAlertesWidget::on_btnNavPrediction_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void ModuleAlertesWidget::on_stackedWidget_currentChanged(int index)
{
    updateNavButtons(index);
    if (index == 2) {
        rafraichirIndicateurs();
    }
}

// -------------------------------------------------------------
// ACTIONS DU TABLEAU DE BORD (SUPERVISION)
// -------------------------------------------------------------
void ModuleAlertesWidget::on_btnDashNewAlert_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    viderFormulaire();
}

void ModuleAlertesWidget::on_btnDashSimulSensor_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    on_btnTriggerAuto_clicked();
}

void ModuleAlertesWidget::on_btnDashTestFrost_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void ModuleAlertesWidget::on_btnDashExportPDF_clicked()
{
    on_btnExportPDF_clicked();
}

// -------------------------------------------------------------
// GESTION DES ROLES ET UTILISATEURS (Section 7 Guide ESPRIT)
// -------------------------------------------------------------
void ModuleAlertesWidget::setUtilisateur(const QString &username, const QString &role)
{
    currentUsername = username;
    currentRole = role;
    appliquerPermissionsRole();
}

void ModuleAlertesWidget::appliquerPermissionsRole()
{
    ui->lblUserBadge->setText(QString("👤 %1").arg(currentUsername));
    ui->lblRoleBadge->setText(currentRole);

    if (currentRole == "Protection civile") {
        ui->btnAjouter->setEnabled(false);
        ui->btnModifier->setEnabled(false);
        ui->btnSupprimer->setEnabled(false);
        ui->btnArchiver->setEnabled(false);
        ui->btnMettreAJourSeverite->setEnabled(false);
        ui->btnTriggerAuto->setEnabled(false);
        ui->btnDeclencherAlerteGel->setEnabled(false);
        ui->btnDashSimulSensor->setEnabled(false);
        ui->btnDashNewAlert->setEnabled(false);

        QString consultTooltip = "Accès en consultation uniquement (Rôle Protection Civile)";
        ui->btnAjouter->setToolTip(consultTooltip);
        ui->btnModifier->setToolTip(consultTooltip);
        ui->btnSupprimer->setToolTip(consultTooltip);
        ui->btnArchiver->setToolTip(consultTooltip);
        ui->btnMettreAJourSeverite->setToolTip(consultTooltip);
        ui->btnTriggerAuto->setToolTip(consultTooltip);
        ui->btnDeclencherAlerteGel->setToolTip(consultTooltip);

        animateBanner("🛡️ Mode Consultation Active (Protection Civile) : Modifications opérationnelles verrouillées.", "#ea580c");
    } else if (currentRole == "Décideur institutionnel") {
        ui->btnAjouter->setEnabled(true);
        ui->btnModifier->setEnabled(true);
        ui->btnSupprimer->setEnabled(true);
        ui->btnArchiver->setEnabled(true);
        ui->btnMettreAJourSeverite->setEnabled(true);
        ui->btnTriggerAuto->setEnabled(true);
        ui->btnDeclencherAlerteGel->setEnabled(true);
        ui->btnDashSimulSensor->setEnabled(true);
        ui->btnDashNewAlert->setEnabled(true);

        ui->stackedWidget->setCurrentIndex(2); // Bascule automatique vers l'onglet Analyses Régionales
        animateBanner("🏛️ Accès Décideur Institutionnel : Tableau de bord stratégique & Synthèse régionale.", "#a855f7");
    } else { // Responsable de la vigilance
        ui->btnAjouter->setEnabled(true);
        ui->btnModifier->setEnabled(true);
        ui->btnSupprimer->setEnabled(true);
        ui->btnArchiver->setEnabled(true);
        ui->btnMettreAJourSeverite->setEnabled(true);
        ui->btnTriggerAuto->setEnabled(true);
        ui->btnDeclencherAlerteGel->setEnabled(true);
        ui->btnDashSimulSensor->setEnabled(true);
        ui->btnDashNewAlert->setEnabled(true);

        ui->btnAjouter->setToolTip("");
        ui->btnModifier->setToolTip("");
        ui->btnSupprimer->setToolTip("");
        ui->btnArchiver->setToolTip("");
        ui->btnMettreAJourSeverite->setToolTip("");
        ui->btnTriggerAuto->setToolTip("");
        ui->btnDeclencherAlerteGel->setToolTip("");

        animateBanner("👤 Responsable de la Vigilance : Tous droits actifs (Supervision, CRUD complet, Déclenchement).", "#10b981");
    }
}

void ModuleAlertesWidget::on_btnSwitchRole_clicked()
{
    LoginDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        setUtilisateur(dlg.getUsername(), dlg.getSelectedRole());
        QMessageBox::information(this, "Session Modifiée", 
            QString("Vous êtes maintenant connecté avec le profil : %1").arg(currentRole));
    }
}

// -------------------------------------------------------------
// GESTION DU TABLEAU ET INDICATEURS
// -------------------------------------------------------------
void ModuleAlertesWidget::rafraichirTableau()
{
    QString terme = ui->txtRecherche->text();
    QString typeFiltre = ui->cmbFiltreType->currentText();
    QString severiteFiltre = ui->cmbFiltreSeverite->currentText();
    QString statutFiltre = ui->cmbFiltreStatut->currentText();

    QSqlQueryModel *model = Alerte::rechercher(terme, typeFiltre, severiteFiltre, statutFiltre);
    ui->tableAlertes->setModel(model);
    ui->tableAlertes->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
     if (ui->tableAlertes->horizontalHeader()->count() > 8) {
        ui->tableAlertes->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
    }

    // Tableau Dashboard synchrone
    QSqlQueryModel *dashModel = Alerte::trier("date", "DESC");
    ui->tableDashAlertes->setModel(dashModel);
    ui->tableDashAlertes->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    if (ui->tableDashAlertes->horizontalHeader()->count() > 8) {
        ui->tableDashAlertes->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
    }

    rafraichirIndicateurs();
}

void ModuleAlertesWidget::rafraichirIndicateurs()
{
    int total = 0;
    int enCours = Alerte::compterAlertesEnCours();
    int rouge = Alerte::compterAlertesCritiques();
    int gel = 0;

    auto typeStats = Alerte::statistiquesParType();
    gel = typeStats.value("Gel", 0);

    auto sevStats = Alerte::statistiquesParSeverite();
    for (int count : sevStats) total += count;

    ui->lblKpiTotalVal->setText(QString::number(total));
    ui->lblKpiEnCoursVal->setText(QString::number(enCours));
    ui->lblKpiCritiqueVal->setText(QString::number(rouge));
    ui->lblKpiGelVal->setText(QString::number(gel));

    if (statsViewWidget) {
        statsViewWidget->refreshData(ui->cmbAnneeStats->currentText().toInt());
    }
}

void ModuleAlertesWidget::viderFormulaire()
{
    on_btnGenererId_clicked();
    ui->cmbTypeAlerte->setCurrentIndex(0);
    ui->cmbSeverite->setCurrentIndex(0);
    ui->txtIdStation->clear();
    ui->cmbGouvernorat->setCurrentIndex(0);
    ui->dtEmission->setDateTime(QDateTime::currentDateTime());
    ui->dtFin->setDateTime(QDateTime::currentDateTime().addDays(1));
    ui->cmbStatut->setCurrentIndex(0);
    ui->txtDescription->clear();
}

void ModuleAlertesWidget::chargerLigneFormulaire(int row)
{
    QAbstractItemModel *model = ui->tableAlertes->model();
    if (!model || row < 0) return;

    ui->txtIdAlerte->setText(model->index(row, 0).data().toString());
    ui->cmbTypeAlerte->setCurrentText(model->index(row, 1).data().toString());
    ui->cmbSeverite->setCurrentText(model->index(row, 2).data().toString());
    
    QDateTime dEmis = QDateTime::fromString(model->index(row, 3).data().toString(), "yyyy-MM-dd hh:mm:ss");
    if (dEmis.isValid()) ui->dtEmission->setDateTime(dEmis);

    QDateTime dFin = QDateTime::fromString(model->index(row, 4).data().toString(), "yyyy-MM-dd hh:mm:ss");
    if (dFin.isValid()) ui->dtFin->setDateTime(dFin);

    ui->txtIdStation->setText(model->index(row, 5).data().toString());
    ui->cmbGouvernorat->setCurrentText(model->index(row, 6).data().toString());
    ui->cmbStatut->setCurrentText(model->index(row, 7).data().toString());
    ui->txtDescription->setText(model->index(row, 8).data().toString());
}

void ModuleAlertesWidget::on_tableAlertes_clicked(const QModelIndex &index)
{
    if (index.isValid()) {
        chargerLigneFormulaire(index.row());
    }
}

void ModuleAlertesWidget::on_tableDashAlertes_clicked(const QModelIndex &index)
{
    if (index.isValid()) {
        chargerLigneFormulaire(index.row());
        ui->stackedWidget->setCurrentIndex(1); // Bascule vers le journal
    }
}

void ModuleAlertesWidget::on_btnGenererId_clicked()
{
    int randNum = QRandomGenerator::global()->bounded(100, 999);
    QString id = QString("ALT-%1-%2").arg(QDateTime::currentDateTime().toString("yyyy")).arg(randNum);
    ui->txtIdAlerte->setText(id);
}

void ModuleAlertesWidget::on_btnViderChamps_clicked()
{
    viderFormulaire();
}

// -------------------------------------------------------------
// OPERATIONS CRUD
// -------------------------------------------------------------
void ModuleAlertesWidget::on_btnAjouter_clicked()
{
    QString id = ui->txtIdAlerte->text().trimmed();
    QString station = ui->txtIdStation->text().trimmed();
    QDateTime dEmis = ui->dtEmission->dateTime();
    QDateTime dFin = ui->dtFin->dateTime();

    if (id.isEmpty()) {
        QMessageBox::warning(this, "Validation", "Veuillez renseigner un ID d'alerte unique.");
        return;
    }
    if (station.isEmpty()) {
        QMessageBox::warning(this, "Validation", "Veuillez renseigner une référence de station météorologique.");
        return;
    }
    if (dFin <= dEmis) {
        QMessageBox::warning(this, "Validation", "La Date de Fin doit être postérieure à la Date d'Émission.");
        return;
    }
    if (Alerte::existe(id)) {
        QMessageBox::warning(this, "Conflit", QString("Une alerte avec l'identifiant %1 existe déjà dans le registre.").arg(id));
        return;
    }

    Alerte a(id,
             ui->cmbTypeAlerte->currentText(),
             ui->cmbSeverite->currentText(),
             dEmis,
             dFin,
             station,
             ui->cmbGouvernorat->currentText(),
             ui->cmbStatut->currentText(),
             ui->txtDescription->toPlainText().trimmed());

    if (a.ajouter()) {
        QMessageBox::information(this, "Succès", "L'alerte a été créée et déclenchée avec succès!");
        animateBanner(QString("Nouvelle alerte %1 enregistrée avec succès.").arg(id), "#22c55e");
        rafraichirTableau();
        viderFormulaire();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'enregistrement de l'alerte dans la base.");
    }
}

void ModuleAlertesWidget::on_btnModifier_clicked()
{
    QString id = ui->txtIdAlerte->text().trimmed();
    if (!Alerte::existe(id)) {
        QMessageBox::warning(this, "Erreur", QString("Aucune alerte existante trouvée avec l'ID %1.").arg(id));
        return;
    }

    Alerte a(id,
             ui->cmbTypeAlerte->currentText(),
             ui->cmbSeverite->currentText(),
             ui->dtEmission->dateTime(),
             ui->dtFin->dateTime(),
             ui->txtIdStation->text().trimmed(),
             ui->cmbGouvernorat->currentText(),
             ui->cmbStatut->currentText(),
             ui->txtDescription->toPlainText().trimmed());

    if (a.modifier()) {
        QMessageBox::information(this, "Succès", "Alerte mise à jour avec succès!");
        animateBanner(QString("Alerte %1 mise à jour.").arg(id), "#38bdf8");
        rafraichirTableau();
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de mettre à jour l'alerte.");
    }
}

void ModuleAlertesWidget::on_btnSupprimer_clicked()
{
    QString id = ui->txtIdAlerte->text().trimmed();
    if (id.isEmpty() || !Alerte::existe(id)) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner une alerte valide à supprimer.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmation", 
        QString("Êtes-vous sûr de vouloir supprimer définitivement l'alerte %1 ?").arg(id),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (Alerte::supprimer(id)) {
            QMessageBox::information(this, "Succès", "Alerte supprimée avec succès.");
            animateBanner(QString("Alerte %1 supprimée du registre.").arg(id), "#ef4444");
            rafraichirTableau();
            viderFormulaire();
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression.");
        }
    }
}

void ModuleAlertesWidget::on_btnArchiver_clicked()
{
    QString id = ui->txtIdAlerte->text().trimmed();
    if (id.isEmpty() || !Alerte::existe(id)) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner une alerte expirée ou active à archiver.");
        return;
    }

    if (Alerte::archiver(id)) {
        QMessageBox::information(this, "Archivage", QString("L'alerte %1 a été archivée avec succès.").arg(id));
        animateBanner(QString("Alerte %1 archivée.").arg(id), "#64748b");
        rafraichirTableau();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'archivage.");
    }
}

void ModuleAlertesWidget::on_btnMettreAJourSeverite_clicked()
{
    QString id = ui->txtIdAlerte->text().trimmed();
    if (id.isEmpty() || !Alerte::existe(id)) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner une alerte dans le tableau pour modifier sa sévérité.");
        return;
    }

    QString currentSev = ui->cmbSeverite->currentText();
    QString nextSev = "Rouge";
    if (currentSev == "Jaune") nextSev = "Orange";
    else if (currentSev == "Orange") nextSev = "Rouge";
    else if (currentSev == "Rouge") nextSev = "Jaune";

    if (Alerte::mettreAJourSeverite(id, nextSev)) {
        ui->cmbSeverite->setCurrentText(nextSev);
        QMessageBox::information(this, "Mise à Jour Sévérité", 
            QString("Le niveau de sévérité de l'alerte %1 a été actualisé à : %2.").arg(id, nextSev));
        animateBanner(QString("Escalade de sévérité sur l'alerte %1 ➔ %2").arg(id, nextSev), "#f97316");
        rafraichirTableau();
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de mettre à jour le niveau de sévérité.");
    }
}

void ModuleAlertesWidget::on_btnTriggerAuto_clicked()
{
    struct EventSimule {
        const char* type;
        const char* sev;
        const char* station;
        const char* gouv;
        const char* desc;
    };

    EventSimule events[] = {
        {"Vent fort", "Rouge", "ST-01_BIZ", "Bizerte", "Déclenchement automatique : Anémomètre capteur nord mesurant 108 km/h (> seuil critique 90 km/h)."},
        {"Sirocco/Canicule", "Rouge", "ST-12_TOZ", "Tozeur", "Déclenchement automatique : Capteur thermo-hygrométrique mesurant 48.2°C (> seuil canicule extrême)."},
        {"Tempête", "Orange", "ST-05_SOU", "Sousse", "Déclenchement automatique : Baromètre en chute brutale (-6 hPa/3h) et détection de foudre intense."},
        {"Gel", "Orange", "ST-04_JEN", "Jendouba", "Déclenchement automatique : Température au sol mesurée à -1.5°C avec rosée négative."}
    };

    int index = QRandomGenerator::global()->bounded(4);
    const EventSimule &ev = events[index];

    int randNum = QRandomGenerator::global()->bounded(100, 999);
    QString autoId = QString("ALT-%1-%2").arg(QDateTime::currentDateTime().toString("yyyy")).arg(randNum);

    Alerte a(autoId, ev.type, ev.sev,
             QDateTime::currentDateTime(),
             QDateTime::currentDateTime().addDays(1),
             ev.station, ev.gouv, "En cours", ev.desc);

    if (a.ajouter()) {
        playAlertSound();
        animateBanner(QString("🚨 ALERTE AUTOMATIQUE : %1 (%2) — Station %3").arg(autoId, ev.type, ev.station), "#ef4444");
        QMessageBox::warning(this, "⚡ Alerte Déclenchée Automatiquement !", 
            QString("Le système de veille a détecté un franchissement de seuil critique :\n\n"
                    "• Alerte : %1\n"
                    "• Risque : %2 (Niveau %3)\n"
                    "• Station : %4 (%5)\n"
                    "• Cause : %6").arg(autoId, ev.type, ev.sev, ev.station, ev.gouv, ev.desc));
        rafraichirTableau();
    }
}

// -------------------------------------------------------------
// RECHERCHE, FILTRAGE ET TRIS
// -------------------------------------------------------------
void ModuleAlertesWidget::on_txtRecherche_textChanged(const QString &)
{
    rafraichirTableau();
}

void ModuleAlertesWidget::on_cmbFiltreType_currentTextChanged(const QString &)
{
    rafraichirTableau();
}

void ModuleAlertesWidget::on_cmbFiltreSeverite_currentTextChanged(const QString &)
{
    rafraichirTableau();
}

void ModuleAlertesWidget::on_cmbFiltreStatut_currentTextChanged(const QString &)
{
    rafraichirTableau();
}

void ModuleAlertesWidget::on_btnTriSeverite_clicked()
{
    QSqlQueryModel *model = Alerte::trier("severite", "ROUGE_FIRST");
    ui->tableAlertes->setModel(model);
    ui->tableAlertes->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    if (ui->tableAlertes->horizontalHeader()->count() > 8) {
        ui->tableAlertes->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
    }
    animateBanner("Tri appliqué : Priorité absolue aux niveaux de sévérité Rouge & Orange.", "#f97316");
}

void ModuleAlertesWidget::on_btnTriDate_clicked()
{
    QSqlQueryModel *model = Alerte::trier("date", "DESC");
    ui->tableAlertes->setModel(model);
    ui->tableAlertes->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    if (ui->tableAlertes->horizontalHeader()->count() > 8) {
        ui->tableAlertes->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
    }
    animateBanner("Tri appliqué : Classement chronologique par date d'émission la plus récente.", "#0284c7");
}

// -------------------------------------------------------------
// EXPORTATIONS
// -------------------------------------------------------------
void ModuleAlertesWidget::on_btnExportPDF_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Exporter le journal des alertes en PDF", 
                                                    "Journal_Alertes_SWM_2026.pdf", 
                                                    "Documents PDF (*.pdf)");
    if (filePath.isEmpty()) return;

    if (Alerte::exporterPDF(filePath, QString("%1 (%2)").arg(currentUsername, currentRole))) {
        animateBanner(QString("Document PDF officiel exporté avec succès : %1").arg(filePath), "#10b981");
        QMessageBox::StandardButton reply = QMessageBox::information(this, "Export PDF Réussi", 
            QString("Le journal officiel des alertes a été exporté au format PDF avec succès :\n%1\n\nSouhaitez-vous ouvrir le fichier ?").arg(filePath),
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
        }
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'exportation PDF.");
    }
}

void ModuleAlertesWidget::on_btnExportTXT_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Exporter le registre des alertes en TXT", 
                                                    "Registre_Alertes_SWM.txt", 
                                                    "Fichiers Texte (*.txt)");
    if (filePath.isEmpty()) return;

    if (Alerte::exporterTXT(filePath)) {
        animateBanner("Registre d'archivage TXT exporté avec succès.", "#0f766e");
        QMessageBox::information(this, "Export TXT Réussi", 
            QString("Le registre a été exporté au format TXT réglementaire avec succès :\n%1").arg(filePath));
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'exportation du fichier TXT.");
    }
}

// -------------------------------------------------------------
// ANALYSES & STATISTIQUES
// -------------------------------------------------------------
void ModuleAlertesWidget::on_cmbAnneeStats_currentIndexChanged(int)
{
    int annee = ui->cmbAnneeStats->currentText().toInt();
    if (statsViewWidget) {
        statsViewWidget->refreshData(annee);
    }
    rafraichirIndicateurs();
}

void ModuleAlertesWidget::on_btnRefreshStats_clicked()
{
    int annee = ui->cmbAnneeStats->currentText().toInt();
    if (statsViewWidget) {
        statsViewWidget->refreshData(annee);
    }
    rafraichirIndicateurs();
    QMessageBox::information(this, "Graphique Actualisé", "Les données statistiques par gouvernorat ont été actualisées.");
}

// -------------------------------------------------------------
// INNOVATION METIER : PREDICTION GEL & SLIDERS SYNCHRONISES
// (Section 23 du guide ESPRIT : QSlider stylesheet)
// -------------------------------------------------------------
void ModuleAlertesWidget::on_sliderTemp_valueChanged(int value)
{
    double valDouble = value / 10.0;
    if (std::abs(ui->spnTemp->value() - valDouble) > 0.05) {
        ui->spnTemp->setValue(valDouble);
    }
    on_btnCalculerPrediction_clicked();
}

void ModuleAlertesWidget::on_spnTemp_valueChanged(double val)
{
    int valInt = static_cast<int>(std::round(val * 10.0));
    if (ui->sliderTemp->value() != valInt) {
        ui->sliderTemp->setValue(valInt);
    }
    on_btnCalculerPrediction_clicked();
}

void ModuleAlertesWidget::on_sliderHum_valueChanged(int value)
{
    if (static_cast<int>(ui->spnHum->value()) != value) {
        ui->spnHum->setValue(value);
    }
    on_btnCalculerPrediction_clicked();
}

void ModuleAlertesWidget::on_spnHum_valueChanged(double val)
{
    int valInt = static_cast<int>(val);
    if (ui->sliderHum->value() != valInt) {
        ui->sliderHum->setValue(valInt);
    }
    on_btnCalculerPrediction_clicked();
}

void ModuleAlertesWidget::on_sliderVent_valueChanged(int value)
{
    double valDouble = value / 10.0;
    if (std::abs(ui->spnVent->value() - valDouble) > 0.05) {
        ui->spnVent->setValue(valDouble);
    }
    on_btnCalculerPrediction_clicked();
}

void ModuleAlertesWidget::on_spnVent_valueChanged(double val)
{
    int valInt = static_cast<int>(std::round(val * 10.0));
    if (ui->sliderVent->value() != valInt) {
        ui->sliderVent->setValue(valInt);
    }
    on_btnCalculerPrediction_clicked();
}

void ModuleAlertesWidget::on_sliderNeb_valueChanged(int value)
{
    if (ui->spnNeb->value() != value) {
        ui->spnNeb->setValue(value);
    }
    on_btnCalculerPrediction_clicked();
}

void ModuleAlertesWidget::on_spnNeb_valueChanged(int val)
{
    if (ui->sliderNeb->value() != val) {
        ui->sliderNeb->setValue(val);
    }
    on_btnCalculerPrediction_clicked();
}

void ModuleAlertesWidget::on_chkSolHumide_toggled(bool)
{
    on_btnCalculerPrediction_clicked();
}

void ModuleAlertesWidget::on_btnCalculerPrediction_clicked()
{
    double temp = ui->spnTemp->value();
    double hum = ui->spnHum->value();
    double vent = ui->spnVent->value();
    int neb = ui->spnNeb->value();
    bool solHum = ui->chkSolHumide->isChecked();

    ResultatPredictionGel res = PredictionGel::calculer(temp, hum, vent, neb, solHum);

    ui->lblTMinValeur->setText(QString("%1 °C").arg(res.tMinEstimee, 0, 'f', 1));
    ui->lblDewValeur->setText(QString("%1 °C").arg(res.pointDeRosee, 0, 'f', 1));
    ui->lblNiveauRisqueText->setText(res.niveauRisque);
    ui->lblProbabiliteText->setText(QString("Probabilité estimée : %1% | Type : %2")
                                   .arg(res.probabiliteGel, 0, 'f', 0)
                                   .arg(res.typeGel));
    ui->txtDiagnostic->setText(res.diagnostic);

    QString recomStr;
    for (const QString &rec : res.recommandations) {
        recomStr += "• " + rec + "\n";
    }
    ui->txtRecommandations->setText(recomStr);

    // Ajustement visuel des badges en thème Daylight Clear
    if (res.niveauSeveriteAlerte == "Rouge") {
        ui->frameBadgeRisque->setStyleSheet("background-color: #fef2f2; border: 2px solid #ef4444; border-radius: 8px; padding: 12px;");
        ui->lblRisqueIcon->setText("🚨");
        ui->lblNiveauRisqueText->setStyleSheet("font-size: 16px; font-weight: bold; color: #b91c1c;");
        ui->lblProbabiliteText->setStyleSheet("font-size: 12px; color: #7f1d1d;");
    } else if (res.niveauSeveriteAlerte == "Orange") {
        ui->frameBadgeRisque->setStyleSheet("background-color: #fff7ed; border: 2px solid #f97316; border-radius: 8px; padding: 12px;");
        ui->lblRisqueIcon->setText("⚠️");
        ui->lblNiveauRisqueText->setStyleSheet("font-size: 16px; font-weight: bold; color: #c2410c;");
        ui->lblProbabiliteText->setStyleSheet("font-size: 12px; color: #9a3412;");
    } else if (res.niveauSeveriteAlerte == "Jaune") {
        ui->frameBadgeRisque->setStyleSheet("background-color: #fefce8; border: 2px solid #eab308; border-radius: 8px; padding: 12px;");
        ui->lblRisqueIcon->setText("⚡");
        ui->lblNiveauRisqueText->setStyleSheet("font-size: 16px; font-weight: bold; color: #a16207;");
        ui->lblProbabiliteText->setStyleSheet("font-size: 12px; color: #854d0e;");
    } else {
        ui->frameBadgeRisque->setStyleSheet("background-color: #f0fdf4; border: 2px solid #10b981; border-radius: 8px; padding: 12px;");
        ui->lblRisqueIcon->setText("✅");
        ui->lblNiveauRisqueText->setStyleSheet("font-size: 16px; font-weight: bold; color: #15803d;");
        ui->lblProbabiliteText->setStyleSheet("font-size: 12px; color: #166534;");
    }
}

void ModuleAlertesWidget::on_btnDeclencherAlerteGel_clicked()
{
    double temp = ui->spnTemp->value();
    double hum = ui->spnHum->value();
    double vent = ui->spnVent->value();
    int neb = ui->spnNeb->value();
    bool solHum = ui->chkSolHumide->isChecked();

    ResultatPredictionGel res = PredictionGel::calculer(temp, hum, vent, neb, solHum);

    if (res.niveauSeveriteAlerte == "Aucune") {
        QMessageBox::information(this, "Information", "Le risque calculé est nul ou faible. Aucun déclenchement d'alerte n'est nécessaire.");
        return;
    }

    QString comboText = ui->cmbStationCible->currentText();
    QString station = comboText.section(' ', 0, 0);
    QString gouv = comboText.section('(', 1, 1).section(" -", 0, 0);

    int randNum = QRandomGenerator::global()->bounded(100, 999);
    QString newId = QString("ALT-%1-%2").arg(QDateTime::currentDateTime().toString("yyyy")).arg(randNum);

    QDateTime dEmis = QDateTime::currentDateTime();
    QDateTime dFin = dEmis.addSecs(10 * 3600);

    Alerte a(newId, "Gel", res.niveauSeveriteAlerte, dEmis, dFin, station, gouv, "En cours", res.diagnostic);

    if (a.ajouter()) {
        playAlertSound();
        animateBanner(QString("🚨 Alerte Gel %1 déclenchée pour la région de %2 !").arg(newId, gouv), "#ef4444");

        QMessageBox::information(this, "Alerte Gel Déclenchée", 
            QString("L'alerte pour Risque de Gel Nocturne a été enregistrée avec succès :\n\n"
                    "• ID : %1\n"
                    "• Région : %2 (%3)\n"
                    "• Niveau : %4\n"
                    "• T° Min. Prévue : %5 °C\n\n"
                    "La Protection Civile et les services régionaux sont notifiés.").arg(newId, gouv, station, res.niveauSeveriteAlerte).arg(res.tMinEstimee, 0, 'f', 1));

        rafraichirTableau();
        ui->stackedWidget->setCurrentIndex(1); // Retour au journal des alertes
    } else {
        QMessageBox::critical(this, "Erreur", "Impossible de déclencher l'alerte Gel.");
    }
}


void ModuleAlertesWidget::rafraichirDonnees()
{
    rafraichirTableau();
    rafraichirIndicateurs();
}
