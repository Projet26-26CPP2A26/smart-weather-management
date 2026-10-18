-- ============================================================================
--  Module 5 : Gestion de la Maintenance & Interventions  (Oracle)
--  A executer APRES schema.sql du Module 3 (tables STATION et RELEVE_METEO existantes).
--  Script re-executable : les objets deja presents sont ignores.
--  SQL Developer : Executer le script (F5) | SQL*Plus : @schema_module5.sql
-- ============================================================================

-- ---------------------------------------------------------------------------
-- 1) STATION : colonnes necessaires au Module 5 (gouvernorat + type de station)
-- ---------------------------------------------------------------------------
BEGIN
    EXECUTE IMMEDIATE 'ALTER TABLE STATION ADD (GOUVERNORAT VARCHAR2(50))';
EXCEPTION WHEN OTHERS THEN IF SQLCODE != -1430 THEN RAISE; END IF;   -- colonne deja presente
END;
/
BEGIN
    EXECUTE IMMEDIATE 'ALTER TABLE STATION ADD (TYPE_STATION VARCHAR2(30) DEFAULT ''Météo'')';
EXCEPTION WHEN OTHERS THEN IF SQLCODE != -1430 THEN RAISE; END IF;
END;
/
-- Renseignez le gouvernorat / type de VOS stations (ici : les 2 stations de test du Module 3)
UPDATE STATION SET GOUVERNORAT = 'Tunis' WHERE ID_STATION = 1 AND GOUVERNORAT IS NULL;
UPDATE STATION SET GOUVERNORAT = 'Sfax'  WHERE ID_STATION = 2 AND GOUVERNORAT IS NULL;
-- Type 'Aéronautique' => delai SLA court. Exemple : UPDATE STATION SET TYPE_STATION='Aéronautique' WHERE ID_STATION=2;
COMMIT;

-- ---------------------------------------------------------------------------
-- 2) EMPLOYE (Module 1) : cree seulement s'il n'existe pas deja
--    Le Module 5 lit : ID_EMPLOYE, NOM, PRENOM
-- ---------------------------------------------------------------------------
BEGIN
    EXECUTE IMMEDIATE 'CREATE TABLE EMPLOYE (
        ID_EMPLOYE NUMBER PRIMARY KEY,
        NOM        VARCHAR2(40) NOT NULL,
        PRENOM     VARCHAR2(40) NOT NULL,
        FONCTION   VARCHAR2(40))';
EXCEPTION WHEN OTHERS THEN IF SQLCODE != -955 THEN RAISE; END IF;   -- table deja presente
END;
/

-- ---------------------------------------------------------------------------
-- 3) Table du module + sequence
-- ---------------------------------------------------------------------------
BEGIN
    EXECUTE IMMEDIATE 'CREATE SEQUENCE SEQ_INTERVENTION START WITH 1 INCREMENT BY 1 NOCACHE';
EXCEPTION WHEN OTHERS THEN IF SQLCODE != -955 THEN RAISE; END IF;
END;
/
BEGIN
    EXECUTE IMMEDIATE q'[CREATE TABLE INTERVENTION (
        ID_INTERVENTION      NUMBER PRIMARY KEY,
        ID_STATION           NUMBER NOT NULL REFERENCES STATION(ID_STATION),
        ID_EMPLOYE           NUMBER NOT NULL REFERENCES EMPLOYE(ID_EMPLOYE),
        TYPE_INTERVENTION    VARCHAR2(30) NOT NULL
            CHECK (TYPE_INTERVENTION IN ('Préventive','Corrective','Calibration','Remplacement capteur')),
        PRIORITE             VARCHAR2(10) NOT NULL
            CHECK (PRIORITE IN ('Basse','Moyenne','Haute','Urgente')),
        DATE_CREATION        DATE DEFAULT SYSDATE NOT NULL,
        DATE_PLANIFIEE       DATE,
        DATE_CLOTURE         DATE,
        STATUT               VARCHAR2(15) DEFAULT 'Planifiée' NOT NULL
            CHECK (STATUT IN ('Planifiée','En cours','Terminée','Annulée')),
        COUT                 NUMBER(10,2) DEFAULT 0,
        RAPPORT_INTERVENTION CLOB,
        ARCHIVE              NUMBER(1) DEFAULT 0 NOT NULL)]';
