#include <iostream>
#include <string>
#include <ctime>
#include <limits>
#include <iomanip>
#include "Database.h"
#include "Task.h"
#include "TaskQueue.h"  // DSA - Queue Data Structure

using namespace std;

void clearScreen() {
    system("cls");
}

void pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void printHeader() {
    cout << "\n";
    cout << "================================================\n";
    cout << "       TODO APP - Task Manager (DSA)\n";
    cout << "       Using: Queue Data Structure + SQLite\n";
    cout << "================================================\n";
}

// Load tasks from database into queue (DSA structure)
void loadTasksIntoQueue(Database& db, TaskQueue& queue, int userId) {
    queue.clear();  // Clear existing queue
    vector<Task> tasks = db.getTasks(userId);
    
    // Enqueue all tasks (FIFO - Queue operation)
    for (const auto& task : tasks) {
        queue.enqueue(task);
    }
    
    cout << "\nLoaded " << tasks.size() << " tasks into Queue (DSA)\n";
}

bool loginMenu(Database& db, int& userId, string& username) {
    while (true) {
        clearScreen();
        printHeader();
        cout << "\n1. Login\n";
        cout << "2. Register\n";
        cout << "3. Exit\n";
        cout << "\nChoice: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        if (choice == 3) return false;
        
        string user, pass;
        cout << "\nUsername: ";
        getline(cin, user);
        cout << "Password: ";
        getline(cin, pass);
        
        if (choice == 1) {
            if (db.loginUser(user, pass)) {
                userId = db.getUserId(user);
                username = user;
                cout << "\n✓ Login successful! Welcome, " << user << "!\n";
                pause();
                return true;
            } else {
                cout << "\n✗ Invalid username or password!\n";
                pause();
            }
        } else if (choice == 2) {
            if (db.registerUser(user, pass)) {
                cout << "\n✓ Registration successful! You can now login.\n";
                pause();
            } else {
                cout << "\n✗ Registration failed! Username may already exist.\n";
                pause();
            }
        }
    }
}

time_t getDateTime() {
    int year, month, day, hour, min;
    cout << "Year (YYYY): "; cin >> year;
    cout << "Month (1-12): "; cin >> month;
    cout << "Day (1-31): "; cin >> day;
    cout << "Hour (0-23): "; cin >> hour;
    cout << "Minute (0-59): "; cin >> min;
    cin.ignore();
    
    struct tm t = {0};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = min;
    
    return mktime(&t);
}

void addTask(Database& db, TaskQueue& queue, int userId) {
    clearScreen();
    printHeader();
    cout << "\n=== ADD NEW TASK ===\n";
    cout << "DSA Operation: ENQUEUE (Add to Queue)\n\n";
    
    string title, desc;
    cout << "Task Title: ";
    getline(cin, title);
    
    cout << "Description (optional): ";
    getline(cin, desc);
    
    cout << "\nSet due date? (y/n): ";
    char ch;
    cin >> ch;
    cin.ignore();
    
    time_t dueDate = 0;
    if (ch == 'y' || ch == 'Y') {
        cout << "\nEnter due date:\n";
        dueDate = getDateTime();
    }
    
    Task task(title, desc, dueDate);
    
    // Save to database first
    if (db.addTask(userId, task)) {
        cout << "\n✓ Task saved to Database!\n";
        
        // Reload queue from database to sync
        loadTasksIntoQueue(db, queue, userId);
        
        cout << "✓ Task enqueued in Queue (DSA)!\n";
        cout << "\n[DSA INFO] Queue Size: " << queue.size() << " tasks\n";
    } else {
        cout << "\n✗ Failed to add task!\n";
    }
    pause();
}

void viewTasks(const TaskQueue& queue) {
    clearScreen();
    printHeader();
    cout << "\n=== VIEW ALL TASKS ===\n";
    cout << "DSA Operation: TRAVERSE Queue\n\n";
    
    // Get all tasks from queue (DSA traversal)
    vector<Task> tasks = queue.getAllTasks();
    
    if (tasks.empty()) {
        cout << "Queue is empty! No tasks found.\n";
    } else {
        cout << "Tasks in Queue (FIFO Order):\n";
        cout << "--------------------------------------------\n";
        
        for (const auto& task : tasks) {
            cout << task.toString() << "\n";
        }
        
        cout << "\n--------------------------------------------\n";
        cout << "[DSA STATS]\n";
        cout << "Total Tasks in Queue: " << queue.getTotalCount() << "\n";
        cout << "Pending: " << queue.getPendingCount() << " | ";
        cout << "Completed: " << queue.getCompletedCount() << "\n";
    }
    pause();
}

