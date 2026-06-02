# Student Record Management System

A console-based application built in C using file handling concepts.
Supports basic CRUD operations on student records with admin authentication.

## Features
- Admin login with password protection (3 attempts)
- Add student records with ID, name, semester, and course marks
- Display all stored student records
- Search students by ID or name (case-insensitive)
- Update existing student details
- Delete student records
- Auto-calculates GPA from course marks
- Detects duplicate student IDs
- Pass/Fail status based on marks

## Tech Stack
C, File Handling, Structs

## How to Run
gcc main.c fileops.c -o sms
./sms
