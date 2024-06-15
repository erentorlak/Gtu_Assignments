#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

typedef struct {
    int x, y;
} Location;

typedef struct {
    char address[256];
    int port;
    int p;
    int q;
} ClientParams;

int cancel_all = 0;     // 0: not cancel, 1: cancel

int num_clients = 0;        // number of clients

pthread_mutex_t normal_end_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t cancel_mutex = PTHREAD_MUTEX_INITIALIZER;

void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("Client shutting down\n");

        pthread_mutex_lock(&cancel_mutex);
        cancel_all = 1;                         // cancel all clients
        pthread_mutex_unlock(&cancel_mutex);
    }
}

void *client_thread(void *arg) {
    ClientParams *params = (ClientParams *)arg;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        free(params);
        return NULL;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(params->port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (inet_pton(AF_INET, params->address, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(client_socket);
        free(params);
        return NULL;
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        free(params);
        return NULL;
    }

    Location loc = {rand() % params->p, rand() % params->q};
    if (write(client_socket, &loc, sizeof(loc)) < 0) {
        perror("Failed to send location to server");
        close(client_socket);
        free(params);
        return NULL;
    }

    printf("Client connected at (%d, %d) to server %s:%d\n", loc.x, loc.y, params->address, params->port);

    char buffer[256];
    int normally_ended = 0;
    while (1) {
        
        if ( normally_ended == 1) {
            loc.x = -1;                     // -1 means cancel
            loc.y = -1;
            if (write(client_socket, &loc, sizeof(loc)) < 0) {
                perror("Failed to send cancellation location to server");
                close(client_socket);
                free(params);
                return NULL;
            }
            break;
        }

        ssize_t ret = read(client_socket, buffer, sizeof(buffer));
        if (ret <= 0) {
            if (ret < 0) perror("Failed to read from server");
            break;
        }

        pthread_mutex_lock(&cancel_mutex);
        if (cancel_all) {                   
            pthread_mutex_unlock(&cancel_mutex);
            break;
        }
        pthread_mutex_unlock(&cancel_mutex);

        printf("Client at (%d, %d): %s\n", loc.x, loc.y, buffer);

        if (strlen(buffer) == 15) { // Order delivered  then exit
            normally_ended = 1;
        }
    }
    pthread_mutex_lock(&cancel_mutex);
    if (cancel_all) {
        pthread_mutex_unlock(&cancel_mutex);
        printf("Client at (%d, %d) ,  client socket: %d disconnecting \n", loc.x, loc.y, client_socket);

        // Send a message to the server to remove the client
        loc.x = -1;
        loc.y = -1;
        if (write(client_socket, &loc, sizeof(loc)) < 0) {
            perror("Failed to send cancellation location to server");
            close(client_socket);
            free(params);
            return NULL;
        }

        printf("Cancellation message sent\n");
    } else {
        pthread_mutex_unlock(&cancel_mutex);
    }
    close(client_socket);
    free(params);
    return NULL;
}

void generate_clients(const char *address, int port, int num_clients, int p, int q) {
    pthread_t threads[num_clients];
    for (int i = 0; i < num_clients; ++i) {
        ClientParams *params = malloc(sizeof(ClientParams));
        strncpy(params->address, address, sizeof(params->address) - 1);
        params->address[sizeof(params->address) - 1] = '\0';
        params->port = port;
        params->p = p;
        params->q = q;
        pthread_create(&threads[i], NULL, client_thread, params);
       
    }
    
    for (int i = 0; i < num_clients; ++i) {
        pthread_join(threads[i], NULL);
        printf("Client %d joined\n", i);
    }
    printf("Clients joined\n");
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s [address] [portnumber] [numberOfClients] [p] [q]\n", argv[0]);
        return 1;
    }

    const char *address = argv[1];
    int port = atoi(argv[2]);
    num_clients = atoi(argv[3]);
    int p = atoi(argv[4]);
    int q = atoi(argv[5]);

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    generate_clients(address, port, num_clients, p, q);

    return 0;
}
