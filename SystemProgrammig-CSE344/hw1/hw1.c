#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define LOG_FILE "log.txt"
#define MAX_STUDENTS 100
#define MAX_LINE_LENGTH 256
#define BUFFER_SIZE 4096

void logActivity(const char *activity)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
        if (fd == -1) {
            perror("Error opening log file in child");
            exit(EXIT_FAILURE);
        }
        else {
            dprintf(fd, "%s\n",
                    activity);  // Write the activity to the log file
            close(fd);
            exit(EXIT_SUCCESS);
        }
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
            printf("Child process succeeded in logging activity.\n");
        }
        else {
            printf("Child process failed to log activity.\n");
        }
    }
}
void createFile(const char *fileName)
{
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        // Child process: attempt to create the file
        int fd = open(fileName, O_WRONLY | O_CREAT | O_EXCL,
                      0644);  // O_EXCL flag ensures that the file is created
                              // only if it doesn't already exist
        if (fd == -1) {
            perror("Error creating file in child");
            exit(EXIT_FAILURE);
        }
        else {
            printf("File '%s' created successfully.\n", fileName);
            close(fd);
            exit(EXIT_SUCCESS);
        }
    }
    else {
        // Parent process: wait for the child to finish
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
            printf("Child process succeeded in creating file.\n");
            logActivity("File created");
        }
        else {
            printf("Child process failed to create file.\n");
            logActivity("File creation failed");
        }
    }
}

void addStudentGrade(const char *studentName, const char *grade,
                     const char *fileName)
{
    pid_t pid = fork();
    if (pid == -1) {
        // Handle error
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        // Child process
        int fd = open(fileName, O_WRONLY | O_APPEND);
        if (fd == -1) {
            perror("Error opening file in child");
            exit(EXIT_FAILURE);
        }
        else {
            dprintf(fd, "%s, %s\n", studentName, grade);
            close(fd);
            exit(EXIT_SUCCESS);
        }
    }
    else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
            printf("Child process succeeded in adding student grade.\n");
            logActivity("Student grade added");
        }
        else {
            printf("Child process failed to add student grade.\n");
            logActivity("Student grade addition failed");
        }
    }
}
void searchStudent(const char *studentName, const char *fileName)
{
    pid_t pid = fork();
    if (pid == -1) {
        // Handle error
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        // Child process
        int fd = open(fileName, O_RDONLY);
        if (fd == -1) {
            perror("Error opening file in child");
            exit(EXIT_FAILURE);
        }
        else {
            char buffer[4096];  // Adjust buffer size as necessary
            ssize_t bytesRead, bufferPos = 0;
            char tempLine[MAX_LINE_LENGTH];
            int tempLinePos = 0;

            while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
                for (bufferPos = 0; bufferPos < bytesRead; bufferPos++) {
                    if (buffer[bufferPos] == '\n' ||
                        tempLinePos == MAX_LINE_LENGTH - 1) {
                        tempLine[tempLinePos] =
                            '\0';  // Null-terminate the line
                        char name[MAX_LINE_LENGTH], grade[MAX_LINE_LENGTH];
                        sscanf(tempLine, "%[^,], %s", name, grade);
                        if (strcmp(name, studentName) == 0) {
                            printf("%s\n", tempLine);
                        }
                        tempLinePos = 0;  // Reset the temporary line position
                        continue;
                    }
                    tempLine[tempLinePos++] = buffer[bufferPos];
                }
            }

            // Handle the last line if it doesn't end with a newline
            if (tempLinePos > 0) {
                tempLine[tempLinePos] = '\0';
                char name[MAX_LINE_LENGTH], grade[MAX_LINE_LENGTH];
                sscanf(tempLine, "%[^,], %s", name, grade);
                if (strcmp(name, studentName) == 0) {
                    printf("%s\n", tempLine);
                }
            }

            close(fd);
            exit(EXIT_SUCCESS);
        }
    }
    else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
            printf("Child process succeeded in searching student.\n");
            logActivity("Student searched");
        }
        else {
            printf("Child process failed to search student.\n");
            logActivity("Student search failed");
        }
    }
}

