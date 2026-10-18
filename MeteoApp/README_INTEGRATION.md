# MeteoApp (Module 3) + Module 5 : Maintenance & Interventions

## Ce qui a ete ajoute
- Bouton violet **« 🛠 Maintenance (Module 5) »** dans la fenetre du Module 3 -> ouvre la fenetre Module 5
  (meme connexion Oracle que le Module 3 : variables ORA_DB / ORA_USER / ORA_PASS).
- Fichiers Module 5 : `maintenance_window.*`, `intervention_dialog.*`, `predictive_dialog.h`, `maintenance_db.h`.
  Le style du Module 5 est integre dans ses propres .ui : il ne modifie pas l'apparence du Module 3.
- `schema_module5.sql` : a executer **apres** `schema.sql` du Module 3.

## Mise en route
1. Executer `schema.sql` (Module 3) puis `schema_module5.sql` dans Oracle.
2. Ouvrir `MeteoApp.pro` dans Qt Creator (Qt 6, driver QOCI), compiler, lancer.
3. Module 3 -> bouton « Maintenance (Module 5) » -> choisir le role.

## Liens entre les modules
| Besoin du Module 5 | Source reelle |
|---|---|
| Stations (liste, nom) | table `STATION` du Module 3 (+ colonnes `GOUVERNORAT`, `TYPE_STATION` ajoutees par le script) |
| Anomalies capteur | vue `V_ANOMALIES_RECENTES` : meme regle que « Anomalies capteurs » du Module 3 (> 3 ecarts-types, critique > 4), sur les releves de `RELEVE_METEO` des 30 derniers jours |
| Heartbeat Score (module 2) | vue `V_HEARTBEAT(ID_STATION, SCORE, DATE_MESURE)` |
| Alerte rouge en cours | Heartbeat < 40 **ou** anomalie critique |
| Techniciens | table `EMPLOYE` (ID_EMPLOYE, NOM, PRENOM) du Module 1 |

## A adapter (je n'ai pas vu les modules 1, 2 et 4)
- **Module 2 (Heartbeat)** : par defaut `V_HEARTBEAT` est *calculee* a partir des anomalies du Module 3.
  Si le Module 2 a sa table de scores, remplacez la vue (exemple dans `schema_module5.sql`).
- **Module 1 (employes)** : si votre table n'a pas les colonnes `ID_EMPLOYE`, `NOM`, `PRENOM`, adaptez
  les requetes dans `maintenance_window.cpp` et `intervention_dialog.h` (recherche : `EMPLOYE`).
- **Module 4 (alertes)** : l'« alerte rouge » ne lit pas encore la table des alertes du Module 4.
  Donnez-moi son nom et ses colonnes pour la brancher.
- **Role** : si une application hote definit `qApp->setProperty("role", "Chef de maintenance")` apres le login,
  le Module 5 reprend ce role sans le redemander. Valeurs : Chef de maintenance, Technicien de maintenance,
  Administrateur réseau, Responsable régional.
