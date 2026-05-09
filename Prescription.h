#pragma once
#include <iostream>
#include <string>
using namespace std;

class Prescription {
private:
    int    prescriptionId;
    int    appointmentId;
    int    patientId;
    int    doctorId;
    string date;
    char   medicines[500];
    char   notes[300];
public:
    Prescription(int prescriptionId, int appointmentId, int patientId, int doctorId,
        string date, const char* medicines, const char* notes);
    ~Prescription();

    // Getters
    int         getPrescriptionId() const;
    int         getAppointmentId()  const;
    int         getPatientId()      const;
    int         getDoctorId()       const;
    string      getDate()           const;
    const char* getMedicines()      const;
    const char* getNotes()          const;

    friend ostream& operator<<(ostream& out, const Prescription& p);
};