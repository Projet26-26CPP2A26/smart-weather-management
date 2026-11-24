#ifndef MODULEARDUINOWIDGET_H
#define MODULEARDUINOWIDGET_H

// ============================================================================
// PROJET SMART WEATHER MANAGEMENT - MODULE 1
// SEMAINE 10 : WIDGET PANNEAU ARDUINO (Station Météo)
// ÉTUDIANTE : SALMA RIDENE
//
// Ce widget gère :
//   - La connexion/déconnexion au port série Arduino
//   - L'affichage en temps réel des relevés (temp, hum, pluie)
//   - La détection automatique des alertes + écriture en DB
//   - L'envoi de commandes (ALERT_ON / ALERT_OFF) vers l'Arduino
//   - Un tableau des derniers relevés (depuis RELEVE_METEO)
// ============================================================================

#include <QWidget>
#include <QTimer>
#include "arduino.h"
#include "stationmeteo.h"

class QLabel;
class QComboBox;
class QPushButton;
class QTableView;
class QSqlQueryModel;
class QProgressBar;
class QDoubleSpinBox;
class QTextEdit;

class ModuleArduinoWidget : public QWidget {
    Q_OBJECT
public:
    explicit ModuleArduinoWidget(QWidget *parent = nullptr);
    ~ModuleArduinoWidget();

private slots:
    void onConnecterClicked();
    void onLigneRecue(const QString &ligne);
    void onConnexionChanged(bool connecte, const QString &info);
    void onAlertOnClicked();
    void onAlertOffClicked();
    void onStatusClicked();
    void rafraichirTableau();

private:
    void construireUI();
    void mettreAJourJauges(double temp, double hum, int pluie, bool gel);
    void evaluerEtAlerte(double temp, double hum, int pluie, bool gel);
    QString typeAlerteMessage(double temp, double hum, int pluie, bool gel) const;

    // ── Objets métier ───────────────────────────────────────────────────────
    Arduino        *m_arduino      = nullptr;
    StationMeteo   *m_station      = nullptr;
    QTimer         *m_timerRefresh = nullptr;
    bool            m_alerteActive = false;

    // ── Widgets UI ──────────────────────────────────────────────────────────
    QComboBox    *m_cmbPort        = nullptr;
    QPushButton  *m_btnConnecter   = nullptr;
    QLabel       *m_lblStatut      = nullptr;

    QLabel       *m_lblTemp        = nullptr;
    QLabel       *m_lblHum         = nullptr;
    QLabel       *m_lblPluie       = nullptr;
    QLabel       *m_lblGel         = nullptr;
    QProgressBar *m_barTemp        = nullptr;
    QProgressBar *m_barHum         = nullptr;
    QProgressBar *m_barPluie       = nullptr;

    QLabel       *m_lblAlerte      = nullptr;
    QPushButton  *m_btnAlertOn     = nullptr;
    QPushButton  *m_btnAlertOff    = nullptr;
    QPushButton  *m_btnStatus      = nullptr;

    QDoubleSpinBox *m_spSeuilTemp  = nullptr;
    QDoubleSpinBox *m_spSeuilHum   = nullptr;

    QTextEdit    *m_logSerial      = nullptr;
    QTableView   *m_tableReleves   = nullptr;
    QSqlQueryModel *m_modelReleves = nullptr;
};

#endif // MODULEARDUINOWIDGET_H

