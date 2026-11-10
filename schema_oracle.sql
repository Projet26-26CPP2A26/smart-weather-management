-- ============================================================================
--  SMART WEATHER MANAGEMENT - Base Oracle (a executer dans SQL Developer)
-- ============================================================================
--  ETAPE 0 (une seule fois, connecte en SYS ou SYSTEM) :
--
--    CREATE USER smartweather IDENTIFIED BY "esprit123";
--    GRANT CONNECT, RESOURCE, UNLIMITED TABLESPACE TO smartweather;
--
--  ETAPE 1 : se connecter en smartweather / esprit123 sur votre PDB
--            (ex. localhost:1521/XEPDB1) puis executer tout ce script.
--
--  ETAPE 2 (cote Windows) : creer une source ODBC 64 bits
--      Panneau de configuration > Outils d'administration > Sources de
--      donnees ODBC (64 bits) > System DSN > Ajouter > Oracle in ...
--      Nom : MonOracleDSN   /   TNS Service Name : XEPDB1
--      User : smartweather  /   Test Connection : OK
--
--  L'application se connecte automatiquement a ce DSN ; si Oracle est
--  inaccessible elle bascule sur la base SQLite locale (smartweather.db).
--  Le compte de connexion (login "1" / mot de passe "1") est cree
--  automatiquement par l'application au premier demarrage si la table
--  UTILISATEUR est vide.
-- ============================================================================

-- ---------------------------------------------------------------------------
-- 1. TABLES
-- ---------------------------------------------------------------------------
-- Si le schema existe deja et doit etre recree, decommenter ces 7 lignes :
-- DROP TABLE attestation;
-- DROP TABLE intervention;
-- DROP TABLE alertes;
-- DROP TABLE releve_meteo;
-- DROP TABLE station;
-- DROP TABLE employe;
-- DROP TABLE utilisateur;

CREATE TABLE utilisateur (
    login         VARCHAR2(50)  NOT NULL,
    mot_de_passe  VARCHAR2(64)  NOT NULL,
    sel           VARCHAR2(64)  NOT NULL,
    role          VARCHAR2(50)  NOT NULL,
    CONSTRAINT pk_utilisateur PRIMARY KEY (login)
);

CREATE TABLE employe (
    id_employe        VARCHAR2(10) NOT NULL,
    nom               VARCHAR2(60),
    prenom            VARCHAR2(60),
    email             VARCHAR2(120),
    poste             VARCHAR2(80),
    structure         VARCHAR2(40),
    gouvernorat       VARCHAR2(40),
    date_recrutement  DATE,
    statut            VARCHAR2(30),
    type_horaire      VARCHAR2(30),
    CONSTRAINT pk_employe PRIMARY KEY (id_employe)
);

-- NOM est un alias de NOM_STATION : certains modules lisent NOM, d'autres
-- NOM_STATION. La colonne calculee evite toute desynchronisation.
CREATE TABLE station (
    id_station         VARCHAR2(10) NOT NULL,
    nom_station        VARCHAR2(80) NOT NULL,
    nom                VARCHAR2(80) GENERATED ALWAYS AS (nom_station) VIRTUAL,
    type_station       VARCHAR2(40),
    gouvernorat        VARCHAR2(40),
    altitude           NUMBER(6,1),
    statut             VARCHAR2(30),
    date_installation  DATE,
    latitude           NUMBER(9,5),
    longitude          NUMBER(9,5),
    CONSTRAINT pk_station PRIMARY KEY (id_station)
);

CREATE TABLE releve_meteo (
    id_releve      NUMBER(10) NOT NULL,
    id_station     VARCHAR2(10),
    horodatage     TIMESTAMP,
    temperature    NUMBER(5,1),
    humidite       NUMBER(5,1),
    pression       NUMBER(7,1),
    vitesse_vent   NUMBER(5,1),
    precipitation  NUMBER(6,1),
    mode_collecte  VARCHAR2(30),
    CONSTRAINT pk_releve PRIMARY KEY (id_releve)
);

CREATE TABLE alertes (
    id_alerte        VARCHAR2(20) NOT NULL,
    type_alerte      VARCHAR2(60),
    niveau_severite  VARCHAR2(20),
    date_emission    TIMESTAMP,
    date_fin         TIMESTAMP,
    id_station       VARCHAR2(10),
    gouvernorat      VARCHAR2(40),
    statut           VARCHAR2(30),
    description      VARCHAR2(500),
    CONSTRAINT pk_alertes PRIMARY KEY (id_alerte)
);

