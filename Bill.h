#pragma once
#include <iostream>
#include <string>
using namespace std;

class Bill {
private:
    int    billId;
    int    patientId;
    int    appointmentId;
    float  amount;
    string status;   // unpaid / paid / cancelled
    string date;     // DD-MM-YYYY
public:
    Bill(int billId, int patientId, int appointmentId,
        float amount, string status, string date);
    ~Bill();

    // Getters
    int    getBillId()       const;
    int    getPatientId()    const;
    int    getAppointmentId()const;
    float  getAmount()       const;
    string getStatus()       const;
    string getDate()         const;

    // Setters
    void setStatus(string status);

    friend ostream& operator<<(ostream& out, const Bill& b);
};