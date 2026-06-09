# Student Record Management System

A console-based application built in C using file handling concepts. Supports basic CRUD operations on student records with binary persistence and enhanced data management features.

## Features
- Add student records with ID, name, semester, and course marks
- Display all stored student records with sort options
- Search students by ID or name (case-insensitive)
- Update existing student details
- Delete student records
- Auto-calculates GPA from course marks
- Detects duplicate student IDs
- Pass/Fail status based on marks
- In-memory linked list storage for loaded records
- BST-based ID search for faster lookup
- Paginated display of records
- Undo/redo support for recent changes
- Class statistics report
- ASCII GPA distribution chart
- Generates per-student grade report files
- CSV export of student data
- Audit logging of actions
- CLI options: `--export`, `--search`

## Tech Stack
C, File Handling, Structs, Linked List, BST

## How to Run
gcc main.c fileops.c -o sms  
./sms
