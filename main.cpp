#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>

using namespace std;

struct Task {
    int id;
    string title;
    int priority;
    string deadline;
    bool completed;
};

struct ComparePriority {
    bool operator()(const Task& a, const Task& b) const {
        if (a.priority != b.priority) {
            return a.priority < b.priority;
        }
        return a.id > b.id;
    }
};

int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number.\n";
    }
}

string readLine(const string& prompt) {
    string line;
    cout << prompt;
    getline(cin, line);
    return line;
}

class TaskManager {
private:
    vector<Task> tasks;
    int nextId = 1;
    string filename;

    string escapeCsv(const string& s) {
        string out = s;
        replace(out.begin(), out.end(), ',', ';');
        return out;
    }

public:
    TaskManager(const string& file) : filename(file) {
        loadFromCsv();
    }

    void loadFromCsv() {
        ifstream in(filename);
        if (!in.is_open()) {
            return;
        }
        string line;
        getline(in, line);
        while (getline(in, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, title, prioStr, deadline, compStr;
            if (getline(ss, idStr, ',') &&
                getline(ss, title, ',') &&
                getline(ss, prioStr, ',') &&
                getline(ss, deadline, ',') &&
                getline(ss, compStr)) {
                Task t;
                t.id = stoi(idStr);
                t.title = title;
                t.priority = stoi(prioStr);
                t.deadline = deadline;
                t.completed = (compStr == "1" || compStr == "true");
                tasks.push_back(t);
                if (t.id >= nextId) {
                    nextId = t.id + 1;
                }
            }
        }
    }

    void saveToCsv() {
        ofstream out(filename);
        out << "ID,Title,Priority,Deadline,Completed\n";
        for (const auto& t : tasks) {
            out << t.id << ","
                << escapeCsv(t.title) << ","
                << t.priority << ","
                << escapeCsv(t.deadline) << ","
                << (t.completed ? 1 : 0) << "\n";
        }
    }

    void addTask(const string& title, int priority, const string& deadline) {
        Task t{nextId++, title, priority, deadline, false};
        tasks.push_back(t);
        cout << "Task added with ID " << t.id << ".\n";
    }

    void listAllTasksSorted() {
        if (tasks.empty()) {
            cout << "No tasks found.\n";
            return;
        }
        vector<Task> sorted = tasks;
        sort(sorted.begin(), sorted.end(), [](const Task& a, const Task& b) {
            if (a.priority != b.priority) return a.priority > b.priority;
            return a.id < b.id;
        });
        for (const auto& t : sorted) {
            cout << "[" << t.id << "] " << t.title
                 << " | Priority: " << t.priority
                 << " | Deadline: " << t.deadline
                 << " | Status: " << (t.completed ? "Completed" : "Pending") << "\n";
        }
    }

    void showNextUrgentTask() {
        priority_queue<Task, vector<Task>, ComparePriority> pq;
        for (const auto& t : tasks) {
            if (!t.completed) {
                pq.push(t);
            }
        }
        if (pq.empty()) {
            cout << "No pending urgent tasks.\n";
            return;
        }
        const Task& top = pq.top();
        cout << "Most Urgent Task -> [" << top.id << "] " << top.title
             << " (Priority: " << top.priority << ", Deadline: " << top.deadline << ")\n";
    }

    void completeTask(int id) {
        for (auto& t : tasks) {
            if (t.id == id) {
                t.completed = true;
                cout << "Task " << id << " marked as completed.\n";
                return;
            }
        }
        cout << "Task ID not found.\n";
    }

    void deleteTask(int id) {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if (it->id == id) {
                tasks.erase(it);
                cout << "Task " << id << " deleted.\n";
                return;
            }
        }
        cout << "Task ID not found.\n";
    }
};

int main() {
    TaskManager manager("tasks.csv");

    while (true) {
        cout << "\n=== C++ Task & Priority Scheduler ===\n";
        cout << "1. Add Task\n";
        cout << "2. View All Tasks (Sorted)\n";
        cout << "3. View Most Urgent Task (Max-Heap)\n";
        cout << "4. Mark Task Completed\n";
        cout << "5. Delete Task\n";
        cout << "0. Exit\n";

        int choice = readInt("Enter choice: ");

        if (choice == 1) {
            string title = readLine("Enter title: ");
            int priority = readInt("Enter priority (higher = more urgent): ");
            string deadline = readLine("Enter deadline: ");
            manager.addTask(title, priority, deadline);
        } else if (choice == 2) {
            manager.listAllTasksSorted();
        } else if (choice == 3) {
            manager.showNextUrgentTask();
        } else if (choice == 4) {
            int id = readInt("Enter Task ID to complete: ");
            manager.completeTask(id);
        } else if (choice == 5) {
            int id = readInt("Enter Task ID to delete: ");
            manager.deleteTask(id);
        } else if (choice == 0) {
            manager.saveToCsv();
            cout << "Saved to tasks.csv. Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice. Pick 0 to 5.\n";
        }
    }

    return 0;
}