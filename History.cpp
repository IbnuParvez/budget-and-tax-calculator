#include "History.h"
#include "BarGraph.h"
#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

HistoryManager::HistoryManager(const string& uname) : username(uname) {
    filename = "history_" + uname + ".txt";
}

void HistoryManager::load() {
    entries.clear();
    ifstream in(filename);
    double income, tax, remaining;
    while (in >> income >> tax >> remaining) {
        entries.push_back({income, tax, remaining});
    }
}

void HistoryManager::addEntry(double income, double tax, double remainingIncome) {
    entries.push_back({income, tax, remainingIncome});

    ofstream out(filename, ios::app);
    if (out) {
        out << income << " " << tax << " " << remainingIncome << "\n";
    }
}

void HistoryManager::showHistoryGraphs() const {
    if (entries.empty()) {
        cout << "\nYou don't have any past calculations yet. Run one from the dashboard first.\n";
        return;
    }

    vector<string> labels;
    vector<double> incomeValues;
    vector<double> taxValues;

    for (size_t i = 0; i < entries.size(); i++) {
        labels.push_back("Calc " + to_string(i + 1));
        incomeValues.push_back(entries[i].income);
        taxValues.push_back(entries[i].tax);
    }

    cout << "\n===== Income History =====\n";
    drawBarGraph(labels, incomeValues);

    cout << "===== Tax Paid History =====\n";
    drawBarGraph(labels, taxValues);

    // "Potential rate increase or decrease" - compare the latest calculation
    // to the one before it, since that's the trend a user actually cares about.
    if (entries.size() >= 2) {
        double prevTax = entries[entries.size() - 2].tax;
        double latestTax = entries.back().tax;

        if (prevTax > 0) {
            double percentChange = ((latestTax - prevTax) / prevTax) * 100.0;
            cout << "Your tax " << (percentChange >= 0 ? "increased" : "decreased")
                 << " by " << fixed << setprecision(1)
                 << (percentChange < 0 ? -percentChange : percentChange)
                 << "% compared to your last calculation.\n";
        }
    }
}

void HistoryManager::clearHistory() {
    entries.clear();

    // Opening in trunc mode with nothing written wipes the file to empty,
    // same idea as resetPassword rewriting records.txt from scratch.
    ofstream out(filename, ios::trunc);
}
