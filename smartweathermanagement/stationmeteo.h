#ifndef STATIONMETEO_H
#define STATIONMETEO_H

// ============================================================================
// PROJET SMART WEATHER MANAGEMENT - MODULE 1 : GESTION DES EMPLOYÉS
// SEMAINE 10 : CLASSE MÉTIER StationMeteo
// ÉTUDIANTE : SALMA RIDENE
//
// Rôle : persistance des relevés Arduino dans la base de données Oracle/SQLite
//        + détection d'alertes météo (température, humidité, gel, pluie)
// ============================================================================

#include <QString>
#include <QSqlQueryModel>

struct ReleverMeteo {
    double temperature = 0.0;   // °C
    double humidite    = 0.0;   // %
    int    pluie       = 0;     // 0–1023 (valeur brute ADC)
    bool   gelDetecte  = false;
    QString dateHeure;          // "YYYY-MM-DD HH:MM:SS"
    QString stationId;          // ex. "Arduino-UNO-INM-01"
};

class StationMeteo {
public:
    // ── Constructeur ───────────────────────────────────────────────────────
    explicit StationMeteo(const QString &stationId = "Arduino-UNO-INM-01");

    // ── DDL : crée les tables si elles n'existent pas ─────────────────────
    static bool initialiserTables();

    // ── CRUD ───────────────────────────────────────────────────────────────
    bool enregistrerReleve(double temp, double hum, int pluie, bool gel);
    QSqlQueryModel *afficherReleves(int limite = 50) const;

    // ── Logique métier ─────────────────────────────────────────────────────
    bool detecterAlerte(const ReleverMeteo &r) const;
    void creerAlerteBD(const ReleverMeteo &r, const QString &message);

    // ── Parsing ligne série ────────────────────────────────────────────────
    // "T:28.50,H:65.30,R:120,GEL:0" → true + valeurs remplies
    static bool parseLigneSerie(const QString &ligne,
                                double &temp, double &hum,
                                int    &pluie, bool &gel);

    // ── Seuils configurables ───────────────────────────────────────────────
    double seuilTempMax  = 35.0;   // °C  → alerte chaleur
    double seuilHumMin   =  20.0;  // %   → alerte sécheresse
    int    seuilPluieMax =  700;   // ADC → alerte pluie forte (sur 1023)

private:
    QString m_stationId;
    static bool tableExiste(const QString &nom);
};

#endif // STATIONMETEO_H

