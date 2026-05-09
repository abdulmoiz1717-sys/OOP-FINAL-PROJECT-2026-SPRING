#include "Bill.h"
#include <iostream>
using namespace std;

Bill::Bill(int billId, int patientId, int appointmentId,
    float amount, string status, string date)
    : billId(billId), patientId(patientId), appointmentId(appointmentId),
    amount(amount), status(status), date(date) {
}

Bill::~Bill() {}

// ── Getters ───────────────────────────────────────────────────────────────────
int    Bill::getBillId()        const { return billId; }
int    Bill::getPatientId()     const { return patientId; }
int    Bill::getAppointmentId() const { return appointmentId; }
float  Bill::getAmount()        const { return amount; }
string Bill::getStatus()        const { return status; }
string Bill::getDate()          const { return date; }

// ── Setters ───────────────────────────────────────────────────────────────────
void Bill::setStatus(string s) { status = s; }

// ── Operator Overloads ────────────────────────────────────────────────────────
ostream& operator<<(ostream& out, const Bill& b) {
    out << "Bill ID: " << b.billId
        << " | Appt ID: " << b.appointmentId
        << " | Amount: PKR " << b.amount
        << " | Status: " << b.status
        << " | Date: " << b.date;
    return out;
}