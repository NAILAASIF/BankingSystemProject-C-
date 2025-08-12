#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <windows.h>
#include <thread>
#include <chrono>
#include <limits>
#include <conio.h>  // For _getch() to hide password input
using namespace std;

// Handle for console colors
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

// Function to set console text color
void setColor(int color) {
    SetConsoleTextAttribute(h, color);
}

// Function to show animated title at the start
void showTitle() {
    setColor(9); // Set text color to blue
    string title = "WELCOME TO BANKING SYSTEM";
    cout << "\n====================================\n";
    
    // Print title with animation effect
    for (char c : title) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(30));
    }
    cout << "\n====================================\n";
    setColor(7); // Reset color to default
}

// BankAccount class for all banking operations
class BankAccount {
private:
    string name;           // Customer name
    int accountNumber;     // Account number
    double balance;        // Account balance

    // Function to check if account number already exists in file
    bool accountExists(int accNum) {
        ifstream inFile("accounts.txt");
        string line;
        while (getline(inFile, line)) {
            size_t pos = line.find('|');
            int acc = stoi(line.substr(0, pos));
            if (acc == accNum) {
                return true; // Account found
            }
        }
        return false; // Account does not exist
    }

public:
    // Constructor initializes values
    BankAccount() : accountNumber(0), balance(0.0) {}

    // Function to create a new account
    void createAccount() {
        system("cls");
        setColor(9);
        cout << "===============================\n";
        cout << "       CREATE NEW ACCOUNT      \n";
        cout << "===============================\n";
        setColor(7);

        // Take account details from user
        cout << "Enter Account Number : ";
        cin >> accountNumber;

        // Check if account number already exists
        if (accountExists(accountNumber)) {
            setColor(4);
            cout << "\nError: Account number already exists!\n";
            setColor(7);
            return;
        }

        // Clear input buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter Full Name      : ";
        getline(cin, name);

        cout << "Enter Initial Balance: ";
        cin >> balance;

        // Balance cannot be negative
        if (balance < 0) {
            setColor(4);
            cout << "\nInitial balance cannot be negative!\n";
            setColor(7);
            return;
        }

        // Save account details in file
        ofstream outFile("accounts.txt", ios::app);
        if (outFile.is_open()) {
            outFile << accountNumber << "|" << name << "|" << balance << "\n";
            outFile.close();
            setColor(2);
            cout << "\nAccount Created Successfully!\n";
        } else {
            setColor(4);
            cout << "\nError: Could not open file!\n";
        }
        setColor(7);
    }

    // Function to deposit money
    void deposit(int accNum, double amount) {
        if (amount <= 0) {
            setColor(4);
            cout << "\nAmount must be greater than 0!\n";
            setColor(7);
            return;
        }

        system("cls");
        bool accountExistsFlag = false;  // Account found or not
        bool transactionDone = false;    // Transaction successful or not
        double updatedBalance = 0.0;

        // Open files: accounts.txt for reading and temp.txt for writing updated data
        ifstream inFile("accounts.txt");
        ofstream tempFile("temp.txt");

        int acc;
        string accName;
        double bal;
        string line;

        // Read file line by line
        while (getline(inFile, line)) {
            size_t pos1 = line.find('|');
            size_t pos2 = line.rfind('|');
            acc = stoi(line.substr(0, pos1));
            accName = line.substr(pos1 + 1, pos2 - pos1 - 1);
            bal = stod(line.substr(pos2 + 1));

            if (acc == accNum) {
                accountExistsFlag = true;
                bal += amount; // Add amount to balance
                updatedBalance = bal;
                transactionDone = true;
            }
            // Write updated data to temp file
            tempFile << acc << "|" << accName << "|" << bal << "\n";
        }

        // Close files
        inFile.close();
        tempFile.close();

        // Replace original file with updated file
        remove("accounts.txt");
        rename("temp.txt", "accounts.txt");

        // Show result to user
        if (transactionDone) {
            setColor(2);
            cout << "\nDeposit successful!\n";
            cout << "New Balance: " << updatedBalance << "\n";
        } else if (!accountExistsFlag) {
            setColor(4);
            cout << "\nAccount not found!\n";
        }
        setColor(7);
    }

