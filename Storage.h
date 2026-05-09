#pragma once
#include <iostream>
using namespace std;

template <typename T>
class Storage {
private:
    T* data[100];
    int count;
public:
    Storage() : count(0) {
        for (int i = 0; i < 100; i++)
            data[i] = nullptr;
    }

    ~Storage() {
        for (int i = 0; i < count; i++) {
            delete data[i];
            data[i] = nullptr;
        }
        count = 0;
    }

    // Add a dynamically allocated object — Storage takes ownership
    bool add(T* obj) {
        if (count >= 100) {
            cout << "Storage full." << endl;
            return false;
        }
        data[count++] = obj;
        return true;
    }

    // Remove by index (internal helper)
    bool removeAt(int index) {
        if (index < 0 || index >= count) return false;
        delete data[index];
        data[index] = nullptr;
        for (int i = index; i < count - 1; i++)
            data[i] = data[i + 1];
        data[--count] = nullptr;
        return true;
    }

    // Remove by ID — T must have getId()
    bool removeById(int id) {
        for (int i = 0; i < count; i++) {
            if (data[i] && data[i]->getId() == id)
                return removeAt(i);
        }
        return false;
    }

    // Find by ID — returns pointer or nullptr
    T* findById(int id) const {
        for (int i = 0; i < count; i++) {
            if (data[i] && data[i]->getId() == id)
                return data[i];
        }
        return nullptr;
    }

    // Get raw array of pointers
    T** getAll() {
        return data;
    }

    int size() const {
        return count;
    }
};