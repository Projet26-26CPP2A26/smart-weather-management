// ============================================================================
// PROJET SMART WEATHER MANAGEMENT - MODULE 1 : GESTION DES EMPLOYÉS
// SEMAINE 10 : IMPLÉMENTATION ModuleArduinoWidget
// ÉTUDIANTE : SALMA RIDENE
// ============================================================================

#include "modulearduinowidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTableView>
#include <QHeaderView>
#include <QProgressBar>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QDateTime>
#include <QMessageBox>

ModuleArduinoWidget::ModuleArduinoWidget(QWidget *parent)
    : QWidget(parent)
{
    m_arduino = new Arduino(this);
    m_station = new StationMeteo("Arduino-UNO-INM-01");
    StationMeteo::initialiserTables();

    construireUI();

    connect(m_arduino, &Arduino::lineReceived,
            this, &ModuleArduinoWidget::onLigneRecue);
    connect(m_arduino, &Arduino::connectionStatusChanged,
            this, &ModuleArduinoWidget::onConnexionChanged);

    m_timerRefresh = new QTimer(this);
    connect(m_timerRefresh, &QTimer::timeout, this, &ModuleArduinoWidget::rafraichirTableau);
    m_timerRefresh->start(3000); // Rafraîchit le tableau toutes les 3s

    rafraichirTableau();
}

ModuleArduinoWidget::~ModuleArduinoWidget()
{
    if (m_arduino && m_arduino->is_connected()) {
        m_arduino->close_arduino();
    }
    delete m_station;
}

