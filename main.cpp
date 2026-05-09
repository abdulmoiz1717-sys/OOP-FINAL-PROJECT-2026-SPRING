#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <ctime>
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "Storage.h"
#include "FileHandler.h"
#include "Validator.h"
#include "HospitalException.h"

using namespace std;

//storing them
Storage<Patient>      patients;
Storage<Doctor>       doctors;
Storage<Appointment>  appointments;
Storage<Bill>         bills;
Storage<Prescription> prescriptions;
Admin* adminUser = nullptr;

//_______________methods other than classes-------------

//time
string getTodayDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[11];
    strftime(buf, 11, "%d-%m-%Y", ltm);
    return string(buf);
}

string getTimestamp() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[20];
    strftime(buf, 20, "%d-%m-%Y %H:%M:%S", ltm);
    return string(buf);
}

// manual int to string
string intToStr(int val) {
    if (val == 0) return "0";
    string result = "";
    bool neg = (val < 0);
    if (neg) val = -val;
    while (val > 0) { result = (char)('0' + val % 10) + result; val /= 10; }
    if (neg) result = "-" + result;
    return result;
}

// manual float to string
string floatToStr(float val) {
    int intPart = (int)val;
    int fracPart = (int)((val - intPart) * 100 + 0.5f);
    string result = intToStr(intPart) + ".";
    if (fracPart < 10) result += "0";
    if (fracPart == 0) result += "00";
    else result += intToStr(fracPart);
    return result;
}

// case-insensitive string compare (manual)
bool strEqualCI(const string& a, const string& b) {
    if (a.size() != b.size()) return false;
    for (int i = 0; i < (int)a.size(); i++)
        if (tolower(a[i]) != tolower(b[i])) return false;
    return true;
}

// days between two DD-MM-YYYY dates using difftime
int daysBetween(const string& d1, const string& d2) {
    // parse d1
    int day1 = (d1[0] - '0') * 10 + (d1[1] - '0');
    int mon1 = (d1[3] - '0') * 10 + (d1[4] - '0');
    int yr1 = (d1[6] - '0') * 1000 + (d1[7] - '0') * 100 + (d1[8] - '0') * 10 + (d1[9] - '0');
    // parse d2
    int day2 = (d2[0] - '0') * 10 + (d2[1] - '0');
    int mon2 = (d2[3] - '0') * 10 + (d2[4] - '0');
    int yr2 = (d2[6] - '0') * 1000 + (d2[7] - '0') * 100 + (d2[8] - '0') * 10 + (d2[9] - '0');

    tm t1 = {}; t1.tm_mday = day1; t1.tm_mon = mon1 - 1; t1.tm_year = yr1 - 1900;
    tm t2 = {}; t2.tm_mday = day2; t2.tm_mon = mon2 - 1; t2.tm_year = yr2 - 1900;

    time_t time1 = mktime(&t1);
    time_t time2 = mktime(&t2);
    double diff = difftime(time2, time1);
    return (int)(diff / 86400);
}

// bubble sort appointments by date ascending
void sortAppAsc(Appointment** arr, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j]->getDate() > arr[j + 1]->getDate())
                swap(arr[j], arr[j + 1]);
}

// bubble sort appointments by date descending
void sortAppDesc(Appointment** arr, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j]->getDate() < arr[j + 1]->getDate())
                swap(arr[j], arr[j + 1]);
}

// bubble sort prescriptions by date descending
void sortPresDesc(Prescription** arr, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j]->getDate() < arr[j + 1]->getDate())
                swap(arr[j], arr[j + 1]);
}

// ═══════════════════════════════════════════════════════════════════════════════
// LOAD ALL DATA ON STARTUP
// ═══════════════════════════════════════════════════════════════════════════════
void loadAllData() {
    try { FileHandler::loadPatients(patients); }
    catch (HospitalException& e) { cout << e.what() << endl; }

    try { FileHandler::loadDoctors(doctors); }
    catch (HospitalException& e) { cout << e.what() << endl; }

    try { FileHandler::loadAppointments(appointments); }
    catch (HospitalException& e) { cout << e.what() << endl; }

    try { FileHandler::loadBills(bills); }
    catch (HospitalException& e) { cout << e.what() << endl; }

    try { FileHandler::loadPrescriptions(prescriptions); }
    catch (HospitalException& e) { cout << e.what() << endl; }

    try { adminUser = FileHandler::loadAdmin(); }
    catch (HospitalException& e) { cout << e.what() << endl; }
}

