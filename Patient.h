#pragma once
#include "Person.h"
#include <iostream>
using namespace std;

class Patient : public Person {
private:
    int    age;
    char   gender;
    string contact;
    float  balance;
public:
    Patient(int id, string name, int age, char gender,
        string contact, string password, float balance);
    ~Patient() override;

    // Getters
    int    getAge()     const;
    char   getGender()  const;
    string getContact() const;
    float  getBalance() const;

    // Setters
    void setAge(int age);
    void setGender(char gender);
    void setContact(string contact);
    void setBalance(float balance);

    // Operator overloads
    Patient& operator+=(float amount);
    Patient& operator-=(float amount);
    bool     operator==(const Patient& other) const;

    friend ostream& operator<<(ostream& out, const Patient& p);

    void displayMenu()  override;
    void displayinfo()  override;
};