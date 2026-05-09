#pragma once
#include <string>
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
using namespace std;

class FileHandler {
public:
    // ── Load all records from file into Storage ───────────────────────────────
    static void    loadPatients(Storage<Patient>& store);
    static void    loadDoctors(Storage<Doctor>& store);
    static void    loadAppointments(Storage<Appointment>& store);
    static void    loadBills(Storage<Bill>& store);
    static void    loadPrescriptions(Storage<Prescription>& store);
    static Admin* loadAdmin();

    // ── Append a new record to the file ──────────────────────────────────────
    static void appendPatient(const Patient& p);
    static void appendDoctor(const Doctor& d);
    static void appendAppointment(const Appointment& a);
    static void appendBill(const Bill& b);
    static void appendPrescription(const Prescription& p);

    // ── Update one field (fieldIndex = 0-based CSV column) by record ID ──────
    static void updateRecord(const string& filename, int id,
        int fieldIndex, const string& newValue);

    // ── Delete the record with the given ID ───────────────────────────────────
    static void deleteRecord(const string& filename, int id);

    // ── Security log ──────────────────────────────────────────────────────────
    static void appendSecurityLog(const string& timestamp, const string& role,
        const string& enteredId, const string& result);

    // ── Archive + delete patient and all related records ──────────────────────
    static void archivePatient(int patientId,
        Storage<Patient>& patients,
        Storage<Appointment>& appointments,
        Storage<Bill>& bills,
        Storage<Prescription>& prescriptions);

    // ── Returns max ID found in column 0 of a file (for auto-increment) ───────
    static int getMaxId(const string& filename);
};