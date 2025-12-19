#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include "Task.h"
#include <vector>

using namespace std;

// Queue Implementation using Linked List (DSA Data Structure)
// FIFO - First In First Out
class TaskQueue {
private:
    // Node structure for linked list
    struct Node {
        Task data;
        Node* next;
        Node(const Task& t) : data(t), next(nullptr) {}
    };

    Node* head;   // Front of queue
    Node* tail;   // Rear of queue
    int count;    // Number of elements

public:
    TaskQueue();
    ~TaskQueue();

    // Core Queue Operations (DSA)
    void enqueue(const Task& task);     // Add to rear - O(1)
    Task dequeue();                      // Remove from front - O(1)
    Task* peek();                        // View front without removing - O(1)
    bool isEmpty() const;                // Check if empty - O(1)
    int size() const;                    // Get size - O(1)

    // Additional Operations
    bool removeById(int id);             // Remove specific task - O(n)
    Task* findById(int id);              // Find task by ID - O(n)
    void markComplete(int id);           // Mark task complete - O(n)
    
    // Get tasks by status
    vector<Task> getAllTasks() const;
    vector<Task> getPendingTasks() const;
    vector<Task> getCompletedTasks() const;
    
    // Utility
    void clear();                        // Clear all tasks
    
    // Statistics
    int getTotalCount() const;
    int getPendingCount() const;
    int getCompletedCount() const;
    
    // Update task in queue
    void updateTask(const Task& task);   // Update existing task
};

#endif