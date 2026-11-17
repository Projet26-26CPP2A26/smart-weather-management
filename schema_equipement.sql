-- ============================================================================
--  MODULE GESTION DES EQUIPEMENTS (a executer APRES schema_oracle.sql)
--  Connecte en smartweather / esprit123.
--  ID_STATION est en VARCHAR2(10) pour correspondre a STATION.ID_STATION.
-- ============================================================================
CREATE TABLE equipement (
    id_equipement     NUMBER(10)    NOT NULL,
    nom_equipement    VARCHAR2(80)  NOT NULL,
    type_equipement   VARCHAR2(30)  NOT NULL,
    marque            VARCHAR2(40),
    numero_serie      VARCHAR2(40),
    date_installation DATE,
    statut            VARCHAR2(20)  DEFAULT 'Opérationnel',
    id_station        VARCHAR2(10)  NOT NULL,
    CONSTRAINT pk_equipement PRIMARY KEY (id_equipement),
    CONSTRAINT uq_equipement_serie UNIQUE (numero_serie),
    CONSTRAINT ck_equipement_type CHECK (type_equipement IN
        ('Capteur','Anémomètre','Baromètre','Pluviomètre','Thermomètre','Autre')),
    CONSTRAINT ck_equipement_statut CHECK (statut IN
        ('Opérationnel','En panne','En maintenance')),
    CONSTRAINT fk_equipement_station FOREIGN KEY (id_station) REFERENCES station (id_station)
);

CREATE SEQUENCE seq_equipement START WITH 1 INCREMENT BY 1 NOCACHE;

INSERT INTO equipement VALUES (seq_equipement.NEXTVAL, 'Anémomètre Tunis', 'Anémomètre', 'Vaisala', 'ANE-0001', TO_DATE('15/01/2020','DD/MM/YYYY'), 'Opérationnel', '1');
INSERT INTO equipement VALUES (seq_equipement.NEXTVAL, 'Baromètre Sousse', 'Baromètre', 'Campbell', 'BAR-0002', TO_DATE('03/06/2021','DD/MM/YYYY'), 'En maintenance', '2');
INSERT INTO equipement VALUES (seq_equipement.NEXTVAL, 'Pluviomètre Sfax', 'Pluviomètre', 'Davis', 'PLU-0003', TO_DATE('20/09/2022','DD/MM/YYYY'), 'En panne', '3');
COMMIT;
