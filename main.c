#include <stdio.h>
#include <string.h>
#include "student.h"
void addStudents();
void displayStudents();
void searchStudents();
void deleteStudentByID();
void updateStudentByID();

// Password verification function
int verifyAdminPassword()
{
    char password[50];
    char correctPassword[] = "Kishan@123456";
    int attempts = 3;

    while (attempts > 0)
    {
        printf("\n========== Admin Login ==========\n");
        printf("Enter Admin Password: ");
        scanf("%s", password);

        if (strcmp(password, correctPassword) == 0)
        {
            printf("Password correct! Access granted.\n");
            printf("===================================\n");
            return 1; // Login successful
        }
        else
        {
            attempts--;
            if (attempts > 0)
            {
                printf("Incorrect password. %d attempts remaining.\n", attempts);
            }
            else
            {
                printf("Too many failed attempts. Access denied!\n");
            }
        }
    }
    return 0; // Login failed
}

int main()
{
    int choice;

    // Password verification before accessing the system
    if (!verifyAdminPassword())
    {
        printf("System locked. Exiting...\n");
        return 1;
    }

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