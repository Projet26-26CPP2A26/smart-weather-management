#include "stationdao.h"
#include "station.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

static bool estSqlite()
{
    return QSqlDatabase::database().driverName() == "QSQLITE";
}

// Expression SQL qui formate la date en JJ/MM/AAAA
static QString exprDate()
{
    return estSqlite()
        ? "strftime('%d/%m/%Y', DATE_INSTALLATION)"
        : "TO_CHAR(DATE_INSTALLATION, 'DD/MM/YYYY')";
}

static QString selectBase()
{
    return "SELECT ID_STATION, NOM_STATION, TYPE_STATION, GOUVERNORAT, ALTITUDE, STATUT, "
           + exprDate() + " AS DATE_INSTALLATION FROM STATION";
}

// SQLite stocke les dates en texte ISO, Oracle accepte un QDate
static QVariant valeurDate(const QDate &d)
{
    return estSqlite() ? QVariant(d.toString("yyyy-MM-dd")) : QVariant(d);
}

static void appliquerEntetes(QSqlQueryModel *model)
{
    model->setHeaderData(0, Qt::Horizontal, "ID Station");
    model->setHeaderData(1, Qt::Horizontal, "Nom Station");
    model->setHeaderData(2, Qt::Horizontal, "Type");
    model->setHeaderData(3, Qt::Horizontal, "Gouvernorat");
    model->setHeaderData(4, Qt::Horizontal, "Altitude (m)");
    model->setHeaderData(5, Qt::Horizontal, "Statut");
    model->setHeaderData(6, Qt::Horizontal, "Date Install.");
}

bool StationDao::creerTable()
{
    QSqlQuery query;
    if (estSqlite()) {
        return query.exec(
            "CREATE TABLE IF NOT EXISTS STATION ("
            "   ID_STATION TEXT PRIMARY KEY,"
            "   NOM_STATION TEXT NOT NULL,"
            "   TYPE_STATION TEXT,"
            "   GOUVERNORAT TEXT,"
            "   ALTITUDE REAL,"
            "   STATUT TEXT DEFAULT 'Active',"
            "   DATE_INSTALLATION TEXT)");
    }
    // Oracle : ORA-00955 = la table existe deja (normal)
    const bool ok = query.exec(
        "CREATE TABLE STATION ("
        "   ID_STATION VARCHAR2(50) PRIMARY KEY,"
        "   NOM_STATION VARCHAR2(100) NOT NULL,"
        "   TYPE_STATION VARCHAR2(50),"
        "   GOUVERNORAT VARCHAR2(50),"
        "   ALTITUDE NUMBER,"
        "   STATUT VARCHAR2(50) DEFAULT 'Active',"
        "   DATE_INSTALLATION DATE)");
    if (!ok) {
        qDebug() << "Creation table STATION :" << query.lastError().text();
        return query.lastError().text().contains("ORA-00955");
    }
    return true;
}

bool StationDao::inserer(Station &s)
{
    QSqlQuery query;
    query.prepare("INSERT INTO STATION (ID_STATION, NOM_STATION, TYPE_STATION, GOUVERNORAT, ALTITUDE, STATUT, DATE_INSTALLATION) "
                  "VALUES (:id, :nom, :type, :gouv, :alt, :statut, :date)");
    query.bindValue(":id", s.getId());
    query.bindValue(":nom", s.getNom());
    query.bindValue(":type", s.getType());
    query.bindValue(":gouv", s.getGouvernorat());
    query.bindValue(":alt", s.getAltitude());
    query.bindValue(":statut", s.getStatut());
    query.bindValue(":date", valeurDate(s.getDateInstallation()));

    if (!query.exec()) {
        qDebug() << "Erreur Ajout SQL :" << query.lastError().text();
        return false;
    }
    return true;
}

