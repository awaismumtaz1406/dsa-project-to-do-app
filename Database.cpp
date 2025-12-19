#include "Database.h"
#include <iostream>

Database::Database() : db(nullptr) {}

Database::~Database() {
    close();
}

bool Database::connect(const string& dbPath) {
    int result = sqlite3_open(dbPath.c_str(), &db);
    
    if (result != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    
    // Create tables
    const char* createUsers = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL
        );
    )";
    
    const char* createTasks = R"(
        CREATE TABLE IF NOT EXISTS tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            title TEXT NOT NULL,
            description TEXT,
            due_date INTEGER DEFAULT 0,
            completed INTEGER DEFAULT 0,
            FOREIGN KEY(user_id) REFERENCES users(id)
        );
    )";
    
    char* errMsg;
    if (sqlite3_exec(db, createUsers, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    if (sqlite3_exec(db, createTasks, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    cout << "Database connected successfully!" << endl;
    return true;
}

void Database::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::registerUser(const string& username, const string& password) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO users (username, password) VALUES (?, ?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement" << endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        cerr << "Username already exists!" << endl;
        return false;
    }
    
    return true;
}

bool Database::loginUser(const string& username, const string& password) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id FROM users WHERE username=? AND password=?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);
    
    bool found = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    
    return found;
}

int Database::getUserId(const string& username) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id FROM users WHERE username=?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    
    int userId = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return userId;
}

bool Database::addTask(int userId, const Task& task) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO tasks (user_id, title, description, due_date, completed) VALUES (?, ?, ?, ?, ?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_text(stmt, 2, task.getTitle().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, task.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, task.getDueAt());
    sqlite3_bind_int(stmt, 5, task.isCompleted() ? 1 : 0);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool Database::deleteTask(int taskId) {
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM tasks WHERE id=?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, taskId);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

bool Database::completeTask(int taskId) {
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE tasks SET completed=1 WHERE id=?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, taskId);
    
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return result == SQLITE_DONE;
}

vector<Task> Database::getTasks(int userId) {
    vector<Task> tasks;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, title, description, due_date, completed FROM tasks WHERE user_id=? ORDER BY id;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return tasks;
    }
    
    sqlite3_bind_int(stmt, 1, userId);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Task task;
        task.setId(sqlite3_column_int(stmt, 0));
        
        const unsigned char* title = sqlite3_column_text(stmt, 1);
        if (title) task.setTitle(reinterpret_cast<const char*>(title));
        
        const unsigned char* desc = sqlite3_column_text(stmt, 2);
        if (desc) task.setDescription(reinterpret_cast<const char*>(desc));
        
        task.setDueAt(sqlite3_column_int64(stmt, 3));
        
        if (sqlite3_column_int(stmt, 4) == 1) {
            task.markComplete();
        }
        
        tasks.push_back(task);
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}