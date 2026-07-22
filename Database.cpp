#include "Database.h"
#include <cstdlib>

namespace {
    // Reads an environment variable, falling back to a default if it isn't
    // set - keeps connect() readable instead of repeating getenv() checks.
    std::string envOr(const char* name, const std::string& fallback) {
        const char* val = std::getenv(name);
        return (val && *val) ? std::string(val) : fallback;
    }
}

Database::Database() : conn(nullptr) {}

Database::~Database() {
    if (conn) {
        mysql_close(conn);
    }
}

void Database::connect() {
    conn = mysql_init(nullptr);
    if (!conn) {
        throw DatabaseError("mysql_init failed (out of memory)");
    }

    std::string host = envOr("DB_HOST", "localhost");
    std::string user = envOr("DB_USER", "root");
    std::string password = envOr("DB_PASSWORD", "");
    std::string dbName = envOr("DB_NAME", "budget_tax_system");
    unsigned int port = static_cast<unsigned int>(std::stoi(envOr("DB_PORT", "3306")));

    // Connect without selecting a database first, so we can CREATE DATABASE
    // IF NOT EXISTS below on a first run instead of failing outright.
    if (!mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(),
                             nullptr, port, nullptr, 0)) {
        std::string err = mysql_error(conn);
        mysql_close(conn);
        conn = nullptr;
        throw DatabaseError("Could not connect to MySQL server: " + err);
    }

    execute("CREATE DATABASE IF NOT EXISTS `" + dbName + "` "
            "CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci");

    if (mysql_select_db(conn, dbName.c_str()) != 0) {
        throw DatabaseError(std::string("Could not select database: ") + mysql_error(conn));
    }

    // Mirrors what AuthManager/HistoryManager used to store in
    // records.txt / history_<user>.txt, just as proper tables.
    execute(
        "CREATE TABLE IF NOT EXISTS users ("
        "  id INT AUTO_INCREMENT PRIMARY KEY,"
        "  username VARCHAR(100) NOT NULL UNIQUE,"
        "  password VARCHAR(255) NOT NULL,"
        "  passphrase VARCHAR(255) NOT NULL"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4"
    );

    execute(
        "CREATE TABLE IF NOT EXISTS history ("
        "  id INT AUTO_INCREMENT PRIMARY KEY,"
        "  username VARCHAR(100) NOT NULL,"
        "  income DOUBLE NOT NULL,"
        "  tax DOUBLE NOT NULL,"
        "  remaining_income DOUBLE NOT NULL,"
        "  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "  INDEX idx_history_username (username),"
        "  CONSTRAINT fk_history_username FOREIGN KEY (username)"
        "    REFERENCES users(username) ON DELETE CASCADE"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4"
    );
}

void Database::execute(const std::string& sql) {
    if (mysql_query(conn, sql.c_str()) != 0) {
        throw DatabaseError(std::string("MySQL query failed: ") + mysql_error(conn) +
                             " (statement: " + sql + ")");
    }
}

std::string Database::escape(const std::string& value) const {
    std::string buffer(value.size() * 2 + 1, '\0');
    unsigned long len = mysql_real_escape_string(conn, &buffer[0], value.c_str(), value.size());
    buffer.resize(len);
    return buffer;
}
