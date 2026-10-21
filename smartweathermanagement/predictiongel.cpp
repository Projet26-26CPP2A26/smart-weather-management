#include "predictiongel.h"
#include <cmath>
#include <algorithm>

PredictionGel::PredictionGel()
{
}

double PredictionGel::calculerPointDeRosee(double temp, double humidite)
{
    // Formule empirique de Magnus-Tetens (norme OMM / WMO)
    // a = 17.27, b = 237.7
    if (humidite <= 0.0) humidite = 1.0;
    if (humidite > 100.0) humidite = 100.0;

    const double a = 17.27;
    const double b = 237.7;
    double alpha = ((a * temp) / (b + temp)) + std::log(humidite / 100.0);
    double dp = (b * alpha) / (a - alpha);
    return dp;
}

ResultatPredictionGel PredictionGel::calculer(double tempActuelle, 
                                              double humidite, 
                                              double ventKmH, 
                                              int nebulositeOctas,
                                              bool solHumide)
{
    ResultatPredictionGel res;

    // 1. Calcul du point de rosée
    res.pointDeRosee = calculerPointDeRosee(tempActuelle, humidite);

    // 2. Facteur de refroidissement radiatif nocturne selon nébulosité (0 ciel clair, 8 couvert)
    // Ciel clair (0 octas) => forte déperdition IR vers l'espace
    double facteurCiel = 1.0 - (0.095 * nebulositeOctas);
    if (facteurCiel < 0.2) facteurCiel = 0.2;

    // 3. Facteur d'effet du vent
    // Un vent très faible (< 4 km/h) favorise une stratification thermique et un refroidissement extrême au sol
    // Un vent fort brasse l'air et réduit le gel radiatif
    double facteurVent = 1.0;
    if (ventKmH <= 4.0) {
        facteurVent = 1.25; // amplification du gel radiatif au sol
    } else if (ventKmH <= 12.0) {
        facteurVent = 1.0;
    } else if (ventKmH <= 25.0) {
        facteurVent = 0.70; // brassage modéré
    } else {
        facteurVent = 0.45; // fort brassage (mais risque de gel d'advection si masse d'air polaire)
    }

    // 4. Estimation de la baisse de température nocturne (modèle de Brunt adapté)
    // La baisse tend à approcher le point de rosée avec une chute radiative supplémentaire
    double ecartRosee = tempActuelle - res.pointDeRosee;
    double chuteBase = ecartRosee * 0.55;
    double perteRadiative = 4.2 * facteurCiel * facteurVent;
    if (solHumide) {
        perteRadiative *= 0.85; // inertie thermique de l'eau
    }

    double deltaT = chuteBase + perteRadiative;
    res.tMinEstimee = tempActuelle - deltaT;

    // Correction physique : la température minimale dépasse rarement le point de rosée de plus de 2.0°C en atmosphère calme
    if (res.tMinEstimee < res.pointDeRosee - 3.0) {
        res.tMinEstimee = res.pointDeRosee - 2.5;
    }

    // 5. Détermination du type de gel
    if (ventKmH > 18.0 && res.tMinEstimee <= 0.0) {
        res.typeGel = "Gel d'advection (Masse d'air froide en mouvement)";
    } else if (nebulositeOctas <= 2 && ventKmH <= 7.0 && res.tMinEstimee <= 2.0) {
        res.typeGel = "Gel radiatif nocturne (Ciel dégagé, air calme)";
    } else {
        res.typeGel = "Gel mixte (Conditions combinées radiatives/thermiques)";
    }

    // 6. Calcul de la probabilité et classification du risque
    if (res.tMinEstimee <= -2.0) {
        res.probabiliteGel = 95.0;
        res.niveauRisque = "Critique (Gel Sévère / Gel Noir)";
        res.niveauSeveriteAlerte = "Rouge";
        res.diagnostic = QString("Alerte Maximale : Température minimale prévue à %1°C. "
                                 "Risque de gel noir sans condensation visible, destructeur pour les bourgeons et cultures hâtives.")
                         .arg(res.tMinEstimee, 0, 'f', 1);
        res.recommandations << "Activer immédiatement les tours à vent (éoliennes antigel) et brassage d'air."
                            << "Démarrer le chauffage des vergers (bougies de paraffine / brûleurs à gaz)."
                            << "Déclencher l'aspersion antigel continue avant le franchissement du 0°C."
                            << "Aviser sans délai la Protection Civile et les coopératives agricoles régionales.";
    } 
    else if (res.tMinEstimee <= 0.0) {
        res.probabiliteGel = 80.0;
        res.niveauRisque = "Élevé (Gel Blanc Fréquent)";
        res.niveauSeveriteAlerte = "Orange";
        res.diagnostic = QString("Vigilance Renforcée : Température prévue autour de %1°C avec point de rosée à %2°C. "
                                 "Risque élevé de formation de gelée blanche au lever du jour.")
                         .arg(res.tMinEstimee, 0, 'f', 1).arg(res.pointDeRosee, 0, 'f', 1);
        res.recommandations << "Déclencher l'aspersion sur frondaison dès que la température atteint +0.5°C."
                            << "Couvrir les pépinières et cultures maraîchères sous voile thermique P17."
                            << "Maintenir la veille météorologique active jusqu'à 08h00.";
    } 
    else if (res.tMinEstimee <= 2.5) {
        res.probabiliteGel = 45.0;
        res.niveauRisque = "Modéré (Gelée Blanche au sol possible)";
        res.niveauSeveriteAlerte = "Jaune";
        res.diagnostic = QString("Vigilance Jaune : Température sous abri prévue à %1°C. "
                                 "En raison du rayonnement, la température au ras de l'herbe peut chuter sous 0°C.")
                         .arg(res.tMinEstimee, 0, 'f', 1);
        res.recommandations << "Surveiller les sondes de température au sol dans les cuvettes topographiques."
                            << "Préparer le matériel de lutte préventive en cas de baisse plus rapide."
                            << "Éviter tout travail du sol qui accentuerait la perte calorifique.";
    } 
    else {
        res.probabiliteGel = 10.0;
        res.niveauRisque = "Nul à Très Faible (Conditions Sûres)";
        res.niveauSeveriteAlerte = "Aucune";
        res.diagnostic = QString("Conditions stables : Température minimale estimée à %1°C. "
                                 "Aucun risque de gelée nocturne détecté pour les prochaines 12 heures.")
                         .arg(res.tMinEstimee, 0, 'f', 1);
        res.recommandations << "Aucune mesure d'urgence requise."
                            << "Poursuivre le suivi météorologique de routine.";
    }

    return res;
}
