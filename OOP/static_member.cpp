#include<iostream>
using namespace std;

class Customer{
    public:
        string name;
        int acc_no;
        int balance;
        static int total_customers; // declare

        Customer(string name, int acc_no, int balance){
            this->name = name;
            this->acc_no = acc_no;
            this->balance = balance;
            total_customers++; // logic
        }

        void display(){
            cout << "Customer Name: " << name << endl;
            cout << "Account Number: " << acc_no << endl;
            cout << "Balance: $" << balance << endl;
            cout << "Total Customers: " << total_customers << endl; // in class access
        }

        static void display_total_customers(){
            cout << "Total Customers: " << total_customers << endl; // static function has only access to static members
        }
};

int Customer::total_customers = 0; // initialise

int main(){
    Customer a("Bhavyam", 1, 10000000);
    a.display();
    Customer b("Rohit", 2, 5000000);
    b.display();
    cout << "Static data and Static function" <<endl;
    cout << "Total Customers: " << Customer::total_customers << endl; // outside class access (if private then not possible)
    Customer::display_total_customers(); // static functions belong to CLASS and not OBJECTS. 
} 