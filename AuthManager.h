#pragma once
#include <string>
#include <vector>
#include "Database.h"

// Owns every operation on user accounts. main() never touches user data or
// SQL directly - it only ever talks to this class. Previously this read
// and rewrote records.txt on every change; now every method talks straight
// to the `users` table in MySQL, so there's no in-memory list to keep in
// sync and no risk of the file and reality drifting apart.
class AuthManager {
private:
    Database& db;

public:
    explicit AuthManager(Database& database);

    // Kept for interface compatibility with the old file-backed version
    // (main() still calls it once at startup) - with MySQL there is no
    // separate "load" step, so this just verifies the connection is live.
    void loadFromFile();

    bool usernameExists(const std::string& uname) const;
    bool registerUser(const std::string& uname, const std::string& password, const std::string& passphrase);
    bool loginUser(const std::string& uname, const std::string& password) const;

    // Resets a password only if the passphrase set at registration matches -
    // that's what stops "forgot password" from being a free password change
    // for anyone who just knows a username.
    bool resetPassword(const std::string& uname, const std::string& passphrase, const std::string& newPassword);

    // Returns just usernames, in registration order - never passwords or
    // passphrases. Powers the "view registered users" menu option.
    std::vector<std::string> listUsernames() const;

    // Deletes an account only if the password AND the passphrase both match -
    // knowing just the username is never enough to remove someone's account.
    // The `history` table's foreign key is ON DELETE CASCADE, so a user's
    // saved history goes with them automatically.
    bool deleteUser(const std::string& uname, const std::string& password, const std::string& passphrase);
};
