#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "student.h"

#define PAGE_SIZE 5
#define MAX_UNDO 50

typedef struct StudentNode
{
    Student student;
    struct StudentNode *next;
} StudentNode;

typedef struct BSTNode
{
    Student *student;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

typedef enum
{
    OP_ADD,
    OP_UPDATE,
    OP_DELETE
} OperationType;

typedef struct
{
    OperationType type;
    Student oldValue;
    Student newValue;
} Operation;

static StudentNode *head = NULL;
static BSTNode *idRoot = NULL;
static Operation undoStack[MAX_UNDO];
static Operation redoStack[MAX_UNDO];
static int undoTop = 0;
static int redoTop = 0;

static void trimNewline(char *text)
{
    size_t len = strlen(text);
    if (len > 0 && text[len - 1] == '\n')
        text[len - 1] = '\0';
}

int safeScanString(const char *prompt, char *buffer, size_t size)
{
    if (prompt)
        printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL)
        return 0;
    trimNewline(buffer);
    while (buffer[0] == '\0')
    {
        printf("Input cannot be empty. %s", prompt ? prompt : "");
        if (fgets(buffer, (int)size, stdin) == NULL)
            return 0;
        trimNewline(buffer);
    }
    return 1;
}

int safeScanInt(const char *prompt, int *value, int min, int max)
{
    char buffer[128];
    char *end;
    long parsed;

    while (1)
    {
        if (prompt)
            printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            return 0;
        trimNewline(buffer);
        if (buffer[0] == '\0')
        {
            printf("Input cannot be empty.\n");
            continue;
        }
        parsed = strtol(buffer, &end, 10);
        if (*end != '\0')
        {
            printf("Please enter a valid integer.\n");
            continue;
        }
        if (parsed < min || parsed > max)
        {
            printf("Please enter a value between %d and %d.\n", min, max);
            continue;
        }
        *value = (int)parsed;
        return 1;
    }
}

static StudentNode *createStudentNode(const Student *student)
{
    StudentNode *node = malloc(sizeof(StudentNode));
    if (!node)
    {
        perror("Unable to allocate memory for student");
        exit(EXIT_FAILURE);
    }
    node->student = *student;
    node->next = NULL;
    return node;
}

static BSTNode *createBSTNode(Student *student)
{
    BSTNode *node = malloc(sizeof(BSTNode));
    if (!node)
    {
        perror("Unable to allocate memory for BST node");
        exit(EXIT_FAILURE);
    }
    node->student = student;
    node->left = node->right = NULL;
    return node;
}

static void appendStudentNode(StudentNode *node)
{
    if (!head)
    {
        head = node;
        return;
    }
    StudentNode *current = head;
    while (current->next)
        current = current->next;
    current->next = node;
}

static BSTNode *insertBST(BSTNode *root, Student *student)
{
    if (!root)
        return createBSTNode(student);
    if (student->id < root->student->id)
        root->left = insertBST(root->left, student);
    else if (student->id > root->student->id)
        root->right = insertBST(root->right, student);
    return root;
}

static BSTNode *findBSTNode(BSTNode *root, int id)
{
    if (!root)
        return NULL;
    if (id < root->student->id)
        return findBSTNode(root->left, id);
    if (id > root->student->id)
        return findBSTNode(root->right, id);
    return root;
}

static BSTNode *findMinBSTNode(BSTNode *root)
{
    while (root && root->left)
        root = root->left;
    return root;
}

static BSTNode *removeBSTNode(BSTNode *root, int id)
{
    if (!root)
        return NULL;
    if (id < root->student->id)
        root->left = removeBSTNode(root->left, id);
    else if (id > root->student->id)
        root->right = removeBSTNode(root->right, id);
    else
    {
        if (!root->left)
        {
            BSTNode *temp = root->right;
            free(root);
            return temp;
        }
        else if (!root->right)
        {
            BSTNode *temp = root->left;
            free(root);
            return temp;
        }
        BSTNode *successor = findMinBSTNode(root->right);
        root->student = successor->student;
        root->right = removeBSTNode(root->right, successor->student->id);
    }
    return root;
}

static void destroyBST(BSTNode *root)
{
    if (!root)
        return;
    destroyBST(root->left);
    destroyBST(root->right);
    free(root);
}