void readLines(int fd, char *students[], int *count)
{
    char buffer[4096];  // Adjust buffer size as necessary
    ssize_t bytesRead, bufferPos = 0;
    char tempLine[MAX_LINE_LENGTH];
    int tempLinePos = 0;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (bufferPos = 0; bufferPos < bytesRead; bufferPos++) {
            if (buffer[bufferPos] == '\n' ||
                tempLinePos == MAX_LINE_LENGTH - 1) {
                tempLine[tempLinePos] = '\0';  // Null-terminate the line
                students[*count] =
                    strdup(tempLine);  // Duplicate the line for storage
                (*count)++;
                tempLinePos = 0;  // Reset the temporary line position
                continue;
            }
            tempLine[tempLinePos++] = buffer[bufferPos];
        }
    }

    // Handle the last line if it doesn't end with a newline
    if (tempLinePos > 0) {
        tempLine[tempLinePos] = '\0';
        students[*count] = strdup(tempLine);
        (*count)++;
    }
}
void bubbleSortByName(char *students[], int count)
{
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            // Dynamically allocate memory for names
            char *nameA = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
            char *nameB = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));

            // Check for allocation failure
            if (!nameA || !nameB) {
                perror("Failed to allocate memory for names");
                // Free the allocated memory to avoid leaks before exiting
                free(nameA);  // Safe to call free on NULL
                free(nameB);
                exit(EXIT_FAILURE);
            }

            sscanf(students[j], "%[^,]", nameA);
            sscanf(students[j + 1], "%[^,]", nameB);

            if (strcmp(nameA, nameB) > 0) {
                // Swap if in wrong order
                char *temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }

            // Free the allocated memory for the names after comparison
            free(nameA);
            free(nameB);
        }
    }
}
void bubbleSortByGrade(char *students[], int count)
{
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            // Directly using pointers for grades since they don't need
            // modification
            char *gradeA = strrchr(students[j], ' ') +
                           1;  // Find the last space in the string
            char *gradeB = strrchr(students[j + 1], ' ') + 1;

            // Dynamically allocate memory for names for secondary comparison
            char *nameA = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
            char *nameB = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));

            if (!nameA || !nameB) {  // Check for allocation failure
                perror("Failed to allocate memory for names");
                free(nameA);  // Safe to free NULL if malloc failed for nameB
                free(nameB);
                exit(EXIT_FAILURE);
            }

            sscanf(students[j], "%[^,]", nameA);
            sscanf(students[j + 1], "%[^,]", nameB);

            int gradeComparison = strcmp(gradeA, gradeB);
            if (gradeComparison > 0 ||
                (gradeComparison == 0 && strcmp(nameA, nameB) > 0)) {
                // Swap if grades are in wrong order or if grades are equal and
                // names are in wrong order
                char *temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }

            // Free the dynamically allocated memory for names after comparison
            free(nameA);
            free(nameB);
        }
    }
}
void sortAll(const char *fileName, const char *isName, const char *isGrade)
{
    int fd = open(fileName, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char *students[MAX_STUDENTS];
    int count = 0;
    readLines(fd, students, &count);

    pid_t pid = fork();  // Create a child process

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        if (strcmp(isGrade, "bygrade") == 0) {
            bubbleSortByGrade(students, count);
            logActivity("Sorted by grade");

            // ascending order
            printf("\nSorted by grade ascending order:\n");
            for (int i = 0; i < count; i++) {
                printf("%s\n", students[i]);
            }
            // descending order
            printf("\nSorted by grade descending order:\n");
            for (int i = count - 1; i >= 0; i--) {
                printf("%s\n", students[i]);
            }
        }
        for (int i = 0; i < count; i++) {
            free(students[i]);
        }
        close(fd);
        exit(EXIT_SUCCESS);
    }
    else {
        // Parent process waits for the child to finish
        waitpid(pid, NULL, 0);

        if (strcmp(isName, "byname") == 0) {
            bubbleSortByName(students, count);
            logActivity("Sorted by name");

            // ascending order
            printf("\nSorted by name ascending order:\n");
            for (int i = 0; i < count; i++) {
                printf("%s\n", students[i]);
            }
            // descending order
            printf("\nSorted by name descending order :\n");
            for (int i = count - 1; i >= 0; i--) {
                printf("%s\n", students[i]);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(students[i]);
    }
    close(fd);
}
void showAll(const char *fileName)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        int fd = open(fileName, O_RDONLY);
        if (fd == -1) {
            perror("Error opening file in child");
            exit(EXIT_FAILURE);
        }
        else {
            char buffer[BUFFER_SIZE];
            ssize_t bytesRead;
            while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
                write(STDOUT_FILENO, buffer, bytesRead);
            }
            close(fd);
            exit(EXIT_SUCCESS);
        }
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
            printf("Child process succeeded in showing all entries.\n");
            logActivity("All entries shown");
        }
        else {
            printf("Child process failed to show all entries.\n");
            logActivity("Failed to show all entries");
        }
    }
}
void listGrades(const char *fileName)
{
    pid_t pid = fork();

    int show = 0;
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        int fd = open(fileName, O_RDONLY);
        if (fd == -1) {
            perror("Error opening file in child");
            exit(EXIT_FAILURE);
        }
        else {
            char *students[MAX_STUDENTS];
            int count = 0;
            readLines(fd, students, &count);

            if (count >
                5) {  // If there are more than 5 entries, only list the first 5
                show = 5;
            }

            for (int i = 0; i < show; i++) {
                char *name = strtok(students[i], ",");
                char *grade = strtok(NULL, ",");
                printf("%s: %s\n", name, grade);
            }

            // Free the memory allocated for each line
            for (int i = 0; i < count; i++) {
                free(students[i]);
            }

            close(fd);
            exit(EXIT_SUCCESS);
        }
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
            printf("Child process succeeded in listing grades.\n");
            logActivity("Grades listed");
        }
        else {
            printf("Child process failed to list grades.\n");
            logActivity("Failed to list grades");
        }
    }
}

