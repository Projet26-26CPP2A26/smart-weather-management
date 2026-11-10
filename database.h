#pragma once
#include <QString>

class Database
{
public:
    // Lit config.ini (a cote de l'executable) et ouvre la connexion Oracle (QOCI).
    // Si Oracle est indisponible, bascule sur la base SQLite locale.
    static bool connect(QString *error = nullptr);

    // Vrai si la connexion active est la base locale SQLite.
    static bool isSqlite();
};
