#ifndef PREDICTIONGEL_H
#define PREDICTIONGEL_H

#include <QString>
#include <QStringList>

struct ResultatPredictionGel {
    double tMinEstimee;          // Température minimale nocturne prévue (°C)
    double pointDeRosee;         // Point de rosée calculé (°C)
    double probabiliteGel;       // Probabilité de gelée (0% à 100%)
    QString niveauRisque;        // "Nul", "Modéré (Jaune)", "Élevé (Orange)", "Critique (Rouge)"
    QString niveauSeveriteAlerte;// "Jaune", "Orange", "Rouge", "Aucune"
    QString typeGel;             // "Gel de rayonnement", "Gel d'advection", "Gel mixte"
    QString diagnostic;          // Diagnostic agrométéorologique détaillé
    QStringList recommandations; // Mesures de prévention agricole conseillées
};

class PredictionGel
{
public:
    PredictionGel();

    // Calcul principal de la prédiction du gel nocturne
    static ResultatPredictionGel calculer(double tempActuelle, 
                                          double humidite, 
                                          double ventKmH, 
                                          int nebulositeOctas,
                                          bool solHumide = false);

    // Calcul précis du point de rosée par formule de Magnus-Tetens
    static double calculerPointDeRosee(double temp, double humidite);
};

#endif // PREDICTIONGEL_H
