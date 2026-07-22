#include "AuthManager.h"
#include <fstream>

AuthManager::AuthManager(const std::string& file) : filename(file) {}

void AuthManager::loadFromFile() {
    std::ifstream in(filename);
    std::string uname, pwd, passphrase;
    while (in >> uname >> pwd >> passphrase) {
        users.push_back(User(uname, pwd, passphrase));
    }
}

bool AuthManager::usernameExists(const std::string& uname) const {
    for (const auto& u : users) {
        if (u.getUsername() == uname) return true;
    }
    return false;
}

bool AuthManager::registerUser(const std::string& uname, const std::string& password, const std::string& passphrase) {
    if (uname.empty() || password.empty() || passphrase.empty()) return false;
    if (usernameExists(uname)) return false;

    // Storing plain text directly - no hashing.
    users.push_back(User(uname, password, passphrase));

    std::ofstream out(filename, std::ios::app);
    if (out) {
        out << uname << " " << password << " " << passphrase << "\n";
    }
    return true;
}

bool AuthManager::loginUser(const std::string& uname, const std::string& password) const {
    for (const auto& u : users) {
        // Direct plain-text comparison - no hashing.
        if (u.getUsername() == uname && u.checkPassword(password)) {
            return true;
        }
    }
    return false;
}

bool AuthManager::resetPassword(const std::string& uname, const std::string& passphrase, const std::string& newPassword) {
    if (newPassword.empty()) return false;

    bool foundUser = false;

    for (auto& u : users) {
        if (u.getUsername() == uname) {
            // Direct plain-text comparison for the passphrase - no hashing.
            if (!u.checkPassphrase(passphrase)) {
                return false; // wrong passphrase - do not reset
            }
            u = User(uname, newPassword, u.getPassphrase());
            foundUser = true;
            break;
        }
    }
    if (!foundUser) return false;

    // Passwords are plain text now, so this rewrites the whole file with
    // the updated values - same approach as before, just no hashing step.
    std::ofstream out(filename, std::ios::trunc);
    for (const auto& u : users) {
        out << u.getUsername() << " " << u.getPassword() << " " << u.getPassphrase() << "\n";
    }
    return true;
}

std::vector<std::string> AuthManager::listUsernames() const {
    std::vector<std::string> names;
    for (const auto& u : users) {
        names.push_back(u.getUsername());
    }
    return names;
}

bool AuthManager::deleteUser(const std::string& uname, const std::string& password, const std::string& passphrase) {
    for (size_t i = 0; i < users.size(); i++) {
        if (users[i].getUsername() == uname) {
            // Direct plain-text comparison for both password and passphrase.
            if (!users[i].checkPassword(password) || !users[i].checkPassphrase(passphrase)) {
                return false; // wrong password or passphrase - do not delete
            }

            users.erase(users.begin() + i);

            // Rewrite the whole file without this user, since a single line
            // can't be removed in place from a plain text file.
            std::ofstream out(filename, std::ios::trunc);
            for (const auto& u : users) {
                out << u.getUsername() << " " << u.getPassword() << " " << u.getPassphrase() << "\n";
            }
            return true;
        }
    }
    return false; // username not found
}
