#pragma once

class HospitalException {
protected:
    char message[200];
public:
    HospitalException(const char* msg);
    virtual const char* what() const;
    virtual ~HospitalException() {}
};

class FileNotFoundException : public HospitalException {
public:
    FileNotFoundException(const char* msg) : HospitalException(msg) {}
};

class InsufficientFundsException : public HospitalException {
public:
    InsufficientFundsException(const char* msg) : HospitalException(msg) {}
};

class InvalidInputException : public HospitalException {
public:
    InvalidInputException(const char* msg) : HospitalException(msg) {}
};

class SlotUnavailableException : public HospitalException {
public:
    SlotUnavailableException(const char* msg) : HospitalException(msg) {}
};