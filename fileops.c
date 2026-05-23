#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

FILE *openStudentFile(const char *mode)
{
    FILE *fp;
    fp = fopen("students.dat", mode);
    if (fp == NULL)
    {
        printf("Error in opening the file.\n");
        return NULL;
    }
    return fp;
}

static float calculateGPA(int course1, int course2, int course3, int course4, int course5, int course6, int course7)
{
    float total = course1 + course2 + course3 + course4 + course5 + course6 + course7;
    float average = total / 7.0f;
    return average / 10.0f; // Convert average marks (0-100) to GPA on a 10-point scale
}

static int isDuplicateID(int id)
{
    Student student;
    FILE *fp = openStudentFile("rb");
    if (fp == NULL)
        return 0; // If file can't be opened, assume no duplicates
    while (fread(&student, sizeof(Student), 1, fp))
    {
        if (student.id == id)
        {
            fclose(fp);
            return 1; // Duplicate ID found
        }
    }
    fclose(fp);
    return 0; // No duplicate ID found
}

void addStudents()
{
    Student student;
    printf("Enter Student ID: ");
    scanf("%d", &student.id);
    if (isDuplicateID(student.id))
    {
        printf("Error: Student ID %d already exists. Please use a unique ID.\n", student.id);
        return;
    }

    FILE *fp = openStudentFile("ab");
    if (fp == NULL)
        return;
    printf("Enter Student Name: ");
    scanf("%s", student.name);
    printf("Enter Student Semester: ");
    scanf("%d", &student.semester);
    printf("Enter Student Course 1: ");
    scanf("%d", &student.course1);
    printf("Enter Student Course 2: ");
    scanf("%d", &student.course2);
    printf("Enter Student Course 3: ");
    scanf("%d", &student.course3);
    printf("Enter Student Course 4: ");
    scanf("%d", &student.course4);
    printf("Enter Student Course 5: ");
    scanf("%d", &student.course5);
    printf("Enter Student Course 6: ");
    scanf("%d", &student.course6);
    printf("Enter Student Course 7: ");
    scanf("%d", &student.course7);

    student.gpa = calculateGPA(student.course1, student.course2, student.course3, student.course4, student.course5, student.course6, student.course7);

    fwrite(&student, sizeof(Student), 1, fp);
    fclose(fp);
}

void displayStudents()
{
    Student student;
    FILE *fp = openStudentFile("rb");
    if (fp == NULL)
        return;
    printf("-------------Student Records-------------\n");
    while (fread(&student, sizeof(Student), 1, fp))
    {
        printf("ID: %d\n", student.id);
        printf("Name: %s\n", student.name);
        printf("Semester: %d\n", student.semester);
        printf("Marks: %d, %d, %d, %d, %d, %d, %d\n", student.course1, student.course2, student.course3, student.course4, student.course5, student.course6, student.course7);
        printf("GPA: %.2f\n", student.gpa);
        printf("---------------------------------------\n");
    }
    fclose(fp);
}

void searchStudents()
{
    Student student;
    FILE *fp = openStudentFile("rb");
    if (fp == NULL)
        return;
    int id, found = 0;
    printf("Enter ID to search: ");
    scanf("%d", &id);
    while (fread(&student, sizeof(Student), 1, fp))
    {
        if (student.id == id)
        {
            printf("ID: %d\n", student.id);
            printf("Name: %s\n", student.name);
            printf("Semester: %d\n", student.semester);
            printf("Marks: %d, %d, %d, %d, %d, %d, %d\n", student.course1, student.course2, student.course3, student.course4, student.course5, student.course6, student.course7);
            printf("GPA: %.2f\n", student.gpa);
            found = 1;
            break;
        }
    }
    if (!found)
    {
        printf("Student not found.\n");
    }
    fclose(fp);
}

void updateStudentByID()
{
    Student student;
    int id, found = 0;
    FILE *fp = openStudentFile("r+b");
    if (fp == NULL)
        return;
    printf("Enter ID to update: ");
    scanf("%d", &id);
    while (fread(&student, sizeof(Student), 1, fp))
    {
        if (student.id == id)
        {
            printf("Enter new name: ");
            scanf("%s", student.name);
            printf("Enter new semester: ");
            scanf("%d", &student.semester);
            printf("Enter new marks for course 1: ");
            scanf("%d", &student.course1);
            printf("Enter new marks for course 2: ");
            scanf("%d", &student.course2);
            printf("Enter new marks for course 3: ");
            scanf("%d", &student.course3);
            printf("Enter new marks for course 4: ");
            scanf("%d", &student.course4);
            printf("Enter new marks for course 5: ");
            scanf("%d", &student.course5);
            printf("Enter new marks for course 6: ");
            scanf("%d", &student.course6);
            printf("Enter new marks for course 7: ");
            scanf("%d", &student.course7);
            student.gpa = calculateGPA(student.course1, student.course2, student.course3, student.course4, student.course5, student.course6, student.course7);
            fseek(fp, -sizeof(Student), SEEK_CUR);
            fwrite(&student, sizeof(Student), 1, fp);
            found = 1;
            break;
        }
    }
    if (!found)
    {
        printf("Student not found.\n");
    }
    fclose(fp);
}

void deleteStudentByID()
{
    Student student;
    int id, found = 0;
    FILE *fp = openStudentFile("rb");
    FILE *temp = fopen("temp.dat", "wb");
    if (fp == NULL || temp == NULL)
        return;
    printf("Enter ID to delete: ");
    scanf("%d", &id);
    while (fread(&student, sizeof(Student), 1, fp))
    {
        if (student.id != id)
        {
            fwrite(&student, sizeof(Student), 1, temp);
        }
    }
    fclose(fp);
    fclose(temp);
    remove("students.dat");
    rename("temp.dat", "students.dat");
}