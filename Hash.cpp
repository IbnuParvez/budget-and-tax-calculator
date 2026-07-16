#include "Hash.h"
#include <functional>

std::string hashPassword(const std::string& password) {
    std::hash<std::string> hasher;
    size_t hashed = hasher(password);
    return std::to_string(hashed);
}
