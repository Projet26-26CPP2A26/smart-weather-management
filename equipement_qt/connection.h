#ifndef MY_CONNECTION_H
#define MY_CONNECTION_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

class Connection
{
private:
    QSqlDatabase db;

    // Constructeur et destructeur privés (Pattern Singleton)
    Connection();
    ~Connection();

    // Suppression de la copie
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

public:
    // Instance unique (Meyer's Singleton)
    static Connection& createInstance();

    // Méthode de connexion
    bool createConnection();
};

#endif // MY_CONNECTION_H