#include <iostream>
#include <windows.h>
#include <conio.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
using namespace std;


//MYSQL DATABASE CONNECTION
sql::Connection* sql_connection() {
    try {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* con;

        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "");
        con->setSchema("details");

        return con;
    }
    catch (sql::SQLException& e) {
        cout << "Connection Error: " << e.what() << endl;
        return nullptr;
    }
}

class customer {
    protected:
        long long account_number, ph_num;
        int age;
        string password;
        double balance;
        string name, gender;
        int pin;
    public:
        customer(){
            account_number = 0;
            ph_num = 0;
            age = 0;
            password = "";
            balance = 0.0;
            name = "";
            gender = "";
            pin = 0;
        }
        customer(long long ac_no, string n, string g, int a, long long ph, string pass, int p, double bal) {
            account_number = ac_no;
            name = n;
            gender = g;
            age = a;
			ph_num = ph;
            password = pass;
            pin = p;
			balance = bal;
        }
        bool login(string pass) {
            if (pass == password) {
                return true;
            }
            else {
                return false;
            }
        }
        void check_bal() {
			cout << "--------------------------------------------" <<endl;
            cout << "   Available Balance : " << balance << endl;
            cout << "--------------------------------------------" << endl;
        }
        void deposit(double amt) {
            balance += amt;
            cout << endl << endl;
            cout << "********************************" << endl;
            cout << "||    DEPOSITED SUCCESFULLY   ||" << endl;
            cout << "********************************" << endl;
        }
        void withdraw(double amt) {
            if (amt > balance) {
                cout << endl << endl;
                cout << "********************************" << endl;
                cout << "||    INSUFFICIENT BALANCE    ||" << endl;
                cout << "********************************" << endl;
            }
                
            else {
                balance -= amt;
                cout << endl << endl;
                cout << "********************************" << endl;
                cout << "||   WITHDRAWAL SUCCESFULL    ||" << endl;
                cout << "********************************" << endl;
            }
        }
        void disp() {
            cout << "=====================================================" << endl;
            cout << "Account No: " << account_number << endl;
            cout << "Name: " << name << endl;
            cout << "Gender: " << gender<< endl;
            cout << "Age: " << age << endl;
            cout << "Phone: " << ph_num << endl;
            cout << "Balance: " << balance << endl;
            cout << "=====================================================" << endl;
        }
        void change_pin(int pin,int npin) {
            if (pin == this->pin) {
                if (npin == pin) {
                    cout << "********************************************" << endl;
                    cout << "||   YOU CAN'T USE PREVIOUSLY USED PIN    ||" << endl;
                    cout << "********************************************" << endl;
                }
                    
                else {
                    this->pin = npin;
                    cout << "********************************" << endl;
                    cout << "||  PIN CHANGED SUCCESFULLY   ||" << endl;
                    cout << "********************************" << endl;

                }
            }
            else {
                cout << "********************************" << endl;
                cout << "||       INCORRECT PIN        ||" << endl;
                cout << "********************************" << endl;
            }
        }
        void update_db() {
            try {
                sql::Connection* con = sql_connection();
                if (con == nullptr) {
                    cout << "Database connection failed!\n";
                    return;
                }
                sql::PreparedStatement* pstmt = con->prepareStatement(
                    "UPDATE accounts SET name = ? , gender = ? , age = ? , ph_num = ? , password = ? , pin = ? , balance = ? WHERE account_number = ?"
                );
                pstmt->setString(1,name);
                pstmt->setString(2,gender);
                pstmt->setInt(3,age);
                pstmt->setInt64(4,ph_num);
                pstmt->setString(5,password);
                pstmt->setInt(6,pin);
                pstmt->setDouble(7,balance);
                pstmt->setInt64(8,account_number);
                int rows = pstmt->executeUpdate();
                if (rows > 0){}
                else
                    cout << "Account not found!\n";
                delete pstmt;
                delete con;
            }
            catch (sql::SQLException& e) {
                cout << "SQL Error: " << e.what() << endl;
            }
        }
        void transfer_update(double bal, long long ac) {
            sql::Connection* con = sql_connection();
            if (con == nullptr) {
                cout << "Database connection failed!\n";
                return;
            }
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "UPDATE accounts SET balance = ? WHERE account_number = ?"
            );
            pstmt->setDouble(1, bal);
            pstmt->setInt64(2, ac);
            int rows = pstmt->executeUpdate();
            if (rows > 0) {}
            else
                cout << "Account not found!\n";

