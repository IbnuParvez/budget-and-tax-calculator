#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <cmath>
#include <cstdio>

#include "AuthManager.h"
#include "TaxPayer.h"
#include "MoneyFormat.h"
#include "Budget.h"
#include "History.h"

using namespace std;

const string USER_FILE = "records.txt";

void printBanner(const string& title) {
    cout << "\n\n=================================================\n";
    cout << "   " << title << "\n";
    cout << "=================================================\n\n";
}

void registrationMenu(AuthManager& auth) {
    string uname, pwd, pwd2, passphrase, passphrase2;

    printBanner("REGISTER");
    cout << "Enter the username : ";
    cin >> uname;
    cout << "Enter the password : ";
    cin >> pwd;
    cout << "Confirm the password : ";
    cin >> pwd2;

    if (pwd != pwd2) {
        cout << "\nPasswords do not match.\n";
        return;
    }

    // This passphrase is not a second password - it's what "forgot password"
    // will ask for later to prove it's really you before resetting anything.
    cout << "Create a passphrase (you'll need this to reset your password later) : ";
    cin >> passphrase;
    cout << "Confirm the passphrase : ";
    cin >> passphrase2;

    if (passphrase != passphrase2) {
        cout << "\nPassphrases do not match.\n";
        return;
    }

    if (auth.registerUser(uname, pwd, passphrase)) {
        cout << "\nRegistration successful! You can now log in.\n";
    } else {
        cout << "\nThat username is already taken, or your input was empty.\n";
    }
}

// Shows every registered username with a number next to it - never
// passwords, never passphrases.
void viewUsersMenu(AuthManager& auth) {
    printBanner("REGISTERED USERS");

    vector<string> names = auth.listUsernames();

    if (names.empty()) {
        cout << "No users have registered yet.\n";
        return;
    }

    for (size_t i = 0; i < names.size(); i++) {
        cout << (i + 1) << ". " << names[i] << "\n";
    }
}

// Deletes an account. Requires the username, password, AND passphrase to
// all match - this stops anyone from deleting an account just by knowing
// (or guessing) the username off the "view registered users" list.
void deleteUserMenu(AuthManager& auth) {
    string uname, pwd, passphrase;

    printBanner("DELETE USER");
    cout << "Enter the username : ";
    cin >> uname;
    cout << "Enter the password : ";
    cin >> pwd;
    cout << "Enter the passphrase : ";
    cin >> passphrase;

    if (auth.deleteUser(uname, pwd, passphrase)) {
        cout << "\nUser \"" << uname << "\" has been deleted.\n";

        // Also remove that user's saved history so nothing orphaned is left
        // behind for a username that no longer exists.
        string historyFile = "history_" + uname + ".txt";
        remove(historyFile.c_str());
    } else {
        cout << "\nCould not delete: username, password, or passphrase was incorrect.\n";
    }
}

// Keeps asking until the three percentages actually add up to 100,
// instead of assuming a fixed 50/30/20 split.
void getBudgetPercentages(double& needsPct, double& personalPct, double& savingsPct) {
    while (true) {
        cout << "\nChoose how to split your income after tax.\n";
        cout << "Needs % : ";
        cin >> needsPct;
        cout << "Personal use % : ";
        cin >> personalPct;
        cout << "Savings % : ";
        cin >> savingsPct;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nPlease enter numbers only.\n";
            continue;
        }

        double total = needsPct + personalPct + savingsPct;
        if (abs(total - 100.0) < 0.01) {
            return;
        }
        cout << "\nThose percentages add up to " << total << "%, not 100%. Please try again.\n";
    }
}

// Fix: cin >> bool only accepts exactly 0 or 1 - anything else (like a
// mistyped "112") sets the stream's fail state, which then silently breaks
// every read that comes after it (e.g. the income prompt gets skipped
// entirely and the calculation runs on garbage). This keeps re-asking
// until it actually gets a clean 0 or 1.
bool readYesNo(const string& prompt) {
    int input;
    while (true) {
        cout << prompt;
        cin >> input;

        if (cin.fail() || (input != 0 && input != 1)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nPlease enter 1 or 0.\n";
            continue;
        }
        return input == 1;
    }
}

// Same idea for income - reject anything that isn't a valid, non-negative number.
double readIncome() {
    double income;
    while (true) {
        cout << "What is your annual income? ";
        cin >> income;

        if (cin.fail() || income < 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nPlease enter a valid non-negative number.\n";
            continue;
        }
        return income;
    }
}

