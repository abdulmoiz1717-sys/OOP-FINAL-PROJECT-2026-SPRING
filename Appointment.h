#pragma once
#include <iostream>
#include <string>
using namespace std;

class Appointment {
private:
    int    appointmentId;
    int    patientId;
    int    doctorId;
    string date;
    string timeSlot;
    string status;
public:
    Appointment(int appointmentId, int patientId, int doctorId,
        string date, string timeSlot, string status);
    ~Appointment();

    // Getters
    int    getAppointmentId() const;
    int    getPatientId()     const;
    int    getDoctorId()      const;
    string getDate()          const;
    string getTimeSlot()      const;
    string getStatus()        const;

    // Setters
    void setStatus(string status);

    // Operator overloads
    // == checks scheduling conflict: same doctor, date, slot, neither cancelled
    bool operator==(const Appointment& other) const;

    friend ostream& operator<<(ostream& out, const Appointment& a);
};