
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <limits>
#include <cstdio>
#include <cstdlib>

using namespace std;

// ==========================================
// STRUCTURES
// ==========================================

struct Expense
{
    int id;
    string username;
    string name;
    string category;
    double amount;
    string date;
};

struct Budget
{
    string username;
    double amount;
};

// ==========================================
// GLOBAL VARIABLES
// ==========================================

string currentUser;
const int MAX_EXPENSES = 1000;

Expense expenses[MAX_EXPENSES];
int expenseCount = 0;

// ==========================================
// HELPER FUNCTIONS
// ==========================================

string cleanField(string value)
{
    int i;

    for (i = 0; i < (int)value.length(); i++)
    {
        if (value[i] == '|')
        {
            value[i] = '/';
        }
    }

    return value;
}

string getCurrentDate()
{
    time_t now = time(0);
    tm *localTime = localtime(&now);

    char date[20];

    sprintf(date, "%04d-%02d-%02d",
            localTime->tm_year + 1900,
            localTime->tm_mon + 1,
            localTime->tm_mday);

    return string(date);
}

double stringToDouble(string value)
{
    stringstream ss(value);
    double number = 0;

    ss >> number;

    return number;
}

int stringToInt(string value)
{
    stringstream ss(value);
    int number = 0;

    ss >> number;

    return number;
}

void clearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

double getAmount()
{
    double amount;

    while (true)
    {
        cin >> amount;

        if (!cin.fail() && amount > 0)
        {
            clearInput();
            return amount;
        }

        cout << "Enter a valid positive amount: ";
        clearInput();
    }
}

// ==========================================
// USER REGISTRATION
// ==========================================

void registerUser()
{
    string username, password;
    string u, p;
    bool exists = false;

    cout << "\n========== USER REGISTRATION ==========\n";

    cout << "Enter Username: ";
    getline(cin, username);

    cout << "Enter Password: ";
    getline(cin, password);

    if (username.empty() || password.empty() ||
        username.find('|') != string::npos ||
        password.find('|') != string::npos)
    {
        cout << "\nInvalid username or password!\n";
        return;
    }

    ifstream file("users.txt");

    while (file >> u >> p)
    {
        if (u == username)
        {
            exists = true;
            break;
        }
    }

    file.close();

    if (exists)
    {
        cout << "\nUsername already exists!\n";
        return;
    }

    ofstream out("users.txt", ios::app);

    out << username << " " << password << endl;

    out.close();

    cout << "\nRegistration Successful!\n";
}

// ==========================================
// USER LOGIN
// ==========================================

bool loginUser()
{
    string username, password;
    string u, p;

    cout << "\n============== USER LOGIN ==============\n";

    cout << "Enter Username: ";
    getline(cin, username);

    cout << "Enter Password: ";
    getline(cin, password);

    ifstream file("users.txt");

    while (file >> u >> p)
    {
        if (u == username && p == password)
        {
            currentUser = username;

            file.close();

            cout << "\nLogin Successful!\n";
            cout << "Welcome, " << currentUser << "!\n";

            return true;
        }
    }

    file.close();

    cout << "\nInvalid Username or Password!\n";

    return false;
}

// ==========================================
// LOAD EXPENSES
// ==========================================

void loadExpenses()
{
    ifstream file("expenses.txt");

    string line;
    string field;
    int i;

    expenseCount = 0;

    while (getline(file, line))
    {
        stringstream ss(line);

        Expense e;

        getline(ss, field, '|');
        e.id = stringToInt(field);

        getline(ss, e.username, '|');
        getline(ss, e.name, '|');
        getline(ss, e.category, '|');

        getline(ss, field, '|');
        e.amount = stringToDouble(field);

        getline(ss, e.date);

        if (e.username == currentUser &&
            expenseCount < MAX_EXPENSES)
        {
            expenses[expenseCount] = e;
            expenseCount++;
        }
    }

    file.close();
}

// ==========================================
// GENERATE NEXT EXPENSE ID
// ==========================================

int nextExpenseID()
{
    ifstream file("expenses.txt");

    string line;
    string field;

    int maxID = 0;

    while (getline(file, line))
    {
        stringstream ss(line);

        getline(ss, field, '|');

        int id = stringToInt(field);

        if (id > maxID)
        {
            maxID = id;
        }
    }

    file.close();

    return maxID + 1;
}

// ==========================================
// ADD EXPENSE
// ==========================================

