#include "User.h"

User::User(const std::string& uname, const std::string& pwd, const std::string& phrase)
    : username(uname), password(pwd), passphrase(phrase) {}

std::string User::getUsername() const {
    return username;
}

std::string User::getPassword() const {
    return password;
}

std::string User::getPassphrase() const {
    return passphrase;
}

bool User::checkPassword(const std::string& attempt) const {
    return password == attempt;
}

bool User::checkPassphrase(const std::string& attempt) const {
    return passphrase == attempt;
}
