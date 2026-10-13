// DSA Lab 06
// Task: 03 Print Server Request Handling System
// Adina Malik
// CMS Id: 540739
// Date: October 13th 2026

#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>
using namespace std;

// Stores information about one print request
struct PrintRequest {
    int requestID;
    int arrivalTime;
    int processingTime;
};

// Implements a circular queue with dynamic resizing
class CircularQueue {
private:
    PrintRequest* items;
    int capacity;
    int frontIndex;
    int rearIndex;
    int count;

public:
    // Initializes an empty queue with the given capacity
    CircularQueue(int initialCapacity = 4) {
        if (initialCapacity < 1)
            initialCapacity = 1;

        capacity = initialCapacity;
        items = new PrintRequest[capacity];
        frontIndex = 0;
        rearIndex = -1;
        count = 0;
    }

    // Releases dynamically allocated memory
    ~CircularQueue() {
        delete[] items;
    }

    // Prevents accidental shallow copies of the dynamic array
    CircularQueue(const CircularQueue&) = delete;
    CircularQueue& operator=(const CircularQueue&) = delete;

    // Returns true when the queue contains no requests
    bool isEmpty() const {
        return count == 0;
    }

    // Returns true when the queue has reached its current capacity
    bool isFull() const {
        return count == capacity;
    }

    // Returns the number of requests currently waiting
    int size() const {
        return count;
    }

    // Returns the current queue capacity
    int getCapacity() const {
        return capacity;
    }

    // Returns the front request without removing it
    bool front(PrintRequest& request) const {
        if (isEmpty())
            return false;

        request = items[frontIndex];
        return true;
    }

    // Changes the capacity while preserving queue order
    bool resize(int newCapacity) {
        if (newCapacity < count || newCapacity < 1)
            return false;

        PrintRequest* newItems = new PrintRequest[newCapacity];

        // Copy requests in their logical FIFO order
        for (int i = 0; i < count; ++i)
            newItems[i] = items[(frontIndex + i) % capacity];

        delete[] items;
        items = newItems;
        capacity = newCapacity;
        frontIndex = 0;
        rearIndex = count - 1;

        return true;
    }

    // Adds a request at the rear of the queue
    bool enqueue(const PrintRequest& request) {
        // Double capacity when the queue is full
        if (isFull()) {
            if (!resize(capacity * 2))
                return false;
        }

        rearIndex = (rearIndex + 1) % capacity;
        items[rearIndex] = request;
        ++count;

        return true;
    }

    // Removes the oldest request from the queue
    bool dequeue(PrintRequest& request) {
        if (isEmpty())
            return false;

        request = items[frontIndex];
        frontIndex = (frontIndex + 1) % capacity;
        --count;

        // Shrink the queue when usage becomes sparse
        if (capacity > 4 && count <= capacity / 4) {
            int newCapacity = capacity / 2;

            if (newCapacity < 4)
                newCapacity = 4;

            if (newCapacity >= count)
                resize(newCapacity);
        }

        // Reset indices when the queue becomes empty
        if (count == 0) {
            frontIndex = 0;
            rearIndex = -1;
        }

        return true;
    }

    // Displays requests in FIFO order
    void display() const {
        cout << "\nQueue contents\n";

        if (isEmpty()) {
            cout << "Queue is empty\n";
            return;
        }

        cout << left
             << setw(12) << "Request ID"
             << setw(15) << "Arrival Time"
             << setw(18) << "Processing Time" << '\n';

        for (int i = 0; i < count; ++i) {
            int index = (frontIndex + i) % capacity;

            cout << left
                 << setw(12) << items[index].requestID
                 << setw(15) << items[index].arrivalTime
                 << setw(18) << items[index].processingTime
                 << '\n';
        }

        cout << "Requests: " << count
             << " | Capacity: " << capacity << '\n';
    }
};

// Manages print requests and waiting-time statistics
class PrintServer {
private:
    CircularQueue queue;
    int nextRequestID;
    int currentTime;
    long long totalWaitingTime;
    int processedCount;

public:
    // Initializes the print server
    PrintServer()
        : queue(4), nextRequestID(1), currentTime(0),
          totalWaitingTime(0), processedCount(0) {}

