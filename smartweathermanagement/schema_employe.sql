-- ============================================================================
-- PROJET SMART WEATHER MANAGEMENT - MODULE 1 : GESTION DES EMPLOYÉS
-- SEMAINE 8 : VALIDATION DU MODÈLE PHYSIQUE DE DONNÉES (AA2.3)
-- ÉTUDIANTE : SALMA RIDENE
-- BASE DE DONNÉES : ORACLE 11g / 19c / 21c (via QODBC - DSN MonOracleDSN)
-- ============================================================================

-- 1. NETTOYAGE PRÉALABLE (IDEMPOTENCE DU SCRIPT)
DROP TABLE EMPLOYE CASCADE CONSTRAINTS;
DROP SEQUENCE seq_employe;

-- 2. CRÉATION DE LA TABLE PHYSIQUE 'EMPLOYE' AVEC TOUTES LES CONTRAINTES
CREATE TABLE EMPLOYE (
    id_employe       VARCHAR2(50),
    nom              VARCHAR2(100) NOT NULL,
    prenom           VARCHAR2(100) NOT NULL,
    email            VARCHAR2(150) NOT NULL,
    poste            VARCHAR2(100) NOT NULL,
    structure        VARCHAR2(50)  NOT NULL,
    gouvernorat      VARCHAR2(100) NOT NULL,
    date_recrutement VARCHAR2(20)  NOT NULL,
    statut           VARCHAR2(50)  NOT NULL,
    type_horaire     VARCHAR2(50)  NOT NULL,

    -- CONTRAINTES D'INTÉGRITÉ PHYSIQUE (AA2.3)
    CONSTRAINT pk_employe PRIMARY KEY (id_employe),
    CONSTRAINT uk_employe_email UNIQUE (email),
    CONSTRAINT chk_employe_poste CHECK (poste IN (
        'Ingénieur prévisionniste',
        'Technicien de maintenance',
        'Agent d''observation',
        'Agent de service',
        'Administratif',
        'Responsable station',
        'Informaticien'
    )),
    CONSTRAINT chk_employe_structure CHECK (structure IN ('Siège', 'Régionale')),
    CONSTRAINT chk_employe_statut CHECK (statut IN ('Actif', 'En congé', 'Stagiaire', 'Archivé')),
    CONSTRAINT chk_employe_horaire CHECK (type_horaire IN ('Jour', 'Garde 24h'))
);

-- 3. CRÉATION D'INDEX POUR L'OPTIMISATION DES RECHERCHES ET DES TRIS (AA2.3 / AA3.4)
CREATE INDEX idx_employe_nom ON EMPLOYE(nom, prenom);
CREATE INDEX idx_employe_poste ON EMPLOYE(poste);
CREATE INDEX idx_employe_gouv ON EMPLOYE(gouvernorat);

-- 4. SÉQUENCE POUR ÉVENTUELLE GÉNÉRATION AUTOMATIQUE D'IDENTIFIANTS
CREATE SEQUENCE seq_employe START WITH 109 INCREMENT BY 1 NOCACHE;

-- 5. JEU D'ESSAI VALIDE POUR LES TESTS DES 4 CRUDS (AA3.1 / AA3.4.2)
INSERT INTO EMPLOYE (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire)
VALUES ('101', 'Ridene', 'Salma', 'salma.ridene@inm.tn', 'Ingénieur prévisionniste', 'Siège', 'Tunis', '2022-09-15', 'Actif', 'Garde 24h');

INSERT INTO EMPLOYE (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire)
VALUES ('102', 'Ben Amor', 'Yassine', 'yassine.benamor@inm.tn', 'Technicien de maintenance', 'Régionale', 'Jendouba', '2021-03-10', 'Actif', 'Garde 24h');

INSERT INTO EMPLOYE (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire)
VALUES ('103', 'Trabelsi', 'Nour', 'nour.trabelsi@inm.tn', 'Agent d''observation', 'Régionale', 'Bizerte', '2023-01-20', 'Actif', 'Jour');

