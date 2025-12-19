#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include "Task.h"

using namespace std;

class Database {
private:
    sqlite3* db;
    
public:
    Database();
    ~Database();
    
    bool connect(const string& dbPath);
    void close();
    
    // User functions
    bool registerUser(const string& username, const string& password);
    bool loginUser(const string& username, const string& password);
    int getUserId(const string& username);
    
    // Task functions
    bool addTask(int userId, const Task& task);
    bool deleteTask(int taskId);
    bool completeTask(int taskId);
    vector<Task> getTasks(int userId);
};

#endif