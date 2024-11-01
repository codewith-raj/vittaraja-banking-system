#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <windows.h>
using namespace std;

#define MIN_BALANCE 500

class InsufficientFunds {};

class Account {
private:
    long accountNumber;
    string firstName;
    string lastName;
    float balance;
    static long NextAccountNumber;

public:
    Account() {}
    Account(string fname, string lname, float balance);
    long getAccNo() { return accountNumber; }
    string getFirstName() { return firstName; }
    string getLastName() { return lastName; }
    float getBalance() { return balance; }

    void Deposit(float amount);
    void Withdraw(float amount);
    static void setLastAccountNumber(long accountNumber);
    static long getLastAccountNumber();
    friend ofstream& operator<<(ofstream& ofs, Account& acc);
    friend ifstream& operator>>(ifstream& ifs, Account& acc);
    friend ostream& operator<<(ostream& os, Account& acc);
};

long Account::NextAccountNumber = 0;

class Bank {
private:
    map<long, Account> accounts;
public:
    Bank();
    Account OpenAccount(string fname, string lname, float balance);
    Account BalanceEnquiry(long accountNumber);
    Account Deposit(long accountNumber, float amount);
    Account Withdraw(long accountNumber, float amount);
    void CloseAccount(long accountNumber);
    void ShowAllAccounts();
    ~Bank();
};
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

int main() {
    setColor(10); // Set text color to green
    std::cout << "***Welcome in Vittaraja***\n";
    setColor(7);
    Bank b;
    Account acc;

    int choice;
    string fname, lname;
    long accountNumber;
    float balance;
    float amount;

    do {
        cout << "\n\tDear user select one option below ";
        cout << "\n\t1 Open an Account";
        cout << "\n\t2 Balance Enquiry";
        cout << "\n\t3 Deposit";
        cout << "\n\t4 Withdrawal";
        cout << "\n\t5 Close an Account";
        cout << "\n\t6 Show All Accounts";
        cout << "\n\t7 Quit";
        cout << "\nEnter your choice: ";
        cin >> choice;
        switch (choice) {
            case 1:
                cout << "Enter First Name: ";
                cin >> fname;
                cout << "Enter Last Name: ";
                cin >> lname;
                cout << "Enter initial Balance: ";
                cin >> balance;
                acc = b.OpenAccount(fname, lname, balance);
                cout << endl << "Congratulations! Account is Created" << endl;
                cout << acc;
                break;
            case 2:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                try {
                    acc = b.BalanceEnquiry(accountNumber);
                    cout << endl << "Your Account Details" << endl;
                    cout << acc;
                } catch (const std::out_of_range&) {
                    cout << "Account not found!" << endl;
                }
                break;
            case 3:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                cout << "Enter Amount to Deposit: ";
                cin >> amount;
                try {
                    acc = b.Deposit(accountNumber, amount);
                    cout << endl << "Amount is Deposited" << endl;
                    cout << acc;
                } catch (const std::out_of_range&) {
                    cout << "Account not found!" << endl;
                }
                break;
            case 4:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                cout << "Enter Amount to Withdraw: ";
                cin >> amount;
                try {
                    acc = b.Withdraw(accountNumber, amount);
                    cout << endl << "Amount Withdrawn" << endl;
                    cout << acc;
                } catch (InsufficientFunds&) {
                    cout << "Insufficient funds!" << endl;
                } catch (const std::out_of_range&) {
                    cout << "Account not found!" << endl;
                }
                break;
            case 5:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                b.CloseAccount(accountNumber);
                cout << endl << "Account is Closed" << endl;
                break; // Added missing break
            case 6:
                b.ShowAllAccounts();
                break;
            case 7:
                cout << "Exiting the system." << endl;
                break;
            default:
                cout << "\nEnter correct choice";
                break; // Changed from exit(0) to break
        }
    } while (choice != 7);

    return 0;
}

Account::Account(string fname, string lname, float balance) {
    NextAccountNumber++;
    accountNumber = NextAccountNumber;
    firstName = fname;
    lastName = lname;
    this->balance = balance;
}

void Account::Deposit(float amount) {
    balance += amount;
}

void Account::Withdraw(float amount) {
    if (balance - amount < MIN_BALANCE)
        throw InsufficientFunds();
    balance -= amount;
}

void Account::setLastAccountNumber(long accountNumber) {
    NextAccountNumber = accountNumber;
}

long Account::getLastAccountNumber() {
    return NextAccountNumber;
}

ofstream& operator<<(ofstream& ofs, Account& acc) {
    ofs << acc.accountNumber << endl;
    ofs << acc.firstName << endl;
    ofs << acc.lastName << endl;
    ofs << acc.balance << endl;
    return ofs;
}

ifstream& operator>>(ifstream& ifs, Account& acc) {
    if (ifs >> acc.accountNumber) { // Check if reading was successful
        ifs >> acc.firstName >> acc.lastName >> acc.balance;
    }
    return ifs;
}

ostream& operator<<(ostream& os, Account& acc) {
    os << "First Name: " << acc.getFirstName() << endl;
    os << "Last Name: " << acc.getLastName() << endl;
    os << "Account Number: " << acc.getAccNo() << endl;
    os << "Balance: " << acc.getBalance() << endl;
    return os;
}

Bank::Bank() {
    Account account;
    ifstream infile("Bank.data"); // Directly open the file

    if (!infile) {
        // File not found, nothing to load
        return;
    }
    while (infile >> account) {
        accounts.insert(pair<long, Account>(account.getAccNo(), account));
    }
    Account::setLastAccountNumber(account.getAccNo());
    infile.close();
}

Account Bank::OpenAccount(string fname, string lname, float balance) {
    ofstream outfile("Bank.data", ios::trunc);
    Account account(fname, lname, balance);
    accounts.insert(pair<long, Account>(account.getAccNo(), account));

    for (auto itr = accounts.begin(); itr != accounts.end(); itr++) {
        outfile << itr->second;
    }
    outfile.close();
    return account;
}

Account Bank::BalanceEnquiry(long accountNumber) {
    map<long, Account>::iterator itr = accounts.find(accountNumber);
    if (itr == accounts.end()) {
        throw std::out_of_range("Account not found"); // Throw exception if account not found
    }
    return itr->second;
}

Account Bank::Deposit(long accountNumber, float amount) {
    map<long, Account>::iterator itr = accounts.find(accountNumber);
    if (itr == accounts.end()) {
        throw std::out_of_range("Account not found"); // Throw exception if account not found
    }
    itr->second.Deposit(amount);
    return itr->second;
}

Account Bank::Withdraw(long accountNumber, float amount) {
    map<long, Account>::iterator itr = accounts.find(accountNumber);
    if (itr == accounts.end()) {
        throw std::out_of_range("Account not found"); // Throw exception if account not found
    }
    itr->second.Withdraw(amount);
    return itr->second;
}

void Bank::CloseAccount(long accountNumber) {
    map<long, Account>::iterator itr = accounts.find(accountNumber);
    if (itr != accounts.end()) {
        cout << "Account Deleted: " << itr->second;
        accounts.erase(itr);
    } else {
        cout << "Account not found!" << endl; // Handle case where account does not exist
    }
}

void Bank::ShowAllAccounts() {
    for (auto itr = accounts.begin(); itr != accounts.end(); itr++) {
        cout << "Account " << itr->first << endl << itr->second << endl;
    }
}

Bank::~Bank() {
    ofstream outfile("Bank.data", ios::trunc);
    for (auto itr = accounts.begin(); itr != accounts.end(); itr++) {
        outfile << itr->second;
    }
    outfile.close();
    return ;
}

