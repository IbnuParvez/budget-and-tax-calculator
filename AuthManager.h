#pragma once
#include <string>
#include <vector>
#include "User.h"

// Owns the list of users and every operation on it. main() never touches
// user data directly anymore - it only ever talks to this class.
class AuthManager {
private:
    std::vector<User> users;
    std::string filename;

public:
    explicit AuthManager(const std::string& file);

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
    bool deleteUser(const std::string& uname, const std::string& password, const std::string& passphrase);
};
