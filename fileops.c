#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "student.h"

/* forward declaration so functions above can use it */
static void toLowerCopy(const char *src, char *dst);

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

static float calculateGPA(const int courses[], int count)
{
    float total = 0;
    for (int i = 0; i < count; i++)
    {
        total += courses[i];
    }
    float average = total / 7.0f;
    return average / 10.0f; // Convert average marks (0-100) to GPA on a 10-point scale
}

static void printStudent(const Student *student)
{
    printf("ID: %d\n", student->id);
    printf("Name: %s\n", student->name);
    printf("Semester: %d\n", student->semester);
    printf("Marks:");
    for (int i = 0; i < COURSE_COUNT; i++)
    {
        printf(" %d", student->courses[i]);
        if (i < COURSE_COUNT - 1)
        {
            printf(",");
        }
    }
    printf("\n");
    printf("GPA: %.2f\n", student->gpa);
    for (int i = 0; i < COURSE_COUNT; i++)
    {
        if (student->courses[i] < 50)
        {
            printf("Status: Failed\n");
            return;
        }
    }
    printf("Status: Passed and congrats on being promoted to next semester!\n");
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
    for (int i = 0; i < COURSE_COUNT; i++)
    {
        printf("Enter Student Course %d: ", i + 1);
        scanf("%d", &student.courses[i]);
    }

    student.gpa = calculateGPA(student.courses, COURSE_COUNT);

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
        printStudent(&student);
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
    int choice, id, found = 0;
    char name[50];
    printf("Search by: 1. ID  2. Name\n");
    printf("Enter choice: ");
    if (scanf("%d", &choice) != 1)
    {
        printf("Invalid input.\n");
        fclose(fp);
        return;
    }

    if (choice == 1)
    {
        printf("Enter ID to search: ");
        scanf("%d", &id);
        while (fread(&student, sizeof(Student), 1, fp))
        {
            if (student.id == id)
            {
                printStudent(&student);
                found = 1;
                break;
            }
        }
        if (!found)
        {
            printf("Student not found.\n");
        }
    }
    else if (choice == 2)
    {
        printf("Enter Name to search: ");
        scanf("%s", name);
        char lower_student[50];
        char lower_name[50];
        toLowerCopy(name, lower_name);
        while (fread(&student, sizeof(Student), 1, fp))
        {
            toLowerCopy(student.name, lower_student);
            if (strstr(lower_student, lower_name) != NULL)
            {
                printStudent(&student);
                printf("---------------------------------------\n");
                found = 1;
                /* continue searching to find all matches */
            }
        }
        if (!found)
        {
            printf("Student not found.\n");
        }
    }
    else
    {
        printf("Invalid choice.\n");
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
            for (int i = 0; i < COURSE_COUNT; i++)
            {
                printf("Enter new marks for course %d: ", i + 1);
                scanf("%d", &student.courses[i]);
            }
            student.gpa = calculateGPA(student.courses, COURSE_COUNT);
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

static void toLowerCopy(const char *src, char *dst)
{
    int i;
    for (i = 0; i < 50 && src[i] != '\0'; i++)
    {
        dst[i] = (char)tolower((unsigned char)src[i]);
    }
    dst[i] = '\0';
}