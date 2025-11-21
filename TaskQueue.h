#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include "Task.h"

class TaskQueue {
private:
    struct Node {
        Task data;
        Node* next;
        Node(const Task& t) : data(t), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int count;

public:
    TaskQueue() : head(nullptr), tail(nullptr), count(0) {}
    
    ~TaskQueue() {
        clear();
    }

    void enqueue(const Task& t) {
        Node* node = new Node(t);
        if (tail == nullptr) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        count++;
    }

    Task dequeue() {
        if (head == nullptr) {
            throw runtime_error("Queue is empty!");
        }
        Node* node = head;
        Task ret = node->data;
        head = head->next;
        if (head == nullptr) tail = nullptr;
        delete node;
        count--;
        return ret;
    }

    Task* peek() {
        if (head == nullptr) return nullptr;
        return &(head->data);
    }

    bool isEmpty() const { 
        return head == nullptr; 
    }
    
    int size() const { 
        return count; 
    }

    Task completeNext() {
        Task t = dequeue();
        t.markComplete();
        return t;
    }

    void printAll() const {
        cout << "\n=== Task Queue (Total: " << count << ") ===\n";
        if (head == nullptr) {
            cout << "(Queue is empty)\n";
            return;
        }
        Node* cur = head;
        int pos = 1;
        while (cur != nullptr) {
            cout << "\n--- Task #" << pos++ << " ---";
            cur->data.showTask();
            cur = cur->next;
        }
    }

    void clear() {
        Node* cur = head;
        while (cur != nullptr) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        head = tail = nullptr;
        count = 0;
    }
};

#endif