# C++ Task Scheduler

A command-line task management application built in C++ that prioritizes tasks using a **Max-Heap** and automatically saves data to a CSV file.

## Features
- **Add Task:** Create tasks with a title, priority level, and deadline.
- **View All Tasks (Sorted):** Display all tasks ordered by priority.
- **View Most Urgent Task:** Quickly fetch the highest-priority task using a Max-Heap.
- **Mark Task Completed:** Update task completion status by Task ID.
- **Delete Task:** Remove tasks by Task ID.
- **Persistent Storage:** Automatically loads from and saves changes to `tasks.csv` on exit.

## Project Structure
- `main.cpp` – Core C++ implementation (Max-Heap priority queue, task manager logic, and CLI menu).
- `tasks.csv` – CSV file storing task records (`ID,Title,Priority,Deadline,Completed`).

## How to Compile and Run

### Using g++ (Windows / Linux / macOS)
```bash
g++ main.cpp -o scheduler
./scheduler