CREATE TABLE intervention (
    id_intervention      NUMBER(10) NOT NULL,
    id_station           VARCHAR2(10),
    id_employe           VARCHAR2(10),
    type_intervention    VARCHAR2(60),
    priorite             VARCHAR2(20),
    date_creation        TIMESTAMP,
    date_planifiee       TIMESTAMP,
    date_cloture         TIMESTAMP,
    statut               VARCHAR2(30),
    cout                 NUMBER(10,2),
    rapport_intervention VARCHAR2(1000),
    archive              NUMBER(1) DEFAULT 0 NOT NULL,
    CONSTRAINT pk_intervention PRIMARY KEY (id_intervention)
);

CREATE TABLE attestation (
    id_attestation   NUMBER(10) NOT NULL,
    numero           VARCHAR2(40),
    demandeur        VARCHAR2(120),
    lieu             VARCHAR2(80),
    latitude         NUMBER(9,5),
    longitude        NUMBER(9,5),
    date_debut       DATE,
    date_fin         DATE,
    id_station       VARCHAR2(10),
    statut           VARCHAR2(30),
    date_demande     TIMESTAMP,
    date_traitement  TIMESTAMP,
    CONSTRAINT pk_attestation PRIMARY KEY (id_attestation)
);

-- ---------------------------------------------------------------------------
-- 2. DONNEES DE DEMONSTRATION (facultatif : supprimer cette section pour une
--    base vide ; l'application fonctionne avec les tables vides)
-- ---------------------------------------------------------------------------

INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire) VALUES ('101', 'Ben Salah', 'Ali', 'ali.bensalah@inm.tn', 'Ingénieur prévisionniste', 'Siège', 'Tunis', TO_DATE('12/03/2015','DD/MM/YYYY'), 'Actif', 'Jour');
INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire) VALUES ('102', 'Trabelsi', 'Salma', 'salma.trabelsi@inm.tn', 'Agent d''observation', 'Régionale', 'Sousse', TO_DATE('03/09/2018','DD/MM/YYYY'), 'Actif', 'Garde 24h');
INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire) VALUES ('103', 'Khelifi', 'Mohamed', 'mohamed.khelifi@inm.tn', 'Technicien de maintenance', 'Régionale', 'Sfax', TO_DATE('20/01/2016','DD/MM/YYYY'), 'Actif', 'Jour');
INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire) VALUES ('104', 'Bouazizi', 'Ines', 'ines.bouazizi@inm.tn', 'Ingénieur prévisionniste', 'Régionale', 'Nabeul', TO_DATE('15/06/2020','DD/MM/YYYY'), 'Actif', 'Garde 24h');
INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire) VALUES ('105', 'Gharbi', 'Youssef', 'youssef.gharbi@inm.tn', 'Technicien de maintenance', 'Siège', 'Tunis', TO_DATE('05/11/2019','DD/MM/YYYY'), 'Actif', 'Jour');
INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire) VALUES ('106', 'Jebali', 'Amira', 'amira.jebali@inm.tn', 'Agent d''observation', 'Régionale', 'Bizerte', TO_DATE('18/02/2021','DD/MM/YYYY'), 'En congé', 'Garde 24h');
INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire) VALUES ('107', 'Ayari', 'Karim', 'karim.ayari@inm.tn', 'Agent d''observation', 'Régionale', 'Gabès', TO_DATE('30/07/2017','DD/MM/YYYY'), 'Actif', 'Garde 24h');
INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire) VALUES ('108', 'Chaabane', 'Nadia', 'nadia.chaabane@inm.tn', 'Administratif', 'Siège', 'Ariana', TO_DATE('02/04/2012','DD/MM/YYYY'), 'Actif', 'Jour');
INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire) VALUES ('109', 'Zouari', 'Hatem', 'hatem.zouari@inm.tn', 'Technicien de maintenance', 'Régionale', 'Kairouan', TO_DATE('10/10/2022','DD/MM/YYYY'), 'Actif', 'Jour');
INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire) VALUES ('110', 'Mzoughi', 'Rania', 'rania.mzoughi@inm.tn', 'Agent d''observation', 'Siège', 'Tunis', TO_DATE('05/01/2026','DD/MM/YYYY'), 'Stagiaire', 'Jour');
INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire) VALUES ('111', 'Ben Amor', 'Sami', 'sami.benamor@inm.tn', 'Agent de service', 'Siège', 'Ben Arous', TO_DATE('21/08/2014','DD/MM/YYYY'), 'Actif', 'Jour');
INSERT INTO employe (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire) VALUES ('112', 'Ferchichi', 'Leila', 'leila.ferchichi@inm.tn', 'Ingénieur prévisionniste', 'Régionale', 'Médenine', TO_DATE('09/05/2023','DD/MM/YYYY'), 'Actif', 'Garde 24h');

