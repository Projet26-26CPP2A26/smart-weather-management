-- ============================================================
--  Module 5 : Gestion de la Maintenance & Interventions (Oracle)
--  A executer avec SQL*Plus / SQL Developer
--  Les tables STATION, EMPLOYE et HEARTBEAT existent normalement
--  deja dans vos autres modules : adaptez ou supprimez les blocs
--  "demo" ci-dessous.
-- ============================================================

-- ---------- Tables de reference (demo) ----------
CREATE TABLE STATION (
    ID_STATION    NUMBER PRIMARY KEY,
    NOM           VARCHAR2(80) NOT NULL,
    GOUVERNORAT   VARCHAR2(40),
    TYPE_STATION  VARCHAR2(30)          -- ex: 'Aéronautique', 'Météo', 'Marine'
);

CREATE TABLE EMPLOYE (
    ID_EMPLOYE    NUMBER PRIMARY KEY,
    NOM           VARCHAR2(40) NOT NULL,
    PRENOM        VARCHAR2(40) NOT NULL,
    FONCTION      VARCHAR2(40)
);

-- Heartbeat Score (module 2) : une mesure par station
CREATE TABLE HEARTBEAT (
    ID_STATION    NUMBER REFERENCES STATION(ID_STATION),
    SCORE         NUMBER(5,2),          -- 0..100
    DATE_MESURE   DATE DEFAULT SYSDATE
);

-- ---------- Table du module ----------
CREATE TABLE INTERVENTION (
    ID_INTERVENTION     NUMBER PRIMARY KEY,
    ID_STATION          NUMBER NOT NULL REFERENCES STATION(ID_STATION),
    ID_EMPLOYE          NUMBER NOT NULL REFERENCES EMPLOYE(ID_EMPLOYE),
    TYPE_INTERVENTION   VARCHAR2(30) NOT NULL
        CHECK (TYPE_INTERVENTION IN ('Préventive','Corrective','Calibration','Remplacement capteur')),
    PRIORITE            VARCHAR2(10) NOT NULL
        CHECK (PRIORITE IN ('Basse','Moyenne','Haute','Urgente')),
    DATE_CREATION       DATE DEFAULT SYSDATE NOT NULL,
    DATE_PLANIFIEE      DATE,
    DATE_CLOTURE        DATE,
    STATUT              VARCHAR2(15) DEFAULT 'Planifiée' NOT NULL
        CHECK (STATUT IN ('Planifiée','En cours','Terminée','Annulée')),
    COUT                NUMBER(10,2) DEFAULT 0,
    RAPPORT_INTERVENTION CLOB,
    ARCHIVE             NUMBER(1) DEFAULT 0 NOT NULL
);

CREATE SEQUENCE SEQ_INTERVENTION START WITH 1 INCREMENT BY 1 NOCACHE;

-- ---------- Donnees de demo ----------
INSERT INTO STATION VALUES (1,'Tunis-Carthage','Tunis','Aéronautique');
INSERT INTO STATION VALUES (2,'Sfax-Thyna','Sfax','Aéronautique');
INSERT INTO STATION VALUES (3,'Bizerte Port','Bizerte','Marine');
INSERT INTO STATION VALUES (4,'Kairouan Centre','Kairouan','Météo');
INSERT INTO EMPLOYE VALUES (1,'Ben Salah','Ahmed','Technicien de maintenance');
INSERT INTO EMPLOYE VALUES (2,'Trabelsi','Sana','Technicien de maintenance');
INSERT INTO HEARTBEAT VALUES (1,92,SYSDATE);
INSERT INTO HEARTBEAT VALUES (2,38,SYSDATE);
INSERT INTO HEARTBEAT VALUES (3,57,SYSDATE);
INSERT INTO HEARTBEAT VALUES (4,81,SYSDATE);
COMMIT;
