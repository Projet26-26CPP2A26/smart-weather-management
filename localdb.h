#pragma once

// Base SQLite locale utilisee quand la connexion Oracle (DSN MonOracleDSN)
// n'est pas disponible sur la machine.
namespace LocalDb {

// Cree les tables manquantes et injecte un jeu de donnees de demonstration
// si la base est vide. La connexion par defaut doit deja etre ouverte.
bool prepare();

} // namespace LocalDb
