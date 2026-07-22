#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <mysql/mysql.h>

// Thin RAII wrapper around the MySQL C API. Everything that used to be a
// flat text file (records.txt, history_<user>.txt) now lives in a MySQL
// database instead - this class owns the one connection the whole program
// shares and gives AuthManager/HistoryManager a couple of small, safe
// primitives to build queries on top of.
//
// Connection settings are read from environment variables so credentials
// never have to be hard-coded or committed:
//   DB_HOST      (default "localhost")
//   DB_PORT      (default 3306)
//   DB_USER      (default "root")
//   DB_PASSWORD  (default "")
//   DB_NAME      (default "budget_tax_system")
class Database {
private:
    MYSQL* conn;

public:
    Database();
    ~Database();

    // No copying - there is exactly one live MYSQL* connection handle.
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Connects using the DB_* environment variables above and makes sure
    // the schema (users / history tables) exists. Throws std::runtime_error
    // on failure, since the whole program is useless without a database.
    void connect();

    // Runs a statement that doesn't return rows (INSERT/UPDATE/DELETE/DDL).
    void execute(const std::string& sql);

    // Escapes a string for safe interpolation into a query. Used instead of
    // full prepared statements to keep this in line with the rest of the
    // project's simple, readable style - every caller still goes through
    // this before building a query, so untrusted input never reaches SQL
    // unescaped.
    std::string escape(const std::string& value) const;

    MYSQL* handle() const { return conn; }
};

// Thrown by Database::connect()/execute() on any MySQL error, with the
// underlying MySQL error message attached.
class DatabaseError : public std::runtime_error {
public:
    explicit DatabaseError(const std::string& msg) : std::runtime_error(msg) {}
};
