#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define SERVER_FIFO "/tmp/server.%ld"
#define CLIENT_FIFO_TEMPLATE "/tmp/client.%ld"
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)
#define SERVER_FIFO_NAME_LEN (sizeof(SERVER_FIFO) + 20)
#define BUFFER_SIZE 4096
#define BUF_SIZE 1024
enum RequestType {
    CONNECT = 0,
    TRY_CONNECT,
    QUIT,
    KILLSERVER,
    HELP,
    LIST,
    READF,
    WRITET,
    UPLOAD,
    DOWNLOAD,
    ARCHSERVER,
    UNKNOWN
};
struct request {
    pid_t pid;
    char buffer[BUFFER_SIZE];
    int type;
    int fd;
    int readingStarted;
    int lineNumber;
    int writingStarted;
    int clientId;
    int uploadContinues;
    char filePath[1024];
    char tar_name[128];
};
struct response {
    int clientId;
    char buffer[BUFFER_SIZE];
    int number;
    int fd;
    int connected;
    int writingFinished;
    int readingFinished;
    char directoryPath[1024];
};
int getInt(char *arg, const char *name)
{
    char buf[BUF_SIZE];
    char *endptr;
    int res;
    snprintf(buf, BUF_SIZE, "%s", arg);
    errno = 0;
    res = strtol(buf, &endptr, 10);
    if (errno != 0 || *endptr != '\0' || endptr == buf) {
        fprintf(stderr, "Invalid argument: %s\n", name);
        exit(EXIT_FAILURE);
    }
    if (res <= 0) {
        fprintf(stderr, "%s  must be greater than 0\n", name);
        exit(EXIT_FAILURE);
    }
    return res;
}
int isInteger(char *str)
{
    if (*str == '\0') {
        return 0;
    }
    while (*str != '\0') {
        if (!isdigit((unsigned char)*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}
sem_t *free_place_num;
sem_t *occupied_place_num;
sem_t *connection_requested;
struct request req;
struct response resp;
char clientFifo[CLIENT_FIFO_NAME_LEN];
void client_send_request(const char *fifo_name, struct request *req)
{
    int fd = open(fifo_name, O_WRONLY);
    if (fd == -1) {
        perror("open error in client_send_request");
        exit(EXIT_FAILURE);
    }
    write(fd, req, sizeof(struct request));
    close(fd);
}
void client_receive_response(const char *fifo_name, struct response *resp)
{
    int fd = open(fifo_name, O_RDONLY);
    if (fd == -1) {
        perror("open error in client_receive_response");
        exit(EXIT_FAILURE);
    }
    read(fd, resp, sizeof(struct response));
    close(fd);
}
void sigIntHandler(int signum)
{
    req.type = QUIT;
    client_send_request(clientFifo, &req);
    sem_post(free_place_num);  // Increase the empty slot for clients
    exit(EXIT_SUCCESS);
}
void sigTermHandler(int signum)
{
    printf("Server is terminated...\n");
    sem_post(free_place_num);
    exit(EXIT_SUCCESS);
}
enum RequestType getRequestType(const char *str)
{
    if (strcmp(str, "Connect") == 0) {
        return CONNECT;
    }
    else if (strcmp(str, "tryConnect") == 0) {
        return TRY_CONNECT;
    }
    else if (strcmp(str, "help") == 0) {
        return HELP;
    }
    else if (strcmp(str, "list") == 0) {
        return LIST;
    }
    else if (strcmp(str, "readF") == 0) {
        return READF;
    }
    else if (strcmp(str, "writeT") == 0) {
        return WRITET;
    }
    else if (strcmp(str, "upload") == 0) {
        return UPLOAD;
    }
    else if (strcmp(str, "download") == 0) {
        return DOWNLOAD;
    }
    else if (strcmp(str, "archServer") == 0) {
        return ARCHSERVER;
    }
    else if (strcmp(str, "quit") == 0) {
        return QUIT;
    }
    else if (strcmp(str, "killServer") == 0) {
        return KILLSERVER;
    }
    else {
        return UNKNOWN;
    }
}
void printUsage(char *argv[])
{
    printf("Usage: %s <Connect/tryConnect> ServerPID\n", argv[0]);
    exit(EXIT_FAILURE);
}
void printHelp(const char *command)
{
    if (strcmp(command, "list") == 0) {
        printf("\tlist: List the files in the server directory.\n");
    }
    else if (strcmp(command, "readF") == 0) {
        printf("\treadF <file> <line#>: Read the given line from the file.\n");
    }
    else if (strcmp(command, "writeT") == 0) {
        printf(
            "\twriteT <file> <line#> <string>: Write the given string to the "
            "given line in the file.\n");
    }
    else if (strcmp(command, "upload") == 0) {
        printf("\tupload <file>: Upload the given file to the server.\n");
    }
    else if (strcmp(command, "download") == 0) {
        printf("\tdownload <file>: Download the given file from the server.\n");
    }
    else if (strcmp(command, "archServer") == 0) {
        printf("\tarchServer <tar_name>: Archive the server directory.\n");
    }
    else if (strcmp(command, "quit") == 0) {
        printf("\tquit: Quit the client.\n");
    }
    else if (strcmp(command, "killServer") == 0) {
        printf("\tkillServer: Kill the server.\n");
    }
    else {
        // Print all the possible commands
        printf(
            "\tlist\n\treadF <file> <line#>\n\twriteT <file> <line#> "
            "<string>\n\tupload <file>\n\tdownload <file>\n\tquit\n\t"
            "killServer\n");
    }
}
int main(int argc, char *argv[])
{
    char serverFifo[SERVER_FIFO_NAME_LEN];
    int clientNumber;
    size_t direct_path_size;
    char *direct_path_arr;
    char currentDirectory[1024];
    char serverDirectory[1024];

    getcwd(currentDirectory, sizeof(currentDirectory));

    free_place_num = sem_open("/free_place_num", O_CREAT, 0644,
                              -1);  // doesnt changes the value of the semaphore
    occupied_place_num = sem_open("/occupied_place_num", O_CREAT, 0644, -1);

    connection_requested = sem_open("/connection_requested", O_CREAT, 0644, 1);

    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printHelp("help");
        exit(EXIT_SUCCESS);
    }

    if (argc < 3) {
        printUsage(argv);
    }

    struct sigaction sa_int;
    sa_int.sa_handler = sigIntHandler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa_term;
    sa_term.sa_handler = sigTermHandler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = SA_RESTART;

    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    enum RequestType connectionType = getRequestType(
        argv[1]);  // Get the connection type from the command line argument

    int serverPid = getInt(argv[2], "ServerPID");
    snprintf(serverFifo, SERVER_FIFO_NAME_LEN, SERVER_FIFO, (long)serverPid);

    umask(0);
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
             (long)getpid());  // Create the client fifo before sending the
                               // connection request as lecture notes
    if (mkfifo(clientFifo, 0666) == -1) {
        printf("mkfifo %s\n", clientFifo);
    }

    req.pid = getpid();
    req.type = connectionType;  // CONNECT or TRY_CONNECT

    sem_post(occupied_place_num);  // Increase the total connected clients count
    sem_getvalue(occupied_place_num, &clientNumber);  // Get the client number
    printf("I am client number : %d . \n", clientNumber);

    req.clientId = clientNumber;  // Set the client number to the request
    client_send_request(serverFifo,
                        &req);  // Send the connection request to the server
    client_receive_response(clientFifo,
                            &resp);  // Receive the response from the server

    // print connection_requested
    int connectionRequestSentValue;
    sem_getvalue(connection_requested, &connectionRequestSentValue);
    // printf(" connection_requested : %d\n", connectionRequestSentValue);

    int emptySlotAvailable;
    sem_getvalue(free_place_num, &emptySlotAvailable);
    // printf(" emptySlotForClients123213 : %d\n", emptySlotAvailable);

    if ((connectionType == CONNECT) &&
        (resp.connected == 0)) {  // If the connection type is CONNECT and the
                                  // server is not connected to the client

        while (1) {
            sem_getvalue(free_place_num, &emptySlotAvailable);
            if (emptySlotAvailable >= 1) {
                sem_wait(connection_requested);  // it decrements the semaphore
                                                 // value by 1 and if the value
                                                 // is less than 0, it blocks
                                                 // the process until the value
                                                 // is greater than 0

                client_send_request(serverFifo, &req);
                client_receive_response(clientFifo, &resp);

                sem_post(connection_requested);  // it increments the semaphore
                                                 // value by 1
                if (resp.connected == 1) {
                    strcpy(serverDirectory, resp.directoryPath);
                    break;
                }
                else {  // it should get here because of sem_wait
                    printf(
                        "error:  client - CONNECT .  \n");  // bazen buraya
                                                            // geliyor  TODO çöz
                }
            }
        }
    }

    if ((connectionType == TRY_CONNECT && resp.connected == 0) == 0) {
        sem_wait(free_place_num);  // it decrements the semaphore value by 1 and
                                   // if the value is less than 0, it blocks the
                                   // process until the value is greater than 0

        while (1) {
            char input[BUF_SIZE];
            char *commands[5];
            int commandIndex = 0;
            printf(">> Enter command: ");
            fgets(input, BUF_SIZE, stdin);
            input[strcspn(input, "\n")] = 0;

            char *token = strtok(input, " ");
            while (token != NULL) {
                commands[commandIndex] = token;
                commandIndex++;
                token = strtok(NULL, " ");
            }
            enum RequestType requestType = getRequestType(commands[0]);
            req.type = requestType;
            req.clientId = clientNumber;

            switch (requestType) {
                case HELP:
                    if (commandIndex >= 2) {
                        printHelp(commands[1]);
                    }
                    else {
                        printf("\tPossible list of client requests:\n");
                        printf(
                            "\tlist\n\treadF <file> <line#>\n\twriteT <file> "
                            "<line#> <string>\n\tupload <file>\n\tdownload "
                            "<file>\n\tquit\n\tkillServer\n");
                    }
                    break;
                case LIST:
                    client_send_request(clientFifo, &req);
                    client_receive_response(clientFifo, &resp);
                    printf("%s\n", resp.buffer);
                    break;
                case QUIT:
                    client_send_request(clientFifo, &req);
                    sem_post(free_place_num);
                    exit(EXIT_SUCCESS);
                    break;
                case READF:
                    if (commandIndex == 2) {
                        strcpy(req.filePath, commands[1]);
                        req.lineNumber = -1;
                        resp.readingFinished = 0;
                        client_send_request(clientFifo, &req);
                        while (1) {
                            client_receive_response(clientFifo, &resp);
                            if (resp.readingFinished == 1) {
                                break;
                            }
                            printf("%s\n", resp.buffer);
                        }
                    }

                    break;
                case WRITET:
                    memset(req.buffer, 0, sizeof(req.buffer));
                    resp.writingFinished = 0;
                    if (isInteger(commands[2])) {
                        // Line is given
                        req.lineNumber = atoi(commands[2]);
                        strcpy(req.buffer, commands[3]);
                    }
                    else {
                        // Line is not given
                        req.lineNumber = -1;
                        strcpy(req.buffer, commands[2]);
                    }
                    strcpy(req.filePath, commands[1]);
                    client_send_request(clientFifo, &req);
                    client_receive_response(clientFifo, &resp);
                    if (resp.writingFinished) {
                        printf("Writing finished.\n");
                    }
                    else {
                        printf("Could not write.\n");
                    }

                    break;
                case DOWNLOAD:
                    direct_path_size =
                        strlen(currentDirectory) + 1 + strlen(commands[1]) + 1;
                    direct_path_arr = malloc(direct_path_size * sizeof(char));
                    snprintf(direct_path_arr, direct_path_size, "%s/%s",
                             currentDirectory, commands[1]);

                    int fileDescriptor =
                        open(direct_path_arr, O_WRONLY | O_CREAT, 0644);
                    if (fileDescriptor == -1) {
                        printf("Failed to open the file.\n");
                        return 1;
                    }
                    if (lseek(fileDescriptor, 0, SEEK_END) != 0) {
                        if (ftruncate(fileDescriptor, 0) == -1) {
                            printf("Failed to clear the file.\n");
                            close(fileDescriptor);
                            return 1;
                        }
                        printf(
                            "The file is exist. It is replaced with the "
                            "downloaded file.\n");
                    }
                    strcpy(req.filePath, commands[1]);
                    client_send_request(clientFifo, &req);
                    printf("\tDownload started...\n");
                    while (1) {
                        client_receive_response(clientFifo, &resp);
                        if (resp.readingFinished == 1) {
                            printf("Download finished...\n");
                            break;
                        }
                        if (write(fileDescriptor, resp.buffer,
                                  strlen(resp.buffer)) == -1) {
                            printf("Error at writing to the file.\n");
                        }
                    }
                    free(direct_path_arr);
                    close(fileDescriptor);
                    break;

                case KILLSERVER:
                    client_send_request(serverFifo, &req);
                    break;

                case UPLOAD:
                    printf("Upload started\n");
                    direct_path_size =
                        strlen(currentDirectory) + 1 + strlen(commands[1]) + 1;
                    direct_path_arr = malloc(direct_path_size * sizeof(char));
                    snprintf(direct_path_arr, direct_path_size, "%s/%s",
                             currentDirectory, commands[1]);
                    strcpy(req.filePath, commands[1]);
                    // Open the file on the client side in read mode
                    FILE *uploadFile = fopen(direct_path_arr, "r");
                    if (uploadFile == NULL) {
                        printf("Failed to open the file for upload.\n");
                        break;
                    }
                    req.uploadContinues = 0;
                    while (fgets(req.buffer, BUFFER_SIZE, uploadFile) != NULL) {
                        req.type = UPLOAD;
                        req.clientId = clientNumber;
                        client_send_request(clientFifo, &req);
                        req.uploadContinues = 1;
                    }

                    fclose(uploadFile);
                    free(direct_path_arr);
                    printf("Upload finished\n");
                    break;
                case ARCHSERVER:

                    // assign the tar_name to the request
                    direct_path_size =
                        strlen(currentDirectory) + 1 + strlen(commands[1]) + 1;
                    direct_path_arr = malloc(direct_path_size * sizeof(char));
                    snprintf(direct_path_arr, direct_path_size, "%s/%s",
                             currentDirectory, commands[1]);
                    strcpy(req.filePath, commands[1]);
                    client_send_request(clientFifo, &req);
                    printf("Archive started...\n");
            }
        }
    }
    else {
        printf(
            "Could not connect to the server... The queue is fullasdasdsad "
            ".\n");
    }

    exit(0);
}