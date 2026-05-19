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

void addStudents()
{
    Student student;
    FILE *fp = openStudentFile("ab");
    if (fp == NULL)
        return;
    printf("Enter Student ID: ");
    scanf("%d", &student.id);
    printf("Enter Student Name: ");
    scanf("%s", student.name);
    printf("Enter Student Course: ");
    scanf("%s", student.course);
    printf("Enter Student GPA: ");
    scanf("%f", &student.gpa);
    printf("Enter Student Semester: ");
    scanf("%d", &student.semester);
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
        printf("Course: %s\n", student.course);
        printf("GPA: %.2f\n", student.gpa);
        printf("Semester: %d\n", student.semester);
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
    printf("Enter ID to search");
    scanf("%d", &id);
    while (fread(&student, sizeof(Student), 1, fp))
    {
        if (student.id == id)
        {
            printf("ID: %d\n", student.id);
            printf("Name: %s\n", student.name);
            printf("Course: %s\n", student.course);
            printf("GPA: %.2f\n", student.gpa);
            printf("Semester: %d\n", student.semester);
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
            printf("Enter new course: ");
            scanf("%s", student.course);
            printf("Enter new GPA: ");
            scanf("%f", &student.gpa);
            printf("Enter new semester: ");
            scanf("%d", &student.semester);
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