void listSome(int numOfEntries, int pageNumber, const char *fileName)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        int fd = open(fileName, O_RDONLY);
        if (fd == -1) {
            perror("Error opening file in child");
            exit(EXIT_FAILURE);
        }
        else {
            char *students[MAX_STUDENTS];
            int count = 0;
            readLines(fd, students, &count);

            int start = (pageNumber - 1) * numOfEntries;
            int end = pageNumber * numOfEntries;
            if (end > count) {
                end = count;
            }

            for (int i = start; i < end; i++) {
                printf("%s\n", students[i]);
            }

            // Free the memory allocated for each line
            for (int i = 0; i < count; i++) {
                free(students[i]);
            }

            close(fd);
            exit(EXIT_SUCCESS);
        }
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
            printf("Child process succeeded in listing some entries.\n");
            logActivity("Some entries listed");
        }
        else {
            printf("Child process failed to list some entries.\n");
            logActivity("Failed to list some entries");
        }
    }
}

void displayUsage()
{
    printf("Commands:\n");
    printf("  gtuStudentGrades \"<fileName>\" \n");
    printf("  addStudentGrade \"<studentName>\" \"<grade>\" \"<fileName>\"\n");
    printf("  searchStudent \"<studentName>\" \"<fileName>\"\n");
    printf("  sortAll \"<fileName>\" \"<byname>\" \"<bygrade>\"\n");
    printf("  showAll \"<fileName>\"\n");
    printf("  listGrades \"<fileName>\"\n");
    printf("  listSome <numOfEntries> <pageNumber> \"<fileName>\"\n");
    printf("  exit\n\n");
}

void readUserInput_createFile(const char *input)
{
    char fileName[MAX_LINE_LENGTH] = {0};
    char command[MAX_LINE_LENGTH] = {0};

    // Attempt to parse input assuming filename is enclosed in quotes
    if (sscanf(input, "%s \"%[^\"]\"", command, fileName) == 2) {
        // Filename parsed successfully with quotes
        createFile(fileName);
    }
    // Attempt to parse input without assuming quotes
    else if (sscanf(input, "%s %s", command, fileName) == 2) {
        // Handle case without quotes, directly attempt to create file
        createFile(fileName);
    }
    else {
        printf("Invalid input. Usage: createFile \"<fileName>\"\n");
    }
}

