#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

static void printUsage(const char *programName)
{
    printf("Usage:\n");
    printf("  %s              Start interactive mode\n", programName);
    printf("  %s --export     Export students to students.csv and exit\n", programName);
    printf("  %s --search <id|name>  Search by ID or name and exit\n", programName);
}

static void handleExport(void)
{
    initializeDatabase();
    exportCSV("students.csv");
    shutdownDatabase();
}

static void handleSearchArg(const char *arg)
{
    Student found;
    initializeDatabase();

    if (arg == NULL)
    {
        printf("Missing search argument. Use id:<number> or name:<string>.\n");
        shutdownDatabase();
        return;
    }

    if (strncmp(arg, "id:", 3) == 0)
    {
        int id = atoi(arg + 3);
        if (searchByID(id, &found))
        {
            printf("Student found by ID %d:\n", id);
            printf("ID: %d\nName: %s\nSemester: %d\nGPA: %.2f\n", found.id, found.name, found.semester, found.gpa);
        }
        else
        {
            printf("No student found with ID %d.\n", id);
        }
    }
    else if (strncmp(arg, "name:", 5) == 0)
    {
        char name[50];
        strncpy(name, arg + 5, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        printf("Searching for name containing '%s'...\n", name);
        if (!searchByName(name))
            printf("No student found with name containing '%s'.\n", name);
    }
    else
    {
        printf("Unknown search argument '%s'. Use id:<number> or name:<string>.\n", arg);
    }

    shutdownDatabase();
}

int main(int argc, char *argv[])
{
    initializeDatabase();

    if (argc > 1)
    {
        if (strcmp(argv[1], "--export") == 0)
        {
            handleExport();
            return 0;
        }
        if (strcmp(argv[1], "--search") == 0)
        {
            handleSearchArg(argc > 2 ? argv[2] : NULL);
            return 0;
        }
        printUsage(argv[0]);
        shutdownDatabase();
        return 1;
    }

    int choice;

    do
    {
        printf("\n-------------Student Management System-------------\n");
        printf("1. Add Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student (by ID or Name)\n");
        printf("4. Delete Student\n");
        printf("5. Update Student\n");
        printf("6. Undo Last Operation\n");
        printf("7. Redo Last Operation\n");
        printf("8. Export Students to CSV\n");
        printf("9. Exit\n");
        if (!safeScanInt("Enter your choice: ", &choice, 1, 9))
        {
            printf("Invalid choice! Please enter a number between 1 and 9.\n");
            continue;
        }
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
            undoLastOperation();
            break;
        case 7:
            redoLastOperation();
            break;
        case 8:
            exportCSV("students.csv");
            break;
        case 9:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 9);

    shutdownDatabase();
    return 0;
}