#ifndef TASK_H
#define TASK_H

#include <iostream>
#include <string>
#include <ctime>
using namespace std;

class Task {
private:
    static int nextId;
    int id;
    string title;
    string description;
    time_t dueAt;
    bool completed;

public:
    Task(const string& t, const string& d = "", time_t due = 0) {
        id = ++nextId;
        title = t;
        description = d;
        dueAt = due;
        completed = false;
    }

    int getId() const { return id; }
    const string& getTitle() const { return title; }
    const string& getDescription() const { return description; }
    time_t getDueAt() const { return dueAt; }
    bool isCompleted() const { return completed; }

    void markComplete() { 
        completed = true; 
        cout << "Task " << id << " marked as completed!\n";
    }

    void showTask() const {
        cout << "\nTask ID: " << id;
        cout << "\nTitle: " << title;
        cout << "\nDescription: " << description;
        cout << "\nStatus: " << (completed ? "Completed" : "Pending");
        if (dueAt != 0) {
            cout << "\nDue Time: " << ctime(&dueAt);
        }
        cout << endl;
    }
};

int Task::nextId = 0;

#endif