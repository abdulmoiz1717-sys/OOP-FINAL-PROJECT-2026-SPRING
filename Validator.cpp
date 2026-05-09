#define _CRT_SECURE_NO_WARNINGS//ctime is giving warning so visual studio recommend to use it
#include "Validator.h"
#include <ctime>
using namespace std;

// ── Internal helper: check all characters are digits ─────────────────────────
static bool allDigits(const string& s) {
    for (int i = 0; i < (int)s.size(); i++)
        if (s[i] < '0' || s[i] > '9') return false;
    return true;
}

// ── Internal helper: extract integer from string slice [start, end) ───────────
static int extractInt(const string& s, int start, int end) {
    int val = 0;
    for (int i = start; i < end; i++)
        val = val * 10 + (s[i] - '0');
    return val;
}

// ── isValidId ─────────────────────────────────────────────────────────────────
bool Validator::isValidId(int id) {
    return id > 0;
}

// ── isValidDate: DD-MM-YYYY, year >= current year ─────────────────────────────
bool Validator::isValidDate(const string& date) {
    if ((int)date.size() != 10)        return false;
    if (date[2] != '-' || date[5] != '-') return false;

    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (date[i] < '0' || date[i] > '9') return false;
    }

    int day = extractInt(date, 0, 2);
    int month = extractInt(date, 3, 5);
    int year = extractInt(date, 6, 10);

    if (day < 1 || day   > 31) return false;
    if (month < 1 || month > 12) return false;

    time_t now = time(0);
    tm* ltm = localtime(&now);
    int currentYear = 1900 + ltm->tm_year;

    if (year < currentYear) return false;

    return true;
}

// ── isValidTimeSlot ───────────────────────────────────────────────────────────
bool Validator::isValidTimeSlot(const string& slot) {
    const string valid[8] = {
        "09:00","10:00","11:00","12:00",
        "13:00","14:00","15:00","16:00"
    };
    for (int i = 0; i < 8; i++)
        if (slot == valid[i]) return true;
    return false;
}

// ── isValidContact: exactly 11 numeric digits ─────────────────────────────────
bool Validator::isValidContact(const string& contact) {
    if ((int)contact.size() != 11) return false;
    return allDigits(contact);
}

// ── isValidPassword: minimum 6 characters ────────────────────────────────────
bool Validator::isValidPassword(const string& password) {
    return (int)password.size() >= 6;
}

// ── isPositiveFloat: must be > 0 ─────────────────────────────────────────────
bool Validator::isPositiveFloat(float value) {
    return value > 0.0f;
}

// ── isValidMenuChoice ─────────────────────────────────────────────────────────
bool Validator::isValidMenuChoice(int choice, int min, int max) {
    return choice >= min && choice <= max;
}