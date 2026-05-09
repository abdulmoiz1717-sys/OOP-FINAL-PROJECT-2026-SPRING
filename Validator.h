#pragma once
#include <string>
using namespace std;

class Validator {
public:
    static bool isValidId(int id);
    static bool isValidDate(const string& date);       // DD-MM-YYYY, current year or later
    static bool isValidTimeSlot(const string& slot);   // one of 8 fixed slots
    static bool isValidContact(const string& contact); // exactly 11 numeric digits
    static bool isValidPassword(const string& password);// minimum 6 characters
    static bool isPositiveFloat(float value);           // > 0
    static bool isValidMenuChoice(int choice, int min, int max);
};