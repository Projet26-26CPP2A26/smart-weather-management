#pragma once
#include <QSqlDatabase>
#include <QString>

// Mode demo = SQLite integre | mode oracle = Oracle (QOCI).
// Les requetes sont ecrites en SQL Oracle ; dbSql() les adapte pour SQLite.
inline bool dbIsSqlite()
{
    return QSqlDatabase::database().driverName() == "QSQLITE";
}

inline QString dbSql(QString s)
{
    if (!dbIsSqlite()) return s;
    s.replace("NVL(", "IFNULL(");
    s.replace("SEQ_INTERVENTION.NEXTVAL", "NULL");
    s.replace("SYSDATE", "strftime('%Y-%m-%dT%H:%M:%S','now','localtime')");
    return s;
}
