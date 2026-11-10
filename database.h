#pragma once
#include <QString>

class Database
{
public:
    // Lit config.ini (a cote de l'executable) et ouvre la connexion Oracle (QOCI).
    static bool connect(QString *error = nullptr);
};
