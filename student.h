#ifndef STUDENT_H
#define STUDENT_H
#define COURSE_COUNT 7
typedef struct
{
    int id;
    int courses[COURSE_COUNT];
    char name[50];
    float gpa;
    int semester;
} Student;
#endif