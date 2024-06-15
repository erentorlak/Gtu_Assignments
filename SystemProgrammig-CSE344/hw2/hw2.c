#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define FIFO1 "/tmp/fifo1"
#define FIFO2 "/tmp/fifo2"

volatile int child_exit_counter = 0;    // Counter to keep track of child processes that have exited

void handle_sigchld(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) { // WNOHANG: return immediately if no child has exited
        printf("Debug: Child %d terminated with exit status %d\n", pid, WEXITSTATUS(status));   // WEXITSTATUS: returns the exit status of the child
        child_exit_counter++;
    }
}

void cleanup() {    // Cleanup function to remove FIFOs
    unlink(FIFO1);
    unlink(FIFO2);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of elements>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    srand(time(NULL)); // Seed the random number generator
    int n = atoi(argv[1]);  // Number of elements
    int numbers[n]; 
    for (int i = 0; i < n; ++i) {
        numbers[i] = rand() % 10; // Generate random numbers between 0 and 9
    }

    cleanup();  // Remove any existing FIFOs

    if (mkfifo(FIFO1, 0666) < 0 || mkfifo(FIFO2, 0666) < 0) {   // Create FIFOs
        perror("Error: Failed to create FIFOs");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa; // Signal handler for SIGCHLD
    memset(&sa, 0, sizeof(sa)); // what memset does is that it fills the first sizeof(sa) bytes of the memory area pointed to by &sa with the constant byte 0 for the signal handler 
    sa.sa_handler = handle_sigchld;
    sigaction(SIGCHLD, &sa, NULL);  // what SIGCHLD does is that it is sent to the parent process when a child process terminates

    pid_t pid1 = fork();    
    if (pid1 == 0) {    // Child 1

        // open with error handling
        int fd1 = open(FIFO1, O_RDONLY);
        if (fd1 < 0) {
            perror("Error: Failed to open FIFO1 for reading");
            exit(EXIT_FAILURE);
        }

        int sum = 0, value;
    
        while (1) {
            int result = read(fd1, &value, sizeof(value)); // Attempt to read from FIFO1
            if (result > 0) {
                sum += value;  // Update sum
                printf("Child 1: Read %d, Current Sum: %d\n", value, sum);
            } else if (result == 0) {
                //printf("End of file reached or pipe closed\n");
                break;  // Exit loop if end of file or pipe is closed
            } else {
                perror("Read failed"); // Use perror to print the error message
                break;  // Exit loop on error
            }
        }
        printf("Child 1: Sum : %d\n", sum);
        close(fd1);

        int fd2 = open(FIFO2, O_WRONLY);    // Write sum to FIFO2
        if (fd2 < 0) {
            perror("Error: Failed to open FIFO2 for writing");
            exit(EXIT_FAILURE);
        }
        sleep(10);    // we make latency for not sending the sum earlier than the parent process sends the numbers and the command.
        if (write(fd2, &sum, sizeof(sum)) < 0) {    
            perror("Error: Failed to write sum to FIFO2");
            close(fd2);
            exit(EXIT_FAILURE);
        }
        
        //printf("Child 1: Wrote sum %d to FIFO2.\n", sum);
        close(fd2);

        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {    // Child 2
        int fd2 = open(FIFO2, O_RDONLY);
        int temp_numbers[n], sum, product = 1;
        char command[10];

        // Read numbers first
        if (read(fd2, temp_numbers, n * sizeof(int)) < 0) {
            perror("Error: Failed to read numbers from FIFO2");
            close(fd2);
            exit(EXIT_FAILURE);
        }

        // Then read command
        if (read(fd2, command, sizeof(command)) < 0) {
            perror("Error: Failed to read command from FIFO2");
            close(fd2);
            exit(EXIT_FAILURE);
        }
        //printf("Child 2: Received command: %s\n", command);

        // Perform multiplication if command is "multiply"
        if (strcmp(command, "multiply") == 0) {
            for (int i = 0; i < n; ++i) {
                product *= temp_numbers[i];
            }
        }
        else {
            perror("Error: Invalid command received");
            close(fd2);
            exit(EXIT_FAILURE);
        }

        sleep(10);      // actually we dont need but pdf says so : All child processes sleep for 10 seconds, execute their tasks, and then exit.

        printf("Child 2: Multiplication result: %d\n", product);

        if (read(fd2, &sum, sizeof(sum)) < 0) {         // Read sum from FIFO2
            perror("Error: Failed to read sum from FIFO2");
            close(fd2);
            exit(EXIT_FAILURE);
        }
        //printf("Child 2: Received sum %d from FIFO2.\n", sum);

        // Combine the sum and product
        int final_result = sum + product;
        printf("Final Output : multiply + sum : %d\n", final_result);

        close(fd2);

        exit(0);
    }

    // Parent sends numbers to FIFO1 for Child 1
    
    int fd1 = open(FIFO1, O_WRONLY);
    write(fd1, numbers, n * sizeof(int));   // Write numbers to FIFO1
    close(fd1);
    
    // Parent writes numbers and command to FIFO2
    int fd2 = open(FIFO2, O_WRONLY);
    write(fd2, numbers, n * sizeof(int)); // First send numbers
    write(fd2, "multiply", strlen("multiply") + 1); // Then send command
    close(fd2);

    while (child_exit_counter < 2) {    // Wait for both children to exit
        printf("Parent: Proceeding, waiting for children to exit...\n");
        sleep(2);   
    }   

    cleanup();  // Remove FIFOs

    return 0;
}
