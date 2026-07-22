#include "AuthManager.h"

AuthManager::AuthManager(Database& database) : db(database) {}

void AuthManager::loadFromFile() {
    // No-op with MySQL - Database::connect() already ensured the `users`
    // table exists before AuthManager is ever constructed. Kept only so
    // main.cpp didn't have to change its startup sequence.
}

bool AuthManager::usernameExists(const std::string& uname) const {
    std::string sql = "SELECT id FROM users WHERE username = '" + db.escape(uname) + "' LIMIT 1";
    db.execute(sql);

    MYSQL_RES* res = mysql_store_result(db.handle());
    bool exists = res && mysql_num_rows(res) > 0;
    if (res) mysql_free_result(res);
    return exists;
}

bool AuthManager::registerUser(const std::string& uname, const std::string& password, const std::string& passphrase) {
    if (uname.empty() || password.empty() || passphrase.empty()) return false;
    if (usernameExists(uname)) return false;

    // Still storing plain text, same as the file-backed version - only the
    // storage medium changed here, not the (lack of) hashing.
    std::string sql = "INSERT INTO users (username, password, passphrase) VALUES ('"
                       + db.escape(uname) + "', '"
                       + db.escape(password) + "', '"
                       + db.escape(passphrase) + "')";
    db.execute(sql);
    return true;
}

bool AuthManager::loginUser(const std::string& uname, const std::string& password) const {
    // Direct plain-text comparison - no hashing - done in SQL now instead
    // of in a loop over an in-memory vector<User>.
    std::string sql = "SELECT id FROM users WHERE username = '" + db.escape(uname)
                       + "' AND password = '" + db.escape(password) + "' LIMIT 1";
    db.execute(sql);

    MYSQL_RES* res = mysql_store_result(db.handle());
    bool ok = res && mysql_num_rows(res) > 0;
    if (res) mysql_free_result(res);
    return ok;
}

bool AuthManager::resetPassword(const std::string& uname, const std::string& passphrase, const std::string& newPassword) {
    if (newPassword.empty()) return false;

    // Direct plain-text comparison for the passphrase - no hashing.
    std::string checkSql = "SELECT id FROM users WHERE username = '" + db.escape(uname)
                            + "' AND passphrase = '" + db.escape(passphrase) + "' LIMIT 1";
    db.execute(checkSql);
    MYSQL_RES* res = mysql_store_result(db.handle());
    bool matched = res && mysql_num_rows(res) > 0;
    if (res) mysql_free_result(res);

    if (!matched) return false; // unknown username, or wrong passphrase - do not reset

    std::string updateSql = "UPDATE users SET password = '" + db.escape(newPassword)
                             + "' WHERE username = '" + db.escape(uname) + "'";
    db.execute(updateSql);
    return true;
}

std::vector<std::string> AuthManager::listUsernames() const {
    std::vector<std::string> names;

    db.execute("SELECT username FROM users ORDER BY id ASC");
    MYSQL_RES* res = mysql_store_result(db.handle());
    if (res) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr) {
            names.push_back(row[0] ? row[0] : "");
        }
        mysql_free_result(res);
    }
    return names;
}

bool AuthManager::deleteUser(const std::string& uname, const std::string& password, const std::string& passphrase) {
    // Direct plain-text comparison for both password and passphrase.
    std::string checkSql = "SELECT id FROM users WHERE username = '" + db.escape(uname)
                            + "' AND password = '" + db.escape(password)
                            + "' AND passphrase = '" + db.escape(passphrase) + "' LIMIT 1";
    db.execute(checkSql);
    MYSQL_RES* res = mysql_store_result(db.handle());
    bool matched = res && mysql_num_rows(res) > 0;
    if (res) mysql_free_result(res);

    if (!matched) return false; // wrong password or passphrase, or username not found

    // ON DELETE CASCADE on history.username takes care of that user's
    // saved history at the database level - see Database::connect().
    std::string deleteSql = "DELETE FROM users WHERE username = '" + db.escape(uname) + "'";
    db.execute(deleteSql);
    return true;
}