void addExpense()
{
    if (expenseCount >= MAX_EXPENSES)
    {
        cout << "\nExpense limit reached!\n";
        return;
    }

    Expense e;

    cout << "\n============= ADD EXPENSE =============\n";

    e.id = nextExpenseID();
    e.username = currentUser;

    cout << "Enter Expense Name: ";
    getline(cin, e.name);

    if (e.name.empty())
    {
        cout << "\nExpense name cannot be empty!\n";
        return;
    }

    e.name = cleanField(e.name);

    cout << "\nSelect Category:\n";
    cout << "1. Food\n";
    cout << "2. Travel\n";
    cout << "3. Shopping\n";
    cout << "4. Education\n";
    cout << "5. Bills\n";
    cout << "6. Entertainment\n";
    cout << "7. Other\n";

    int choice;

    cout << "\nEnter Choice: ";
    cin >> choice;
    clearInput();

    switch (choice)
    {
        case 1:
            e.category = "Food";
            break;

        case 2:
            e.category = "Travel";
            break;

        case 3:
            e.category = "Shopping";
            break;

        case 4:
            e.category = "Education";
            break;

        case 5:
            e.category = "Bills";
            break;

        case 6:
            e.category = "Entertainment";
            break;

        case 7:
            e.category = "Other";
            break;

        default:
            cout << "\nInvalid Category!\n";
            return;
    }

    cout << "Enter Amount: Rs. ";
    e.amount = getAmount();

    e.date = getCurrentDate();

    ofstream file("expenses.txt", ios::app);

    file << e.id << "|"
         << e.username << "|"
         << e.name << "|"
         << e.category << "|"
         << e.amount << "|"
         << e.date << endl;

    file.close();

    loadExpenses();

    cout << "\nExpense Added Successfully!\n";
    cout << "Expense ID: " << e.id << endl;
    cout << "Date: " << e.date << endl;
}

// ==========================================
// VIEW EXPENSES
// ==========================================

void viewExpenses()
{
    loadExpenses();

    cout << "\n============== EXPENSE HISTORY ==============\n";

    if (expenseCount == 0)
    {
        cout << "No Expenses Found!\n";
        return;
    }

    cout << left
         << setw(8) << "ID"
         << setw(20) << "Name"
         << setw(18) << "Category"
         << setw(12) << "Amount"
         << setw(15) << "Date" << endl;

    cout << "-------------------------------------------------------------\n";

    int i;

    double total = 0;

    for (i = 0; i < expenseCount; i++)
    {
        cout << left
             << setw(8) << expenses[i].id
             << setw(20) << expenses[i].name
             << setw(18) << expenses[i].category
             << setw(12) << fixed << setprecision(2)
             << expenses[i].amount
             << setw(15) << expenses[i].date
             << endl;

        total += expenses[i].amount;
    }

    cout << "-------------------------------------------------------------\n";

    cout << "Total Expenses: Rs. "
         << fixed << setprecision(2) << total << endl;
}

// ==========================================
// CATEGORY WISE REPORT
// ==========================================

void categoryReport()
{
    loadExpenses();

    string categories[7] =
    {
        "Food",
        "Travel",
        "Shopping",
        "Education",
        "Bills",
        "Entertainment",
        "Other"
    };

    double totals[7] = {0, 0, 0, 0, 0, 0, 0};

    int i, j;

    cout << "\n========== CATEGORY WISE EXPENSES ==========\n";

    for (i = 0; i < expenseCount; i++)
    {
        for (j = 0; j < 7; j++)
        {
            if (expenses[i].category == categories[j])
            {
                totals[j] += expenses[i].amount;
            }
        }
    }

    for (i = 0; i < 7; i++)
    {
        cout << left << setw(20) << categories[i]
             << "Rs. " << fixed << setprecision(2)
             << totals[i] << endl;
    }
}

// ==========================================
// MONTHLY REPORT
// ==========================================

void monthlyReport()
{
    loadExpenses();

    int month, year;

    cout << "\n============= MONTHLY REPORT =============\n";

    cout << "Enter Month (1-12): ";
    cin >> month;

    cout << "Enter Year: ";
    cin >> year;

    clearInput();

    if (month < 1 || month > 12 || year < 1)
    {
        cout << "\nInvalid Month or Year!\n";
        return;
    }

    stringstream ss;
    ss << year << "-" << setw(2) << setfill('0') << month;

    string selectedMonth = ss.str();

    double total = 0;

    int count = 0;
    int i;

    cout << "\nExpenses for " << selectedMonth << "\n";

    for (i = 0; i < expenseCount; i++)
    {
        if (expenses[i].date.substr(0, 7) == selectedMonth)
        {
            cout << expenses[i].name
                 << " | " << expenses[i].category
                 << " | Rs. " << expenses[i].amount
                 << " | " << expenses[i].date << endl;

            total += expenses[i].amount;
            count++;
        }
    }

    cout << "\nTotal Transactions: " << count << endl;

    cout << "Monthly Expense: Rs. "
         << fixed << setprecision(2) << total << endl;
}