void viewPendingTasks(const TaskQueue& queue) {
    clearScreen();
    printHeader();
    cout << "\n=== PENDING TASKS ===\n";
    cout << "DSA Operation: FILTER Queue\n\n";
    
    vector<Task> tasks = queue.getPendingTasks();
    
    if (tasks.empty()) {
        cout << "No pending tasks!\n";
    } else {
        cout << "Pending Tasks (" << tasks.size() << "):\n";
        cout << "--------------------------------------------\n";
        for (const auto& task : tasks) {
            cout << task.toString() << "\n";
        }
    }
    pause();
}

void viewCompletedTasks(const TaskQueue& queue) {
    clearScreen();
    printHeader();
    cout << "\n=== COMPLETED TASKS ===\n";
    cout << "DSA Operation: FILTER Queue\n\n";
    
    vector<Task> tasks = queue.getCompletedTasks();
    
    if (tasks.empty()) {
        cout << "No completed tasks!\n";
    } else {
        cout << "Completed Tasks (" << tasks.size() << "):\n";
        cout << "--------------------------------------------\n";
        for (const auto& task : tasks) {
            cout << task.toString() << "\n";
        }
    }
    pause();
}

void completeTask(Database& db, TaskQueue& queue, int userId) {
    clearScreen();
    printHeader();
    cout << "\n=== MARK TASK COMPLETE ===\n";
    cout << "DSA Operation: SEARCH & UPDATE in Queue\n\n";
    
    vector<Task> tasks = queue.getPendingTasks();
    
    if (tasks.empty()) {
        cout << "No pending tasks!\n";
        pause();
        return;
    }
    
    cout << "Pending Tasks:\n";
    cout << "--------------------------------------------\n";
    for (const auto& task : tasks) {
        cout << task.toString() << "\n";
    }
    
    cout << "\nEnter Task ID: ";
    int id;
    cin >> id;
    cin.ignore();
    
    // Update in database
    if (db.completeTask(id)) {
        // Update in queue (DSA operation)
        queue.markComplete(id);
        
        cout << "\n✓ Task marked complete in Database!\n";
        cout << "✓ Task updated in Queue (DSA)!\n";
        cout << "\n[DSA INFO] Pending tasks: " << queue.getPendingCount() << "\n";
    } else {
        cout << "\n✗ Failed to update task!\n";
    }
    pause();
}

void deleteTask(Database& db, TaskQueue& queue, int userId) {
    clearScreen();
    printHeader();
    cout << "\n=== DELETE TASK ===\n";
    cout << "DSA Operation: REMOVE from Queue\n\n";
    
    vector<Task> tasks = queue.getAllTasks();
    
    if (tasks.empty()) {
        cout << "Queue is empty! No tasks to delete.\n";
        pause();
        return;
    }
    
    cout << "All Tasks:\n";
    cout << "--------------------------------------------\n";
    for (const auto& task : tasks) {
        cout << task.toString() << "\n";
    }
    
    cout << "\nEnter Task ID to delete: ";
    int id;
    cin >> id;
    cin.ignore();
    
    cout << "Are you sure? (y/n): ";
    char confirm;
    cin >> confirm;
    cin.ignore();
    
    if (confirm == 'y' || confirm == 'Y') {
        // Delete from database
        if (db.deleteTask(id)) {
            // Remove from queue (DSA operation)
            if (queue.removeById(id)) {
                cout << "\n✓ Task deleted from Database!\n";
                cout << "✓ Task removed from Queue (DSA)!\n";
                cout << "\n[DSA INFO] Queue size: " << queue.size() << " tasks\n";
            }
        } else {
            cout << "\n✗ Failed to delete task!\n";
        }
    }
    pause();
}