// ═══════════════════════════════════════════════════════════════════════════════
// PATIENT MENU FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

void bookAppointment(Patient* pat) {
    string spec;
    cout << "Enter specialization to search: ";
    cin >> spec;

    bool found = false;
    for (int i = 0; i < doctors.size(); i++) {
        Doctor* d = doctors.getAll()[i];
        if (d && strEqualCI(d->getSpecialization(), spec)) {
            cout << "ID: " << d->getId() << " | Name: " << d->getName()
                << " | Fee: PKR " << d->getFee() << endl;
            found = true;
        }
    }
    if (!found) { cout << "No doctors available for that specialization." << endl; return; }

    int docId;
    cout << "Enter Doctor ID: ";
    cin >> docId;
    Doctor* selectedDoc = doctors.findById(docId);
    if (!selectedDoc) { cout << "Doctor not found." << endl; return; }

    // date with 3 attempts
    string date;
    int dateAttempts = 0;
    while (dateAttempts < 3) {
        cout << "Enter date (DD-MM-YYYY): ";
        cin >> date;
        if (Validator::isValidDate(date)) break;
        cout << "Invalid date. Use format DD-MM-YYYY." << endl;
        dateAttempts++;
        if (dateAttempts == 3) { cout << "Too many invalid attempts." << endl; return; }
    }

    // show available slots
    string allSlots[8] = { "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00" };
    cout << "Available slots: ";
    for (int s = 0; s < 8; s++) {
        bool taken = false;
        for (int i = 0; i < appointments.size(); i++) {
            Appointment* a = appointments.getAll()[i];
            if (a && a->getDoctorId() == docId && a->getDate() == date
                && a->getTimeSlot() == allSlots[s] && a->getStatus() != "cancelled") {
                taken = true; break;
            }
        }
        if (!taken) cout << allSlots[s] << " ";
    }
    cout << endl;

    // slot input
    string slot;
    while (true) {
        cout << "Enter time slot (e.g. 09:00): ";
        cin >> slot;
        if (!Validator::isValidTimeSlot(slot)) { cout << "Invalid slot." << endl; continue; }
        try {
            for (int i = 0; i < appointments.size(); i++) {
                Appointment* a = appointments.getAll()[i];
                if (a && a->getDoctorId() == docId && a->getDate() == date
                    && a->getTimeSlot() == slot && a->getStatus() != "cancelled")
                    throw SlotUnavailableException("Slot already taken. Choose another.");
            }
            break;
        }
        catch (SlotUnavailableException& e) {
            cout << e.what() << endl;
            cout << "Available slots: ";
            for (int s = 0; s < 8; s++) {
                bool taken = false;
                for (int i = 0; i < appointments.size(); i++) {
                    Appointment* a = appointments.getAll()[i];
                    if (a && a->getDoctorId() == docId && a->getDate() == date
                        && a->getTimeSlot() == allSlots[s] && a->getStatus() != "cancelled") {
                        taken = true; break;
                    }
                }
                if (!taken) cout << allSlots[s] << " ";
            }
            cout << endl;
        }
    }

    // check balance
    try {
        if (pat->getBalance() < selectedDoc->getFee())
            throw InsufficientFundsException("Insufficient balance to book appointment.");
    }
    catch (InsufficientFundsException& e) { cout << e.what() << endl; return; }

    *pat -= selectedDoc->getFee();

    int newAppId = FileHandler::getMaxId("appointments.txt") + 1;
    int newBillId = FileHandler::getMaxId("bills.txt") + 1;
    string today = getTodayDate();

    Appointment* newApp = new Appointment(newAppId, pat->getId(), docId, date, slot, "pending");
    appointments.add(newApp);
    FileHandler::appendAppointment(*newApp);

    Bill* newBill = new Bill(newBillId, pat->getId(), newAppId, selectedDoc->getFee(), "unpaid", today);
    bills.add(newBill);
    FileHandler::appendBill(*newBill);

    FileHandler::updateRecord("patients.txt", pat->getId(), 6, floatToStr(pat->getBalance()));

    cout << "Appointment booked successfully. Appointment ID: " << newAppId << endl;
}