            delete pstmt;
            delete con;
        }
        void transfer(long long ac , double amt) {
            double new_bal=0;
            try {
                sql::Connection* con = sql_connection();
                if (con == nullptr) {
                    cout << "Database connection failed!\n";
                    return;
                }
                balance -= amt;
                sql::PreparedStatement* pstmt = con->prepareStatement(
                    "SELECT balance FROM accounts WHERE account_number = ?"
                );

                pstmt->setInt64(1, ac);

                sql::ResultSet* res = pstmt->executeQuery();

                if (res->next()) {
                    new_bal = res->getDouble("balance");
                }
                else {
                    cout << "Customer not found!\n";
                }
                delete res;
                delete pstmt;

				new_bal += amt;

              
                update_db();
				transfer_update(new_bal, ac);
                cout << endl << endl;
				cout << "***************  MINI STATEMENT *****************" << endl;
				cout << "FROM ACCOUNT: " << account_number << endl;
				cout << "TO ACCOUNT: " << ac << endl;
				cout << "AMOUNT TRANSFERRED: " << amt << endl;
				cout << "CURRENT BALANCE: " << balance << endl;
				cout << "*************************************************" << endl;

                
            }
            catch (sql::SQLException& e) {
                cout << "SQL Error: " << e.what() << endl;
            }
        }

};

class admin : public customer {
private:
    string username = "admin", password = "pass6425";
public:
    bool login(string user, string pass) {
        if (user == username && pass == password) {
            return true;
        }
        else {
            return false;
        }
    }
};

void add_customer() {
    string name, gender, password;
    long long int ph_num, account_number;
    int age, pin;
    double balance;

	cout << "ENTER CUSTOMER DETAILS" <<endl;
	cout << "ENTER CUSTOMER NAME: ";
    cin >> name;
    cout << "ENTER CUSTOMER AGE: ";
    cin >> age;
    cout << "ENTER CUSTOMER GENDER: ";
    cin >> gender;
    cout << "ENTER CUSTOMER PHONE NUMBER: ";
    cin >> ph_num;
    cout << "ENTER CUSTOMER PASSWORD: ";
    cin >> password;
    cout << "ENTER CUSTOMER INITIAL BALANCE: ";
    cin >> balance;
	cout << "ENTER CUSTOMER 4-DIGIT PIN: ";
    cin >> pin;
	cout << "ENTER CUSTOMER ACCOUNT NUMBER: ";
    cin >> account_number;


    try {
        sql::Connection* con = sql_connection();

        sql::Statement* stmt = con->createStatement();

        string query = "INSERT INTO accounts VALUES (" +
            to_string(account_number) + ", '" +
            name + "', '" +
            gender + "', " +
            to_string(age) + ", " +
            to_string(ph_num) + ", '" +
            password + "', " +
            to_string(pin) + ", " +
            to_string(balance) + ")";

        stmt->execute(query);
		cout << endl << endl;
		cout << "********************************" << endl;
        cout << "|| CUSTOMER ADDED SUCCESFULLY ||" << endl;
        cout << "********************************" << endl;

        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cout << "SQL Error: " << e.what() << endl;
    }
}

void delete_customer() {
    long long int account_number;

    cout << "ENTER ACCOUNT NUMBER TO DELETE: ";
    cin >> account_number;

    try {
        sql::Connection* con = sql_connection();

        if (con == nullptr) {
            cout << "Database connection failed!\n";
            return;
        }

        sql::PreparedStatement* pstmt = con->prepareStatement(
            "DELETE FROM accounts WHERE account_number = ?"
        );

        pstmt->setInt64(1, account_number);

        int rows = pstmt->executeUpdate();

        if (rows > 0) {
            cout << endl << endl;
            cout << "**********************************" << endl;
            cout << "|| CUSTOMER DELETED SUCCESFULLY ||" << endl;
            cout << "**********************************" << endl;
        }
        else
            cout << "Account not found!\n";

        delete pstmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cout << "SQL Error: " << e.what() << endl;
    }
}

