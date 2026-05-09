#pragma once
#include "Person.h"

class Admin : public Person {
public:
    Admin(int id, string name, string password);
    ~Admin() override;

    void displayMenu()  override;
    void displayinfo()  override;
};