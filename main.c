#include <stdio.h>
#include "student.h"
void addStudents();
void displayStudents();
void searchStudents();
void deleteStudentByID();
void updateStudentByID();
int main()
{
    int choice;
    do
    {
        printf("\n-------------Student Management System-------------\n");
        printf("1. Add Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student\n");
        printf("4. Delete Student\n");
        printf("5. Update Student\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            addStudents();
            break;
        case 2:
            displayStudents();
            break;
        case 3:
            searchStudents();
            break;
        case 4:
            deleteStudentByID();
            break;
        case 5:
            updateStudentByID();
            break;
        case 6:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 6);
    return 0;
}