#pragma once
#include <string>
#include <vector>

struct HistoryEntry {
    double income;
    double tax;
    double remainingIncome;
};

// Tracks one user's past tax/budget calculations across sessions, stored in
// "history_<username>.txt". This is what powers the dashboard's "View
// History" option - bar graphs across past calculations, not just one.
class HistoryManager {
private:
    std::string username;
    std::string filename;
    std::vector<HistoryEntry> entries;

public:
    explicit HistoryManager(const std::string& uname);

    void load();
    void addEntry(double income, double tax, double remainingIncome);
    void showHistoryGraphs() const;
    void clearHistory();
};
