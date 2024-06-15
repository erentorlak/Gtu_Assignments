#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/syscall.h>

#define PATH_MAX 4096  // Maximum path length

typedef struct {
    char source_path[PATH_MAX];
    char dest_path[PATH_MAX];
} file_info;

typedef struct {
    file_info *buffer; // Circular buffer array
    int in;            // Index for next item to produce
    int out;           // Index for next item to consume
    int count;         // Number of items currently in the buffer
    int buffer_size;   // Maximum size of the buffer
    pthread_mutex_t mutex;      // Mutex for synchronizing access to the buffer
    pthread_cond_t not_full;    // Condition variable to signal buffer not full
    pthread_cond_t not_empty;   // Condition variable to signal buffer not empty
    volatile int done; // Flag to indicate if production is done
} buffer_t;

buffer_t buffer; // Global buffer
int num_workers; // Number of worker threads
long total_bytes_copied = 0; // Total bytes copied by all worker threads
int total_files = 0;         // Total number of regular files
int total_directories = 0;   // Total number of directories
int total_fifos = 0;         // Total number of FIFO files
struct timeval start_time, end_time; // Variables to track time
pthread_mutex_t total_bytes_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for total bytes copied

pthread_barrier_t barrier;   // Barrier to synchronize threads

void signal_handler(int signum) {
    printf("Signal %d received, cleaning up...\n", signum);
    pthread_mutex_lock(&buffer.mutex);
    buffer.done = 1;
    pthread_cond_broadcast(&buffer.not_empty);
    pthread_cond_broadcast(&buffer.not_full);
    pthread_mutex_unlock(&buffer.mutex);
}

void init_buffer(buffer_t *buffer, int buffer_size) {
    buffer->buffer = malloc(buffer_size * sizeof(file_info));
    pthread_mutex_init(&buffer->mutex, NULL);
    pthread_cond_init(&buffer->not_full, NULL);
    pthread_cond_init(&buffer->not_empty, NULL);
    buffer->in = buffer->out = buffer->count = 0;
    buffer->buffer_size = buffer_size;
    buffer->done = 0;
    //printf("Buffer initialized.\n");
}

void destroy_buffer(buffer_t *buffer) {
    free(buffer->buffer);
    pthread_mutex_destroy(&buffer->mutex);
    pthread_cond_destroy(&buffer->not_full);
    pthread_cond_destroy(&buffer->not_empty);
    //printf("Buffer destroyed.\n");
}

void process_directory(const char *source_dir, const char *dest_dir) {
    DIR *dp;
    struct dirent *entry;
    dp = opendir(source_dir);
    if (!dp) {
        perror("Failed to open directory");
        return;
    }
    //printf("Processing directory: %s\n", source_dir);

    mkdir(dest_dir, 0755);
    //printf("Destination directory created: %s\n", dest_dir);

    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char source_path[PATH_MAX], dest_path[PATH_MAX];
        snprintf(source_path, sizeof(source_path), "%s/%s", source_dir, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, entry->d_name);

        struct stat statbuf;
        if (stat(source_path, &statbuf) != 0) continue;

        if (S_ISDIR(statbuf.st_mode)) {
            total_directories++;
            //printf("Directory found, recursive call: %s\n", source_path);
            process_directory(source_path, dest_path);
        } else if (S_ISREG(statbuf.st_mode)) {
            pthread_mutex_lock(&buffer.mutex);
            while (buffer.count == buffer.buffer_size && !buffer.done) {
                //printf("Buffer full, manager waiting...\n");
                pthread_cond_wait(&buffer.not_full, &buffer.mutex);
            }

            if (buffer.done) {
                pthread_mutex_unlock(&buffer.mutex);
                break;
            }

            strncpy(buffer.buffer[buffer.in].source_path, source_path, PATH_MAX);
            strncpy(buffer.buffer[buffer.in].dest_path, dest_path, PATH_MAX);
            buffer.in = (buffer.in + 1) % buffer.buffer_size;
            buffer.count++;
            total_files++;
            //printf("File added to buffer: %s\n", source_path);

            pthread_cond_signal(&buffer.not_empty);
            pthread_mutex_unlock(&buffer.mutex);
        } else if (S_ISFIFO(statbuf.st_mode)) {
            total_fifos++;
            //printf("FIFO file found: %s\n", source_path);
        }
    }
    closedir(dp);
    //printf("Finished processing directory: %s\n", source_dir);
}