EXCEPTION WHEN OTHERS THEN IF SQLCODE != -955 THEN RAISE; END IF;
END;
/

-- ---------------------------------------------------------------------------
-- 4) Liaison avec le Module 3 : anomalies capteur DETECTEES dans RELEVE_METEO
--    Meme regle que « Anomalies capteurs » du Module 3 : ecart > 3 ecarts-types
--    par rapport a la moyenne historique de la station (>= 10 releves requis).
--    Anomalie critique = ecart > 4 ecarts-types. Fenetre : 30 derniers jours.
-- ---------------------------------------------------------------------------
CREATE OR REPLACE VIEW V_ANOMALIES_RECENTES AS
WITH ref AS (
    SELECT ID_STATION,
           AVG(TEMPERATURE) MT, STDDEV(TEMPERATURE) ST_T,
           AVG(HUMIDITE)    MH, STDDEV(HUMIDITE)    ST_H,
           AVG(PRESSION)    MP, STDDEV(PRESSION)    ST_P,
           AVG(VITESSE_VENT) MV, STDDEV(VITESSE_VENT) ST_V
    FROM RELEVE_METEO
    GROUP BY ID_STATION
    HAVING COUNT(*) >= 10
), z AS (
    SELECT r.ID_STATION, r.ID_RELEVE,
           GREATEST(
               NVL(CASE WHEN f.ST_T > 0 THEN ABS(r.TEMPERATURE  - f.MT) / f.ST_T END, 0),
               NVL(CASE WHEN f.ST_H > 0 THEN ABS(r.HUMIDITE     - f.MH) / f.ST_H END, 0),
               NVL(CASE WHEN f.ST_P > 0 THEN ABS(r.PRESSION     - f.MP) / f.ST_P END, 0),
               NVL(CASE WHEN f.ST_V > 0 THEN ABS(r.VITESSE_VENT - f.MV) / f.ST_V END, 0)
           ) AS ZMAX
    FROM RELEVE_METEO r
    JOIN ref f ON f.ID_STATION = r.ID_STATION
    WHERE r.HORODATAGE >= SYSDATE - 30
)
SELECT ID_STATION,
       COUNT(*)                               AS NB_ANO,
       SUM(CASE WHEN ZMAX > 4 THEN 1 ELSE 0 END) AS NB_CRIT
FROM z
WHERE ZMAX > 3
GROUP BY ID_STATION;

-- ---------------------------------------------------------------------------
-- 5) Heartbeat Score (Module 2) vu par le Module 5 : V_HEARTBEAT(ID_STATION, SCORE, DATE_MESURE)
--    Version par defaut calculee a partir des anomalies du Module 3 :
--    100 - 10 par anomalie - 20 par anomalie critique (min 0).
--    >>> Si le Module 2 a sa propre table de scores, remplacez cette vue, par exemple :
--    CREATE OR REPLACE VIEW V_HEARTBEAT AS
--      SELECT ID_STATION, SCORE, DATE_MESURE FROM <TABLE_HEARTBEAT_DU_MODULE_2>;
-- ---------------------------------------------------------------------------
CREATE OR REPLACE VIEW V_HEARTBEAT AS
SELECT s.ID_STATION,
       GREATEST(0, 100 - 10 * NVL(a.NB_ANO, 0) - 20 * NVL(a.NB_CRIT, 0)) AS SCORE,
       SYSDATE AS DATE_MESURE
FROM STATION s
LEFT JOIN V_ANOMALIES_RECENTES a ON a.ID_STATION = s.ID_STATION;

