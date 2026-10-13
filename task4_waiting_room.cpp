// DSA Lab 06
// Task: 04 Waiting Room Management System
// Adina Malik
// CMS Id: 540739
// Date: October 13th 2026

#include <iostream>
using namespace std;

// Implements an array-based queue for managing patients
class WaitingRoom {
private:
    static const int CAPACITY = 100;
    int patients[CAPACITY];
    int frontIndex;
    int rearIndex;
    int count;
    int nextPatientID;

public:
    // Initializes an empty waiting room
    WaitingRoom() {
        frontIndex = 0;
        rearIndex = -1;
        count = 0;
        nextPatientID = 1;
    }

    // Registers a patient and assigns a unique ID
    void RegisterPatient() {
        if (count == CAPACITY) {
            cout << "Waiting room is full\n";
            return;
        }

        int patientID = nextPatientID++;

        rearIndex = (rearIndex + 1) % CAPACITY;
        patients[rearIndex] = patientID;
        ++count;

        cout << "Patient registered successfully\n";
        cout << "Patient ID: " << patientID << '\n';
    }

    // Serves the patient at the front of the queue
    void ServePatient() {
        if (CanDoctorGoHome()) {
            cout << "No patients are waiting\n";
            return;
        }

        int patientID = patients[frontIndex];
        frontIndex = (frontIndex + 1) % CAPACITY;
        --count;

        cout << "Patient " << patientID << " is now being served\n";

        // Reset queue indices when the waiting room becomes empty
        if (count == 0) {
            frontIndex = 0;
            rearIndex = -1;
        }
    }

    // Cancels all appointments currently in the waiting room
    void CancelAll() {
        if (count == 0) {
            cout << "No appointments to cancel\n";
            return;
        }

        int cancelledCount = count;

        frontIndex = 0;
        rearIndex = -1;
        count = 0;

        cout << cancelledCount << " appointment(s) cancelled\n";
    }

    // Checks whether the doctor has any patients left to serve
    bool CanDoctorGoHome() const {
        return count == 0;
    }

    // Displays waiting patient IDs in ascending order
    void ShowAllPatient() const {
        if (count == 0) {
            cout << "No patients are waiting\n";
            return;
        }

        cout << "Waiting patient IDs in sorted order: ";

        // Patient IDs are assigned in increasing order and never reused
        for (int i = 0; i < count; ++i) {
            int index = (frontIndex + i) % CAPACITY;
            cout << patients[index];

            if (i < count - 1)
                cout << " ";
        }

        cout << '\n';
    }

    // Returns the number of patients currently waiting
    int getPatientCount() const {
        return count;
    }
};

// Main function provides a menu for waiting room operations
int main() {
    WaitingRoom room;
    int choice;

    do {
        cout << "\n===== TASK 4: WAITING ROOM MANAGEMENT =====\n";
        cout << "1. Register patient\n";
        cout << "2. Serve patient\n";
        cout << "3. Cancel all appointments\n";
        cout << "4. Check if doctor can go home\n";
        cout << "5. Show all waiting patients\n";
        cout << "6. Show waiting patient count\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input\n";
            return 1;
        }

        switch (choice) {
            case 1:
                room.RegisterPatient();
                break;

            case 2:
                room.ServePatient();
                break;

            case 3:
                room.CancelAll();
                break;

            case 4:
                if (room.CanDoctorGoHome())
                    cout << "Yes, the doctor can go home\n";
                else
                    cout << "No, patients are still waiting\n";
                break;

            case 5:
                room.ShowAllPatient();
                break;

            case 6:
                cout << "Patients waiting: "
                     << room.getPatientCount() << '\n';
                break;

            case 0:
                cout << "Waiting room system closed\n";
                break;

            default:
                cout << "Invalid menu choice\n";
        }

    } while (choice != 0);

    return 0;
}