#ifndef STUDENT_H
#define STUDENT_H
typedef struct
{
    int id;
    char name[50];
    char course[50];
    float gpa;
    int semester;
} Student;
#endif