void view_customers() {
    try {
        sql::Connection* con = sql_connection();

        if (con == nullptr) {
            cout << "Database connection failed!\n";
            return;
        }

        sql::Statement* stmt = con->createStatement();

        sql::ResultSet* res = stmt->executeQuery("SELECT * FROM accounts");

        cout << "\n----------------------------- Customer List ----------------------------\n";

        while (res->next()) {
            cout << "Account No: " << res->getInt64("account_number") << endl;
            cout << "Name: " << res->getString("name") << endl;
            cout << "Gender: " << res->getString("gender") << endl;
            cout << "Age: " << res->getInt("age") << endl;
            cout << "Phone: " << res->getInt64("ph_num") << endl;
            cout << "Balance: " << res->getDouble("balance") << endl;
            cout << "---------------------------------------------------------------------\n";
        }

        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cout << "SQL Error: " << e.what() << endl;
    }
}
void update_customer_details() {
    long long int account_number;
    int choice;
    cout << "ENTER ACCOUNT NUMBER: ";
    cin >> account_number; 
    cout << "\nWHAT DO YOU WANT TO UPDATE?\n";
    cout << "1. NAME\n2. PHONE NUMBER\n3. PASSWORD\n4. BALANCE\n\nChoice: ";
    cin >> choice;
    try {
        sql::Connection* con = sql_connection();
        if (con == nullptr) {
            cout << "Database connection failed!\n";
            return;
        }
        sql::PreparedStatement* pstmt;
        switch (choice) {
        case 1: {
            string name;
            cout << "ENTER NEW NAME: ";
            cin >> name;

            pstmt = con->prepareStatement(
                "UPDATE accounts SET name = ? WHERE account_number = ?"
            );
            pstmt->setString(1, name);
            pstmt->setInt64(2, account_number);
            break;
        }
        case 2: {
            long long ph;
            cout << "ENTER NEW PHONE NUMBER: ";
            cin >> ph;

            pstmt = con->prepareStatement(
                "UPDATE accounts SET ph_num = ? WHERE account_number = ?"
            );
            pstmt->setInt64(1, ph);
            pstmt->setInt64(2, account_number);
            break;
        }
        case 3: {
            string password;
            cout << "ENTER NEW PASSWORD: ";
            cin >> password;

            pstmt = con->prepareStatement(
                "UPDATE accounts SET password = ? WHERE account_number = ?"
            );
            pstmt->setString(1, password);
            pstmt->setInt64(2, account_number);
            break;
        }
        case 4: {
            double balance;
            cout << "ENTER NEW BALANCE: ";
            cin >> balance;

            pstmt = con->prepareStatement(
                "UPDATE accounts SET balance = ? WHERE account_number = ?"
            );
            pstmt->setDouble(1, balance);
            pstmt->setInt64(2, account_number);
            break;
        }
        default:
            cout << "Invalid choice!\n";
            return;
        }
        int rows = pstmt->executeUpdate();
        if (rows > 0) {
            cout << endl << endl;
            cout << "**********************************" << endl;
            cout << "|| DETAILS UPDATED SUCCESFULLY  ||" << endl;
            cout << "**********************************" << endl;
        }
        else {
            cout << endl << endl;
            cout << "**********************************" << endl;
            cout << "||      ACCOUNT NOT FOUND       ||" << endl;
            cout << "**********************************" << endl;
        }
        delete pstmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cout << "SQL Error: " << e.what() << endl;
    }
}

void search_customer() {
    long long int account_number;

    cout << "ENTER ACCOUNT NUMBER TO SEARCH: ";
    cin >> account_number;

    try {
        sql::Connection* con = sql_connection();

        if (con == nullptr) {
            cout << "Database connection failed!\n";
            return;
        }

        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT * FROM accounts WHERE account_number = ?"
        );

        pstmt->setInt64(1, account_number);

        sql::ResultSet* res = pstmt->executeQuery();

        if (res->next()) {
            cout << "\n-------------------------------- Customer Found --------------------------------\n";

            cout << "Account No: " << res->getInt64("account_number") << endl;
            cout << "Name: " << res->getString("name") << endl;
            cout << "Gender: " << res->getString("gender") << endl;
            cout << "Age: " << res->getInt("age") << endl;
            cout << "Phone: " << res->getInt64("ph_num") << endl;
            cout << "Balance: " << res->getDouble("balance") << endl;

            cout << "\n--------------------------------------------------------------------------------\n";
        }
        else {
            cout << "Customer not found!\n";
        }

        delete res;
        delete pstmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        cout << "SQL Error: " << e.what() << endl;
    }
}