// ==========================================
// SET MONTHLY BUDGET
// ==========================================

void setBudget()
{
    double amount;

    cout << "\n============= SET MONTHLY BUDGET =============\n";

    cout << "Enter Monthly Budget: Rs. ";
    amount = getAmount();

    ifstream in("budget.txt");

    string line;
    string u, value;

    bool found = false;

    ofstream out("budget_temp.txt");

    while (getline(in, line))
    {
        stringstream ss(line);

        getline(ss, u, '|');
        getline(ss, value);

        if (u == currentUser)
        {
            out << currentUser << "|" << amount << endl;
            found = true;
        }
        else
        {
            out << line << endl;
        }
    }

    in.close();

    if (!found)
    {
        out << currentUser << "|" << amount << endl;
    }

    out.close();

    remove("budget.txt");
    rename("budget_temp.txt", "budget.txt");

    cout << "\nMonthly Budget Saved Successfully!\n";
}

// ==========================================
// GET USER BUDGET
// ==========================================

double getBudget()
{
    ifstream file("budget.txt");

    string line;
    string u, value;

    while (getline(file, line))
    {
        stringstream ss(line);

        getline(ss, u, '|');
        getline(ss, value);

        if (u == currentUser)
        {
            file.close();
            return stringToDouble(value);
        }
    }

    file.close();

    return 0;
}

// ==========================================
// BUDGET WARNING
// ==========================================

void budgetWarning()
{
    loadExpenses();

    double budget = getBudget();

    if (budget <= 0)
    {
        cout << "\nPlease Set Your Monthly Budget First!\n";
        return;
    }

    int i;

    double total = 0;

    for (i = 0; i < expenseCount; i++)
    {
        total += expenses[i].amount;
    }

    cout << "\n============= BUDGET STATUS =============\n";

    cout << "Monthly Budget: Rs. " << budget << endl;
    cout << "Total Expenses: Rs. " << total << endl;

    double remaining = budget - total;

    if (remaining >= 0)
    {
        cout << "Remaining Budget: Rs. " << remaining << endl;

        if (total >= budget * 0.8)
        {
            cout << "\nWARNING: You have used 80% or more of your budget!\n";
        }
        else
        {
            cout << "\nYou are within your budget.\n";
        }
    }
    else
    {
        cout << "Extra Spending: Rs. " << -remaining << endl;
        cout << "\nWARNING: Your Budget Has Been Exceeded!\n";
    }
}

// ==========================================
// BUDGET PREDICTION
// ==========================================

void budgetPrediction()
{
    loadExpenses();

    int i;

    double total = 0;

    int daysPassed;
    int daysInMonth;

    cout << "\n========== BUDGET PREDICTION ==========\n";

    cout << "Enter Days Passed in Current Month: ";
    cin >> daysPassed;

    cout << "Enter Total Days in Current Month: ";
    cin >> daysInMonth;

    clearInput();

    if (daysPassed <= 0 ||
        daysInMonth <= 0 ||
        daysPassed > daysInMonth)
    {
        cout << "\nInvalid Number of Days!\n";
        return;
    }

    for (i = 0; i < expenseCount; i++)
    {
        total += expenses[i].amount;
    }

    double average = total / daysPassed;

    double prediction = average * daysInMonth;

    double budget = getBudget();

    cout << "\nTotal Expense Till Now: Rs. " << total << endl;

    cout << "Average Daily Expense: Rs. "
         << fixed << setprecision(2) << average << endl;

    cout << "Predicted Monthly Expense: Rs. "
         << prediction << endl;

    if (budget > 0)
    {
        cout << "Monthly Budget: Rs. " << budget << endl;

        if (prediction > budget)
        {
            cout << "\nWARNING: You may exceed your monthly budget!\n";
        }
        else
        {
            cout << "\nYour predicted expense is within budget.\n";
        }
    }
}

// ==========================================
// SEARCH EXPENSE
// ==========================================