void ModuleArduinoWidget::construireUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(16);

    // ── Titre du module ─────────────────────────────────────────────────────
    auto *headerBox = new QHBoxLayout();
    auto *titre = new QLabel("🛰️ Station Météo Intelligente (Arduino Uno & Capteurs)", this);
    titre->setStyleSheet("font-size: 18px; font-weight: bold; color: #1E293B;");
    headerBox->addWidget(titre);
    headerBox->addStretch();
    mainLayout->addLayout(headerBox);

    // ── Barre de connexion Série ────────────────────────────────────────────
    auto *grpConnexion = new QGroupBox("Connexion Série / Wokwi Bridge", this);
    grpConnexion->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #CBD5E1; border-radius: 8px; margin-top: 10px; padding-top: 15px; }"
                                "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; color: #0284C7; }");
    auto *connLayout = new QHBoxLayout(grpConnexion);

    auto *lblPort = new QLabel("Port Série :", grpConnexion);
    m_cmbPort = new QComboBox(grpConnexion);
    m_cmbPort->setMinimumWidth(160);
    QStringList ports = Arduino::available_ports();
    if (ports.isEmpty()) {
        m_cmbPort->addItem("COM1 (Simulateur / Démo)");
        m_cmbPort->addItem("COM2");
        m_cmbPort->addItem("COM3");
    } else {
        m_cmbPort->addItems(ports);
    }

    auto *btnActualiserPorts = new QPushButton("🔄 Rafraîchir", grpConnexion);
    connect(btnActualiserPorts, &QPushButton::clicked, this, [this]() {
        m_cmbPort->clear();
        QStringList p = Arduino::available_ports();
        if (p.isEmpty()) {
            m_cmbPort->addItem("COM1 (Simulateur / Démo)");
            m_cmbPort->addItem("COM2");
            m_cmbPort->addItem("COM3");
        } else {
            m_cmbPort->addItems(p);
        }
    });

    m_btnConnecter = new QPushButton("🔌 Connecter", grpConnexion);
    m_btnConnecter->setStyleSheet("background-color: #0284C7; color: white; font-weight: bold; padding: 6px 14px; border-radius: 6px;");
    connect(m_btnConnecter, &QPushButton::clicked, this, &ModuleArduinoWidget::onConnecterClicked);

    m_lblStatut = new QLabel("⚫ Déconnecté", grpConnexion);
    m_lblStatut->setStyleSheet("color: #64748B; font-weight: bold;");

    connLayout->addWidget(lblPort);
    connLayout->addWidget(m_cmbPort);
    connLayout->addWidget(btnActualiserPorts);
    connLayout->addWidget(m_btnConnecter);
    connLayout->addWidget(m_lblStatut);
    connLayout->addStretch();
    mainLayout->addWidget(grpConnexion);

    // ── Cartes de télémétrie en direct ──────────────────────────────────────
    auto *telemetryGrid = new QGridLayout();
    telemetryGrid->setSpacing(12);

    // Carte Température
    auto *cardTemp = new QGroupBox("🌡️ Température (DHT22)", this);
    cardTemp->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #E2E8F0; border-radius: 8px; padding-top: 15px; }");
    auto *layTemp = new QVBoxLayout(cardTemp);
    m_lblTemp = new QLabel("--.- °C", cardTemp);
    m_lblTemp->setStyleSheet("font-size: 26px; font-weight: bold; color: #EA580C;");
    m_lblTemp->setAlignment(Qt::AlignCenter);
    m_barTemp = new QProgressBar(cardTemp);
    m_barTemp->setRange(-10, 50);
    m_barTemp->setValue(25);
    m_barTemp->setTextVisible(false);
    m_barTemp->setStyleSheet("QProgressBar::chunk { background-color: #F97316; border-radius: 3px; }");
    layTemp->addWidget(m_lblTemp);
    layTemp->addWidget(m_barTemp);
    telemetryGrid->addWidget(cardTemp, 0, 0);

    // Carte Humidité
    auto *cardHum = new QGroupBox("💧 Humidité (DHT22)", this);
    cardHum->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #E2E8F0; border-radius: 8px; padding-top: 15px; }");
    auto *layHum = new QVBoxLayout(cardHum);
    m_lblHum = new QLabel("-- %", cardHum);
    m_lblHum->setStyleSheet("font-size: 26px; font-weight: bold; color: #0284C7;");
    m_lblHum->setAlignment(Qt::AlignCenter);
    m_barHum = new QProgressBar(cardHum);
    m_barHum->setRange(0, 100);
    m_barHum->setValue(50);
    m_barHum->setTextVisible(false);
    m_barHum->setStyleSheet("QProgressBar::chunk { background-color: #0284C7; border-radius: 3px; }");
    layHum->addWidget(m_lblHum);
    layHum->addWidget(m_barHum);
    telemetryGrid->addWidget(cardHum, 0, 1);

    // Carte Pluie
    auto *cardPluie = new QGroupBox("🌧️ Capteur Pluie (ADC)", this);
    cardPluie->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #E2E8F0; border-radius: 8px; padding-top: 15px; }");
    auto *layPluie = new QVBoxLayout(cardPluie);
    m_lblPluie = new QLabel("--- / 1023", cardPluie);
    m_lblPluie->setStyleSheet("font-size: 26px; font-weight: bold; color: #475569;");
    m_lblPluie->setAlignment(Qt::AlignCenter);
    m_barPluie = new QProgressBar(cardPluie);
    m_barPluie->setRange(0, 1023);
    m_barPluie->setValue(100);
    m_barPluie->setTextVisible(false);
    m_barPluie->setStyleSheet("QProgressBar::chunk { background-color: #64748B; border-radius: 3px; }");
    layPluie->addWidget(m_lblPluie);
    layPluie->addWidget(m_barPluie);
    telemetryGrid->addWidget(cardPluie, 0, 2);

    // Carte Détection Gel / Statut
    auto *cardGel = new QGroupBox("❄️ Risque de Gel", this);
    cardGel->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #E2E8F0; border-radius: 8px; padding-top: 15px; }");
    auto *layGel = new QVBoxLayout(cardGel);
    m_lblGel = new QLabel("NON DÉTECTÉ", cardGel);
    m_lblGel->setStyleSheet("font-size: 18px; font-weight: bold; color: #16A34A;");
    m_lblGel->setAlignment(Qt::AlignCenter);
    m_lblAlerte = new QLabel("Statut : Nominal", cardGel);
    m_lblAlerte->setStyleSheet("font-size: 12px; color: #64748B;");
    m_lblAlerte->setAlignment(Qt::AlignCenter);
    layGel->addWidget(m_lblGel);
    layGel->addWidget(m_lblAlerte);
    telemetryGrid->addWidget(cardGel, 0, 3);

    mainLayout->addLayout(telemetryGrid);

    // ── Commandes Actionneurs & Seuils ──────────────────────────────────────
    auto *cmdBox = new QHBoxLayout();

    auto *grpAction = new QGroupBox("Actionneurs Arduino (Retour vers Microcontrôleur)", this);
    grpAction->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #CBD5E1; border-radius: 8px; margin-top: 5px; padding-top: 12px; }");
    auto *actLayout = new QHBoxLayout(grpAction);

    m_btnAlertOn = new QPushButton("🚨 Activer Alarme (Buzzer + LED Rouge)", grpAction);
    m_btnAlertOn->setStyleSheet("background-color: #DC2626; color: white; font-weight: bold; padding: 6px 12px; border-radius: 6px;");
    connect(m_btnAlertOn, &QPushButton::clicked, this, &ModuleArduinoWidget::onAlertOnClicked);

    m_btnAlertOff = new QPushButton("✅ Couper Alarme (LED Verte)", grpAction);
    m_btnAlertOff->setStyleSheet("background-color: #16A34A; color: white; font-weight: bold; padding: 6px 12px; border-radius: 6px;");
    connect(m_btnAlertOff, &QPushButton::clicked, this, &ModuleArduinoWidget::onAlertOffClicked);

    m_btnStatus = new QPushButton("ℹ️ Interroger Statut", grpAction);
    m_btnStatus->setStyleSheet("background-color: #475569; color: white; padding: 6px 12px; border-radius: 6px;");
    connect(m_btnStatus, &QPushButton::clicked, this, &ModuleArduinoWidget::onStatusClicked);

    actLayout->addWidget(m_btnAlertOn);
    actLayout->addWidget(m_btnAlertOff);
    actLayout->addWidget(m_btnStatus);
    cmdBox->addWidget(grpAction, 2);

    auto *grpSeuils = new QGroupBox("Seuils Automatiques BD", this);
    grpSeuils->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #CBD5E1; border-radius: 8px; margin-top: 5px; padding-top: 12px; }");
    auto *seuilsLayout = new QHBoxLayout(grpSeuils);

    seuilsLayout->addWidget(new QLabel("Seuil Temp (°C) :", grpSeuils));
    m_spSeuilTemp = new QDoubleSpinBox(grpSeuils);
    m_spSeuilTemp->setRange(0, 60);
    m_spSeuilTemp->setValue(35.0);
    m_spSeuilTemp->setSingleStep(1.0);
    seuilsLayout->addWidget(m_spSeuilTemp);

    seuilsLayout->addWidget(new QLabel("Seuil Hum Min (%) :", grpSeuils));
    m_spSeuilHum = new QDoubleSpinBox(grpSeuils);
    m_spSeuilHum->setRange(0, 100);
    m_spSeuilHum->setValue(20.0);
    m_spSeuilHum->setSingleStep(5.0);
    seuilsLayout->addWidget(m_spSeuilHum);

    cmdBox->addWidget(grpSeuils, 1);
    mainLayout->addLayout(cmdBox);

    // ── Table des Relevés Météo Enregistrés en Base de Données ──────────────
    auto *grpTable = new QGroupBox("Historique des Relevés Enregistrés en Base de Données (RELEVE_METEO)", this);
    grpTable->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #CBD5E1; border-radius: 8px; margin-top: 5px; padding-top: 12px; }");
    auto *layTable = new QVBoxLayout(grpTable);

    m_tableReleves = new QTableView(grpTable);
    m_tableReleves->setAlternatingRowColors(true);
    m_tableReleves->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableReleves->horizontalHeader()->setStretchLastSection(true);
    m_tableReleves->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layTable->addWidget(m_tableReleves);

    mainLayout->addWidget(grpTable, 2);

    // ── Journal Série (Log) ─────────────────────────────────────────────────
    auto *grpLog = new QGroupBox("Console Série (Traces de communication UART)", this);
    grpLog->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #CBD5E1; border-radius: 8px; margin-top: 5px; padding-top: 12px; }");
    auto *layLog = new QVBoxLayout(grpLog);

    m_logSerial = new QTextEdit(grpLog);
    m_logSerial->setReadOnly(true);
    m_logSerial->setMaximumHeight(90);
    m_logSerial->setStyleSheet("background-color: #0F172A; color: #38BDF8; font-family: Consolas, monospace; font-size: 11px;");
    layLog->addWidget(m_logSerial);

    mainLayout->addWidget(grpLog, 1);
}