void cancelAppointment(Patient* pat) {
    bool hasPending = false;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (a && a->getPatientId() == pat->getId() && a->getStatus() == "pending") {
            Doctor* d = doctors.findById(a->getDoctorId());
            cout << "ID: " << a->getAppointmentId()
                << " | Doctor: " << (d ? d->getName() : "Unknown")
                << " | Date: " << a->getDate()
                << " | Slot: " << a->getTimeSlot() << endl;
            hasPending = true;
        }
    }
    if (!hasPending) { cout << "You have no pending appointments." << endl; return; }

    int appId;
    cout << "Enter Appointment ID to cancel: ";
    cin >> appId;

    Appointment* target = nullptr;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (a && a->getAppointmentId() == appId
            && a->getPatientId() == pat->getId()
            && a->getStatus() == "pending") {
            target = a; break;
        }
    }
    if (!target) { cout << "Invalid appointment ID." << endl; return; }

    Doctor* d = doctors.findById(target->getDoctorId());
    float fee = d ? d->getFee() : 0;

    target->setStatus("cancelled");
    FileHandler::updateRecord("appointments.txt", appId, 5, "cancelled");

    *pat += fee;
    FileHandler::updateRecord("patients.txt", pat->getId(), 6, floatToStr(pat->getBalance()));

    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.getAll()[i];
        if (b && b->getAppointmentId() == appId) {
            b->setStatus("cancelled");
            FileHandler::updateRecord("bills.txt", b->getBillId(), 4, "cancelled");
            break;
        }
    }
    cout << "Appointment cancelled. PKR " << fee << " refunded to your balance." << endl;
}

void viewMyAppointments(Patient* pat) {
    Appointment* arr[100]; int n = 0;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (a && a->getPatientId() == pat->getId()) arr[n++] = a;
    }
    if (n == 0) { cout << "No appointments found." << endl; return; }

    sortAppAsc(arr, n);
    cout << "\nID | Doctor | Specialization | Date | Slot | Status" << endl;
    cout << "------------------------------------------------------" << endl;
    for (int i = 0; i < n; i++) {
        Doctor* d = doctors.findById(arr[i]->getDoctorId());
        cout << arr[i]->getAppointmentId() << " | "
            << (d ? d->getName() : "Unknown") << " | "
            << (d ? d->getSpecialization() : "Unknown") << " | "
            << arr[i]->getDate() << " | "
            << arr[i]->getTimeSlot() << " | "
            << arr[i]->getStatus() << endl;
    }
}

void viewMyMedicalRecords(Patient* pat) {
    Prescription* arr[100]; int n = 0;
    for (int i = 0; i < prescriptions.size(); i++) {
        Prescription* p = prescriptions.getAll()[i];
        if (p && p->getPatientId() == pat->getId()) arr[n++] = p;
    }
    if (n == 0) { cout << "No medical records found." << endl; return; }

    sortPresDesc(arr, n);
    cout << "\nDate | Doctor | Medicines | Notes" << endl;
    cout << "------------------------------------" << endl;
    for (int i = 0; i < n; i++) {
        Doctor* d = doctors.findById(arr[i]->getDoctorId());
        cout << arr[i]->getDate() << " | "
            << (d ? d->getName() : "Unknown") << " | "
            << arr[i]->getMedicines() << " | "
            << arr[i]->getNotes() << endl;
    }
}