    // Registers a request and adds it to the queue
    void addRequest(int arrivalTime, int processingTime) {
        if (arrivalTime < 0 || processingTime <= 0) {
            cout << "Arrival time must be non-negative and processing "
                 << "time must be positive\n";
            return;
        }

        PrintRequest request = {
            nextRequestID,
            arrivalTime,
            processingTime
        };

        if (queue.enqueue(request)) {
            ++nextRequestID;
            cout << "Request " << request.requestID
                 << " added successfully\n";
            queue.display();
        } else {
            cout << "Unable to add request\n";
        }
    }

    // Processes the oldest request and records its waiting time
    void processRequest() {
        PrintRequest request;

        if (!queue.dequeue(request)) {
            cout << "No requests to process\n";
            queue.display();
            return;
        }

        // The server begins when both the request has arrived
        // and the server is available
        int startTime = currentTime;

        if (request.arrivalTime > startTime)
            startTime = request.arrivalTime;

        int waitingTime = startTime - request.arrivalTime;
        currentTime = startTime + request.processingTime;

        totalWaitingTime += waitingTime;
        ++processedCount;

        cout << "Processed request ID: " << request.requestID << '\n';
        cout << "Arrival time: " << request.arrivalTime << '\n';
        cout << "Processing time: " << request.processingTime << '\n';
        cout << "Service start time: " << startTime << '\n';
        cout << "Waiting time: " << waitingTime << '\n';
        cout << "Service finish time: " << currentTime << '\n';

        queue.display();
    }

    // Displays the request at the front of the queue
    void showFront() const {
        PrintRequest request;

        if (queue.front(request)) {
            cout << "Front request ID: " << request.requestID << '\n';
            cout << "Arrival time: " << request.arrivalTime << '\n';
            cout << "Processing time: " << request.processingTime << '\n';
        } else {
            cout << "Queue is empty\n";
        }
    }

    // Displays the average waiting time of processed requests
    void showAverageWaitingTime() const {
        if (processedCount == 0) {
            cout << "No requests have been processed yet\n";
            return;
        }

        double average = static_cast<double>(totalWaitingTime) /
                         processedCount;

        cout << fixed << setprecision(2);
        cout << "Processed requests: " << processedCount << '\n';
        cout << "Total waiting time: " << totalWaitingTime << '\n';
        cout << "Average waiting time: " << average << '\n';
    }

    // Provides access to the queue state
    void showQueue() const {
        queue.display();
    }

    // Displays the current queue status
    void showStatus() const {
        cout << "Queue empty: "
             << (queue.isEmpty() ? "Yes" : "No") << '\n';
        cout << "Queue full: "
             << (queue.isFull() ? "Yes" : "No") << '\n';
        cout << "Queue size: " << queue.size() << '\n';
        cout << "Queue capacity: " << queue.getCapacity() << '\n';
    }
};

// Main function provides a menu for print server operations
int main() {
    PrintServer server;
    int choice;

    do {
        cout << "\n===== TASK 3: PRINT SERVER =====\n";
        cout << "1. Enqueue print request\n";
        cout << "2. Dequeue and process request\n";
        cout << "3. Show front request\n";
        cout << "4. Display queue\n";
        cout << "5. Show queue status\n";
        cout << "6. Show average waiting time\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input\n";
            return 1;
        }

        if (choice == 0)
            break;

        switch (choice) {
            case 1: {
                int arrivalTime;
                int processingTime;

                cout << "Enter arrival time: ";
                if (!(cin >> arrivalTime)) {
                    cout << "Invalid arrival time\n";
                    return 1;
                }

                cout << "Enter processing time: ";
                if (!(cin >> processingTime)) {
                    cout << "Invalid processing time\n";
                    return 1;
                }

                server.addRequest(arrivalTime, processingTime);
                break;
            }

            case 2:
                server.processRequest();
                break;

            case 3:
                server.showFront();
                break;

            case 4:
                server.showQueue();
                break;

            case 5:
                server.showStatus();
                break;

            case 6:
                server.showAverageWaitingTime();
                break;

            default:
                cout << "Invalid menu choice\n";
        }

    } while (true);

    cout << "Print server closed\n";
    return 0;
}