    // Function to withdraw money
    void withdraw(int accNum, double amount) {
        if (amount <= 0) {
            setColor(4);
            cout << "\nAmount must be greater than 0!\n";
            setColor(7);
            return;
        }

        system("cls");
        bool accountExistsFlag = false;
        bool transactionDone = false;
        double updatedBalance = 0.0;

        ifstream inFile("accounts.txt");
        ofstream tempFile("temp.txt");

        int acc;
        string accName;
        double bal;
        string line;

        while (getline(inFile, line)) {
            size_t pos1 = line.find('|');
            size_t pos2 = line.rfind('|');
            acc = stoi(line.substr(0, pos1));
            accName = line.substr(pos1 + 1, pos2 - pos1 - 1);
            bal = stod(line.substr(pos2 + 1));

            if (acc == accNum) {
                accountExistsFlag = true;
                if (bal >= amount) {
                    bal -= amount; // Deduct amount
                    updatedBalance = bal;
                    transactionDone = true;
                }
            }
            tempFile << acc << "|" << accName << "|" << bal << "\n";
        }

        inFile.close();
        tempFile.close();
        remove("accounts.txt");
        rename("temp.txt", "accounts.txt");

        // Show result
        if (transactionDone) {
            setColor(2);
            cout << "\nWithdrawal successful!\n";
            cout << "New Balance: " << updatedBalance << "\n";
        } else if (accountExistsFlag) {
            setColor(4);
            cout << "\nInsufficient balance!\n";
        } else {
            setColor(4);
            cout << "\nAccount not found!\n";
        }
        setColor(7);
    }

    // Delete an account completely
    void deleteAccount(int accNum) {
        system("cls");
        ifstream inFile("accounts.txt");
        ofstream tempFile("temp.txt");
        bool found = false;
        string line;

        while (getline(inFile, line)) {
            size_t pos = line.find('|');
            int acc = stoi(line.substr(0, pos));

            if (acc != accNum) {
                tempFile << line << "\n";
            } else {
                found = true;
            }
        }

        inFile.close();
        tempFile.close();
        remove("accounts.txt");
        rename("temp.txt", "accounts.txt");

        if (found) {
            setColor(2);
            cout << "\nAccount deleted successfully!\n";
        } else {
            setColor(4);
            cout << "\nAccount not found!\n";
        }
        setColor(7);
    }

    // Check account balance
    void checkBalance(int accNum) {
        system("cls");
        bool found = false;
        ifstream inFile("accounts.txt");

        if (!inFile) {
            setColor(4);
            cout << "\nError: No accounts found!\n";
            setColor(7);
            return;
        }

        string line;
        int acc;
        string accName;
        double bal;

        while (getline(inFile, line)) {
            size_t pos1 = line.find('|');
            size_t pos2 = line.rfind('|');
            acc = stoi(line.substr(0, pos1));
            accName = line.substr(pos1 + 1, pos2 - pos1 - 1);
            bal = stod(line.substr(pos2 + 1));

            if (acc == accNum) {
                setColor(9);
                cout << "\n=============================\n";
                cout << "      ACCOUNT INFORMATION    \n";
                cout << "=============================\n";
                setColor(7);
                cout << left << setw(15) << "Account No:" << acc << "\n";
                cout << left << setw(15) << "Name:" << accName << "\n";
                cout << left << setw(15) << "Balance:" << bal << "\n";
                setColor(9);
                cout << "=============================\n";
                setColor(7);
                found = true;
                break;
            }
        }

        if (!found) {
            setColor(4);
            cout << "\nAccount not found!\n";
        }

        inFile.close();
    }