void *manager_thread(void *arg) {
    char *source_dir = ((char **)arg)[0];
    char *dest_dir = ((char **)arg)[1];

    gettimeofday(&start_time, NULL);
    //printf("Manager thread (Thread ID: %ld) started.\n", syscall(SYS_gettid));
    process_directory(source_dir, dest_dir);

    pthread_mutex_lock(&buffer.mutex);
    buffer.done = 1;
    pthread_cond_broadcast(&buffer.not_empty);
    pthread_mutex_unlock(&buffer.mutex);
    //printf("Manager thread (Thread ID: %ld) signaling completion.\n", syscall(SYS_gettid));

    pthread_barrier_wait(&barrier);

    return NULL;
}

void *worker_thread(void *arg) {
    //printf("Worker thread (Thread ID: %ld) started.\n", syscall(SYS_gettid));

    while (1) {
        pthread_mutex_lock(&buffer.mutex);
        while (buffer.count == 0 && !buffer.done) {
            //printf("Worker thread (Thread ID: %ld) waiting, buffer empty...\n", syscall(SYS_gettid));
            pthread_cond_wait(&buffer.not_empty, &buffer.mutex);
        }
        if (buffer.count == 0 && buffer.done) {
            pthread_cond_signal(&buffer.not_full);
            pthread_mutex_unlock(&buffer.mutex);
            break;
        }

        file_info file = buffer.buffer[buffer.out];
        buffer.out = (buffer.out + 1) % buffer.buffer_size;
        buffer.count--;
        pthread_cond_signal(&buffer.not_full);
        pthread_mutex_unlock(&buffer.mutex);

        int source_fd = open(file.source_path, O_RDONLY);
        int dest_fd = open(file.dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (source_fd < 0 || dest_fd < 0) {
            perror("Error opening files");
            if (source_fd >= 0) close(source_fd);
            if (dest_fd >= 0) close(dest_fd);
            continue;
        }

        printf("Copying file: %s to %s (Thread ID: %ld)\n", file.source_path, file.dest_path, syscall(SYS_gettid));
        char buf[1024];
        ssize_t n;
        while ((n = read(source_fd, buf, sizeof(buf))) > 0) {
            if (write(dest_fd, buf, n) != n) {
                perror("Error writing to file");
                break;
            }
            pthread_mutex_lock(&total_bytes_mutex); // Lock mutex for updating total bytes copied
            total_bytes_copied += n;
            pthread_mutex_unlock(&total_bytes_mutex); // Unlock mutex
        }
        close(source_fd);
        close(dest_fd);
    }

    pthread_barrier_wait(&barrier);
    printf("Worker thread (Thread ID: %ld) exiting after barrier.\n", syscall(SYS_gettid));

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <buffer size> <number of workers> <source dir> <dest dir>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);

    int buffer_size = atoi(argv[1]);
    num_workers = atoi(argv[2]);
    char *source_dir = argv[3];
    char *dest_dir = argv[4];

    pthread_t manager;
    pthread_t *workers = malloc(num_workers * sizeof(pthread_t));

    init_buffer(&buffer, buffer_size);
    pthread_barrier_init(&barrier, NULL, num_workers + 1);

    pthread_create(&manager, NULL, manager_thread, (void *)(char *[]){source_dir, dest_dir});
    for (int i = 0; i < num_workers; i++) {
        pthread_create(&workers[i], NULL, worker_thread, NULL);
    }

    pthread_join(manager, NULL);
    for (int i = 0; i < num_workers; i++) {
        pthread_join(workers[i], NULL);
    }

    free(workers);
    destroy_buffer(&buffer);
    pthread_barrier_destroy(&barrier);

    gettimeofday(&end_time, NULL);
    long seconds = end_time.tv_sec - start_time.tv_sec;
    long microseconds = end_time.tv_usec - start_time.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;

    printf("\n---------------STATISTICS--------------------\n");
    printf("Consumers: %d - Buffer Size: %d\n", num_workers, buffer_size);
    printf("Number of Regular Files: %d\n", total_files);
    printf("Number of Directories: %d\n", total_directories);
    printf("Number of FIFO Files: %d\n", total_fifos);
    printf("TOTAL BYTES COPIED: %ld\n", total_bytes_copied);
    printf("TOTAL TIME: %.3f seconds\n", elapsed);

    return 0;
}
