#pragma once
#include <string>

// Stores one user's credentials. Password and passphrase are stored as
// plain text in this simple version of the project (no hashing) to avoid complexities
class User {
private:
    std::string username;
    std::string password;
    std::string passphrase;

public:
    User(const std::string& uname, const std::string& pwd, const std::string& phrase);

    std::string getUsername() const;
    std::string getPassword() const;      // needed so AuthManager can save it to file
    std::string getPassphrase() const;    // needed so AuthManager can save it to file
    bool checkPassword(const std::string& attempt) const;
    bool checkPassphrase(const std::string& attempt) const;
};
