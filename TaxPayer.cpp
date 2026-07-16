#include "TaxPayer.h"
#include <algorithm>

TaxPayer::TaxPayer() {
    // Fix from the original: tax and taxableIncome must start at 0,
    // otherwise they begin as whatever random value was already in memory.
    taxableIncome = 0;
    tax = 0;
}

void TaxPayer::setTaxableIncome(double income) { taxableIncome = income; }
double TaxPayer::getTaxableIncome() const { return taxableIncome; }
double TaxPayer::getTax() const { return tax; }

Individual::Individual(double startingIncome) {
    taxableIncome = startingIncome;
    calculateTax();
}

void Individual::calculateTax() {
    double remainingIncome = taxableIncome;
    double limits[] = {24000.0, 8333.33, 467666.67, 300000.0};
    double rates[]  = {0.10, 0.25, 0.30, 0.325};

    tax = 0;
    for (int i = 0; i < 4 && remainingIncome > 0; i++) {
        double amountToTax = std::min(remainingIncome, limits[i]);
        tax += amountToTax * rates[i];
        remainingIncome -= amountToTax;
    }
    if (remainingIncome > 0) {
        tax += remainingIncome * 0.35;
    }
    tax -= 2400.0; // personal relief
    if (tax < 0) tax = 0.0;
}

// Fix from the original: a constructor should not do cin >> local itself -
// that mixes input-handling with object logic. The caller (main) now asks
// the question and passes the answer in.
NonIndividual::NonIndividual(double totalIncome, bool local) : isLocalCompany(local) {
    taxableIncome = totalIncome;
    calculateTax();
}

void NonIndividual::calculateTax() {
    tax = isLocalCompany ? taxableIncome * 0.3 : taxableIncome * 0.375;
}
