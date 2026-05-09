#pragma once
#include <string>
using namespace std;

class Person {
protected:
    int id;
    string name;
    string password;
public:
    Person(int id, string name, string password);
    virtual ~Person();

    int    getId()       const;
    string getName()     const;
    string getPassword() const;

    void setId(int id);
    void setName(string name);

    virtual void displayMenu() = 0;
    virtual void displayinfo() = 0;
};