bool StationDao::mettreAJour(Station &s, const QString &id)
{
    QSqlQuery query;
    query.prepare("UPDATE STATION SET NOM_STATION = :nom, TYPE_STATION = :type, GOUVERNORAT = :gouv, "
                  "ALTITUDE = :alt, STATUT = :statut, DATE_INSTALLATION = :date WHERE ID_STATION = :id");
    query.bindValue(":nom", s.getNom());
    query.bindValue(":type", s.getType());
    query.bindValue(":gouv", s.getGouvernorat());
    query.bindValue(":alt", s.getAltitude());
    query.bindValue(":statut", s.getStatut());
    query.bindValue(":date", valeurDate(s.getDateInstallation()));
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur Modifier SQL :" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;   // 0 ligne => l'ID n'existe pas
}

bool StationDao::supprimer(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM STATION WHERE ID_STATION = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur Supprimer SQL :" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

QSqlQueryModel* StationDao::rechercher(const QString &val, const QString &critere)
{
    QSqlQueryModel* model = new QSqlQueryModel();

    QString sql = selectBase();
    const bool filtre = !val.trimmed().isEmpty();

    if (filtre) {
        sql += " WHERE UPPER(NOM_STATION) LIKE UPPER(:v1)"
               " OR UPPER(TYPE_STATION) LIKE UPPER(:v2)"
               " OR UPPER(GOUVERNORAT) LIKE UPPER(:v3)";
    }

    // Liste blanche : pas d'injection SQL
    if (critere == "Altitude")
        sql += " ORDER BY ALTITUDE ASC";
    else if (critere == "Type")
        sql += " ORDER BY TYPE_STATION ASC";
    else if (critere == "Gouvernorat")
        sql += " ORDER BY GOUVERNORAT ASC";

    QSqlQuery query;
    query.prepare(sql);
    if (filtre) {
        const QString p = "%" + val.trimmed() + "%";
        query.bindValue(":v1", p);
        query.bindValue(":v2", p);
        query.bindValue(":v3", p);
    }
    if (!query.exec())
        qDebug() << "Erreur Recherche SQL :" << query.lastError().text();

    model->setQuery(std::move(query));
    appliquerEntetes(model);
    return model;
}

QMap<QString,int> StationDao::repartition(const QString &champ)
{
    QMap<QString,int> res;

    QString col;                                   // liste blanche
    if (champ == "Type")        col = "TYPE_STATION";
    else if (champ == "Statut") col = "STATUT";
    else                        return res;

    QSqlQuery query;
    query.prepare("SELECT " + col + ", COUNT(*) FROM STATION GROUP BY " + col);
    if (!query.exec()) {
        qDebug() << "Erreur Stats SQL :" << query.lastError().text();
        return res;
    }
    while (query.next())
        res[query.value(0).toString()] = query.value(1).toInt();
    return res;
}

QMap<QString,int> StationDao::activesParGouvernorat()
{
    QMap<QString,int> res;
    QSqlQuery query;
    query.prepare("SELECT GOUVERNORAT, COUNT(*) FROM STATION "
                  "WHERE STATUT = 'Active' GROUP BY GOUVERNORAT");
    if (!query.exec()) {
        qDebug() << "Erreur Suggestion SQL :" << query.lastError().text();
        return res;
    }
    while (query.next())
        res[query.value(0).toString()] = query.value(1).toInt();
    return res;
}

QList<StationRow> StationDao::toutesLesStations()
{
    QList<StationRow> liste;
    QSqlQuery q;
    q.prepare("SELECT ID_STATION, STATUT, " + exprDate() + " FROM STATION");
    if (!q.exec()) {
        qDebug() << "Erreur Heartbeat SQL :" << q.lastError().text();
        return liste;
    }
    while (q.next()) {
        StationRow r;
        r.id     = q.value(0).toString();
        r.statut = q.value(1).toString();
        r.date   = QDate::fromString(q.value(2).toString(), "dd/MM/yyyy");
        liste.append(r);
    }
    return liste;
}

bool StationDao::changerStatut(const QString &id, const QString &statut)
{
    QSqlQuery u;
    u.prepare("UPDATE STATION SET STATUT = :st WHERE ID_STATION = :id");
    u.bindValue(":st", statut);
    u.bindValue(":id", id);
    return u.exec();
}