void viewMyBills(Patient* pat) {
    bool found = false;
    float totalUnpaid = 0;
    cout << "\nBill ID | Appt ID | Amount | Status | Date" << endl;
    cout << "--------------------------------------------" << endl;
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.getAll()[i];
        if (b && b->getPatientId() == pat->getId()) {
            cout << b->getBillId() << " | "
                << b->getAppointmentId() << " | PKR "
                << b->getAmount() << " | "
                << b->getStatus() << " | "
                << b->getDate() << endl;
            if (b->getStatus() == "unpaid") totalUnpaid += b->getAmount();
            found = true;
        }
    }
    if (!found) { cout << "No bills found." << endl; return; }
    cout << "Total Outstanding: PKR " << totalUnpaid << endl;
}

void payBill(Patient* pat) {
    bool hasUnpaid = false;
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.getAll()[i];
        if (b && b->getPatientId() == pat->getId() && b->getStatus() == "unpaid") {
            cout << "Bill ID: " << b->getBillId()
                << " | Amount: PKR " << b->getAmount()
                << " | Date: " << b->getDate() << endl;
            hasUnpaid = true;
        }
    }
    if (!hasUnpaid) { cout << "No unpaid bills." << endl; return; }

    int billId;
    cout << "Enter Bill ID to pay: ";
    cin >> billId;

    Bill* target = nullptr;
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.getAll()[i];
        if (b && b->getBillId() == billId
            && b->getPatientId() == pat->getId()
            && b->getStatus() == "unpaid") {
            target = b; break;
        }
    }
    if (!target) { cout << "Invalid bill ID." << endl; return; }

    try {
        if (pat->getBalance() < target->getAmount())
            throw InsufficientFundsException("Insufficient balance to pay bill.");
    }
    catch (InsufficientFundsException& e) { cout << e.what() << endl; return; }

    *pat -= target->getAmount();
    target->setStatus("paid");
    FileHandler::updateRecord("bills.txt", billId, 4, "paid");
    FileHandler::updateRecord("patients.txt", pat->getId(), 6, floatToStr(pat->getBalance()));

    cout << "Bill paid successfully. Remaining balance: PKR " << pat->getBalance() << endl;
}

void topUpBalance(Patient* pat) {
    int attempts = 0;
    while (attempts < 3) {
        float amount;
        cout << "Enter amount to add (PKR): ";
        cin >> amount;
        try {
            if (!Validator::isPositiveFloat(amount))
                throw InvalidInputException("Amount must be greater than 0.");
            *pat += amount;
            FileHandler::updateRecord("patients.txt", pat->getId(), 6, floatToStr(pat->getBalance()));
            cout << "Balance updated. New balance: PKR " << pat->getBalance() << endl;
            return;
        }
        catch (InvalidInputException& e) {
            cout << e.what() << endl;
            attempts++;
        }
    }
    cout << "Too many invalid attempts." << endl;
}

void patientMenu(Patient* pat) {
    int choice;
    do {
        pat->displayMenu();
        cout << "Enter choice: ";
        cin >> choice;
        switch (choice) {
        case 1: bookAppointment(pat);      break;
        case 2: cancelAppointment(pat);    break;
        case 3: viewMyAppointments(pat);   break;
        case 4: viewMyMedicalRecords(pat); break;
        case 5: viewMyBills(pat);          break;
        case 6: payBill(pat);              break;
        case 7: topUpBalance(pat);         break;
        case 8: cout << "Logging out..." << endl; break;
        default: cout << "Invalid choice." << endl;
        }
    } while (choice != 8);
}

// ═══════════════════════════════════════════════════════════════════════════════
// DOCTOR MENU FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

void viewTodaysAppointments(Doctor* doc) {
    string today = getTodayDate();
    Appointment* arr[100]; int n = 0;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (a && a->getDoctorId() == doc->getId() && a->getDate() == today)
            arr[n++] = a;
    }
    if (n == 0) { cout << "No appointments scheduled for today." << endl; return; }

    // sort by time slot ascending
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j]->getTimeSlot() > arr[j + 1]->getTimeSlot())
                swap(arr[j], arr[j + 1]);

    cout << "\nAppt ID | Patient | Slot | Status" << endl;
    cout << "------------------------------------" << endl;
    for (int i = 0; i < n; i++) {
        Patient* p = patients.findById(arr[i]->getPatientId());
        cout << arr[i]->getAppointmentId() << " | "
            << (p ? p->getName() : "Unknown") << " | "
            << arr[i]->getTimeSlot() << " | "
            << arr[i]->getStatus() << endl;
    }
}