static void destroyStudentList(void)
{
    StudentNode *current = head;
    while (current)
    {
        StudentNode *next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}

static void logAction(const char *action, const Student *student)
{
    FILE *fp = fopen("log.txt", "a");
    if (!fp)
        return;
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d | %s | ID=%d | Name=%s | Semester=%d | GPA=%.2f\n",
            local->tm_year + 1900,
            local->tm_mon + 1,
            local->tm_mday,
            local->tm_hour,
            local->tm_min,
            local->tm_sec,
            action,
            student->id,
            student->name,
            student->semester,
            student->gpa);
    fclose(fp);
}

static void saveDatabase(void)
{
    FILE *fp = fopen("students.dat", "wb");
    if (!fp)
    {
        perror("Unable to save students.dat");
        return;
    }
    StudentNode *current = head;
    while (current)
    {
        fwrite(&current->student, sizeof(Student), 1, fp);
        current = current->next;
    }
    fclose(fp);
}

void initializeDatabase(void)
{
    FILE *fp = fopen("students.dat", "rb");
    if (!fp)
        return;
    Student temp;
    while (fread(&temp, sizeof(Student), 1, fp) == 1)
    {
        StudentNode *node = createStudentNode(&temp);
        appendStudentNode(node);
        idRoot = insertBST(idRoot, &node->student);
    }
    fclose(fp);
}

void shutdownDatabase(void)
{
    saveDatabase();
    destroyStudentList();
    destroyBST(idRoot);
    idRoot = NULL;
}

static void toLowerCopy(const char *src, char *dst)
{
    while (*src)
    {
        *dst++ = (char)tolower((unsigned char)*src++);
    }
    *dst = '\0';
}