void peekFrontTask(const TaskQueue& queue) {
    clearScreen();
    printHeader();
    cout << "\n=== PEEK FRONT TASK ===\n";
    cout << "DSA Operation: PEEK (View front without removing)\n\n";
    
    Task* frontTask = const_cast<TaskQueue&>(queue).peek();
    
    if (frontTask) {
        cout << "Front of Queue (Next task to be processed):\n";
        cout << "--------------------------------------------\n";
        cout << frontTask->toString() << "\n";
        cout << "\n[DSA INFO] This is a PEEK operation - task remains in queue\n";
    } else {
        cout << "Queue is empty!\n";
    }
    pause();
}

void showQueueStats(const TaskQueue& queue) {
    clearScreen();
    printHeader();
    cout << "\n=== QUEUE STATISTICS (DSA) ===\n\n";
    
    cout << "Queue Data Structure Information:\n";
    cout << "--------------------------------------------\n";
    cout << "Implementation: Linked List (FIFO)\n";
    cout << "Total Nodes: " << queue.getTotalCount() << "\n";
    cout << "Pending Tasks: " << queue.getPendingCount() << "\n";
    cout << "Completed Tasks: " << queue.getCompletedCount() << "\n";
    cout << "Is Empty: " << (queue.isEmpty() ? "Yes" : "No") << "\n";
    cout << "\nQueue Operations Available:\n";
    cout << "  - Enqueue (Add to rear) - O(1)\n";
    cout << "  - Dequeue (Remove from front) - O(1)\n";
    cout << "  - Peek (View front) - O(1)\n";
    cout << "  - Search by ID - O(n)\n";
    cout << "  - Remove by ID - O(n)\n";
    cout << "  - Traverse - O(n)\n";
    
    pause();
}

void mainMenu(Database& db, TaskQueue& queue, int userId, const string& username) {
    while (true) {
        clearScreen();
        printHeader();
        cout << "\nLogged in as: " << username << "\n";
        cout << "\n[QUEUE STATUS] Size: " << queue.size() 
             << " | Pending: " << queue.getPendingCount() 
             << " | Completed: " << queue.getCompletedCount() << "\n";
        cout << "\n=== MAIN MENU ===\n";
        cout << "1. Add New Task (ENQUEUE)\n";
        cout << "2. View All Tasks (TRAVERSE)\n";
        cout << "3. View Pending Tasks\n";
        cout << "4. View Completed Tasks\n";
        cout << "5. Mark Task Complete\n";
        cout << "6. Delete Task (REMOVE)\n";
        cout << "7. Peek Front Task (PEEK)\n";
        cout << "8. Queue Statistics (DSA)\n";
        cout << "9. Logout\n";
        cout << "\nChoice: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        switch (choice) {
            case 1: addTask(db, queue, userId); break;
            case 2: viewTasks(queue); break;
            case 3: viewPendingTasks(queue); break;
            case 4: viewCompletedTasks(queue); break;
            case 5: completeTask(db, queue, userId); break;
            case 6: deleteTask(db, queue, userId); break;
            case 7: peekFrontTask(queue); break;
            case 8: showQueueStats(queue); break;
            case 9: return;
            default: 
                cout << "\nInvalid choice!\n";
                pause();
        }
    }
}

int main() {
    cout << "================================================\n";
    cout << "  TODO APP - DSA Project\n";
    cout << "  Data Structure: Queue (Linked List)\n";
    cout << "  Database: SQLite\n";
    cout << "================================================\n";
    pause();
    
    Database db;
    
    if (!db.connect("todoapp.db")) {
        cerr << "\n✗ Failed to connect to database!\n";
        pause();
        return 1;
    }
    
    int userId;
    string username;
    
    if (!loginMenu(db, userId, username)) {
        cout << "\nGoodbye!\n";
        return 0;
    }
    
    // Create Queue (DSA Data Structure)
    TaskQueue queue;
    cout << "\n[DSA] Initializing Queue data structure...\n";
    
    // Load tasks from database into queue
    loadTasksIntoQueue(db, queue, userId);
    pause();
    
    mainMenu(db, queue, userId, username);
    
    db.close();
    cout << "\n[DSA] Clearing Queue...\n";
    queue.clear();
    cout << "Thank you for using TODO App!\n";
    
    return 0;
}