void searchExpense()
{
    loadExpenses();

    int choice;

    cout << "\n============= SEARCH EXPENSE =============\n";

    cout << "1. Search by ID\n";
    cout << "2. Search by Category\n";
    cout << "3. Search by Date\n";

    cout << "Enter Choice: ";
    cin >> choice;

    clearInput();

    bool found = false;

    int i;

    if (choice == 1)
    {
        int id;

        cout << "Enter Expense ID: ";
        cin >> id;

        clearInput();

        for (i = 0; i < expenseCount; i++)
        {
            if (expenses[i].id == id)
            {
                found = true;

                cout << "\nExpense Found!\n";
                cout << "Name: " << expenses[i].name << endl;
                cout << "Category: " << expenses[i].category << endl;
                cout << "Amount: Rs. " << expenses[i].amount << endl;
                cout << "Date: " << expenses[i].date << endl;
            }
        }
    }
    else if (choice == 2)
    {
        string category;

        cout << "Enter Category: ";
        getline(cin, category);

        for (i = 0; i < expenseCount; i++)
        {
            if (expenses[i].category == category)
            {
                found = true;

                cout << expenses[i].id << " | "
                     << expenses[i].name << " | Rs. "
                     << expenses[i].amount << " | "
                     << expenses[i].date << endl;
            }
        }
    }
    else if (choice == 3)
    {
        string date;

        cout << "Enter Date (YYYY-MM-DD): ";
        getline(cin, date);

        for (i = 0; i < expenseCount; i++)
        {
            if (expenses[i].date == date)
            {
                found = true;

                cout << expenses[i].id << " | "
                     << expenses[i].name << " | "
                     << expenses[i].category << " | Rs. "
                     << expenses[i].amount << endl;
            }
        }
    }
    else
    {
        cout << "\nInvalid Choice!\n";
        return;
    }

    if (!found)
    {
        cout << "\nNo Matching Expense Found!\n";
    }
}

// ==========================================
// DELETE EXPENSE
// ==========================================

void deleteExpense()
{
    loadExpenses();

    int id;

    cout << "\n============= DELETE EXPENSE =============\n";

    cout << "Enter Expense ID to Delete: ";
    cin >> id;

    clearInput();

    bool found = false;

    int i;

    for (i = 0; i < expenseCount; i++)
    {
        if (expenses[i].id == id)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        cout << "\nExpense Not Found!\n";
        return;
    }

    ifstream in("expenses.txt");

    ofstream out("expenses_temp.txt");

    string line;
    string field;

    while (getline(in, line))
    {
        stringstream ss(line);

        getline(ss, field, '|');

        int fileID = stringToInt(field);

        getline(ss, field, '|');

        if (!(fileID == id && field == currentUser))
        {
            out << line << endl;
        }
    }

    in.close();
    out.close();

    remove("expenses.txt");
    rename("expenses_temp.txt", "expenses.txt");

    loadExpenses();

    cout << "\nExpense Deleted Successfully!\n";
}

// ==========================================
// MAIN MENU
// ==========================================

void mainMenu()
{
    int choice;

    do
    {
        cout << "\n\n";
        cout << "============================================\n";
        cout << "       SMART DIGITAL EXPENSE TRACKER\n";
        cout << "============================================\n";

        cout << "Logged in as: " << currentUser << endl;

        cout << "\n1. Add Expense\n";
        cout << "2. View Expenses\n";
        cout << "3. Category Wise Report\n";
        cout << "4. Monthly Report\n";
        cout << "5. Set Monthly Budget\n";
        cout << "6. Budget Warning\n";
        cout << "7. Budget Prediction\n";
        cout << "8. Search Expense\n";
        cout << "9. Delete Expense\n";
        cout << "10. Logout\n";

        cout << "\nEnter Your Choice: ";

        cin >> choice;
        clearInput();

        switch (choice)
        {
            case 1:
                addExpense();
                break;

            case 2:
                viewExpenses();
                break;

            case 3:
                categoryReport();
                break;

            case 4:
                monthlyReport();
                break;

            case 5:
                setBudget();
                break;

            case 6:
                budgetWarning();
                break;

            case 7:
                budgetPrediction();
                break;

            case 8:
                searchExpense();
                break;

            case 9:
                deleteExpense();
                break;

            case 10:
                cout << "\nLogging Out...\n";
                break;

            default:
                cout << "\nInvalid Choice! Try Again.\n";
        }

    }
    while (choice != 10);
}

// ==========================================
// MAIN FUNCTION
// ==========================================

int main()
{
    int choice;

    do
    {
        cout << "\n\n";
        cout << "============================================\n";
        cout << "       SMART DIGITAL EXPENSE TRACKER\n";
        cout << "============================================\n";

        cout << "\n1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";

        cout << "\nEnter Your Choice: ";

        cin >> choice;
        clearInput();

        switch (choice)
        {
            case 1:
                registerUser();
                break;

            case 2:
                if (loginUser())
                {
                    mainMenu();
                }
                break;

            case 3:
                cout << "\nThank You for Using Expense Tracker!\n";
                break;

            default:
                cout << "\nInvalid Choice! Try Again.\n";
        }

    }
    while (choice != 3);

    return 0;
}

