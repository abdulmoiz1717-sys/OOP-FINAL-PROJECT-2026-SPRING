#include "Prescription.h"
#include <iostream>
using namespace std;

Prescription::Prescription(int prescriptionId, int appointmentId, int patientId,
    int doctorId, string date,
    const char* meds, const char* nts)
    : prescriptionId(prescriptionId), appointmentId(appointmentId),
    patientId(patientId), doctorId(doctorId), date(date) {
    
    int i = 0;
    while (meds[i] != '\0' && i < 499) { medicines[i] = meds[i]; i++; }
    medicines[i] = '\0';


    int j = 0;
    while (nts[j] != '\0' && j < 299) { notes[j] = nts[j]; j++; }
    notes[j] = '\0';
}

Prescription::~Prescription() {}

// ── Getters ───────────────────────────────────────────────────────────────────
int         Prescription::getPrescriptionId() const { return prescriptionId; }
int         Prescription::getAppointmentId()  const { return appointmentId; }
int         Prescription::getPatientId()      const { return patientId; }
int         Prescription::getDoctorId()       const { return doctorId; }
string      Prescription::getDate()           const { return date; }
const char* Prescription::getMedicines()      const { return medicines; }
const char* Prescription::getNotes()          const { return notes; }

// ── Operator Overloads ────────────────────────────────────────────────────────
ostream& operator<<(ostream& out, const Prescription& p) {
    out << "Pres ID: " << p.prescriptionId
        << " | Appt ID: " << p.appointmentId
        << " | Date: " << p.date
        << " | Medicines: " << p.medicines
        << " | Notes: " << p.notes;
    return out;
}