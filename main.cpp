#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <string>
#include <cstdlib>

using namespace std;

class Task {
private:
    int id;
    string title;
    int priority;
    string deadline;
    bool completed;

public:
    Task() : id(0), priority(1), completed(false) {}

    Task(int id, string title, int priority, string deadline, bool completed)
        : id(id), title(title), priority(priority), deadline(deadline), completed(completed) {}

    int getId() const { return id; }
    string getTitle() const { return title; }
    int getPriority() const { return priority; }
    string getDeadline() const { return deadline; }
    bool isCompleted() const { return completed; }

    void markCompleted() { completed = true; }

    string toCSV() const {
        ostringstream oss;
        oss << id << "," << title << "," << priority << "," << deadline << "," << (completed ? "1" : "0");
        return oss.str();
    }

    void display() const {
        cout << "[" << (completed ? "DONE" : "PENDING") << "] "
             << "ID: " << id
             << " | Priority: P" << priority
             << " | Due: " << deadline
             << " | " << title << "\n";
    }
};

struct ComparePriority {
    bool operator()(const Task& a, const Task& b) const {
        if (a.getPriority() == b.getPriority()) {
            return a.getDeadline() > b.getDeadline();
        }
        return a.getPriority() < b.getPriority();
    }
};

bool compareAllTasks(const Task& a, const Task& b) {
    if (a.isCompleted() != b.isCompleted())
        return !a.isCompleted();
    if (a.getPriority() != b.getPriority())
        return a.getPriority() > b.getPriority();
    return a.getDeadline() < b.getDeadline();
}

class TaskManager {
private:
    map<int, Task> taskMap;
    string filename;
    int nextId;

public:
    TaskManager(string file) : filename(file), nextId(1) {
        loadFromFile();
    }

    ~TaskManager() {
        saveToFile();
    }

    void addTask(const string& title, int priority, const string& deadline) {
        if (priority < 1 || priority > 5) {
            cout << "Invalid priority! Setting default priority to 3.\n";
            priority = 3;
        }
        Task newTask(nextId, title, priority, deadline, false);
        taskMap[nextId] = newTask;
        cout << "Task added successfully with ID: " << nextId << "\n";
        nextId++;
        saveToFile();
    }

    void completeTask(int id) {
        map<int, Task>::iterator it = taskMap.find(id);
        if (it == taskMap.end()) {
            cout << "Task ID " << id << " not found.\n";
            return;
        }
        it->second.markCompleted();
        cout << "Marked Task #" << id << " as completed.\n";
        saveToFile();
    }

    void deleteTask(int id) {
        if (taskMap.erase(id) > 0) {
            cout << "Deleted Task #" << id << ".\n";
            saveToFile();
        } else {
            cout << "Task ID " << id << " not found.\n";
        }
    }

    void showNextUrgentTask() const {
        priority_queue<Task, vector<Task>, ComparePriority> pq;
        for (map<int, Task>::const_iterator it = taskMap.begin(); it != taskMap.end(); ++it) {
            if (!it->second.isCompleted()) {
                pq.push(it->second);
            }
        }
        if (pq.empty()) {
            cout << "No pending tasks! You're all caught up.\n";
            return;
        }
        cout << "\n--- Most Urgent Pending Task ---\n";
        pq.top().display();
    }

    void listAllTasksSorted() const {
        if (taskMap.empty()) {
            cout << "No tasks available.\n";
            return;
        }
        vector<Task> list;
        for (map<int, Task>::const_iterator it = taskMap.begin(); it != taskMap.end(); ++it) {
            list.push_back(it->second);
        }
        sort(list.begin(), list.end(), compareAllTasks);

        cout << "\n--- All Tasks (Sorted by Status & Priority) ---\n";
        for (size_t i = 0; i < list.size(); ++i) {
            list[i].display();
        }
    }

    void saveToFile() const {
        ofstream outFile(filename.c_str());
        if (!outFile) {
            cerr << "Error saving to file!\n";
            return;
        }
        for (map<int, Task>::const_iterator it = taskMap.begin(); it != taskMap.end(); ++it) {
            outFile << it->second.toCSV() << "\n";
        }
        outFile.close();
    }

    void loadFromFile() {
        ifstream inFile(filename.c_str());
        if (!inFile) return;

        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, title, prioStr, deadline, compStr;

            getline(ss, idStr, ',');
            getline(ss, title, ',');
            getline(ss, prioStr, ',');
            getline(ss, deadline, ',');
            getline(ss, compStr, ',');

            int id = atoi(idStr.c_str());
            int priority = atoi(prioStr.c_str());
            bool completed = (compStr == "1");

            taskMap[id] = Task(id, title, priority, deadline, completed);
            if (id + 1 > nextId) {
                nextId = id + 1;
            }
        }
        inFile.close();
    }
};

int main() {
    TaskManager manager("tasks.csv");
    int choice;

    while (true) {
        cout << "\n=== C++ Task & Priority Scheduler ===\n"
             << "1. Add Task\n"
             << "2. View All Tasks (Sorted)\n"
             << "3. View Most Urgent Task (Max-Heap)\n"
             << "4. Mark Task Completed\n"
             << "5. Delete Task\n"
             << "0. Exit\n"
             << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Enter a number.\n";
            continue;
        }
        cin.ignore();

        if (choice == 0) {
            cout << "Saved all changes. Goodbye!\n";
            break;
        } else if (choice == 1) {
            string title, deadline;
            int priority;
            cout << "Enter task title (avoid commas): ";
            getline(cin, title);
            cout << "Enter priority (1-5, 5 = highest): ";
            cin >> priority;
            cin.ignore();
            cout << "Enter deadline (YYYY-MM-DD): ";
            getline(cin, deadline);
            manager.addTask(title, priority, deadline);
        } else if (choice == 2) {
            manager.listAllTasksSorted();
        } else if (choice == 3) {
            manager.showNextUrgentTask();
        } else if (choice == 4) {
            int id;
            cout << "Enter Task ID to complete: ";
            cin >> id;
            manager.completeTask(id);
        } else if (choice == 5) {
            int id;
            cout << "Enter Task ID to delete: ";
            cin >> id;
            manager.deleteTask(id);
        } else {
            cout << "Invalid option.\n";
        }
    }
    return 0;
}