// Runs one tax + budget calculation, prints the result, and records it
// into this user's history so trends show up under "View History".
void runCalculation(HistoryManager& history) {
    cout << "\n----- Tax Calculator -----\n";
    bool identity = readYesNo("Are you an Individual? (1 for yes, 0 for no): ");
    double income = readIncome();

    // Base-class pointer holding a derived object - this IS polymorphism:
    // payer->getTax() below always runs the correct calculateTax() version.
    TaxPayer* payer = nullptr;

    if (identity) {
        payer = new Individual(income);
    } else {
        cout << "So you are a company.\n";
        bool local = readYesNo("Are you a local company? (1 for yes, 0 for no): ");
        payer = new NonIndividual(income, local);
    }

    double taxOwed = payer->getTax();
    cout << "\nYour tax payable is: " << formatMoney(taxOwed) << "\n";

    double remainingIncome = income - taxOwed;

    double needsPct, personalPct, savingsPct;
    getBudgetPercentages(needsPct, personalPct, savingsPct);
    showBudgetAllocation(remainingIncome, needsPct, personalPct, savingsPct);

    history.addEntry(income, taxOwed, remainingIncome);

    delete payer;
}

// The user lands here after logging in and STAYS here - calculating again,
// checking history, or logging out are all just menu options, none of them
// throw the user back to the login screen except logout.
void dashboardMenu(const string& username, AuthManager& auth) {
    HistoryManager history(username);
    history.load();

    int choice;
    while (true) {
        printBanner("DASHBOARD - " + username);
        cout << "| Press 1 to CALCULATE tax & budget      |\n";
        cout << "| Press 2 to VIEW HISTORY (bar graph)    |\n";
        cout << "| Press 3 to CLEAR HISTORY               |\n";
        cout << "| Press 4 to LOGOUT                      |\n";
        cout << "\nPlease enter your choice : ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1:
                runCalculation(history);
                break;
            case 2:
                history.showHistoryGraphs();
                break;
            case 3: {
                // Clearing history can't be undone, so confirm first rather
                // than wiping years of records on a fat-fingered "3".
                bool confirmed = readYesNo("Are you sure you want to clear your history? (1 for yes, 0 for no): ");
                if (confirmed) {
                    history.clearHistory();
                    cout << "\nYour history has been cleared.\n";
                } else {
                    cout << "\nCancelled. Your history was not cleared.\n";
                }
                break;
            }
            case 4:
                cout << "\nLogging out...\n";
                return; // back to the login menu, not exiting the program
            default:
                cout << "\nPlease select from the options given above.\n";
        }
    }
}

void loginMenu(AuthManager& auth) {
    string uname, pwd;

    printBanner("LOGIN");
    cout << "USERNAME : ";
    cin >> uname;
    cout << "PASSWORD : ";
    cin >> pwd;

    if (auth.loginUser(uname, pwd)) {
        cout << "\n" << uname << ", your LOGIN was successful! Thanks for logging in!\n";
        dashboardMenu(uname, auth);
    } else {
        cout << "\nLOGIN ERROR. Please check your username and password.\n";
    }
}

void forgotMenu(AuthManager& auth) {
    int option;

    while (true) {
        printBanner("FORGOT PASSWORD");
        cout << "Press 1 to reset your password\n";
        cout << "Press 2 to go back to the main menu\n";
        cout << "Enter your choice : ";
        cin >> option;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nPlease enter a number.\n";
            continue;
        }

        if (option == 1) {
            string uname, passphrase, newPwd, newPwd2;
            cout << "\nEnter your username : ";
            cin >> uname;

            if (!auth.usernameExists(uname)) {
                cout << "\nSorry, that account was not found.\n";
                return;
            }

            cout << "Enter your passphrase : ";
            cin >> passphrase;

            cout << "Enter a new password : ";
            cin >> newPwd;
            cout << "Confirm the new password : ";
            cin >> newPwd2;

            if (newPwd != newPwd2) {
                cout << "\nPasswords do not match.\n";
                return;
            }

            if (auth.resetPassword(uname, passphrase, newPwd)) {
                cout << "\nPassword reset successful. You can now log in.\n";
            } else {
                cout << "\nThat passphrase does not match our records. Password was not changed.\n";
            }
            return;
        } else if (option == 2) {
            return;
        } else {
            cout << "\nWrong choice! Please try again.\n";
        }
    }
}

int main() {
    cout << fixed << setprecision(2);   // normal decimal formatting, no scientific notation

    AuthManager auth(USER_FILE);
    auth.loadFromFile();

    int c;
    while (true) {
        printBanner("WELCOME TO THE LOGIN PAGE");
        cout << "| Press 1 to LOGIN                       |\n";
        cout << "| Press 2 to REGISTER                    |\n";
        cout << "| Press 3 to UPDATE YOUR PASSWORD        |\n";
        cout << "| Press 4 to VIEW REGISTERED USERS       |\n";
        cout << "| Press 5 to DELETE A USER               |\n";
        cout << "| Press 6 to EXIT                        |\n";
        cout << "\nPlease enter your choice : ";
        cin >> c;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a number.\n";
            continue;
        }

        switch (c) {
            case 1: loginMenu(auth); break;
            case 2: registrationMenu(auth); break;
            case 3: forgotMenu(auth); break;
            case 4: viewUsersMenu(auth); break;
            case 5: deleteUserMenu(auth); break;
            case 6: cout << "\nThank you!\n\n"; return 0;
            default: cout << "\nPlease select from the options given above.\n";
        }
    }

    return 0;
}
