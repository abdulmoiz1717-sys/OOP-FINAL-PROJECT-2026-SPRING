#include "Patient.h"
#include <iostream>
using namespace std;

Patient::Patient(int id, string name, int age, char gender,
    string contact, string password, float balance)
    : Person(id, name, password), age(age), gender(gender),
    contact(contact), balance(balance) {
}

Patient::~Patient() {}

// ── Getters ───────────────────────────────────────────────────────────────────
int    Patient::getAge()     const { return age; }
char   Patient::getGender()  const { return gender; }
string Patient::getContact() const { return contact; }
float  Patient::getBalance() const { return balance; }

// ── Setters ───────────────────────────────────────────────────────────────────
void Patient::setAge(int a) { age = a; }
void Patient::setGender(char g) { gender = g; }
void Patient::setContact(string c) { contact = c; }
void Patient::setBalance(float b) { balance = b; }

// ── Operator Overloads ────────────────────────────────────────────────────────
Patient& Patient::operator+=(float amount) {
    balance += amount;
    return *this;
}

Patient& Patient::operator-=(float amount) {
    balance -= amount;
    return *this;
}

bool Patient::operator==(const Patient& other) const {
    return id == other.id;
}

ostream& operator<<(ostream& out, const Patient& p) {
    out << "ID: " << p.id
        << " | Name: " << p.name
        << " | Age: " << p.age
        << " | Gender: " << p.gender
        << " | Contact: " << p.contact
        << " | Balance: PKR " << p.balance;
    return out;
}

// ── Menu ──────────────────────────────────────────────────────────────────────
void Patient::displayMenu() {
    cout << "\nWelcome, " << name << endl;
    cout << "Balance: PKR " << balance << endl;
    cout << "========================" << endl;
    cout << "1. Book Appointment" << endl;
    cout << "2. Cancel Appointment" << endl;
    cout << "3. View My Appointments" << endl;
    cout << "4. View My Medical Records" << endl;
    cout << "5. View My Bills" << endl;
    cout << "6. Pay Bill" << endl;
    cout << "7. Top Up Balance" << endl;
    cout << "8. Logout" << endl;
}

void Patient::displayinfo() {
    cout << *this << endl;
}