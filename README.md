# Module 5 – Maintenance & Interventions (Qt C++ / Oracle)

## Mise en route
1. Oracle : exécuter `schema.sql` (adapter STATION / EMPLOYE / HEARTBEAT si elles existent déjà dans vos autres modules).
2. Qt : ouvrir `maintenance.pro` dans Qt Creator. Il faut les modules **Qt Charts**, **Qt SQL** et le plugin **QOCI** (driver Oracle).
3. Lancer une première fois : `config.ini` est créé à côté de l'exécutable. Y mettre host / port / service / user / password.
4. Relancer.

## Fonctionnalités
- CRUD : ajouter, modifier (double-clic), archiver, supprimer
- Tri (priorité, date planifiée, statut, coût) + recherche (station, technicien, type, gouvernorat) + filtre statut
- Export CSV et PDF de l'historique
- Statistiques : camemberts par type / statut, coût par gouvernorat, temps moyen de résolution
- Innovant : Planification préventive prédictive (Heartbeat Score) → suggestion de date + création pré-remplie
- Innovant : Ordre de mission PDF (numéro unique OM-AAAA-xxxxx), délai SLA selon la criticité
  (station aéronautique = délai divisé par 2), lignes en retard colorées en rouge + escalade vers le responsable régional

## SLA utilisé (modifiable dans `SlaModel::slaHours`)
Basse 168 h · Moyenne 72 h · Haute 24 h · Urgente 4 h (÷2 si station aéronautique)