void markAppointmentComplete(Doctor* doc) {
    string today = getTodayDate();
    cout << "Today's pending appointments:" << endl;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (a && a->getDoctorId() == doc->getId()
            && a->getDate() == today && a->getStatus() == "pending") {
            Patient* p = patients.findById(a->getPatientId());
            cout << "ID: " << a->getAppointmentId()
                << " | Patient: " << (p ? p->getName() : "Unknown") << endl;
        }
    }

    int appId;
    cout << "Enter Appointment ID: ";
    cin >> appId;

    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (a && a->getAppointmentId() == appId
            && a->getDoctorId() == doc->getId()
            && a->getStatus() == "pending"
            && a->getDate() == today) {
            a->setStatus("completed");
            FileHandler::updateRecord("appointments.txt", appId, 5, "completed");
            cout << "Appointment marked as completed." << endl;
            return;
        }
    }
    cout << "Invalid appointment ID." << endl;
}

void markNoShow(Doctor* doc) {
    string today = getTodayDate();
    int appId;
    cout << "Enter Appointment ID: ";
    cin >> appId;

    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (a && a->getAppointmentId() == appId
            && a->getDoctorId() == doc->getId()
            && a->getStatus() == "pending"
            && a->getDate() == today) {
            a->setStatus("noshow");
            FileHandler::updateRecord("appointments.txt", appId, 5, "noshow");
            for (int j = 0; j < bills.size(); j++) {
                Bill* b = bills.getAll()[j];
                if (b && b->getAppointmentId() == appId) {
                    b->setStatus("cancelled");
                    FileHandler::updateRecord("bills.txt", b->getBillId(), 4, "cancelled");
                    break;
                }
            }
            cout << "Appointment marked as no-show." << endl;
            return;
        }
    }
    cout << "Invalid appointment ID." << endl;
}

void writePrescription(Doctor* doc) {
    int appId;
    cout << "Enter Appointment ID: ";
    cin >> appId;

    Appointment* target = nullptr;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (a && a->getAppointmentId() == appId
            && a->getDoctorId() == doc->getId()
            && a->getStatus() == "completed") {
            target = a; break;
        }
    }
    if (!target) { cout << "Invalid appointment or not completed." << endl; return; }

    for (int i = 0; i < prescriptions.size(); i++) {
        Prescription* p = prescriptions.getAll()[i];
        if (p && p->getAppointmentId() == appId) {
            cout << "Prescription already written for this appointment." << endl;
            return;
        }
    }

    cin.ignore();
    char medicines[500], notes[300];
    cout << "Enter medicines (e.g. Paracetamol 500mg;Amoxicillin 250mg): ";
    cin.getline(medicines, 500);
    cout << "Enter notes (max 300 chars): ";
    cin.getline(notes, 300);

    int newPresId = FileHandler::getMaxId("prescriptions.txt") + 1;
    string today = getTodayDate();

    Prescription* pres = new Prescription(newPresId, appId, target->getPatientId(),
        doc->getId(), today, medicines, notes);
    prescriptions.add(pres);
    FileHandler::appendPrescription(*pres);
    cout << "Prescription saved." << endl;
}

void viewPatientMedicalHistory(Doctor* doc) {
    int patId;
    cout << "Enter Patient ID: ";
    cin >> patId;

    Patient* pat = patients.findById(patId);
    if (!pat) {
        cout << "Access denied. You can only view records of your own patients." << endl;
        return;
    }

    bool hasCompleted = false;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (a && a->getPatientId() == patId
            && a->getDoctorId() == doc->getId()
            && a->getStatus() == "completed") {
            hasCompleted = true; break;
        }
    }
    if (!hasCompleted) {
        cout << "Access denied. You can only view records of your own patients." << endl;
        return;
    }

    Prescription* arr[100]; int n = 0;
    for (int i = 0; i < prescriptions.size(); i++) {
        Prescription* p = prescriptions.getAll()[i];
        if (p && p->getPatientId() == patId && p->getDoctorId() == doc->getId())
            arr[n++] = p;
    }
    if (n == 0) { cout << "No records found." << endl; return; }

    sortPresDesc(arr, n);
    for (int i = 0; i < n; i++) cout << *arr[i] << endl;
}