void ModuleArduinoWidget::onConnecterClicked()
{
    if (m_arduino->is_connected()) {
        m_arduino->close_arduino();
        m_btnConnecter->setText("🔌 Connecter");
        m_btnConnecter->setStyleSheet("background-color: #0284C7; color: white; font-weight: bold; padding: 6px 14px; border-radius: 6px;");
        m_lblStatut->setText("⚫ Déconnecté");
        m_lblStatut->setStyleSheet("color: #64748B; font-weight: bold;");
        m_logSerial->append("[" + QDateTime::currentDateTime().toString("HH:mm:ss") + "] Port fermé.");
    } else {
        QString port = m_cmbPort->currentText().split(' ').first();
        int res = m_arduino->connect_arduino(port);
        if (res == 0) {
            m_btnConnecter->setText("❌ Déconnecter");
            m_btnConnecter->setStyleSheet("background-color: #EF4444; color: white; font-weight: bold; padding: 6px 14px; border-radius: 6px;");
            m_lblStatut->setText("🟢 Connecté à " + port);
            m_lblStatut->setStyleSheet("color: #16A34A; font-weight: bold;");
            m_logSerial->append("[" + QDateTime::currentDateTime().toString("HH:mm:ss") + "] Connexion établie sur " + port);
        } else {
            // Mode simulation si aucun port physique disponible
            m_lblStatut->setText("🟡 Mode Démo / Simulé");
            m_lblStatut->setStyleSheet("color: #D97706; font-weight: bold;");
            m_logSerial->append("[" + QDateTime::currentDateTime().toString("HH:mm:ss") + "] Port inaccessible. Testez sur Wokwi ou lancez un relevé simulé.");
            // Générer un relevé de test pour montrer le fonctionnement
            onLigneRecue("T:28.40,H:62.50,R:145,GEL:0");
        }
    }
}

