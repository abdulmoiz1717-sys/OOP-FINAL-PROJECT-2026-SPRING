#include "FileHandler.h"
#include "HospitalException.h"
#include <fstream>
#include <iostream>
using namespace std;

// ═══════════════════════════════════════════════════════════════════════════════
// INTERNAL HELPER: split a CSV line into a string array
// caller must delete[] the returned array
// ═══════════════════════════════════════════════════════════════════════════════
static string* splitCSV(const string& line, int& count) {
    count = 1;
    for (int i = 0; i < (int)line.size(); i++)
        if (line[i] == ',') count++;

    string* tokens = new string[count];
    int idx = 0;
    string token = "";
    for (int i = 0; i < (int)line.size(); i++) {
        if (line[i] == ',') {
            tokens[idx++] = token;
            token = "";
        }
        else {
            token += line[i];
        }
    }
    tokens[idx] = token;
    return tokens;
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERNAL HELPER: string to int (manual, no stoi)
// ═══════════════════════════════════════════════════════════════════════════════
static int toInt(const string& s) {
    int val = 0;
    int start = 0;
    if (!s.empty() && s[0] == '-') start = 1;
    for (int i = start; i < (int)s.size(); i++)
        val = val * 10 + (s[i] - '0');
    return (start == 1) ? -val : val;
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERNAL HELPER: string to float (manual, no stof)
// ═══════════════════════════════════════════════════════════════════════════════
static float toFloat(const string& s) {
    float intPart = 0, fracPart = 0;
    float divisor = 1;
    bool  inFrac = false;
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == '.') { inFrac = true; continue; }
        if (inFrac) { divisor *= 10; fracPart = fracPart * 10 + (s[i] - '0'); }
        else { intPart = intPart * 10 + (s[i] - '0'); }
    }
    return intPart + fracPart / divisor;
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERNAL HELPER: float to string (manual, no to_string)
// ═══════════════════════════════════════════════════════════════════════════════
static string floatToStr(float val) {
    int intPart = (int)val;
    int fracPart = (int)((val - intPart) * 100 + 0.5f); // 2 decimal places

    // int part
    string result = "";
    if (intPart == 0) {
        result = "0";
    }
    else {
        string tmp = "";
        int n = intPart;
        while (n > 0) { tmp = (char)('0' + n % 10) + tmp; n /= 10; }
        result = tmp;
    }

    result += ".";
    if (fracPart < 10) result += "0";
    string tmp = "";
    if (fracPart == 0) { result += "00"; }
    else {
        int n = fracPart;
        while (n > 0) { tmp = (char)('0' + n % 10) + tmp; n /= 10; }
        result += tmp;
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERNAL HELPER: int to string (manual, no to_string)
// ═══════════════════════════════════════════════════════════════════════════════
static string intToStr(int val) {
    if (val == 0) return "0";
    string result = "";
    bool neg = (val < 0);
    if (neg) val = -val;
    while (val > 0) { result = (char)('0' + val % 10) + result; val /= 10; }
    if (neg) result = "-" + result;
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// getMaxId
// ═══════════════════════════════════════════════════════════════════════════════
int FileHandler::getMaxId(const string& filename) {
    ifstream fin(filename.c_str());
    if (!fin) return 0;

    int maxId = 0;
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        int count;
        string* tokens = splitCSV(line, count);
        int id = toInt(tokens[0]);
        if (id > maxId) maxId = id;
        delete[] tokens;
    }
    fin.close();
    return maxId;
}

// ═══════════════════════════════════════════════════════════════════════════════
// LOAD FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

void FileHandler::loadPatients(Storage<Patient>& store) {
    ifstream fin("patients.txt");
    if (!fin) throw FileNotFoundException("Cannot open patients.txt");

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        int count;
        string* t = splitCSV(line, count);
        if (count < 7) { delete[] t; continue; }

        // patient_id, name, age, gender, contact, password, balance
        int   id = toInt(t[0]);
        string name = t[1];
        int   age = toInt(t[2]);
        char  gender = t[3][0];
        string contact = t[4];
        string pass = t[5];
        float balance = toFloat(t[6]);

        store.add(new Patient(id, name, age, gender, contact, pass, balance));
        delete[] t;
    }
    fin.close();
}

void FileHandler::loadDoctors(Storage<Doctor>& store) {
    ifstream fin("doctors.txt");
    if (!fin) throw FileNotFoundException("Cannot open doctors.txt");

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        int count;
        string* t = splitCSV(line, count);
        if (count < 6) { delete[] t; continue; }

        // doctor_id, name, specialization, contact, password, fee
        int   id = toInt(t[0]);
        string name = t[1];
        string spec = t[2];
        string con = t[3];
        string pass = t[4];
        float fee = toFloat(t[5]);

        store.add(new Doctor(id, name, spec, con, pass, fee));
        delete[] t;
    }
    fin.close();
}

Admin* FileHandler::loadAdmin() {
    ifstream fin("admin.txt");
    if (!fin) throw FileNotFoundException("Cannot open admin.txt");

    string line;
    if (getline(fin, line) && !line.empty()) {
        int count;
        string* t = splitCSV(line, count);
        if (count >= 3) {
            // admin_id, name, password
            int    id = toInt(t[0]);
            string name = t[1];
            string pass = t[2];
            Admin* a = new Admin(id, name, pass);
            delete[] t;
            fin.close();
            return a;
        }
        delete[] t;
    }
    fin.close();
    return nullptr;
}

void FileHandler::loadAppointments(Storage<Appointment>& store) {
    ifstream fin("appointments.txt");
    if (!fin) throw FileNotFoundException("Cannot open appointments.txt");

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        int count;
        string* t = splitCSV(line, count);
        if (count < 6) { delete[] t; continue; }

        // appointment_id, patient_id, doctor_id, date, time_slot, status
        int    appId = toInt(t[0]);
        int    patId = toInt(t[1]);
        int    docId = toInt(t[2]);
        string date = t[3];
        string slot = t[4];
        string stat = t[5];

        store.add(new Appointment(appId, patId, docId, date, slot, stat));
        delete[] t;
    }
    fin.close();
}

