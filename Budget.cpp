#include "Budget.h"
#include "MoneyFormat.h"
#include <iostream>
using namespace std;

void showBudgetAllocation(double remainingIncome, double needsPct, double personalPct, double savingsPct) {
    string categories[3] = {"Needs", "Personal use", "Savings"};
    double percentages[3] = {needsPct / 100.0, personalPct / 100.0, savingsPct / 100.0};

    cout << "\n----- Monthly Budget Allocation -----\n";
    cout << "Based on your income after tax: " << formatMoney(remainingIncome) << "\n";

    for (int i = 0; i < 3; i++) {
        double amount = remainingIncome * percentages[i];
        cout << categories[i] << " (" << percentages[i] * 100 << "%): " << formatMoney(amount) << "\n";
    }
}