void doctorMenu(Doctor* doc) {
    int choice;
    do {
        doc->displayMenu();
        cout << "Enter choice: ";
        cin >> choice;
        switch (choice) {
        case 1: viewTodaysAppointments(doc);    break;
        case 2: markAppointmentComplete(doc);   break;
        case 3: markNoShow(doc);                break;
        case 4: writePrescription(doc);         break;
        case 5: viewPatientMedicalHistory(doc); break;
        case 6: cout << "Logging out..." << endl; break;
        default: cout << "Invalid choice." << endl;
        }
    } while (choice != 6);
}

// ═══════════════════════════════════════════════════════════════════════════════
// ADMIN MENU FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

void addDoctor() {
    string name, spec, contact, password;
    float fee;
    cin.ignore();
    cout << "Enter name: ";          getline(cin, name);
    cout << "Enter specialization: ";getline(cin, spec);
    cout << "Enter contact (11 digits): "; cin >> contact;
    if (!Validator::isValidContact(contact)) { cout << "Invalid contact." << endl; return; }
    cout << "Enter password (min 6 chars): "; cin >> password;
    if (!Validator::isValidPassword(password)) { cout << "Invalid password." << endl; return; }
    cout << "Enter consultation fee: "; cin >> fee;
    if (!Validator::isPositiveFloat(fee)) { cout << "Invalid fee." << endl; return; }

    int newId = FileHandler::getMaxId("doctors.txt") + 1;
    Doctor* d = new Doctor(newId, name, spec, contact, password, fee);
    doctors.add(d);
    FileHandler::appendDoctor(*d);
    cout << "Doctor added successfully. ID: " << newId << endl;
}

void removeDoctor() {
    for (int i = 0; i < doctors.size(); i++) {
        Doctor* d = doctors.getAll()[i];
        if (d) cout << *d << endl;
    }
    int docId;
    cout << "Enter Doctor ID to remove: ";
    cin >> docId;

    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (a && a->getDoctorId() == docId && a->getStatus() == "pending") {
            cout << "Cannot remove doctor with pending appointments." << endl;
            return;
        }
    }
    FileHandler::deleteRecord("doctors.txt", docId);
    doctors.removeById(docId);
    cout << "Doctor removed." << endl;
}

void viewAllPatients() {
    cout << "\nID | Name | Age | Gender | Contact | Balance | Unpaid Bills" << endl;
    cout << "--------------------------------------------------------------" << endl;
    for (int i = 0; i < patients.size(); i++) {
        Patient* p = patients.getAll()[i];
        if (!p) continue;
        int unpaid = 0;
        for (int j = 0; j < bills.size(); j++) {
            Bill* b = bills.getAll()[j];
            if (b && b->getPatientId() == p->getId() && b->getStatus() == "unpaid")
                unpaid++;
        }
        cout << p->getId() << " | " << p->getName() << " | "
            << p->getAge() << " | " << p->getGender() << " | "
            << p->getContact() << " | PKR " << p->getBalance()
            << " | " << unpaid << endl;
    }
}

void viewAllDoctors() {
    cout << "\nID | Name | Specialization | Contact | Fee" << endl;
    cout << "--------------------------------------------" << endl;
    for (int i = 0; i < doctors.size(); i++) {
        Doctor* d = doctors.getAll()[i];
        if (d) cout << *d << endl;
    }
}

