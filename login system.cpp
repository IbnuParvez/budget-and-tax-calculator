#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
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
    } else {
        cout << "\nLOGIN ERROR. Please check your username and password.\n";
    }
}

void forgot() {
    int option;

    printBanner("FORGOT PASSWORD");
    cout << "Press 1 to search your password by username" << endl;
    cout << "Press 2 to go back to the main menu" << endl;
    cout << "Enter your choice : ";
    cin >> option;

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
    } else if (option == 2) {
        return;
    } else {
        cout << "\nWrong choice! Please try again.\n";
        forgot();
    }
}

int main() {
    int c;
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
