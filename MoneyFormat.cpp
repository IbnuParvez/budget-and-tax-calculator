#include "MoneyFormat.h"

std::string formatMoney(double amount) {
    long long whole = (long long)(amount);
    int cents = (int)((amount - whole) * 100 + 0.5);

    // Fix: rounding can push cents to exactly 100 (e.g. 14884.995 rounds up),
    // which must carry into the whole number instead of printing ".100".
    if (cents >= 100) {
        whole += 1;
        cents -= 100;
    }

    std::string wholeStr = std::to_string(whole < 0 ? -whole : whole);

    std::string withCommas;
    int count = 0;
    for (int i = (int)wholeStr.size() - 1; i >= 0; i--) {
        withCommas = wholeStr[i] + withCommas;
        count++;
        if (count % 3 == 0 && i != 0) {
            withCommas = "," + withCommas;
        }
    }

    std::string result = (amount < 0 ? "-" : "") + withCommas + "."
                          + (cents < 10 ? "0" : "") + std::to_string(cents);
    return result;
}
