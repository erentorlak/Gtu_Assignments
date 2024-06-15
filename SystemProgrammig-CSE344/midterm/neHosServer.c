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
int nextClientPid;
pid_t *childPids;
int currentIndexChildPids = 0;
FILE *log_fp;
sem_t *free_place_num;
sem_t *occupied_place_num;
sem_t *connection_requested;
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
typedef struct mynode {
    int data;
    struct mynode *next;
} mynode;
typedef struct {
    mynode *front;
    mynode *rear;
} Queue;
Queue *createQueue()
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}
int isEmpty(Queue *queue) { return queue->front == NULL; }
void add(Queue *queue, int data)
{
    mynode *newNode = (mynode *)malloc(sizeof(mynode));
    newNode->data = data;
    newNode->next = NULL;
    if (isEmpty(queue)) {
        queue->front = newNode;
        queue->rear = newNode;
    }
    else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}
int dequeue(Queue *queue)
{
    mynode *temp = queue->front;
    int data = temp->data;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    return data;
}
int peek(Queue *queue) { return queue->front->data; }
int contains(Queue *queue, int element)
{
    mynode *current = queue->front;
    while (current != NULL) {
        if (current->data == element) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}
void server_send_response(const char *fifo_name, struct response *resp)
{
    int fd = open(fifo_name, O_WRONLY);
    if (fd == -1) {
        perror("server send open error");
        exit(EXIT_FAILURE);
    }
    write(fd, resp, sizeof(struct response));
    fprintf(log_fp, " Server sent a response to a client. \n");
    close(fd);
}
void get_connection_request(const char *fifo_name, struct request *req)
{
    int fd = open(fifo_name, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    read(fd, req, sizeof(struct request));
    fprintf(log_fp, "Server received a request from a client...\n");
    close(fd);
}
void sigChildHandler(int signum)
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) >
           0) {  // Wait for the child process to terminate
        int foundIndex = -1;
        for (int i = 0; i < currentIndexChildPids; i++) {
            if (childPids[i] == pid) {
                foundIndex = i;
                break;
            }
        }
        if (foundIndex != -1) {
            // Shift elements to the left
            for (int i = foundIndex; i < currentIndexChildPids - 1;
                 i++) {  // Remove the terminated child PID from the childPids
                // array
                childPids[i] = childPids[i + 1];
            }
            currentIndexChildPids--;
        }
    }
    fprintf(log_fp, "Child process with pid %d terminated.\n", pid);
    printf("Child process with pid %d terminated.\n", pid);
}
int lock_file(FILE *fp)
{
    int file = fileno(fp);
    struct flock lock;
    lock.l_len = 0;
    lock.l_start = 0;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_pid = getpid();
    fcntl(file, F_SETLKW, &lock);
    lock.l_len = 0;
    lock.l_start = 0;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_pid = getpid();
    fcntl(file, F_SETLK, &lock);

    return 0;
}
int unlock_file(FILE *fp)
{
    int file = fileno(fp);
    struct flock lock;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_pid = getpid();
    fcntl(file, F_SETLKW, &lock);
    fcntl(file, F_SETLK, &lock);
    return 0;
}
Queue *queue;
void sigIntHandler(int signum)
{
    for (int i = 0; i < currentIndexChildPids; i++) {
        if (kill(childPids[i], SIGTERM) == -1) {
            fprintf(log_fp, "Failed to send kill signal to pid %d\n",
                    childPids[i]);
            printf("Failed to send kill signal to pid %d\n", childPids[i]);
        }
    }
    fprintf(log_fp, "bye...\n");
    printf("bye...\n");
    sem_close(free_place_num);
    sem_unlink("/free_place_num");
    sem_close(occupied_place_num);
    sem_unlink("/occupied_place_num");
    fclose(log_fp);
    free(queue);
    free(childPids);
    exit(EXIT_SUCCESS);
}
int main(int argc, char *argv[])
{
    // if already created unlink
    sem_unlink("/free_place_num");
    sem_unlink("/occupied_place_num");
    sem_unlink("/connection_requested");
    char serverFifo[SERVER_FIFO_NAME_LEN];
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    int maxClients;
    int free_place;
    size_t direct_path_size;
    char *direct_path_arr;
    struct request req;
    struct response resp;

    char log_file[300];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <dirname> <maximum number of Clients>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *dirname = argv[1];
    maxClients = atoi(argv[2]);

    pid_t clients[maxClients];  // Array to store client PIDs
    for (int i = 0; i < maxClients; i++) {
        clients[i] = -1;  // Initialize the array with -1 to indicate that the
                          // place is empty
    }

    childPids = (pid_t *)malloc(maxClients *
                                sizeof(pid_t));  // Array to store child PIDs

    snprintf(serverFifo, SERVER_FIFO_NAME_LEN, SERVER_FIFO,
             (long)getpid());  // Create the server FIFO name
    umask(0);
    if (mkfifo(serverFifo, 0666) == -1 && errno != EEXIST)
        printf("mkfifo %s", serverFifo);

    printf("Server PID: %d\n", getpid());

    if (mkdir(dirname, 0777) ==
        -1) {  // Create the directory if it does not exist
        printf("Directory exists\n");
    }
    DIR *directory = opendir(dirname);

    snprintf(log_file, sizeof(log_file), "%s/server%d.log", dirname,
             getpid());  // Create the log file name
    log_fp = fopen(log_file, "w");

    printf("logfile: %s created...\n", log_file);

    fprintf(log_fp, "Max clients = %d\n", maxClients);
    fprintf(log_fp, "Server PID : %d\n", getpid());

    struct sigaction sa_chld;              // Signal handler for SIGCHLD
    sa_chld.sa_handler = sigChildHandler;  // Assign the signal handler function
    sigemptyset(&sa_chld.sa_mask);         // Initialize the signal set
    sa_chld.sa_flags =
        SA_RESTART;  // Restart the system call if it is interrupted by a signal

    if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa_int;
    sa_int.sa_handler = sigIntHandler;  // Assign the signal handler function
                                        // for SIGINT (Ctrl+C)
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    free_place_num = sem_open(
        "/free_place_num", O_CREAT, 0644,
        maxClients);  // Create a semaphore to keep track of the empty slots
    occupied_place_num = sem_open(
        "/occupied_place_num", O_CREAT, 0644,
        0);  // Create a semaphore to keep track of the total connected clients

    printf("Waiting for clients...\n");
    fprintf(log_fp, "Waiting for clients...\n");

    queue = createQueue();

    while (1) {
        get_connection_request(
            serverFifo, &req);  // Receive a connection request from a client

        sem_getvalue(free_place_num,
                     &free_place);  // how many empty slots are there

        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
                 (long)req.pid);  // already created from client side

        if (req.type == KILLSERVER) {
            for (int i = 0; i < currentIndexChildPids; i++) {
                if (kill(childPids[i], SIGTERM) == -1) {
                    fprintf(log_fp, "Failed to send kill signal to pid %d\n",
                            childPids[i]);
                    printf("Failed to send kill signal to pid %d\n",
                           childPids[i]);
                }
            }
            fprintf(log_fp, "bye...\n");
            printf("bye...\n");
            sem_close(free_place_num);
            sem_close(occupied_place_num);
            sem_unlink("/free_place_num");
            sem_unlink("/occupied_place_num");
            fclose(log_fp);
            free(queue);
            free(childPids);
            exit(EXIT_SUCCESS);
        }

        if (isEmpty(queue)) {  // If the front of the queue is empty
            nextClientPid = req.pid;
        }
        if (free_place == 0) {  // If there is no empty place

            if (!contains(queue,
                          req.pid)) {  // If the queue does not contain the PID
                fprintf(log_fp, "Connection request PID %ld... Queue FULL\n",
                        (long)req.pid);
                printf("Connection request PID %ld... Queue FULL\n",
                       (long)req.pid);
                add(queue, req.pid);  // Add the PID to the queue to handle it
                                      // later when a place is available
                childPids[currentIndexChildPids++] =
                    req.pid;  // Add the PID to the childPids array
            }
            resp.connected = 0;  // not connected
            resp.clientId = req.clientId;
            server_send_response(clientFifo, &resp);
            continue;
        }
        else if (free_place >= 1 &&
                 nextClientPid ==
                     req.pid) {  // If there is an empty place and the next PID
                                 // is equal to the received PID
            int removed = -1;

            if (!isEmpty(queue)) {  // If the queue is not empty
                removed = dequeue(queue);
                if (!isEmpty(queue)) {
                    nextClientPid =
                        peek(queue);  // Get the next PID from the queue
                }
            }
            if (removed != -1) {  // If a client is removed from the queue
                int foundIndex = -1;
                for (int i = 0; i < currentIndexChildPids; i++) {
                    if (childPids[i] ==
                        removed) {  // Find the index of the removed PID in the
                                    // childPids array
                        foundIndex = i;
                        break;
                    }
                }
                if (foundIndex != -1) {  // If the removed PID is found
                    for (int i = foundIndex; i < currentIndexChildPids - 1;
                         i++) {  // Shift elements to the left
                        childPids[i] = childPids[i + 1];
                    }
                    currentIndexChildPids--;
                }
            }

            resp.connected = 1;
            resp.clientId = req.clientId;
            strcpy(resp.directoryPath, dirname);
            server_send_response(clientFifo, &resp);
        }

        childPids[currentIndexChildPids] =
            req.pid;  // Add the PID to the childPids array.
        currentIndexChildPids++;

        pid_t childPid = fork();

        if (childPid == 0) {  // Child process
            struct request reqClient;
            struct response respClient;

            snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
                     (long)req.pid);

            for (int i = 0; i < maxClients;
                 i++) {  // Find the empty place and assign the PID to the place
                if (clients[i] == -1) {
                    clients[i] = req.pid;
                }
            }

            fprintf(log_fp, ">> Client PID %ld connected as client%02d\n",
                    (long)req.pid, req.clientId);
            printf(">> Client PID %ld connected as client%02d\n", (long)req.pid,
                   req.clientId);

            while (1) {
                get_connection_request(
                    clientFifo,
                    &reqClient);  // Receive a request from the client
                switch (reqClient.type) {
                    case QUIT:
                        for (int i = 0; i < maxClients; i++) {
                            if (clients[i] == reqClient.pid) {
                                clients[i] = -1;
                            }
                        }
                        int foundIndex = -1;
                        for (int i = 0; i < currentIndexChildPids; i++) {
                            if (childPids[i] == reqClient.pid) {
                                foundIndex = i;
                                break;
                            }
                        }
                        if (foundIndex != -1) {
                            // Shift elements to the left
                            for (int i = foundIndex;
                                 i < currentIndexChildPids - 1; i++) {
                                childPids[i] = childPids[i + 1];
                            }
                            currentIndexChildPids--;
                        }

                        fprintf(log_fp, ">> client%02d disconnected..\n",
                                reqClient.clientId);
                        printf(">> client%02d disconnected..\n",
                               reqClient.clientId);
                        exit(EXIT_SUCCESS);
                    case LIST:
                        struct dirent *entry;
                        struct stat fileStat;
                        char contentBuf[BUFFER_SIZE] = "";

                        DIR *directory = opendir(dirname);
                        if (directory == NULL) {
                            fprintf(log_fp, "Error at opening directory..\n");
                            break;
                        }

                        while ((entry = readdir(directory)) != NULL) {
                            if (strcmp(entry->d_name, ".") == 0 ||
                                strcmp(entry->d_name, "..") == 0) {
                                continue;
                            }
                            char temp[1024];
                            snprintf(temp, 1024, "%s\n", entry->d_name);
                            strcat(contentBuf, temp);
                        }
                        fprintf(log_fp,
                                "List of files in the directory sent to "
                                "client%02d\n",
                                reqClient.clientId);
                        closedir(directory);

                        strcpy(respClient.buffer, contentBuf);
                        respClient.number = 0;
                        server_send_response(clientFifo, &respClient);
                        break;

                    case READF:

                        direct_path_size = strlen(dirname) + 1 +
                                           strlen(reqClient.filePath) + 1;
                        direct_path_arr =
                            malloc(direct_path_size * sizeof(char));

                        snprintf(direct_path_arr, direct_path_size, "%s/%s",
                                 dirname,
                                 reqClient.filePath);  // Create the path of the
                                                       // file to read

                        int readFd = open(direct_path_arr, O_RDONLY, 0666);
                        ssize_t bytesRead;
                        respClient.readingFinished = 0;

                        while (1) {
                            bytesRead = read(readFd, respClient.buffer,
                                             BUFFER_SIZE - 1);
                            if (bytesRead <= 0) {
                                respClient.fd = bytesRead;
                                respClient.readingFinished = 1;
                            }
                            server_send_response(clientFifo, &respClient);

                            if (respClient.readingFinished ==
                                1) {  // If the reading is finished, break the
                                      // loop
                                fprintf(log_fp,
                                        "File read operation is finished..\n");
                                break;
                            }
                        }

                        close(readFd);
                        free(direct_path_arr);
                        break;

                    case WRITET:
                        direct_path_size = strlen(dirname) + 1 +
                                           strlen(reqClient.filePath) + 1;
                        direct_path_arr =
                            malloc(direct_path_size * sizeof(char));
                        char *temp_direct_path_arr =
                            malloc(direct_path_size * sizeof(char));
                        snprintf(direct_path_arr, direct_path_size, "%s/%s",
                                 dirname, reqClient.filePath);
                        snprintf(temp_direct_path_arr, direct_path_size,
                                 "%s/%s", dirname,
                                 "temp.txt");  // temp as a temporary file to
                                               // write the buffer

                        FILE *file = fopen(direct_path_arr, "r");
                        FILE *temp;
                        if (reqClient.lineNumber == -1) {
                            temp =
                                fopen(temp_direct_path_arr,
                                      "a");  // write the to the end of the file
                        }
                        else {
                            temp = fopen(temp_direct_path_arr,
                                         "w");  // write the buffer to the
                                                // specified line number
                        }

                        if (temp == NULL) {
                            fprintf(log_fp, " Error at opening temp file..\n");
                            break;
                        }

                        char temp_buffer[BUFFER_SIZE];  // Buffer to store the
                                                        // lines of the file
                        int count = 1;

                        if (reqClient.lineNumber ==
                            -1) {  // If the lineNumber is -1, write the buffer
                                   // to the end of the file
                            lock_file(temp);
                            fputs(reqClient.buffer, temp);
                            unlock_file(temp);
                        }
                        else {
                            lock_file(temp);
                            while (fgets(temp_buffer, BUFFER_SIZE, file) !=
                                   NULL) {  // Read the file line by line
                                if (count ==
                                    reqClient
                                        .lineNumber) {  // If the current line
                                                        // number is equal to
                                                        // the specified line
                                                        // number, write the
                                                        // buffer to the file
                                    fputs(reqClient.buffer, temp);
                                    fputs("\n", temp);
                                }
                                fputs(temp_buffer, temp);
                                count++;
                            }
                            unlock_file(temp);
                        }

                        if (file !=
                            NULL) {  // Close the file if it is  already open
                            fclose(file);
                        }

                        fclose(temp);
                        remove(direct_path_arr);  // Remove the original file
                        rename(temp_direct_path_arr,
                               direct_path_arr);  // Rename the temp file to the
                                                  // original file
                        free(direct_path_arr);

                        respClient.writingFinished = 1;
                        server_send_response(clientFifo, &respClient);

                        break;
                    case DOWNLOAD:
                        direct_path_size = strlen(dirname) + 1 +
                                           strlen(reqClient.filePath) + 1;
                        direct_path_arr =
                            malloc(direct_path_size * sizeof(char));
                        snprintf(direct_path_arr, direct_path_size, "%s/%s",
                                 dirname, reqClient.filePath);
                        int fileDescriptor =
                            open(direct_path_arr, O_RDONLY, 0666);
                        if (fileDescriptor == -1) {
                            fprintf(log_fp,
                                    "Error at opening file for download..\n");
                            break;
                        }
                        respClient.readingFinished = 0;
                        while (1) {
                            sleep(1);
                            int status = read(fileDescriptor, respClient.buffer,
                                              sizeof(respClient.buffer) - 1);
                            if (status == -1) {
                                fprintf(log_fp,
                                        "Error at downloading file..\n");
                                break;
                            }
                            else if (status == 0) {
                                respClient.readingFinished = 1;
                            }
                            server_send_response(clientFifo, &respClient);
                            if (status == 0) {
                                break;
                            }
                        }
                        memset(
                            respClient.buffer, 0,
                            sizeof(respClient
                                       .buffer));  // Clear the buffer after the
                                                   // download is finished
                        close(fileDescriptor);
                        free(direct_path_arr);
                        break;
                    case UPLOAD:
                        direct_path_size = strlen(dirname) + 1 +
                                           strlen(reqClient.filePath) + 1;
                        direct_path_arr =
                            malloc(direct_path_size * sizeof(char));
                        snprintf(direct_path_arr, direct_path_size, "%s/%s",
                                 dirname, reqClient.filePath);

                        FILE *uploadFile;
                        if (reqClient.uploadContinues == 1) {
                            uploadFile = fopen(direct_path_arr, "a");
                        }
                        else {
                            uploadFile = fopen(direct_path_arr, "w");
                        }

                        if (uploadFile == NULL) {
                            fprintf(log_fp,
                                    "Failed to open the file for upload.\n");
                            break;
                        }
                        lock_file(uploadFile);
                        if (fwrite(reqClient.buffer, sizeof(char),
                                   strlen(reqClient.buffer),
                                   uploadFile) != strlen(reqClient.buffer)) {
                            fprintf(log_fp, "Error writing to the file.\n");
                            break;
                        }
                        unlock_file(uploadFile);

                        fclose(uploadFile);
                        free(direct_path_arr);
                        break;
                    case ARCHSERVER:
                        fprintf(log_fp,
                                "Server is archiving the directory..\n");
                        char archivePath[1024];
                        snprintf(archivePath, 1024, "%s/%s", dirname,
                                 "archive");
                        if (mkdir(archivePath, 0777) == -1) {
                            fprintf(log_fp,
                                    "Error at creating archive directory..\n");
                            break;
                        }
                        char archiveFile[1024];

                        snprintf(archiveFile, 1024, "%s/%s", archivePath,
                                 reqClient.filePath);
                        char *argv[] = {"tar", "-cf", archiveFile, dirname,
                                        NULL};
                        execvp("tar", argv);

                        // Move the archive file to the client side
                        char clientArchivePath[1024];
                        snprintf(clientArchivePath, 1024, "%s/%s", dirname,
                                 reqClient.filePath);
                        if (rename(archiveFile, clientArchivePath) == -1) {
                            fprintf(log_fp,
                                    "Error at moving archive file to client "
                                    "side..\n");
                        }
                        break;

                    case UNKNOWN:
                        fprintf(log_fp, "Unknown request type..\n");
                        break;
                }
            }
            exit(EXIT_SUCCESS);
        }
        else if (childPid == -1) {
            fprintf(log_fp, "Error at fork..\n");
            printf("Error at fork..\n");
        }
        else {  // Parent process
            childPids[currentIndexChildPids] = childPid;
            currentIndexChildPids++;
        }
    }

    sem_close(free_place_num);
    sem_close(occupied_place_num);
    sem_unlink("/free_place_num");
    sem_unlink("/occupied_place_num");
    closedir(directory);
    free(queue);
    fclose(log_fp);
    free(childPids);

    fprintf(log_fp, "Server terminated..\n");
    printf("Server terminated..\n");
    return 0;
}