void ModuleArduinoWidget::onConnexionChanged(bool connecte, const QString &info)
{
    if (connecte) {
        m_lblStatut->setText("🟢 " + info);
        m_lblStatut->setStyleSheet("color: #16A34A; font-weight: bold;");
    } else {
        m_lblStatut->setText("⚫ " + info);
        m_lblStatut->setStyleSheet("color: #64748B; font-weight: bold;");
    }
}

void ModuleArduinoWidget::onLigneRecue(const QString &ligne)
{
    m_logSerial->append("[" + QDateTime::currentDateTime().toString("HH:mm:ss") + "] RX: " + ligne);

    double temp = 0.0, hum = 0.0;
    int pluie = 0;
    bool gel = false;

    if (StationMeteo::parseLigneSerie(ligne, temp, hum, pluie, gel)) {
        // 1. Mettre à jour l'affichage
        mettreAJourJauges(temp, hum, pluie, gel);

        // 2. Persistance en Base de Données (Condition d'excellence Note A)
        m_station->enregistrerReleve(temp, hum, pluie, gel);

        // 3. Détection de seuils & création d'alerte en DB
        evaluerEtAlerte(temp, hum, pluie, gel);
    }
}

void ModuleArduinoWidget::mettreAJourJauges(double temp, double hum, int pluie, bool gel)
{
    m_lblTemp->setText(QString::number(temp, 'f', 1) + " °C");
    m_barTemp->setValue(static_cast<int>(temp));

    m_lblHum->setText(QString::number(hum, 'f', 1) + " %");
    m_barHum->setValue(static_cast<int>(hum));

    m_lblPluie->setText(QString::number(pluie) + " / 1023");
    m_barPluie->setValue(pluie);

    if (gel) {
        m_lblGel->setText("⚠️ RISQUE DE GEL !");
        m_lblGel->setStyleSheet("font-size: 18px; font-weight: bold; color: #DC2626;");
    } else {
        m_lblGel->setText("NON DÉTECTÉ");
        m_lblGel->setStyleSheet("font-size: 18px; font-weight: bold; color: #16A34A;");
    }
}