bool customer_ini(long long account_number,string pass) {
    sql::Connection* con = sql_connection();
    if (con == nullptr) {
        cout << "Database connection failed!\n";
        return false;
    }
    sql::PreparedStatement* pstmt = con->prepareStatement(
        "SELECT * FROM accounts WHERE account_number = ?"
    );
    pstmt->setInt64(1, account_number);

    sql::ResultSet* res = pstmt->executeQuery();

    if (res->next()) {
        customer obj(res->getInt64("account_number"), res->getString("name"), res->getString("gender"), res->getInt("age"), res->getInt64("ph_num"), res->getString("password"), res->getInt("pin"), res->getDouble("balance"));
        if(obj.login(pass)){
            return true;
        }
        else{
            cout << "Incorrect password!\n";
            return false;
		}
    }
    else {
        cout << "Customer not found!\n";
    }

    delete res;
    delete pstmt;
    delete con;
    return false;
}


void admin_operations() {
	system("cls");
    up:
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
    cout << "                                                 ADMIN PORTAL                                                    " << endl;
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
    cout << "**********************************" << endl;
    cout << "||1. Add Customer               ||" << endl;
    cout << "||2. Delete Customer            ||" << endl;
    cout << "||3. View Customers             ||" << endl;
	cout << "||4. Search Customer            ||" << endl;
    cout << "||5. Update Customer Details    ||" << endl;
    cout << "||6. Logout                     ||" << endl;
    cout << "**********************************" << endl;
    int choice;
    cout << "Enter your choice: ";
    cin >> choice;
    switch (choice) {
    case 1:
        system("cls");
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        cout << "                                                 ADD CUSTOMER                                                    " << endl;
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        add_customer();
        Sleep(2000);
		system("cls");
        goto up;
    case 2:
        system("cls");
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        cout << "                                                 DELETE CUSTOMER                                                 " << endl;
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        delete_customer();
        Sleep(2000);
        system("cls");
        goto up;
    case 3:
		system("cls");
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        cout << "                                                 VIEW ALL CUSTOMERS                                              " << endl;
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        view_customers();
        _getch();
        system("cls");
        goto up;
    case 4:
		system("cls");
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        cout << "                                                 SEARCH CUSTOMER                                                 " << endl;
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        search_customer();
		_getch();
		system("cls");
		goto up;
    case 5:
        system("cls");
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        cout << "                                         UPDATE CUSTOMER DETAILS                                                 " << endl;
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        update_customer_details();
        Sleep(2000);
        system("cls");
        goto up;
    case 6:
        cout << endl << endl;
        cout << "**********************************" << endl;
        cout << "|| LOGGING OUT................. ||" << endl;
        cout << "**********************************" << endl;
        Sleep(2000);
		system("cls");
        break;
    default:
        cout << endl << endl;
        cout << "**********************************" << endl;
        cout << "||        INVALID CHOICE        ||" << endl;
        cout << "**********************************" << endl;
        Sleep(2000);
        goto up;
    }
}