INSERT INTO EMPLOYE (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire)
VALUES ('104', 'Mejri', 'Karim', 'karim.mejri@inm.tn', 'Ingénieur prévisionniste', 'Siège', 'Tunis', '2020-06-01', 'Actif', 'Jour');

INSERT INTO EMPLOYE (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire)
VALUES ('105', 'Gharbi', 'Sarra', 'sarra.gharbi@inm.tn', 'Technicien de maintenance', 'Régionale', 'Sfax', '2021-11-12', 'Actif', 'Garde 24h');

INSERT INTO EMPLOYE (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire)
VALUES ('106', 'Khemiri', 'Ahmed', 'ahmed.khemiri@inm.tn', 'Agent d''observation', 'Régionale', 'Siliana', '2022-04-18', 'En congé', 'Jour');

INSERT INTO EMPLOYE (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire)
VALUES ('107', 'Mansouri', 'Ines', 'ines.mansouri@inm.tn', 'Administratif', 'Siège', 'Tunis', '2019-10-05', 'Actif', 'Jour');

INSERT INTO EMPLOYE (id_employe, nom, prenom, email, poste, structure, gouvernorat, date_recrutement, statut, type_horaire)
VALUES ('108', 'Ayari', 'Mohamed', 'mohamed.ayari@inm.tn', 'Technicien de maintenance', 'Régionale', 'Kasserine', '2023-08-01', 'Stagiaire', 'Jour');

COMMIT;

-- 6. TABLE POUR LE JOURNAL D'AUDIT ET TRAÇABILITÉ DES ACTIONS RH (SEMAINE 9 : MÉTIERS)
DROP TABLE HISTORIQUE_RH CASCADE CONSTRAINTS;
DROP SEQUENCE seq_historique_rh;

CREATE TABLE HISTORIQUE_RH (
    id_log       NUMBER,
    date_action  VARCHAR2(30)  NOT NULL,
    utilisateur  VARCHAR2(50)  NOT NULL,
    action       VARCHAR2(100) NOT NULL,
    details      VARCHAR2(255) NOT NULL,
    CONSTRAINT pk_historique_rh PRIMARY KEY (id_log)
);

CREATE SEQUENCE seq_historique_rh START WITH 1 INCREMENT BY 1 NOCACHE;

CREATE OR REPLACE TRIGGER trg_historique_rh_id
BEFORE INSERT ON HISTORIQUE_RH
FOR EACH ROW
BEGIN
    IF :NEW.id_log IS NULL THEN
        SELECT seq_historique_rh.NEXTVAL INTO :NEW.id_log FROM DUAL;
    END IF;
END;
/

INSERT INTO HISTORIQUE_RH (date_action, utilisateur, action, details)
VALUES (TO_CHAR(SYSDATE, 'YYYY-MM-DD HH24:MI:SS'), 'admin', 'Initialisation Système', 'Validation du registre du personnel INM - Module Employés');
COMMIT;

-- 7. REQUÊTES DE VALIDATION POUR LES SÉANCES D'ÉVALUATION S8 & S9
-- Affichage de l'ensemble du personnel
SELECT * FROM EMPLOYE ORDER BY id_employe ASC;

-- Répartition par poste
SELECT poste, COUNT(*) AS effectif FROM EMPLOYE GROUP BY poste ORDER BY effectif DESC;

-- Répartition par structure opérationnelle
SELECT structure, COUNT(*) AS effectif FROM EMPLOYE GROUP BY structure;

-- Consultation du journal d'audit RH
SELECT * FROM HISTORIQUE_RH ORDER BY id_log DESC;


-- ============================================================================
-- SEMAINE 10 : MODÈLE PHYSIQUE POUR LE SCÉNARIO ARDUINO (STATION MÉTÉO)
-- PERSISTANCE DES RELEVÉS TÉLÉMÉTRIQUES ET HISTORIQUE DES ALERTES
-- ÉTUDIANTE : SALMA RIDENE
-- ============================================================================

