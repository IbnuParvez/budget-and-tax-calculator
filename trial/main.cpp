#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm> // needed for min() used in the tax calculation
#include <iomanip>   // needed to force normal decimal formatting (no scientific notation)
using namespace std;

const string USER_FILE = "records.txt";

struct UserRecord {
    string username;
    string password;
};
vector<UserRecord> users;

void login();
void registration();
void forgot();
void printBanner(const string& title);
void loadUsersFromFile();
void startTaxCalculation(); // new: runs after a successful login

// ==================================================
// TAX CLASSES
// (same idea as your Individual/NonIndividual example)
// ==================================================

class TaxPayer {
  protected:
    double taxableIncome;
    double tax;
  public:
    // Fix: tax must start at 0, otherwise it begins as
    // whatever random value was already sitting in memory.
    TaxPayer() {
        taxableIncome = 0;
        tax = 0;
    }

    double setTaxableIncome(double s) {
        return taxableIncome = s;
    }
    double getTaxableIncome() {
        return taxableIncome;
    }
    double getTax() {
        return tax;
    }
};

class NonIndividual : public TaxPayer {
  public:
    NonIndividual(double totalIncome) {
        taxableIncome = totalIncome;
        bool local;
        cout << "\nAre you a local company? (1 for yes, 0 for no): ";
        cin >> local;

        if (local) {
            tax = totalIncome * 0.3;
        } else {
            tax = totalIncome * 0.375;
        }
    }
};

class Individual : public TaxPayer {
  public:
    Individual(double startingIncome) {
        taxableIncome = startingIncome;
        double remainingIncome = taxableIncome;

        double limits[] = {24000.0, 8333.33, 467666.67, 300000.0};
        double rates[]  = {0.10, 0.25, 0.30, 0.325};

        for (int i = 0; i < 4 && remainingIncome > 0; i++) {
            double amountToTax = min(remainingIncome, limits[i]);
            tax += amountToTax * rates[i];
            remainingIncome -= amountToTax;
        }

        if (remainingIncome > 0) {
            tax += remainingIncome * 0.35;
        }

        tax -= 2400.0; // personal relief
        if (tax < 0) {
            tax = 0.0;
        }
    }
};

// ==================================================
// FORMATTING HELPER
// Turns 2032070000000.00 into 2,032,070,000,000.00
// ==================================================
string formatMoney(double amount) {
    // Round to 2 decimal places first
    long long whole = (long long)(amount);
    int cents = (int)((amount - whole) * 100 + 0.5);

    string wholeStr = to_string(whole < 0 ? -whole : whole);

    // Insert commas every 3 digits from the right
    string withCommas;
    int count = 0;
    for (int i = (int)wholeStr.size() - 1; i >= 0; i--) {
        withCommas = wholeStr[i] + withCommas;
        count++;
        if (count % 3 == 0 && i != 0) {
            withCommas = "," + withCommas;
        }
    }

    string result = (amount < 0 ? "-" : "") + withCommas + "."
                     + (cents < 10 ? "0" : "") + to_string(cents);
    return result;
}

// ==================================================
// LOGIN SYSTEM
// ==================================================

void printBanner(const string& title) {
    cout << "\n\n";
    cout << "=================================================\n";
    cout << "   " << title << "\n";
    cout << "=================================================\n\n";
}

void loadUsersFromFile() {
    ifstream file(USER_FILE);
    string id, pass;
    while (file >> id >> pass) {
        users.push_back({id, pass});
    }
    file.close();
}

void registration() {
    string ruserId, rpassword;

    printBanner("REGISTER");
    cout << "Enter the username : ";
    cin >> ruserId;
    cout << "Enter the password : ";
    cin >> rpassword;

    for (const auto& u : users) {
        if (u.username == ruserId) {
            cout << "\nThat username is already taken.\n";
            return;
        }
    }

    users.push_back({ruserId, rpassword});

    ofstream f1(USER_FILE, ios::app);
    if (f1) {
        f1 << ruserId << " " << rpassword << endl;
        f1.close();
    }

    cout << "\nRegistration successful! You can now log in.\n";
}