void viewAllAppointments() {
    Appointment* arr[500]; int n = 0;
    for (int i = 0; i < appointments.size(); i++)
        if (appointments.getAll()[i]) arr[n++] = appointments.getAll()[i];

    sortAppDesc(arr, n);
    cout << "\nID | Patient | Doctor | Date | Slot | Status" << endl;
    cout << "-----------------------------------------------" << endl;
    for (int i = 0; i < n; i++) {
        Patient* p = patients.findById(arr[i]->getPatientId());
        Doctor* d = doctors.findById(arr[i]->getDoctorId());
        cout << arr[i]->getAppointmentId() << " | "
            << (p ? p->getName() : "Unknown") << " | "
            << (d ? d->getName() : "Unknown") << " | "
            << arr[i]->getDate() << " | "
            << arr[i]->getTimeSlot() << " | "
            << arr[i]->getStatus() << endl;
    }
}

void viewUnpaidBills() {
    string today = getTodayDate();
    cout << "\nBill ID | Patient | Amount | Date" << endl;
    cout << "----------------------------------" << endl;
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.getAll()[i];
        if (!b || b->getStatus() != "unpaid") continue;
        Patient* p = patients.findById(b->getPatientId());
        string dateCol = b->getDate();
        if (daysBetween(b->getDate(), today) > 7) dateCol += " [OVERDUE]";
        cout << b->getBillId() << " | "
            << (p ? p->getName() : "Unknown") << " | PKR "
            << b->getAmount() << " | " << dateCol << endl;
    }
}

void dischargePatient() {
    int patId;
    cout << "Enter Patient ID: ";
    cin >> patId;

    Patient* pat = patients.findById(patId);
    if (!pat) { cout << "Patient not found." << endl; return; }

    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.getAll()[i];
        if (b && b->getPatientId() == patId && b->getStatus() == "unpaid") {
            cout << "Cannot discharge patient with unpaid bills." << endl; return;
        }
    }
    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (a && a->getPatientId() == patId && a->getStatus() == "pending") {
            cout << "Cannot discharge patient with pending appointments." << endl; return;
        }
    }

    FileHandler::archivePatient(patId, patients, appointments, bills, prescriptions);
    patients.removeById(patId);
    cout << "Patient discharged and archived successfully." << endl;
}

void viewSecurityLog() {
    ifstream fin("security_log.txt");
    if (!fin) { cout << "No security events logged." << endl; return; }
    string line;
    bool empty = true;
    while (getline(fin, line)) {
        if (!line.empty()) { cout << line << endl; empty = false; }
    }
    if (empty) cout << "No security events logged." << endl;
    fin.close();
}

void generateDailyReport() {
    string today = getTodayDate();
    int total = 0, pending = 0, completed = 0, noshow = 0, cancelled = 0;
    float revenue = 0;

    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (!a || a->getDate() != today) continue;
        total++;
        if (a->getStatus() == "pending")   pending++;
        else if (a->getStatus() == "completed") completed++;
        else if (a->getStatus() == "noshow")    noshow++;
        else if (a->getStatus() == "cancelled") cancelled++;
    }

    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.getAll()[i];
        if (b && b->getStatus() == "paid" && b->getDate() == today)
            revenue += b->getAmount();
    }

    cout << "\n===== Daily Report: " << today << " =====" << endl;
    cout << "Total appointments: " << total
        << " (Pending: " << pending
        << " Completed: " << completed
        << " No-show: " << noshow
        << " Cancelled: " << cancelled << ")" << endl;
    cout << "Revenue collected today: PKR " << revenue << endl;

    cout << "\nPatients with outstanding unpaid bills:" << endl;
    for (int i = 0; i < patients.size(); i++) {
        Patient* p = patients.getAll()[i];
        if (!p) continue;
        float owed = 0;
        for (int j = 0; j < bills.size(); j++) {
            Bill* b = bills.getAll()[j];
            if (b && b->getPatientId() == p->getId() && b->getStatus() == "unpaid")
                owed += b->getAmount();
        }
        if (owed > 0) cout << p->getName() << " | PKR " << owed << endl;
    }

    cout << "\nDoctor-wise summary:" << endl;
    for (int i = 0; i < doctors.size(); i++) {
        Doctor* d = doctors.getAll()[i];
        if (!d) continue;
        int dc = 0, dp = 0, dn = 0;
        for (int j = 0; j < appointments.size(); j++) {
            Appointment* a = appointments.getAll()[j];
            if (!a || a->getDoctorId() != d->getId() || a->getDate() != today) continue;
            if (a->getStatus() == "completed") dc++;
            else if (a->getStatus() == "pending")   dp++;
            else if (a->getStatus() == "noshow")    dn++;
        }
        if (dc + dp + dn > 0)
            cout << d->getName()
            << " | Completed: " << dc
            << " | Pending: " << dp
            << " | No-show: " << dn << endl;
    }
}