-- ---------------------------------------------------------------------------
-- 6) DONNEES DE DEMO (optionnel) : supprimez cette section en production
-- ---------------------------------------------------------------------------
-- Techniciens (uniquement si la table EMPLOYE est vide)
INSERT INTO EMPLOYE (ID_EMPLOYE, NOM, PRENOM, FONCTION)
SELECT 1, 'Ben Salah', 'Ahmed', 'Technicien de maintenance' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM EMPLOYE);
INSERT INTO EMPLOYE (ID_EMPLOYE, NOM, PRENOM, FONCTION)
SELECT 2, 'Trabelsi', 'Sana', 'Technicien de maintenance' FROM DUAL
WHERE (SELECT COUNT(*) FROM EMPLOYE) = 1 AND EXISTS (SELECT 1 FROM EMPLOYE WHERE ID_EMPLOYE = 1 AND NOM = 'Ben Salah');

-- Station Sud (id 2) : 30 releves normaux + 1 releve aberrant (capteur de temperature defaillant)
-- => anomalie critique => alerte rouge, delai SLA court, suggestion preventive "Urgente"
DECLARE
    v_n NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_n FROM RELEVE_METEO WHERE ID_STATION = 2 AND HORODATAGE >= SYSDATE - 30;
    IF v_n < 10 THEN
        FOR i IN 1 .. 30 LOOP
            INSERT INTO RELEVE_METEO (ID_RELEVE, ID_STATION, HORODATAGE, TEMPERATURE, HUMIDITE, PRESSION,
                                      VITESSE_VENT, PRECIPITATION, MODE_COLLECTE)
            VALUES (SEQ_RELEVE.NEXTVAL, 2, SYSTIMESTAMP - NUMTODSINTERVAL(i + 1, 'HOUR'),
                    24 + MOD(i, 5), 55 + MOD(i, 7), 1010 + MOD(i, 3), 8 + MOD(i, 4), 0, 'Automatique');
        END LOOP;
        INSERT INTO RELEVE_METEO (ID_RELEVE, ID_STATION, HORODATAGE, TEMPERATURE, HUMIDITE, PRESSION,
                                  VITESSE_VENT, PRECIPITATION, MODE_COLLECTE)
        VALUES (SEQ_RELEVE.NEXTVAL, 2, SYSTIMESTAMP - NUMTODSINTERVAL(30, 'MINUTE'), 71, 55, 1011, 9, 0, 'Automatique');
    END IF;
END;
/

-- Interventions de demo (seulement si la table est vide)
DECLARE
    v_n NUMBER; v_e1 NUMBER; v_e2 NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_n FROM INTERVENTION;
    IF v_n = 0 THEN
        SELECT MIN(ID_EMPLOYE), MAX(ID_EMPLOYE) INTO v_e1, v_e2 FROM EMPLOYE;
        INSERT INTO INTERVENTION (ID_INTERVENTION, ID_STATION, ID_EMPLOYE, TYPE_INTERVENTION, PRIORITE,
                                  DATE_CREATION, DATE_PLANIFIEE, STATUT, COUT)
        VALUES (SEQ_INTERVENTION.NEXTVAL, 2, v_e1, 'Corrective', 'Urgente', SYSDATE - 2, SYSDATE - 1, 'En cours', 350);
        INSERT INTO INTERVENTION (ID_INTERVENTION, ID_STATION, ID_EMPLOYE, TYPE_INTERVENTION, PRIORITE,
                                  DATE_CREATION, DATE_PLANIFIEE, STATUT, COUT)
        VALUES (SEQ_INTERVENTION.NEXTVAL, 1, v_e2, 'Calibration', 'Moyenne', SYSDATE, SYSDATE + 5, 'Planifiée', 120);
        INSERT INTO INTERVENTION (ID_INTERVENTION, ID_STATION, ID_EMPLOYE, TYPE_INTERVENTION, PRIORITE,
                                  DATE_CREATION, DATE_PLANIFIEE, DATE_CLOTURE, STATUT, COUT, RAPPORT_INTERVENTION)
        VALUES (SEQ_INTERVENTION.NEXTVAL, 1, v_e2, 'Remplacement capteur', 'Haute', SYSDATE - 10, SYSDATE - 9,
                SYSDATE - 8, 'Terminée', 480, 'Capteur remplace et teste.');
    END IF;
END;
/
COMMIT;
