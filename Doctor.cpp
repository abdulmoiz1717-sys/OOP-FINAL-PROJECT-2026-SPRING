#include "Doctor.h"
#include <iostream>
using namespace std;

Doctor::Doctor(int id, string name, string specialization,
    string contact, string password, float fee)
    : Person(id, name, password), specialization(specialization),
    contact(contact), fee(fee) {
}

Doctor::~Doctor() {}

// ── Getters ───────────────────────────────────────────────────────────────────
string Doctor::getSpecialization() const { return specialization; }
string Doctor::getContact()        const { return contact; }
float  Doctor::getFee()            const { return fee; }

// ── Setters ───────────────────────────────────────────────────────────────────
void Doctor::setSpecialization(string spec) { specialization = spec; }
void Doctor::setContact(string c) { contact = c; }
void Doctor::setFee(float f) { fee = f; }

// ── Operator Overloads ────────────────────────────────────────────────────────
bool Doctor::operator==(const Doctor& other) const {
    return id == other.id;
}

ostream& operator<<(ostream& out, const Doctor& d) {
    out << "ID: " << d.id
        << " | Name: " << d.name
        << " | Spec: " << d.specialization
        << " | Contact: " << d.contact
        << " | Fee: PKR " << d.fee;
    return out;
}

// ── Menu ──────────────────────────────────────────────────────────────────────
void Doctor::displayMenu() {
    cout << "\nWelcome, Dr. " << name
        << " | Specialization: " << specialization << endl;
    cout << "===============================================" << endl;
    cout << "1. View Today's Appointments" << endl;
    cout << "2. Mark Appointment Complete" << endl;
    cout << "3. Mark Appointment No-Show" << endl;
    cout << "4. Write Prescription" << endl;
    cout << "5. View Patient Medical History" << endl;
    cout << "6. Logout" << endl;
}

void Doctor::displayinfo() {
    cout << *this << endl;
}