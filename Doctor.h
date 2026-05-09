#pragma once
#include "Person.h"
#include <iostream>
using namespace std;

class Doctor : public Person {
private:
    string specialization;
    string contact;
    float  fee;
public:
    Doctor(int id, string name, string specialization,
        string contact, string password, float fee);
    ~Doctor() override;

    // Getters
    string getSpecialization() const;
    string getContact()        const;
    float  getFee()            const;

    // Setters
    void setSpecialization(string spec);
    void setContact(string contact);
    void setFee(float fee);

    // Operator overloads
    bool operator==(const Doctor& other) const;

    friend ostream& operator<<(ostream& out, const Doctor& d);

    void displayMenu()  override;
    void displayinfo()  override;
};