void FileHandler::loadBills(Storage<Bill>& store) {
    ifstream fin("bills.txt");
    if (!fin) throw FileNotFoundException("Cannot open bills.txt");

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        int count;
        string* t = splitCSV(line, count);
        if (count < 6) { delete[] t; continue; }

        // bill_id, patient_id, appointment_id, amount, status, date
        int    billId = toInt(t[0]);
        int    patId = toInt(t[1]);
        int    appId = toInt(t[2]);
        float  amt = toFloat(t[3]);
        string stat = t[4];
        string date = t[5];

        store.add(new Bill(billId, patId, appId, amt, stat, date));
        delete[] t;
    }
    fin.close();
}

void FileHandler::loadPrescriptions(Storage<Prescription>& store) {
    ifstream fin("prescriptions.txt");
    if (!fin) throw FileNotFoundException("Cannot open prescriptions.txt");

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        int count;
        string* t = splitCSV(line, count);
        if (count < 7) { delete[] t; continue; }

        // prescription_id, appointment_id, patient_id, doctor_id, date, medicines, notes
        int    presId = toInt(t[0]);
        int    appId = toInt(t[1]);
        int    patId = toInt(t[2]);
        int    docId = toInt(t[3]);
        string date = t[4];
        string meds = t[5];
        string nts = t[6];

        store.add(new Prescription(presId, appId, patId, docId,
            date, meds.c_str(), nts.c_str()));
        delete[] t;
    }
    fin.close();
}

// ═══════════════════════════════════════════════════════════════════════════════
// APPEND FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

void FileHandler::appendPatient(const Patient& p) {
    ofstream fout("patients.txt", ios::app);
    if (!fout) throw FileNotFoundException("Cannot open patients.txt for writing");
    fout << p.getId() << ","
        << p.getName() << ","
        << p.getAge() << ","
        << p.getGender() << ","
        << p.getContact() << ","
        << p.getPassword() << ","
        << floatToStr(p.getBalance()) << "\n";
    fout.close();
}

void FileHandler::appendDoctor(const Doctor& d) {
    ofstream fout("doctors.txt", ios::app);
    if (!fout) throw FileNotFoundException("Cannot open doctors.txt for writing");
    fout << d.getId() << ","
        << d.getName() << ","
        << d.getSpecialization() << ","
        << d.getContact() << ","
        << d.getPassword() << ","
        << floatToStr(d.getFee()) << "\n";
    fout.close();
}

void FileHandler::appendAppointment(const Appointment& a) {
    ofstream fout("appointments.txt", ios::app);
    if (!fout) throw FileNotFoundException("Cannot open appointments.txt for writing");
    fout << a.getAppointmentId() << ","
        << a.getPatientId() << ","
        << a.getDoctorId() << ","
        << a.getDate() << ","
        << a.getTimeSlot() << ","
        << a.getStatus() << "\n";
    fout.close();
}

void FileHandler::appendBill(const Bill& b) {
    ofstream fout("bills.txt", ios::app);
    if (!fout) throw FileNotFoundException("Cannot open bills.txt for writing");
    fout << b.getBillId() << ","
        << b.getPatientId() << ","
        << b.getAppointmentId() << ","
        << floatToStr(b.getAmount()) << ","
        << b.getStatus() << ","
        << b.getDate() << "\n";
    fout.close();
}

