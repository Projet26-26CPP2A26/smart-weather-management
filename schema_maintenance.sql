-- ============================================================
--  Smart Weather : tables du module Maintenance
--  A executer une seule fois dans la base Oracle (utilisateur smartweather).
--  La table STATION et la table Employe existent deja : ne pas les recreer.
-- ============================================================

CREATE TABLE INTERVENTION (
    ID_INTERVENTION     NUMBER PRIMARY KEY,
    ID_STATION          NUMBER NOT NULL REFERENCES STATION(ID_STATION),
    ID_EMPLOYE          NUMBER NOT NULL REFERENCES Employe(ID_Employe),
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

COMMIT;
