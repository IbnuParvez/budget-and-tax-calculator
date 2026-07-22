#include "History.h"
#include "BarGraph.h"
#include <iostream>
#include <iomanip>
using namespace std;

namespace {
    // Pulls this user's history rows out in insertion order - shared by
    // showHistoryGraphs() (needs all of them) and the trend line (needs the
    // last two), so there's one place that knows how to read the table.
    vector<HistoryEntry> fetchEntries(Database& db, const string& username) {
        vector<HistoryEntry> entries;

        string sql = "SELECT income, tax, remaining_income FROM history "
                     "WHERE username = '" + db.escape(username) + "' ORDER BY id ASC";
        db.execute(sql);

        MYSQL_RES* res = mysql_store_result(db.handle());
        if (res) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                HistoryEntry e;
                e.income = row[0] ? atof(row[0]) : 0.0;
                e.tax = row[1] ? atof(row[1]) : 0.0;
                e.remainingIncome = row[2] ? atof(row[2]) : 0.0;
                entries.push_back(e);
            }
            mysql_free_result(res);
        }
        return entries;
    }
}

HistoryManager::HistoryManager(Database& database, const string& uname) : db(database), username(uname) {}

void HistoryManager::load() {
    // No-op with MySQL - see the comment on load() in History.h.
}

void HistoryManager::addEntry(double income, double tax, double remainingIncome) {
    string sql = "INSERT INTO history (username, income, tax, remaining_income) VALUES ('"
                 + db.escape(username) + "', "
                 + to_string(income) + ", "
                 + to_string(tax) + ", "
                 + to_string(remainingIncome) + ")";
    db.execute(sql);
}

void HistoryManager::showHistoryGraphs() const {
    vector<HistoryEntry> entries = fetchEntries(db, username);

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
    string sql = "DELETE FROM history WHERE username = '" + db.escape(username) + "'";
    db.execute(sql);
}
