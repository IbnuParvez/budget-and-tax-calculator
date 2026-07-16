#pragma once
#include <string>

// Turns a plain-text password into a scrambled, fixed-form string.
// Not cryptographic-grade security, but enough to demonstrate that
// raw passwords should never be stored or compared directly - and it's
// genuinely one-way, so "forgot password" has to RESET, not reveal.
std::string hashPassword(const std::string& password);