INSERT INTO station (id_station, nom_station, type_station, gouvernorat, altitude, statut, date_installation, latitude, longitude) VALUES ('1', 'Tunis-Carthage', 'Aéronautique', 'Tunis', 4, 'Active', TO_DATE('15/01/1998','DD/MM/YYYY'), 36.8510, 10.2270);
INSERT INTO station (id_station, nom_station, type_station, gouvernorat, altitude, statut, date_installation, latitude, longitude) VALUES ('2', 'Sousse', 'Synoptique', 'Sousse', 25, 'Active', TO_DATE('20/05/2003','DD/MM/YYYY'), 35.8256, 10.6084);
INSERT INTO station (id_station, nom_station, type_station, gouvernorat, altitude, statut, date_installation, latitude, longitude) VALUES ('3', 'Sfax', 'Synoptique', 'Sfax', 22, 'Active', TO_DATE('12/09/2005','DD/MM/YYYY'), 34.7406, 10.7603);
INSERT INTO station (id_station, nom_station, type_station, gouvernorat, altitude, statut, date_installation, latitude, longitude) VALUES ('4', 'Bizerte', 'Climatologique', 'Bizerte', 12, 'En maintenance', TO_DATE('08/03/2010','DD/MM/YYYY'), 37.2744, 9.8739);
INSERT INTO station (id_station, nom_station, type_station, gouvernorat, altitude, statut, date_installation, latitude, longitude) VALUES ('5', 'Gabès', 'Climatologique', 'Gabès', 8, 'Active', TO_DATE('01/11/2012','DD/MM/YYYY'), 33.8815, 10.0982);
INSERT INTO station (id_station, nom_station, type_station, gouvernorat, altitude, statut, date_installation, latitude, longitude) VALUES ('6', 'Kairouan', 'Radar', 'Kairouan', 62, 'Active', TO_DATE('25/06/2008','DD/MM/YYYY'), 35.6781, 10.0963);

INSERT INTO alertes (id_alerte, type_alerte, niveau_severite, date_emission, date_fin, id_station, gouvernorat, statut, description) VALUES ('A-2026-001', 'Vents forts', 'Orange', SYSTIMESTAMP - INTERVAL '1' DAY, SYSTIMESTAMP + INTERVAL '2' DAY, '1', 'Tunis', 'En cours', 'Rafales de 90 km/h attendues sur le littoral nord.');
INSERT INTO alertes (id_alerte, type_alerte, niveau_severite, date_emission, date_fin, id_station, gouvernorat, statut, description) VALUES ('A-2026-002', 'Pluies intenses', 'Rouge', SYSTIMESTAMP, SYSTIMESTAMP + INTERVAL '3' DAY, '4', 'Bizerte', 'En cours', 'Cumuls supérieurs à 60 mm en 24 h, risque d''inondation.');
INSERT INTO alertes (id_alerte, type_alerte, niveau_severite, date_emission, date_fin, id_station, gouvernorat, statut, description) VALUES ('A-2026-003', 'Brouillard dense', 'Jaune', SYSTIMESTAMP - INTERVAL '2' DAY, SYSTIMESTAMP + INTERVAL '1' DAY, '2', 'Sousse', 'En cours', 'Visibilité inférieure à 200 m entre 04 h et 09 h.');
INSERT INTO alertes (id_alerte, type_alerte, niveau_severite, date_emission, date_fin, id_station, gouvernorat, statut, description) VALUES ('A-2026-004', 'Chaleur', 'Orange', SYSTIMESTAMP - INTERVAL '12' DAY, SYSTIMESTAMP + INTERVAL '9' DAY, '3', 'Sfax', 'Archivée', 'Températures maximales proches de 44 °C.');
INSERT INTO alertes (id_alerte, type_alerte, niveau_severite, date_emission, date_fin, id_station, gouvernorat, statut, description) VALUES ('A-2026-005', 'Orages', 'Jaune', SYSTIMESTAMP - INTERVAL '20' DAY, SYSTIMESTAMP + INTERVAL '19' DAY, '6', 'Kairouan', 'Terminée', 'Orages isolés avec fortes rafales.');
INSERT INTO alertes (id_alerte, type_alerte, niveau_severite, date_emission, date_fin, id_station, gouvernorat, statut, description) VALUES ('A-2026-006', 'Vents de sable', 'Orange', SYSTIMESTAMP - INTERVAL '30' DAY, SYSTIMESTAMP + INTERVAL '28' DAY, '5', 'Gabès', 'Archivée', 'Réduction de visibilité liée au sable soulevé.');