static int compareStringsCaseInsensitive(const char *a, const char *b)
{
    while (*a && *b)
    {
        int da = tolower((unsigned char)*a);
        int db = tolower((unsigned char)*b);
        if (da != db)
            return da - db;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

static float calculateGPA(const int courses[], int count)
{
    float total = 0.0f;
    for (int i = 0; i < count; i++)
        total += courses[i];
    return (total / count) / 10.0f;
}

static int isDuplicateID(int id)
{
    return findBSTNode(idRoot, id) != NULL;
}

static void pushUndo(Operation op)
{
    if (undoTop < MAX_UNDO)
        undoStack[undoTop++] = op;
    else
    {
        memmove(undoStack, undoStack + 1, sizeof(Operation) * (MAX_UNDO - 1));
        undoStack[MAX_UNDO - 1] = op;
    }
    redoTop = 0;
}

static void pushRedo(Operation op)
{
    if (redoTop < MAX_UNDO)
        redoStack[redoTop++] = op;
    else
    {
        memmove(redoStack, redoStack + 1, sizeof(Operation) * (MAX_UNDO - 1));
        redoStack[MAX_UNDO - 1] = op;
    }
}

static int deleteStudentNodeByID(int id)
{
    StudentNode *current = head;
    StudentNode *previous = NULL;
    while (current)
    {
        if (current->student.id == id)
        {
            if (previous)
                previous->next = current->next;
            else
                head = current->next;
            free(current);
            idRoot = removeBSTNode(idRoot, id);
            return 1;
        }
        previous = current;
        current = current->next;
    }
    return 0;
}

static void generateGradeReportCard(const Student *student)
{
    char filename[64];
    snprintf(filename, sizeof(filename), "report_%d.txt", student->id);
    FILE *fp = fopen(filename, "w");
    if (!fp)
        return;
    fprintf(fp, "Student Report Card\n");
    fprintf(fp, "===================\n");
    fprintf(fp, "ID: %d\n", student->id);
    fprintf(fp, "Name: %s\n", student->name);
    fprintf(fp, "Semester: %d\n", student->semester);
    fprintf(fp, "GPA: %.2f\n", student->gpa);
    fprintf(fp, "Marks:\n");
    for (int i = 0; i < COURSE_COUNT; i++)
        fprintf(fp, "  Course %d: %d\n", i + 1, student->courses[i]);
    fprintf(fp, "Status: %s\n", student->gpa >= 5.0f ? "Passed" : "Failed");
    fclose(fp);
}

static void generateAllReportCards(void)
{
    StudentNode *current = head;
    while (current)
    {
        generateGradeReportCard(&current->student);
        current = current->next;
    }
}

static int compareID(const void *a, const void *b)
{
    const Student *s1 = *(const Student **)a;
    const Student *s2 = *(const Student **)b;
    return s1->id - s2->id;
}

static int compareName(const void *a, const void *b)
{
    const Student *s1 = *(const Student **)a;
    const Student *s2 = *(const Student **)b;
    return compareStringsCaseInsensitive(s1->name, s2->name);
}

static int compareGPA(const void *a, const void *b)
{
    const Student *s1 = *(const Student **)a;
    const Student *s2 = *(const Student **)b;
    if (s1->gpa < s2->gpa)
        return -1;
    if (s1->gpa > s2->gpa)
        return 1;
    return 0;
}

static int compareSemester(const void *a, const void *b)
{
    const Student *s1 = *(const Student **)a;
    const Student *s2 = *(const Student **)b;
    return s1->semester - s2->semester;
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
            printf(",");
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

static void printAsciiGpaDistribution(void)
{
    int buckets[5] = {0};
    StudentNode *current = head;
    while (current)
    {
        int bucket = (int)(current->student.gpa / 2.0f);
        if (bucket < 0)
            bucket = 0;
        if (bucket > 4)
            bucket = 4;
        buckets[bucket]++;
        current = current->next;
    }
    printf("\nGPA Distribution:\n");
    const char *labels[5] = {"0-1.99", "2.0-3.99", "4.0-5.99", "6.0-7.99", "8.0-10.0"};
    for (int i = 0; i < 5; i++)
    {
        printf("%7s | ", labels[i]);
        for (int j = 0; j < buckets[i]; j++)
            putchar('*');
        printf(" (%d)\n", buckets[i]);
    }
}

static void displayStatisticsReport(void)
{
    if (!head)
    {
        printf("No students available for statistics.\n");
        return;
    }
    int count = 0;
    float totalGpa = 0.0f;
    int passCount = 0;
    StudentNode *current = head;
    Student *topStudent = NULL;
    float semesterSum[12] = {0.0f};
    int semesterCount[12] = {0};

    while (current)
    {
        count++;
        totalGpa += current->student.gpa;
        int failed = 0;
        for (int i = 0; i < COURSE_COUNT; i++)
        {
            if (current->student.courses[i] < 50)
                failed = 1;
        }
        if (!failed)
            passCount++;
        if (!topStudent || current->student.gpa > topStudent->gpa)
            topStudent = &current->student;
        if (current->student.semester >= 1 && current->student.semester <= 12)
        {
            semesterSum[current->student.semester - 1] += current->student.gpa;
            semesterCount[current->student.semester - 1]++;
        }
        current = current->next;
    }

    printf("\nClass Statistics:\n");
    printf("Total students: %d\n", count);
    printf("Class average GPA: %.2f\n", totalGpa / count);
    if (topStudent)
        printf("Top scorer: %s (ID %d) with GPA %.2f\n", topStudent->name, topStudent->id, topStudent->gpa);
    printf("Pass rate: %.2f%%\n", count ? (passCount * 100.0f / count) : 0.0f);
    printf("Per-semester average GPA:\n");
    for (int i = 0; i < 12; i++)
        if (semesterCount[i] > 0)
            printf("  Semester %d: %.2f\n", i + 1, semesterSum[i] / semesterCount[i]);
}

void exportCSV(const char *filename)
{
    if (!filename)
        filename = "students.csv";
    FILE *fp = fopen(filename, "w");
    if (!fp)
    {
        perror("Unable to create CSV file");
        return;
    }
    fprintf(fp, "ID,Name,Semester,GPA,Course1,Course2,Course3,Course4,Course5,Course6,Course7\n");
    StudentNode *current = head;
    while (current)
    {
        Student *student = &current->student;
        fprintf(fp, "%d,%s,%d,%.2f", student->id, student->name, student->semester, student->gpa);
        for (int i = 0; i < COURSE_COUNT; i++)
            fprintf(fp, ",%d", student->courses[i]);
        fprintf(fp, "\n");
        current = current->next;
    }
    fclose(fp);
    printf("Exported %s successfully.\n", filename);
}

int searchByID(int id, Student *outStudent)
{
    BSTNode *found = findBSTNode(idRoot, id);
    if (!found)
        return 0;
    if (outStudent)
        *outStudent = *found->student;
    return 1;
}

int searchByName(const char *query)
{
    if (!query || !head)
        return 0;

    char lowerQuery[50];
    toLowerCopy(query, lowerQuery);
    int found = 0;
    StudentNode *current = head;
    while (current)
    {
        char lowerName[50];
        toLowerCopy(current->student.name, lowerName);
        if (strstr(lowerName, lowerQuery))
        {
            printStudent(&current->student);
            printf("---------------------------------------\n");
            generateGradeReportCard(&current->student);
            found = 1;
        }
        current = current->next;
    }
    return found;
}

void addStudents(void)
{
    Student student;
    if (!safeScanInt("Enter Student ID: ", &student.id, 1, 1000000))
        return;
    if (isDuplicateID(student.id))
    {
        printf("Error: Student ID %d already exists. Please use a unique ID.\n", student.id);
        return;
    }
    if (!safeScanString("Enter Student Name: ", student.name, sizeof(student.name)))
        return;
    if (!safeScanInt("Enter Student Semester: ", &student.semester, 1, 12))
        return;
    for (int i = 0; i < COURSE_COUNT; i++)
    {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "Enter Student Course %d mark (0-100): ", i + 1);
        if (!safeScanInt(prompt, &student.courses[i], 0, 100))
            return;
    }
    student.gpa = calculateGPA(student.courses, COURSE_COUNT);
    StudentNode *node = createStudentNode(&student);
    appendStudentNode(node);
    idRoot = insertBST(idRoot, &node->student);
    Operation op = {.type = OP_ADD, .oldValue = {0}, .newValue = student};
    pushUndo(op);
    saveDatabase();
    logAction("ADD", &student);
    generateGradeReportCard(&student);
    printf("Student added successfully.\n");
}

void displayStudents(void)
{
    if (!head)
    {
        printf("No students available to display.\n");
        return;
    }
    int count = 0;
    StudentNode *current = head;
    while (current)
    {
        count++;
        current = current->next;
    }
    Student **students = malloc(sizeof(Student *) * count);
    if (!students)
    {
        perror("Unable to allocate memory for sorting");
        return;
    }
    current = head;
    for (int i = 0; i < count; i++)
    {
        students[i] = &current->student;
        current = current->next;
    }
    printf("Sort by:\n");
    printf("1. ID\n");
    printf("2. Name\n");
    printf("3. GPA\n");
    printf("4. Semester\n");
    int sortChoice;
    if (!safeScanInt("Select sort order: ", &sortChoice, 1, 4))
    {
        free(students);
        return;
    }
    int orderChoice;
    if (!safeScanInt("1. Ascending  2. Descending\nChoose order: ", &orderChoice, 1, 2))
    {
        free(students);
        return;
    }
    int (*cmp)(const void *, const void *) = compareID;
    switch (sortChoice)
    {
    case 1:
        cmp = compareID;
        break;
    case 2:
        cmp = compareName;
        break;
    case 3:
        cmp = compareGPA;
        break;
    case 4:
        cmp = compareSemester;
        break;
    }
    qsort(students, count, sizeof(Student *), cmp);
    if (orderChoice == 2)
    {
        for (int i = 0; i < count / 2; i++)
        {
            Student *tmp = students[i];
            students[i] = students[count - i - 1];
            students[count - i - 1] = tmp;
        }
    }
    int page = 0;
    int pages = (count + PAGE_SIZE - 1) / PAGE_SIZE;
    while (page < pages)
    {
        int start = page * PAGE_SIZE;
        int end = start + PAGE_SIZE;
        if (end > count)
            end = count;
        printf("\n-------------Student Records (Page %d of %d)-------------\n", page + 1, pages);
        for (int i = start; i < end; i++)
        {
            printStudent(students[i]);
            printf("---------------------------------------\n");
        }
        if (page < pages - 1)
        {
            printf("Press Enter to view next page...");
            getchar();
            page++;
            continue;
        }
        break;
    }
    free(students);
    generateAllReportCards();
    displayStatisticsReport();
    printAsciiGpaDistribution();
    printf("Generated grade report cards for displayed students.\n");
}

void searchStudents(void)
{
    if (!head)
    {
        printf("No students available to search.\n");
        return;
    }
    printf("Search by: 1. ID  2. Name\n");
    int choice;
    if (!safeScanInt("Enter choice: ", &choice, 1, 2))
        return;
    if (choice == 1)
    {
        int id;
        if (!safeScanInt("Enter ID to search: ", &id, 1, 1000000))
            return;
        Student student;
        if (searchByID(id, &student))
        {
            printStudent(&student);
            generateGradeReportCard(&student);
        }
        else
        {
            printf("Student not found.\n");
        }
    }
    else
    {
        char query[50];
        if (!safeScanString("Enter Name to search: ", query, sizeof(query)))
            return;
        if (!searchByName(query))
            printf("Student not found.\n");
    }
}

void updateStudentByID(void)
{
    if (!head)
    {
        printf("No students available to update.\n");
        return;
    }
    int id;
    if (!safeScanInt("Enter ID to update: ", &id, 1, 1000000))
        return;
    BSTNode *found = findBSTNode(idRoot, id);
    if (!found)
    {
        printf("Student not found.\n");
        return;
    }
    Student oldValue = *found->student;
    if (!safeScanString("Enter new name: ", found->student->name, sizeof(found->student->name)))
        return;
    if (!safeScanInt("Enter new semester: ", &found->student->semester, 1, 12))
        return;
    for (int i = 0; i < COURSE_COUNT; i++)
    {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "Enter new marks for course %d (0-100): ", i + 1);
        if (!safeScanInt(prompt, &found->student->courses[i], 0, 100))
            return;
    }
    found->student->gpa = calculateGPA(found->student->courses, COURSE_COUNT);
    Operation op = {.type = OP_UPDATE, .oldValue = oldValue, .newValue = *found->student};
    pushUndo(op);
    saveDatabase();
    logAction("UPDATE", found->student);
    printf("Student updated successfully.\n");
}

void deleteStudentByID(void)
{
    if (!head)
    {
        printf("No students available to delete.\n");
        return;
    }
    int id;
    if (!safeScanInt("Enter ID to delete: ", &id, 1, 1000000))
        return;
    BSTNode *found = findBSTNode(idRoot, id);
    if (!found)
    {
        printf("Student not found.\n");
        return;
    }
    Student oldValue = *found->student;
    if (!deleteStudentNodeByID(id))
    {
        printf("Failed to delete student.\n");
        return;
    }
    Operation op = {.type = OP_DELETE, .oldValue = oldValue, .newValue = {0}};
    pushUndo(op);
    saveDatabase();
    logAction("DELETE", &oldValue);
    printf("Student deleted successfully.\n");
}

void undoLastOperation(void)
{
    if (undoTop == 0)
    {
        printf("Nothing to undo.\n");
        return;
    }
    Operation op = undoStack[--undoTop];
    switch (op.type)
    {
    case OP_ADD:
        deleteStudentNodeByID(op.newValue.id);
        logAction("UNDO_ADD", &op.newValue);
        pushRedo(op);
        break;
    case OP_UPDATE:
    {
        BSTNode *found = findBSTNode(idRoot, op.oldValue.id);
        if (found)
        {
            *found->student = op.oldValue;
            logAction("UNDO_UPDATE", found->student);
            pushRedo(op);
        }
        break;
    }
    case OP_DELETE:
    {
        StudentNode *node = createStudentNode(&op.oldValue);
        appendStudentNode(node);
        idRoot = insertBST(idRoot, &node->student);
        logAction("UNDO_DELETE", &op.oldValue);
        pushRedo(op);
        break;
    }
    }
    saveDatabase();
    printf("Undo completed.\n");
}

void redoLastOperation(void)
{
    if (redoTop == 0)
    {
        printf("Nothing to redo.\n");
        return;
    }
    Operation op = redoStack[--redoTop];
    switch (op.type)
    {
    case OP_ADD:
    {
        StudentNode *node = createStudentNode(&op.newValue);
        appendStudentNode(node);
        idRoot = insertBST(idRoot, &node->student);
        logAction("REDO_ADD", &op.newValue);
        pushUndo(op);
        break;
    }
    case OP_UPDATE:
    {
        BSTNode *found = findBSTNode(idRoot, op.newValue.id);
        if (found)
        {
            *found->student = op.newValue;
            logAction("REDO_UPDATE", found->student);
            pushUndo(op);
        }
        break;
    }
    case OP_DELETE:
        deleteStudentNodeByID(op.oldValue.id);
        logAction("REDO_DELETE", &op.oldValue);
        pushUndo(op);
        break;
    }
    saveDatabase();
    printf("Redo completed.\n");
}
