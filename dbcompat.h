#pragma once
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>

// Petites aides pour garder les requetes valides sur Oracle (production)
// et sur SQLite (base locale utilisee quand le DSN Oracle est absent).
namespace DbCompat {

inline bool isSqlite()
{
    const QSqlDatabase db = QSqlDatabase::database();
    return db.isValid() && db.driverName().compare(QLatin1String("QSQLITE"), Qt::CaseInsensitive) == 0;
}

// Equivalent de TO_CHAR(colonne, formatOracle) sur le backend courant.
inline QString dateExpr(const QString &colonne, const QString &formatOracle)
{
    if (!isSqlite())
        return QStringLiteral("TO_CHAR(%1, '%2')").arg(colonne, formatOracle);

    QString fmt = formatOracle;
    fmt.replace("YYYY", "%Y").replace("MM", "%m").replace("DD", "%d")
       .replace("HH24", "%H").replace("MI", "%M").replace("SS", "%S");
    return QStringLiteral("strftime('%1', %2)").arg(fmt, colonne);
}

// Remplace les sequences Oracle (SEQ_x.NEXTVAL) par MAX(id)+1.
inline int nextId(const QString &table, const QString &idColonne)
{
    QSqlQuery q;
    if (!q.exec(QStringLiteral("SELECT COALESCE(MAX(%1), 0) + 1 FROM %2").arg(idColonne, table)))
        return 1;
    return q.next() ? q.value(0).toInt() : 1;
}

} // namespace DbCompat