INSERT INTO intervention (id_intervention, id_station, id_employe, type_intervention, priorite, date_creation, date_planifiee, date_cloture, statut, cout, rapport_intervention, archive) VALUES (1, '1', '103', 'Préventive', 'Haute', SYSTIMESTAMP - INTERVAL '4' DAY, SYSTIMESTAMP + INTERVAL '2' DAY, NULL, 'En cours', 450.00, NULL, 0);
INSERT INTO intervention (id_intervention, id_station, id_employe, type_intervention, priorite, date_creation, date_planifiee, date_cloture, statut, cout, rapport_intervention, archive) VALUES (2, '4', '105', 'Corrective', 'Urgente', SYSTIMESTAMP - INTERVAL '6' DAY, SYSTIMESTAMP + INTERVAL '5' DAY, NULL, 'En cours', 1200.00, NULL, 0);
INSERT INTO intervention (id_intervention, id_station, id_employe, type_intervention, priorite, date_creation, date_planifiee, date_cloture, statut, cout, rapport_intervention, archive) VALUES (3, '2', '109', 'Calibration', 'Moyenne', SYSTIMESTAMP - INTERVAL '20' DAY, SYSTIMESTAMP + INTERVAL '18' DAY, SYSTIMESTAMP + INTERVAL '17' DAY, 'Terminée', 300.00, 'Capteur de pression recalibré.', 0);
INSERT INTO intervention (id_intervention, id_station, id_employe, type_intervention, priorite, date_creation, date_planifiee, date_cloture, statut, cout, rapport_intervention, archive) VALUES (4, '3', '103', 'Remplacement capteur', 'Basse', SYSTIMESTAMP - INTERVAL '30' DAY, SYSTIMESTAMP + INTERVAL '25' DAY, SYSTIMESTAMP + INTERVAL '24' DAY, 'Terminée', 780.00, 'Anémomètre remplacé.', 0);
INSERT INTO intervention (id_intervention, id_station, id_employe, type_intervention, priorite, date_creation, date_planifiee, date_cloture, statut, cout, rapport_intervention, archive) VALUES (5, '5', '105', 'Préventive', 'Moyenne', SYSTIMESTAMP - INTERVAL '2' DAY, SYSTIMESTAMP + INTERVAL '3' DAY, NULL, 'Planifiée', 250.00, NULL, 0);

-- Releves meteo : 46 jours x 3 mesures/jour (00 h, 08 h, 16 h) x 3 stations.
DECLARE
    v_id  NUMBER := 0;
    v_tmp NUMBER(5,1);
    v_hum NUMBER(5,1);
    v_pre NUMBER(7,1);
    v_ven NUMBER(5,1);
BEGIN
    FOR j IN 0..45 LOOP
        FOR h IN 0..2 LOOP
            FOR st IN 1..3 LOOP
                v_id  := v_id + 1;
                v_tmp := ROUND(18 + st * 1.3 + 6 * SIN((45 - j) / 7) + COS((45 - j) + h * 8) * 2.5, 1);
                v_hum := ROUND(55 + 12 * SIN((45 - j) / 4 + st), 1);
                v_pre := ROUND(1013 + 4 * COS((45 - j) / 5), 1);
                v_ven := ROUND(8 + ABS(SIN((45 - j) + st)) * 14, 1);

                INSERT INTO releve_meteo (id_releve, id_station, horodatage, temperature, humidite,
                                          pression, vitesse_vent, precipitation, mode_collecte)
                VALUES (v_id, TO_CHAR(st),
                        SYSTIMESTAMP - NUMTODSINTERVAL(j * 24 + h * 8, 'HOUR'),
                        v_tmp, v_hum, v_pre, v_ven,
                        CASE WHEN MOD(45 - j, 9) = 0 AND h = 1 THEN 3.4 ELSE 0 END,
                        'Automatique');
            END LOOP;
        END LOOP;
    END LOOP;
END;
/

COMMIT;

-- ---------------------------------------------------------------------------
-- 3. VERIFICATION
-- ---------------------------------------------------------------------------
-- SELECT COUNT(*) FROM employe;         -- 12
-- SELECT COUNT(*) FROM station;         -- 6
-- SELECT COUNT(*) FROM releve_meteo;    -- 414
-- SELECT COUNT(*) FROM alertes;         -- 6
-- SELECT COUNT(*) FROM intervention;    -- 5
-- SELECT login, role FROM utilisateur;  -- "1" cree au 1er lancement de l'app