void ModuleArduinoWidget::evaluerEtAlerte(double temp, double hum, int pluie, bool gel)
{
    m_station->seuilTempMax = m_spSeuilTemp->value();
    m_station->seuilHumMin  = m_spSeuilHum->value();

    ReleverMeteo r;
    r.temperature = temp;
    r.humidite    = hum;
    r.pluie       = pluie;
    r.gelDetecte  = gel;

    if (m_station->detecterAlerte(r)) {
        QString msg = typeAlerteMessage(temp, hum, pluie, gel);
        m_lblAlerte->setText("⚠️ " + msg);
        m_lblAlerte->setStyleSheet("font-size: 12px; font-weight: bold; color: #DC2626;");

        // Enregistrer l'alerte en base de données
        m_station->creerAlerteBD(r, msg);

        // Déclencher automatiquement l'actionneur Arduino si pas déjà actif
        if (!m_alerteActive && m_arduino->is_connected()) {
            m_arduino->write_to_arduino("ALERT_ON\n");
            m_alerteActive = true;
        }
    } else {
        m_lblAlerte->setText("Statut : Nominal");
        m_lblAlerte->setStyleSheet("font-size: 12px; color: #64748B;");
        if (m_alerteActive && m_arduino->is_connected()) {
            m_arduino->write_to_arduino("ALERT_OFF\n");
            m_alerteActive = false;
        }
    }
}

QString ModuleArduinoWidget::typeAlerteMessage(double temp, double hum, int pluie, bool gel) const
{
    QStringList msgs;
    if (temp > m_spSeuilTemp->value()) msgs << QString("Canicule (%1°C)").arg(temp, 0, 'f', 1);
    if (hum < m_spSeuilHum->value())   msgs << QString("Sécheresse (%1%)").arg(hum, 0, 'f', 1);
    if (pluie > m_station->seuilPluieMax) msgs << QString("Précipitations intenses (%1)").arg(pluie);
    if (gel) msgs << "Gel imminent";
    return msgs.isEmpty() ? "Alerte Météo" : msgs.join(" | ");
}

void ModuleArduinoWidget::onAlertOnClicked()
{
    m_alerteActive = true;
    m_arduino->write_to_arduino("ALERT_ON\n");
    m_logSerial->append("[" + QDateTime::currentDateTime().toString("HH:mm:ss") + "] TX: ALERT_ON");
}

void ModuleArduinoWidget::onAlertOffClicked()
{
    m_alerteActive = false;
    m_arduino->write_to_arduino("ALERT_OFF\n");
    m_logSerial->append("[" + QDateTime::currentDateTime().toString("HH:mm:ss") + "] TX: ALERT_OFF");
}

void ModuleArduinoWidget::onStatusClicked()
{
    m_arduino->write_to_arduino("STATUS\n");
    m_logSerial->append("[" + QDateTime::currentDateTime().toString("HH:mm:ss") + "] TX: STATUS");
}

void ModuleArduinoWidget::rafraichirTableau()
{
    if (m_modelReleves) {
        delete m_modelReleves;
    }
    m_modelReleves = m_station->afficherReleves(30);
    m_tableReleves->setModel(m_modelReleves);
}
