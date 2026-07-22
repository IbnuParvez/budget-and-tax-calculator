#pragma once
#include <string>
#include <vector>
#include "Database.h"

struct HistoryEntry {
    double income;
    double tax;
    double remainingIncome;
};

// Tracks one user's past tax/budget calculations across sessions, now in
// the `history` table (previously "history_<username>.txt"). This is what
// powers the dashboard's "View History" option - bar graphs across past
// calculations, not just one.
class HistoryManager {
private:
    Database& db;
    std::string username;

public:
    HistoryManager(Database& database, const std::string& uname);

    // Kept for interface compatibility with the old file-backed version -
    // with MySQL, entries are fetched fresh whenever they're needed instead
    // of being cached in memory, so this is a no-op.
    void load();

    void addEntry(double income, double tax, double remainingIncome);
    void showHistoryGraphs() const;
    void clearHistory();
};