void adminMenu() {
    int choice;
    do {
        adminUser->displayMenu();
        cout << "Enter choice: ";
        cin >> choice;
        switch (choice) {
        case 1:  addDoctor();           break;
        case 2:  removeDoctor();        break;
        case 3:  viewAllPatients();     break;
        case 4:  viewAllDoctors();      break;
        case 5:  viewAllAppointments(); break;
        case 6:  viewUnpaidBills();     break;
        case 7:  dischargePatient();    break;
        case 8:  viewSecurityLog();     break;
        case 9:  generateDailyReport(); break;
        case 10: cout << "Logging out..." << endl; break;
        default: cout << "Invalid choice." << endl;
        }
    } while (choice != 10);
}

// ═══════════════════════════════════════════════════════════════════════════════
// LOGIN
// ═══════════════════════════════════════════════════════════════════════════════
void loginPatient() {
    int attempts = 0;
    while (attempts < 3) {
        int id; string password;
        cout << "Enter Patient ID: ";   cin >> id;
        cout << "Enter Password: ";     cin >> password;
        Patient* p = patients.findById(id);
        if (p && p->getPassword() == password) { patientMenu(p); return; }
        cout << "Invalid credentials." << endl;
        FileHandler::appendSecurityLog(getTimestamp(), "Patient", intToStr(id), "FAILED");
        attempts++;
    }
    cout << "Account locked. Contact admin." << endl;
}

void loginDoctor() {
    int attempts = 0;
    while (attempts < 3) {
        int id; string password;
        cout << "Enter Doctor ID: ";  cin >> id;
        cout << "Enter Password: ";   cin >> password;
        Doctor* d = doctors.findById(id);
        if (d && d->getPassword() == password) { doctorMenu(d); return; }
        cout << "Invalid credentials." << endl;
        FileHandler::appendSecurityLog(getTimestamp(), "Doctor", intToStr(id), "FAILED");
        attempts++;
    }
    cout << "Account locked. Contact admin." << endl;
}

void loginAdmin() {
    int attempts = 0;
    while (attempts < 3) {
        int id; string password;
        cout << "Enter Admin ID: ";   cin >> id;
        cout << "Enter Password: ";   cin >> password;
        if (adminUser && adminUser->getId() == id && adminUser->getPassword() == password) {
            adminMenu(); return;
        }
        cout << "Invalid credentials." << endl;
        FileHandler::appendSecurityLog(getTimestamp(), "Admin", intToStr(id), "FAILED");
        attempts++;
    }
    cout << "Account locked. Contact admin." << endl;
}

void showMainMenu() {
    cout << "\nWelcome to MediCore Hospital Management System" << endl;
    cout << "===============================================" << endl;
    cout << "1. Patient" << endl;
    cout << "2. Doctor" << endl;
    cout << "3. Admin" << endl;
    cout << "4. Exit" << endl;
    cout << "Enter choice: ";
}

// ═══════════════════════════════════════════════════════════════════════════════
// MAIN
// ═══════════════════════════════════════════════════════════════════════════════
int main() {
    loadAllData();

    int choice;
    do {
        showMainMenu();
        cin >> choice;
        switch (choice) {
        case 1: loginPatient(); break;
        case 2: loginDoctor();  break;
        case 3: loginAdmin();   break;
        case 4: cout << "Goodbye!" << endl; break;
        default: cout << "Invalid choice." << endl;
        }
    } while (choice != 4);

    delete adminUser;
    return 0;
}