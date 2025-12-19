#ifndef TASK_H
#define TASK_H

#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

class Task {
private:
    int id;
    string title;
    string description;
    time_t dueAt;
    bool completed;

public:
    // Constructors
    Task() : id(0), title(""), description(""), dueAt(0), completed(false) {}
    
    Task(const string& t, const string& d = "", time_t due = 0)
        : id(0), title(t), description(d), dueAt(due), completed(false) {}

    // Getters
    int getId() const { return id; }
    string getTitle() const { return title; }
    string getDescription() const { return description; }
    time_t getDueAt() const { return dueAt; }
    bool isCompleted() const { return completed; }

    // Setters
    void setId(int newId) { id = newId; }
    void setTitle(const string& t) { title = t; }
    void setDescription(const string& d) { description = d; }
    void setDueAt(time_t t) { dueAt = t; }
    
    void markComplete() { completed = true; }
    void markIncomplete() { completed = false; }

    // Display method
    string toString() const {
        stringstream ss;
        ss << "[ID:" << id << "] " << title;
        
        if (!description.empty()) {
            ss << " - " << description;
        }
        
        if (dueAt != 0) {
            char buffer[80];
            struct tm* timeinfo = localtime(&dueAt);
            strftime(buffer, 80, "%Y-%m-%d %H:%M", timeinfo);
            ss << " | Due: " << buffer;
        }
        
        ss << " | Status: " << (completed ? "✓ Completed" : "○ Pending");
        
        return ss.str();
    }
};

#endif