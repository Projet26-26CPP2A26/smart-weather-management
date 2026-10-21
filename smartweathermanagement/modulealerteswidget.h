#ifndef MODULEALERTESWIDGET_H
#define MODULEALERTESWIDGET_H

#include <QWidget>
#include <QItemSelection>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "alerte.h"
#include "statsview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ModuleAlertesWidget; }
QT_END_NAMESPACE

class ModuleAlertesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleAlertesWidget(QWidget *parent = nullptr);
    ~ModuleAlertesWidget();

    void setUtilisateur(const QString &username, const QString &role);
    void rafraichirDonnees();

private slots:
    // Navigation Sidebar (QStackedWidget)
    void on_btnNavDashboard_clicked();
    void on_btnNavAlertes_clicked();
    void on_btnNavStats_clicked();
    void on_btnNavPrediction_clicked();
    void on_stackedWidget_currentChanged(int index);

    // Dashboard Quick Actions
    void on_btnDashNewAlert_clicked();
    void on_btnDashSimulSensor_clicked();
    void on_btnDashTestFrost_clicked();
    void on_btnDashExportPDF_clicked();

    // Session & Rôles
    void on_btnSwitchRole_clicked();

    // CRUD Slots
    void on_btnAjouter_clicked();
    void on_btnModifier_clicked();
    void on_btnSupprimer_clicked();
    void on_btnArchiver_clicked();
    void on_btnMettreAJourSeverite_clicked();
    void on_btnViderChamps_clicked();
    void on_btnGenererId_clicked();
    void on_btnTriggerAuto_clicked();

    // Table & Selection Slots
    void on_tableAlertes_clicked(const QModelIndex &index);
    void on_tableDashAlertes_clicked(const QModelIndex &index);

    // Recherche, Filtrage et Tri
    void on_txtRecherche_textChanged(const QString &arg1);
    void on_cmbFiltreType_currentTextChanged(const QString &arg1);
    void on_cmbFiltreSeverite_currentTextChanged(const QString &arg1);
    void on_cmbFiltreStatut_currentTextChanged(const QString &arg1);
    void on_btnTriSeverite_clicked();
    void on_btnTriDate_clicked();

    // Exportations
    void on_btnExportPDF_clicked();
    void on_btnExportTXT_clicked();

    // Statistiques & Analyses
    void on_cmbAnneeStats_currentIndexChanged(int index);
    void on_btnRefreshStats_clicked();

    // Innovation : Prédiction Gel & Sliders Synchronisés
    void on_btnCalculerPrediction_clicked();
    void on_btnDeclencherAlerteGel_clicked();
    void on_sliderTemp_valueChanged(int value);
    void on_spnTemp_valueChanged(double arg1);
    void on_sliderHum_valueChanged(int value);
    void on_spnHum_valueChanged(double arg1);
    void on_sliderVent_valueChanged(int value);
    void on_spnVent_valueChanged(double arg1);
    void on_sliderNeb_valueChanged(int value);
    void on_spnNeb_valueChanged(int arg1);
    void on_chkSolHumide_toggled(bool checked);

    // Timers Slots (Horloge temps réel & Veille capteurs)
    void updateClock();
    void watchdogSensorCheck();

private:
    Ui::ModuleAlertesWidget *ui;
    QString currentUsername;
    QString currentRole;
    StatsView *statsViewWidget;

    // Timers
    QTimer *clockTimer;
    QTimer *watchdogTimer;

    // Animation de bannière d'alerte
    QGraphicsOpacityEffect *bannerOpacityEffect;
    QPropertyAnimation *bannerAnimation;

    // Méthodes auxiliaires
    void rafraichirTableau();
    void rafraichirIndicateurs();
    void appliquerPermissionsRole();
    void updateNavButtons(int activeIndex);
    void animateBanner(const QString &message, const QString &styleColor);
    void playAlertSound();
    void viderFormulaire();
    void chargerLigneFormulaire(int row);
};

#endif // MODULEALERTESWIDGET_H