    // Show all accounts in a table (Access by Admin only)
    void viewAllAccounts() {
        system("cls");
        ifstream inFile("accounts.txt");

        if (!inFile) {
            setColor(4);
            cout << "\nError: No accounts found!\n";
            setColor(7);
            return;
        }

        setColor(9);
        cout << "\n=============================================\n";
        cout << "           ALL ACCOUNTS LIST                \n";
        cout << "=============================================\n";
        setColor(7);
        cout << left << setw(15) << "Account No" << setw(25) << "Name" << "Balance\n";
        cout << "---------------------------------------------\n";

        string line;
        while (getline(inFile, line)) {
            size_t pos1 = line.find('|');
            size_t pos2 = line.rfind('|');
            int acc = stoi(line.substr(0, pos1));
            string accName = line.substr(pos1 + 1, pos2 - pos1 - 1);
            double bal = stod(line.substr(pos2 + 1));

            cout << left << setw(15) << acc << setw(25) << accName << bal << "\n";
        }

        setColor(9);
        cout << "=============================================\n";
        setColor(7);

        inFile.close();
    }
};

int main() {
    BankAccount bank;
    int choice;
    bool firstTime = true;

    do {
        system("cls");
        if (firstTime) {
            showTitle();  // Show animation first time
            firstTime = false;
        } else {
            // Display simple title
            setColor(9);
            cout << "\n====================================\n";
            cout << "      WELCOME TO BANKING SYSTEM     \n";
            cout << "====================================\n";
            setColor(7);
        }

        // Display main menu
        cout << "1. Create Account\n";
        cout << "2. Deposit\n";
        cout << "3. Withdraw\n";
        cout << "4. Check Balance\n";
        cout << "5. Delete Account\n";
        cout << "6. View All Accounts (Admin)\n";
        cout << "7. Exit\n";
        setColor(9);
        cout << "====================================\n";
        setColor(7);
        cout << "Enter your choice: ";
        cin >> choice;

        // Perform action based on choice
        switch (choice) {
        case 1:
            bank.createAccount();
            break;
        case 2: {
            int acc;
            double amount;
            cout << "\nEnter Account Number: ";
            cin >> acc;
            cout << "Enter Amount to Deposit: ";
            cin >> amount;
            bank.deposit(acc, amount);
            break;
        }
        case 3: {
            int acc;
            double amount;
            cout << "\nEnter Account Number: ";
            cin >> acc;
            cout << "Enter Amount to Withdraw: ";
            cin >> amount;
            bank.withdraw(acc, amount);
            break;
        }
        case 4: {
            int acc;
            cout << "\nEnter Account Number: ";
            cin >> acc;
            bank.checkBalance(acc);
            break;
        }
        case 5: {
            int acc;
            cout << "\nEnter Account Number to Delete: ";
            cin >> acc;
            bank.deleteAccount(acc);
            break;
        }
        case 6: {
            // Admin password check with hidden input
            string password = "";
            char ch;
            cout << "\nEnter Admin Password: ";
            while ((ch = _getch()) != 13) {  // 13 = Enter key
                if (ch == '\b') { // Handle backspace
                    if (!password.empty()) {
                        password.pop_back();
                        cout << "\b \b"; // Erase *
                    }
                } else {
                    password.push_back(ch);
                    cout << "*"; // Mask input
                }
            }
            cout << endl;

            if (password == "admin123") { // check Admin password 
                bank.viewAllAccounts();
            } else {
                setColor(4);
                cout << "\nAccess Denied! Incorrect Password.\n";
                setColor(7);
            }
            break;
        }
        case 7:
            setColor(2);
            cout << "\nThank you for using the Banking System!\n";
            setColor(7);
            system("pause");
            exit(0);
        default:
            setColor(4);
            cout << "\nInvalid choice! Try again.\n";
            setColor(7);
        }

        if (choice != 7) {
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
        }
    } while (choice != 7);

    return 0;
}
