#include "connection.h" // <-- METTEZ ICI LE NOM EXACT DU FICHIER (Majuscule ou Minuscule)

// Constructeur
Connection::Connection() {
    db = QSqlDatabase::addDatabase("QODBC");
}

// Destructeur
Connection::~Connection() {
    if (db.isOpen())
        db.close();
}

// Singleton Meyer's
Connection& Connection::createInstance() {
    static Connection instance;
    return instance;
}

bool Connection::createConnection() {
    // Connexion via le DSN ODBC 64-bit configuré sous Windows
    db.setDatabaseName("MonOracleDSN");
    db.setUserName("smartweather");
    db.setPassword("esprit123");

    if (db.open()) {
        qDebug() << "Connexion établie avec succès !";
        return true;
    }

    qDebug() << "Échec de la connexion :" << db.lastError().text();
    return false;
}