-- 8. TABLE RELEVE_METEO (RELEVÉS TRANSMIS PAR L'ARDUINO EN TEMPS RÉEL)
DROP TABLE RELEVE_METEO CASCADE CONSTRAINTS;
DROP SEQUENCE seq_releve_meteo;

CREATE TABLE RELEVE_METEO (
    id_releve    NUMBER,
    date_releve  VARCHAR2(30)  NOT NULL,
    temperature  NUMBER(6,2)   NOT NULL,
    humidite     NUMBER(5,2)   NOT NULL,
    pluie_adc    NUMBER(5)     NOT NULL,
    gel_detecte  VARCHAR2(3)   DEFAULT 'Non',
    station_id   VARCHAR2(50)  NOT NULL,
    CONSTRAINT pk_releve_meteo PRIMARY KEY (id_releve),
    CONSTRAINT chk_releve_gel CHECK (gel_detecte IN ('Oui', 'Non'))
);

CREATE SEQUENCE seq_releve_meteo START WITH 1 INCREMENT BY 1 NOCACHE;

CREATE OR REPLACE TRIGGER trg_releve_meteo_id
BEFORE INSERT ON RELEVE_METEO
FOR EACH ROW
BEGIN
    IF :NEW.id_releve IS NULL THEN
        SELECT seq_releve_meteo.NEXTVAL INTO :NEW.id_releve FROM DUAL;
    END IF;
END;
/

-- 9. TABLE ALERTE_METEO (ALERTES GÉNÉRÉES SUITE AU DÉPASSEMENT DES SEUILS CRITIQUES)
DROP TABLE ALERTE_METEO CASCADE CONSTRAINTS;
DROP SEQUENCE seq_alerte_meteo;

CREATE TABLE ALERTE_METEO (
    id_alerte    NUMBER,
    date_alerte  VARCHAR2(30)  NOT NULL,
    station_id   VARCHAR2(50)  NOT NULL,
    type_alerte  VARCHAR2(100) NOT NULL,
    temperature  NUMBER(6,2),
    humidite     NUMBER(5,2),
    pluie_adc    NUMBER(5),
    gel_detecte  VARCHAR2(3)   DEFAULT 'Non',
    traitee      VARCHAR2(3)   DEFAULT 'Non',
    CONSTRAINT pk_alerte_meteo PRIMARY KEY (id_alerte),
    CONSTRAINT chk_al_gel CHECK (gel_detecte IN ('Oui', 'Non')),
    CONSTRAINT chk_al_traitee CHECK (traitee IN ('Oui', 'Non'))
);

CREATE SEQUENCE seq_alerte_meteo START WITH 1 INCREMENT BY 1 NOCACHE;

CREATE OR REPLACE TRIGGER trg_alerte_meteo_id
BEFORE INSERT ON ALERTE_METEO
FOR EACH ROW
BEGIN
    IF :NEW.id_alerte IS NULL THEN
        SELECT seq_alerte_meteo.NEXTVAL INTO :NEW.id_alerte FROM DUAL;
    END IF;
END;
/

-- 10. JEU DE DONNÉES DE TEST POUR LE SCÉNARIO ARDUINO
INSERT INTO RELEVE_METEO (date_releve, temperature, humidite, pluie_adc, gel_detecte, station_id)
VALUES (TO_CHAR(SYSDATE, 'YYYY-MM-DD HH24:MI:SS'), 26.50, 60.20, 120, 'Non', 'Arduino-UNO-INM-01');

INSERT INTO RELEVE_METEO (date_releve, temperature, humidite, pluie_adc, gel_detecte, station_id)
VALUES (TO_CHAR(SYSDATE, 'YYYY-MM-DD HH24:MI:SS'), 37.80, 18.40, 45, 'Non', 'Arduino-UNO-INM-01');

INSERT INTO ALERTE_METEO (date_alerte, station_id, type_alerte, temperature, humidite, pluie_adc, gel_detecte, traitee)
VALUES (TO_CHAR(SYSDATE, 'YYYY-MM-DD HH24:MI:SS'), 'Arduino-UNO-INM-01', 'Canicule & Sécheresse', 37.80, 18.40, 45, 'Non', 'Non');

COMMIT;
