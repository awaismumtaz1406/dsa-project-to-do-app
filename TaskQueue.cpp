#include "TaskQueue.h"
#include <stdexcept>
#include <iostream>

using namespace std;

// Constructor - Initialize empty queue
TaskQueue::TaskQueue() : head(nullptr), tail(nullptr), count(0) {}

// Destructor - Clean up all nodes
TaskQueue::~TaskQueue() {
    clear();
}

// ENQUEUE: Add task to rear of queue - O(1) time complexity
void TaskQueue::enqueue(const Task& task) {
    Node* newNode = new Node(task);
    
    if (tail == nullptr) {
        // Queue is empty - both head and tail point to new node
        head = tail = newNode;
    } else {
        // Add to rear
        tail->next = newNode;
        tail = newNode;
    }
    count++;
}

// DEQUEUE: Remove and return task from front - O(1) time complexity
Task TaskQueue::dequeue() {
    if (head == nullptr) {
        throw runtime_error("Queue is empty! Cannot dequeue.");
    }
    
    Node* temp = head;
    Task task = temp->data;
    
    head = head->next;
    if (head == nullptr) {
        // Queue is now empty
        tail = nullptr;
    }
    
    delete temp;
    count--;
    
    return task;
}

// PEEK: View front task without removing - O(1) time complexity
Task* TaskQueue::peek() {
    if (head == nullptr) {
        return nullptr;
    }
    return &(head->data);
}

// Check if queue is empty - O(1)
bool TaskQueue::isEmpty() const {
    return head == nullptr;
}

// Get number of tasks - O(1)
int TaskQueue::size() const {
    return count;
}

// Remove task by ID - O(n) time complexity (linear search)
bool TaskQueue::removeById(int id) {
    Node* current = head;
    Node* previous = nullptr;
    
    while (current != nullptr) {
        if (current->data.getId() == id) {
            // Found the task to remove
            if (previous == nullptr) {
                // Removing head
                head = current->next;
                if (head == nullptr) {
                    tail = nullptr;
                }
            } else {
                // Removing middle or tail
                previous->next = current->next;
                if (current == tail) {
                    tail = previous;
                }
            }
            
            delete current;
            count--;
            return true;
        }
        
        previous = current;
        current = current->next;
    }
    
    return false;  // Task not found
}

// Find task by ID - O(n) time complexity
Task* TaskQueue::findById(int id) {
    Node* current = head;
    
    while (current != nullptr) {
        if (current->data.getId() == id) {
            return &(current->data);
        }
        current = current->next;
    }
    
    return nullptr;  // Task not found
}

// Mark task as complete - O(n) time complexity
void TaskQueue::markComplete(int id) {
    Task* task = findById(id);
    if (task) {
        task->markComplete();
    }
}

// Update task in queue - O(n) time complexity
void TaskQueue::updateTask(const Task& task) {
    Task* existingTask = findById(task.getId());
    if (existingTask) {
        *existingTask = task;
    }
}

// Get all tasks as vector - O(n) traversal
vector<Task> TaskQueue::getAllTasks() const {
    vector<Task> tasks;
    Node* current = head;
    
    while (current != nullptr) {
        tasks.push_back(current->data);
        current = current->next;
    }
    
    return tasks;
}

// Get pending tasks - O(n) traversal with filter
vector<Task> TaskQueue::getPendingTasks() const {
    vector<Task> tasks;
    Node* current = head;
    
    while (current != nullptr) {
        if (!current->data.isCompleted()) {
            tasks.push_back(current->data);
        }
        current = current->next;
    }
    
    return tasks;
}

// Get completed tasks - O(n) traversal with filter
vector<Task> TaskQueue::getCompletedTasks() const {
    vector<Task> tasks;
    Node* current = head;
    
    while (current != nullptr) {
        if (current->data.isCompleted()) {
            tasks.push_back(current->data);
        }
        current = current->next;
    }
    
    return tasks;
}

// Clear all tasks - O(n) deletion
void TaskQueue::clear() {
    Node* current = head;
    
    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    
    head = tail = nullptr;
    count = 0;
}

// Get total count - O(1)
int TaskQueue::getTotalCount() const {
    return count;
}

// Get pending count - O(n) traversal
int TaskQueue::getPendingCount() const {
    int pending = 0;
    Node* current = head;
    
    while (current != nullptr) {
        if (!current->data.isCompleted()) {
            pending++;
        }
        current = current->next;
    }
    
    return pending;
}

// Get completed count - O(n) traversal
int TaskQueue::getCompletedCount() const {
    int completed = 0;
    Node* current = head;
    
    while (current != nullptr) {
        if (current->data.isCompleted()) {
            completed++;
        }
        current = current->next;
    }
    
    return completed;
}