void readUserInput_addStudentGrade(const char *input)
{
    char command[MAX_LINE_LENGTH] = {0};
    char studentName[MAX_LINE_LENGTH] = {0};
    char grade[MAX_LINE_LENGTH] = {0};
    char fileName[MAX_LINE_LENGTH] = {0};

    if (sscanf(input, "%s \"%[^\"]\" \"%[^\"]\" \"%[^\"]\"",
               command,  // format is like "addStudentGrade "studentName"
                         // "grade" "fileName"
               studentName, grade, fileName) != 4) {
        printf(
            "Invalid input. Usage: addStudentGrade \"<studentName>\" "
            "\"<grade>\" \"<fileName>\"\n");
        return;
    }

    addStudentGrade(studentName, grade, fileName);
}
void readUserInput_searchStudent(const char *input)
{
    char command[MAX_LINE_LENGTH] = {0};
    char studentName[MAX_LINE_LENGTH] = {0};
    char fileName[MAX_LINE_LENGTH] = {0};

    if (sscanf(input, "%s \"%[^\"]\" \"%[^\"]\"", command, studentName,
               fileName) !=
        3) {  // format is like "searchStudent "studentName" "fileName""
        printf(
            "Invalid input. Usage: searchStudent \"<studentName>\" "
            "\"<fileName>\"\n");
        return;
    }
    searchStudent(studentName, fileName);
}
void readUserInput_sortAll(const char *input)
{
    char command[MAX_LINE_LENGTH] = {0};
    char fileName[MAX_LINE_LENGTH] = {0};
    char isName[MAX_LINE_LENGTH] = {0};
    char isGrade[MAX_LINE_LENGTH] = {0};

    if (sscanf(input, "%s \"%[^\"]\" \"%[^\"]\" \"%[^\"]\"", command, fileName,
               isName, isGrade) != 4) {
        printf(
            "Invalid input. Usage: sortAll \"<fileName>\" \"<isName>\" "
            "\"<isGrade>\"\n");
        return;
    }

    sortAll(fileName, isName, isGrade);
}
void readUserInput_showAll(const char *input)
{
    char command[MAX_LINE_LENGTH] = {0};
    char fileName[MAX_LINE_LENGTH] = {0};

    if (sscanf(input, "%s \"%[^\"]\"", command, fileName) != 2) {
        printf("Invalid input. Usage: showAll \"<fileName>\"\n");
        return;
    }

    showAll(fileName);
}
void readUserInput_listGrades(const char *input)
{
    char command[MAX_LINE_LENGTH] = {0};
    char fileName[MAX_LINE_LENGTH] = {0};

    if (sscanf(input, "%s \"%[^\"]\"", command, fileName) != 2) {
        printf("Invalid input. Usage: listGrades \"<fileName>\"\n");
        return;
    }

    listGrades(fileName);
}
void readUserInput_listSome(const char *input)
{
    char command[MAX_LINE_LENGTH] = {0};
    int numOfEntries = 0;
    int pageNumber = 0;
    char fileName[MAX_LINE_LENGTH] = {0};

    if (sscanf(input, "%s %d %d \"%[^\"]\"", command, &numOfEntries,
               &pageNumber, fileName) != 4) {
        printf(
            "Invalid input. Usage: listSome <numOfEntries> <pageNumber> "
            "\"<fileName>\"\n");
        return;
    }
    listSome(numOfEntries, pageNumber, fileName);
}

char *getFirstWord(const char *input)
{
    char *firstWord = malloc(MAX_LINE_LENGTH);
    if (firstWord == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    if (sscanf(input, "%s", firstWord) != 1) {
        printf("Invalid input. Usage: createFile <fileName>\n");
        free(firstWord);
        return NULL;
    }
    return firstWord;
}

int main()
{
    char input[MAX_LINE_LENGTH] = {0};

    while (1) {
        printf("Enter command: ");
        fflush(stdout);
        ssize_t bytesRead = read(STDIN_FILENO, input, MAX_LINE_LENGTH - 1);
        if (bytesRead > 0) {
            input[bytesRead - 1] =
                '\0';  // Replace newline with null terminator
        }
        else if (bytesRead == 0) {
            printf("No input read. Exiting.\n");
            exit(EXIT_FAILURE);
        }
        else {
            perror("read");
            exit(EXIT_FAILURE);
        }

        char *firstWord = getFirstWord(input);
        if (firstWord == NULL) {
            continue;  // If getFirstWord returns NULL, we still loop again
        }

        if (strcmp(firstWord, "gtuStudentGrades") == 0) {
            if (strcmp(input, "gtuStudentGrades") == 0) {
                displayUsage();
            }
            else {
                readUserInput_createFile(input);
            }
        }
        else if (strcmp(firstWord, "addStudentGrade") == 0) {
            readUserInput_addStudentGrade(input);
        }
        else if (strcmp(firstWord, "searchStudent") == 0) {
            readUserInput_searchStudent(input);
        }
        else if (strcmp(firstWord, "sortAll") == 0) {
            readUserInput_sortAll(input);
        }
        else if (strcmp(firstWord, "showAll") == 0) {
            readUserInput_showAll(input);
        }
        else if (strcmp(firstWord, "listGrades") == 0) {
            readUserInput_listGrades(input);
        }
        else if (strcmp(firstWord, "listSome") == 0) {
            readUserInput_listSome(input);
        }
        else if (strcmp(firstWord, "exit") == 0) {
            free(firstWord);  // Free memory before exiting
            break;
        }
        else {
            printf("Invalid command. Please try again.\n");
        }

        free(firstWord);
    }

    return 0;
}
