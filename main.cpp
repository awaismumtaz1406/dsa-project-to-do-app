#include <iostream>
#include "TaskQueue.h"
using namespace std;

int main() {
    try {
        TaskQueue q;

        time_t now = time(0);
        Task t1("Study Chapter 3", "Read and take notes", now + 3600);
        Task t2("Do Homework", "Math assignment");
        Task t3("Complete Project", "Finish coding assignment", now + 7200);

        cout << "Adding tasks to queue...\n";
        q.enqueue(t1);
        q.enqueue(t2);
        q.enqueue(t3);

        q.printAll();

        cout << "\n\nCompleting next task...\n";
        Task completed = q.completeNext();

        q.printAll();

        cout << "\n\nQueue size: " << q.size() << endl;

    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        return 1;
    }

    return 0;
}