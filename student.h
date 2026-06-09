#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COURSE_COUNT 7

typedef struct
{
    int id;
    int courses[COURSE_COUNT];
    char name[50];
    float gpa;
    int semester;
} Student;

void initializeDatabase(void);
void shutdownDatabase(void);
void addStudents(void);
void displayStudents(void);
void searchStudents(void);
void deleteStudentByID(void);
void updateStudentByID(void);
void exportCSV(const char *filename);
void undoLastOperation(void);
void redoLastOperation(void);
int searchByID(int id, Student *outStudent);
int searchByName(const char *query);
int safeScanInt(const char *prompt, int *value, int min, int max);
int safeScanString(const char *prompt, char *buffer, size_t size);

#endif