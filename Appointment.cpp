#include "Appointment.h"
#include <iostream>
using namespace std;

Appointment::Appointment(int appointmentId, int patientId, int doctorId,
    string date, string timeSlot, string status)
    : appointmentId(appointmentId), patientId(patientId), doctorId(doctorId),
    date(date), timeSlot(timeSlot), status(status) {
}

Appointment::~Appointment() {}

// ── Getters ───────────────────────────────────────────────────────────────────
int    Appointment::getAppointmentId() const { return appointmentId; }
int    Appointment::getPatientId()     const { return patientId; }
int    Appointment::getDoctorId()      const { return doctorId; }
string Appointment::getDate()          const { return date; }
string Appointment::getTimeSlot()      const { return timeSlot; }
string Appointment::getStatus()        const { return status; }

// ── Setters ───────────────────────────────────────────────────────────────────
void Appointment::setStatus(string s) { status = s; }

// ── Operator Overloads ────────────────────────────────────────────────────────

// Conflict: same doctor, same date, same slot, neither is cancelled
bool Appointment::operator==(const Appointment& other) const {
    return (doctorId == other.doctorId &&
        date == other.date &&
        timeSlot == other.timeSlot &&
        status != "cancelled" &&
        other.status != "cancelled");
}

ostream& operator<<(ostream& out, const Appointment& a) {
    out << "Appt ID: " << a.appointmentId
        << " | Patient: " << a.patientId
        << " | Doctor: " << a.doctorId
        << " | Date: " << a.date
        << " | Slot: " << a.timeSlot
        << " | Status: " << a.status;
    return out;
}