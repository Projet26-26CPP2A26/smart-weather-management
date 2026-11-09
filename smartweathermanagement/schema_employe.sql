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

-- 6. REQUÊTES DE VALIDATION POUR LA SÉANCE D'ÉVALUATION S8
-- Affichage de l'ensemble du personnel
SELECT * FROM EMPLOYE ORDER BY id_employe ASC;

-- Répartition par poste
SELECT poste, COUNT(*) AS effectif FROM EMPLOYE GROUP BY poste ORDER BY effectif DESC;

-- Répartition par structure opérationnelle
SELECT structure, COUNT(*) AS effectif FROM EMPLOYE GROUP BY structure;