void FileHandler::appendPrescription(const Prescription& p) {
    ofstream fout("prescriptions.txt", ios::app);
    if (!fout) throw FileNotFoundException("Cannot open prescriptions.txt for writing");
    fout << p.getPrescriptionId() << ","
        << p.getAppointmentId() << ","
        << p.getPatientId() << ","
        << p.getDoctorId() << ","
        << p.getDate() << ","
        << p.getMedicines() << ","
        << p.getNotes() << "\n";
    fout.close();
}

// ═══════════════════════════════════════════════════════════════════════════════
// UPDATE RECORD
// Reads all lines, replaces fieldIndex column for the matching ID, writes back
// ═══════════════════════════════════════════════════════════════════════════════
void FileHandler::updateRecord(const string& filename, int id,
    int fieldIndex, const string& newValue) {
    ifstream fin(filename.c_str());
    if (!fin) throw FileNotFoundException("Cannot open file for update");

    const int MAX = 500;
    string* lines = new string[MAX];
    int lineCount = 0;
    string line;
    while (getline(fin, line) && lineCount < MAX)
        lines[lineCount++] = line;
    fin.close();

    for (int i = 0; i < lineCount; i++) {
        if (lines[i].empty()) continue;
        int count;
        string* t = splitCSV(lines[i], count);
        if (toInt(t[0]) == id) {
            string newLine = "";
            for (int j = 0; j < count; j++) {
                newLine += (j == fieldIndex) ? newValue : t[j];
                if (j < count - 1) newLine += ",";
            }
            lines[i] = newLine;
            delete[] t;
            break;
        }
        delete[] t;
    }

    ofstream fout(filename.c_str());
    if (!fout) { delete[] lines; throw FileNotFoundException("Cannot write to file"); }
    for (int i = 0; i < lineCount; i++)
        if (!lines[i].empty()) fout << lines[i] << "\n";
    fout.close();
    delete[] lines;
}

// ═══════════════════════════════════════════════════════════════════════════════
// DELETE RECORD
// Reads all lines, skips the matching ID, writes the rest back
// ═══════════════════════════════════════════════════════════════════════════════
void FileHandler::deleteRecord(const string& filename, int id) {
    ifstream fin(filename.c_str());
    if (!fin) throw FileNotFoundException("Cannot open file for deletion");

    const int MAX = 500;
    string* lines = new string[MAX];
    int lineCount = 0;
    string line;
    while (getline(fin, line) && lineCount < MAX)
        lines[lineCount++] = line;
    fin.close();

    ofstream fout(filename.c_str());
    if (!fout) { delete[] lines; throw FileNotFoundException("Cannot write to file"); }
    for (int i = 0; i < lineCount; i++) {
        if (lines[i].empty()) continue;
        int count;
        string* t = splitCSV(lines[i], count);
        if (toInt(t[0]) != id)
            fout << lines[i] << "\n";
        delete[] t;
    }
    fout.close();
    delete[] lines;
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECURITY LOG
// ═══════════════════════════════════════════════════════════════════════════════
void FileHandler::appendSecurityLog(const string& timestamp, const string& role,
    const string& enteredId, const string& result) {
    ofstream fout("security_log.txt", ios::app);
    if (!fout) return;
    fout << timestamp << "," << role << "," << enteredId << "," << result << "\n";
    fout.close();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ARCHIVE PATIENT (Discharge)
// ═══════════════════════════════════════════════════════════════════════════════
void FileHandler::archivePatient(int patientId,
    Storage<Patient>& patients,
    Storage<Appointment>& appointments,
    Storage<Bill>& bills,
    Storage<Prescription>& prescriptions) {
   
  

    
    

    
    // delete from all files
    deleteRecord("patients.txt", patientId);

    for (int i = 0; i < appointments.size(); i++) {
        Appointment* a = appointments.getAll()[i];
        if (a && a->getPatientId() == patientId)
            deleteRecord("appointments.txt", a->getAppointmentId());
    }
    for (int i = 0; i < bills.size(); i++) {
        Bill* b = bills.getAll()[i];
        if (b && b->getPatientId() == patientId)
            deleteRecord("bills.txt", b->getBillId());
    }
    for (int i = 0; i < prescriptions.size(); i++) {
        Prescription* pr = prescriptions.getAll()[i];
        if (pr && pr->getPatientId() == patientId)
            deleteRecord("prescriptions.txt", pr->getPrescriptionId());
    }
}