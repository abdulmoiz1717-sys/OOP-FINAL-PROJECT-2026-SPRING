#include "Person.h"

Person::Person(int id, string name, string password)
    : id(id), name(name), password(password) {
}

Person::~Person() {}

int Person::getId() const {
    return id;
}

string Person::getName() const {
    return name;
}

string Person::getPassword() const {
    return password;
}

void Person::setId(int i) {
    id = i;
}

void Person::setName(string n) {
    name = n;
}