void login() {
    string userId, password;

    printBanner("LOGIN");
    cout << "USERNAME : ";
    cin >> userId;
    cout << "PASSWORD : ";
    cin >> password;

    bool found = false;
    for (const auto& u : users) {
        if (u.username == userId && u.password == password) {
            found = true;
            break;
        }
    }

    if (found) {
        cout << "\n" << userId << ", your LOGIN was successful! Thanks for logging in!\n";
        startTaxCalculation(); // go straight into the tax calculator
    } else {
        cout << "\nLOGIN ERROR. Please check your username and password.\n";
    }
}

void forgot() {
    int option;

    while (true) {
        printBanner("FORGOT PASSWORD");
        cout << "Press 1 to search your password by username" << endl;
        cout << "Press 2 to go back to the main menu" << endl;
        cout << "Enter your choice : ";
        cin >> option;

        // Fix: if the user types letters instead of a number, cin enters
        // a fail state and every future read fails instantly without
        // waiting for input, causing an infinite "Wrong choice!" loop.
        // Clear the error and throw away the bad input before continuing.
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nPlease enter a number.\n";
            continue;
        }

        if (option == 1) {
            string suserId;
            cout << "\nEnter the username you remember : ";
            cin >> suserId;

            bool found = false;
            string foundPassword;
            for (const auto& u : users) {
                if (u.username == suserId) {
                    found = true;
                    foundPassword = u.password;
                    break;
                }
            }

            if (found) {
                cout << "\nYour account was found.\n";
                cout << "Your password is: " << foundPassword << "\n";
            } else {
                cout << "\nSorry, your account was not found.\n";
            }
            return;
        } else if (option == 2) {
            return;
        } else {
            cout << "\nWrong choice! Please try again.\n";
        }
    }
}

// ==================================================
// BUDGET ALLOCATION
// Splits whatever income is LEFT after tax using the
// classic 50/30/20 rule (Needs / Wants / Savings).
// Uses arrays + a loop, same style as your notes.
// ==================================================
void showBudgetAllocation(double remainingIncome) {
    string categories[3] = {"Needs (50%)", "Wants (30%)", "Savings (20%)"};
    double percentages[3] = {0.50, 0.30, 0.20};

    cout << "\n----- Monthly Budget Allocation -----\n";
    cout << "Based on your income after tax: " << formatMoney(remainingIncome) << "\n";

    for (int i = 0; i < 3; i++) {
        double amount = remainingIncome * percentages[i];
        cout << categories[i] << ": " << formatMoney(amount) << "\n";
    }
}

// ==================================================
// TAX CALCULATION - asks the user for input, then
// creates the correct object (Individual or NonIndividual)
// ==================================================
void startTaxCalculation() {
    bool identity;
    double income;

    cout << "\n----- Tax Calculator -----\n";
    cout << "Are you an Individual? (1 for yes, 0 for no): ";
    cin >> identity;

    cout << "What is your annual income? ";
    cin >> income;

    double taxOwed;

    if (identity) {
        Individual me(income);
        taxOwed = me.getTax();
        cout << "\nYour tax payable is: " << formatMoney(taxOwed) << "\n";
    } else {
        NonIndividual us(income);
        taxOwed = us.getTax();
        cout << "\nYour tax payable is: " << formatMoney(taxOwed) << "\n";
    }

    double remainingIncome = income - taxOwed;
    showBudgetAllocation(remainingIncome);
}

// ==================================================
// MAIN
// ==================================================
int main() {
    int c;

    // Fix: without this, cout switches to scientific notation
    // (e.g. 2.03207e+12) for large numbers. Fixed + setprecision
    // forces normal decimal formatting like 2032070000000.00
    cout << fixed << setprecision(2);

    loadUsersFromFile();

    while (true) {
        printBanner("WELCOME TO THE LOGIN PAGE");
        cout << "| Press 1 to LOGIN                       |" << endl;
        cout << "| Press 2 to REGISTER                    |" << endl;
        cout << "| Press 3 if you forgot your PASSWORD    |" << endl;
        cout << "| Press 4 to EXIT                        |" << endl;
        cout << "\nPlease enter your choice : ";
        cin >> c;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a number.\n";
            continue;
        }

        switch (c) {
            case 1:
                login();
                break;
            case 2:
                registration();
                break;
            case 3:
                forgot();
                break;
            case 4:
                cout << "\nThank you!\n\n";
                return 0;
            default:
                cout << "\nPlease select from the options given above.\n";
        }
    }

    return 0;
}