void customer_operations(long long account_number) {
    sql::Connection* con = sql_connection();
    if (con == nullptr) {
        cout << "Database connection failed!\n";
        return;
    }
    sql::PreparedStatement* pstmt = con->prepareStatement(
        "SELECT * FROM accounts WHERE account_number = ?"
    );
    pstmt->setInt64(1, account_number);

    sql::ResultSet* res = pstmt->executeQuery();

    if (res->next()) {
        customer obj(res->getInt64("account_number"), res->getString("name"), res->getString("gender"), res->getInt("age"), res->getInt64("ph_num"), res->getString("password"), res->getInt("pin"), res->getDouble("balance"));
        mathi:
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        cout << "                                              CUSTOMER PORTAL                                                    " << endl;
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        cout << endl << endl;
        cout << "===========================================" << endl;
        cout << "|| 1. Check Balance                      ||" << endl;
        cout << "|| 2. Deposit                            ||" << endl;
        cout << "|| 3. Withdraw                           ||" << endl;
        cout << "|| 4. View Account Details               ||" << endl;
        cout << "|| 5. Change Pin                         ||" << endl;
        cout << "|| 6. Quick Transfer                     ||" << endl;
        cout << "|| 7. Logout                             ||" << endl;
        cout << "===========================================" << endl <<endl;
		cout << "Enter your choice: ";
        int choice, npin, pin;
        long long ac;
        double depo, with ,amt;
        cin >> choice;
        switch (choice) {
        case 1:
			system("cls");
            obj.check_bal();
            Sleep(2000);
            goto mathi;
        case 2:
            system("cls");
            cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
            cout << "                                                 DEPOSIT                                                         " << endl;
            cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
			cout << "ENTER AMOUNT TO DEPOSIT: ";
            cin >> depo;
            obj.deposit(depo);
            obj.update_db();
			Sleep(2000);
            system("cls");
            goto mathi;
        case 3:
            system("cls");
            cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
            cout << "                                                 WITHDRAWAL                                                      " << endl;
            cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
            cout << "ENTER AMOUNT TO WITHDRAW: ";
            cin >> with;
            obj.withdraw(with);
            obj.update_db();
            Sleep(2000);
            system("cls");
            goto mathi;
        case 4:
            system("cls");
            cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
            cout << "                                               ACCOUNT DETAILS                                                   " << endl;
            cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
            obj.disp();
			_getch();
            system("cls");
            goto mathi;
        case 5:
            system("cls");
            cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
            cout << "                                               CHANGE PIN                                                        " << endl;
            cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
			cout << "ENTER CURRENT PIN: ";
            cin >> pin;
			cout << "ENTER NEW PIN: ";
            cin >> npin;
            obj.change_pin(pin, npin);
            obj.update_db();
            Sleep(2000);
            system("cls");
            goto mathi;
        case 6:
            cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
            cout << "                                               QUICK TRANSFER                                                    " << endl;
            cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
			cout << "ENTER ACCOUNT NUMBER TO TRANSFER TO: ";
            cin >> ac;
			cout << "ENTER AMOUNT TO TRANSFER: ";
            cin >> amt;
            obj.transfer(ac,amt);
            _getch();
            system("cls");
            goto mathi;
        case 7:
            cout << endl << endl;
            cout << "**********************************" << endl;
            cout << "|| LOGGING OUT................. ||" << endl;
            cout << "**********************************" << endl;
			Sleep(2000);
            break;
        default:
            cout << endl << endl;
            cout << "**********************************" << endl;
            cout << "||        INVALID CHOICE        ||" << endl;
            cout << "**********************************" << endl;
            Sleep(2000);
            system("cls");
			goto mathi;
        }

    }
}

int main() {
    string username, password;
    long long ac_no;
    end:
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
    cout << "                                                ECO BANKING SYSTEM                                               " << endl;
	cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
    cout << endl;
    cout << "********************************************" << endl;
	cout << "|  PLEASE LOGIN TO CONTINUE                |" << endl; 
    cout << "|  1. Admin Login                          |" << endl;
    cout << "|  2. Customer Login                       |" << endl;
    cout << "|  3. Exit                                 |" << endl;
    cout << "********************************************"<< endl;
    int choice;
    cout << "Enter your choice: ";
    cin >> choice;
    cout << endl << endl;
    admin obj;
    int attempts = 0;
	system("cls");
    switch (choice) {
    case 1:
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        cout << "                                                 ADMIN PORTAL                                                    " << endl;
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        top:
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;
        if (obj.login(username, password)) {
            admin_operations();
            goto end;
        }
        else {
            if (attempts < 3) {
                cout << "INCORRECT USERNAME OR PASSWORD" << endl;
                attempts++;
				cout << "ATTEMPTS REMAINING: " << 4 - attempts << endl;
                cout << endl << endl;
                goto top;
            }
            else {
                cout << "Too many attempts. Exiting..." << endl;
                Sleep(2000);
				system("cls");
                main();
            }
        }
        break;
    case 2:
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        cout << "                                              CUSTOMER PORTAL                                                    " << endl;
        cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
        topp:
        cout << "Enter Account Number: ";
        cin >> ac_no;
        cout << "Enter password: ";
        cin >> password;
        if (customer_ini(ac_no, password)){
			system("cls");
            customer_operations(ac_no);
            system("cls");
            goto end;
        }
        else {
            if (attempts < 3) {
                cout << "INCORRECT ACCOUNT NUMBER OR PASSWORD" << endl;
                attempts++;
                cout << "ATTEMPTS REMAINING: " << 4 - attempts << endl;
                cout << endl << endl;
                goto topp;
            }
            else {
                cout << "Too many attempts. Exiting..." << endl;
                Sleep(2000);
                system("cls");
                main();
            }
        }
        break;
    case 3:
		cout << endl << endl;
		cout << "**********************************" << endl;
		cout << "||     THANK YOU FOR USING THE   ||" << endl;
		cout << "||         BANKING SYSTEM        ||" << endl;
		cout << "**********************************" << endl;
		Sleep(3000);
        break;
    default:
        cout << "Invalid choice" << endl;
    }
}