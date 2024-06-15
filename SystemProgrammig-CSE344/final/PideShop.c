// PideShop.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <complex.h>
#include <math.h>
#include <errno.h>

#define MAX_ORDERS 100
#define MAX_COOKS 10
#define MAX_DELIVERIES 10
#define MAX_OVEN_CAPACITY 6
#define MAX_DELIVERY_BAG 3
typedef struct {
    double real;
    double imag;
} Complex;

#define ROWS 30
#define COLS 40

Complex complex_multiply(Complex a, Complex b) {
    Complex result;
    result.real = a.real * b.real - a.imag * b.imag;
    result.imag = a.real * b.imag + a.imag * b.real;
    return result;
}

Complex complex_add(Complex a, Complex b) {
    Complex result;
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

Complex complex_subtract(Complex a, Complex b) {
    Complex result;
    result.real = a.real - b.real;
    result.imag = a.imag - b.imag;
    return result;
}

Complex complex_divide(Complex a, Complex b) {
    Complex result;
    double denom = b.real * b.real + b.imag * b.imag;
    result.real = (a.real * b.real + a.imag * b.imag) / denom;
    result.imag = (a.imag * b.real - a.real * b.imag) / denom;
    return result;
}

Complex complex_conjugate(Complex a) {
    Complex result;
    result.real = a.real;
    result.imag = -a.imag;
    return result;
}

void transpose(Complex matrix[ROWS][COLS], Complex result[COLS][ROWS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            result[j][i] = matrix[i][j];
        }
    }
}

void matrix_multiply(Complex A[COLS][ROWS], Complex B[ROWS][COLS], Complex result[COLS][COLS]) {
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < COLS; j++) {
            result[i][j].real = 0;
            result[i][j].imag = 0;
            for (int k = 0; k < ROWS; k++) {
                result[i][j] = complex_add(result[i][j], complex_multiply(A[i][k], B[k][j]));
            }
        }
    }
}

int invert_matrix(Complex matrix[COLS][COLS], Complex inverse[COLS][COLS]) {
    int i, j, k;
    sleep(1);
    Complex ratio, a;
    for (i = 0; i < COLS; i++) {
        for (j = 0; j < COLS; j++) {
            if (i == j)
                inverse[i][j] = (Complex){1.0, 0.0};
            else
                inverse[i][j] = (Complex){0.0, 0.0};
        }
    }
    for (i = 0; i < COLS; i++) {
        a = matrix[i][i];
        if (a.real == 0 && a.imag == 0) return 0;
        for (j = 0; j < COLS; j++) {
            matrix[i][j] = complex_divide(matrix[i][j], a);
            inverse[i][j] = complex_divide(inverse[i][j], a);
        }
        for (j = 0; j < COLS; j++) {
            if (i != j) {
                ratio = matrix[j][i];
                for (k = 0; k < COLS; k++) {
                    matrix[j][k] = complex_subtract(matrix[j][k], complex_multiply(ratio, matrix[i][k]));
                    inverse[j][k] = complex_subtract(inverse[j][k], complex_multiply(ratio, inverse[i][k]));
                }
            }
        }
    }
    return 1;
}
Complex matrix[ROWS][COLS];
    Complex transpose_matrix[COLS][ROWS];
    Complex product[COLS][COLS];
    Complex inverse[COLS][COLS];

void psodo_inverse() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            matrix[i][j].real = i + j;
            matrix[i][j].imag = i - j;
        }
    }
    transpose(matrix, transpose_matrix);
    matrix_multiply(matrix, transpose_matrix, product);
    invert_matrix(product, inverse);
}


int available_oven_aparatus = 3;
int available_oven_capacity = MAX_OVEN_CAPACITY;
int shop_open = 1;
int server_socket;

FILE *log_file;

typedef struct {
    int x, y;
} Location;

typedef struct {
    int order_id;
    Location loc;
    int prepared;  // 0 = no, 1 = yes
    int cooked;    // 0 = no, 1 = yes
    int delivered; // 0 = no, 1 = yes
    
    int arrived; // 0 = no, 1 = yes
    
    int cancelled; // 0 = no, 1 = yes
    int client_socket;
} Order;

typedef struct {
    int id;
    int bag_capacity;
    Order *bag[MAX_DELIVERY_BAG];
} DeliveryPerson;

Order orders[MAX_ORDERS];
DeliveryPerson delivery_persons[MAX_DELIVERIES];
pthread_mutex_t order_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t order_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t oven_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t oven_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t delivery_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t delivery_cond = PTHREAD_COND_INITIALIZER;

int rem = 0;

int order_count = 0;
int cook_pool_size, delivery_pool_size;
int delivery_speed;
int client_cancel_all = 0;

int amount_of_delivery_per_person[MAX_DELIVERIES] = {0};
int amount_of_meal_per_cook[MAX_COOKS] = {0};

void log_event(const char *event) {
    fprintf(log_file, "%s\n", event);
    fflush(log_file);
}
void notify_client(int client_socket, const char *message) {
    ssize_t ret = write(client_socket, message, strlen(message) + 1);
    if (ret < 0) {
        if (errno == EPIPE) {
            perror("Client disconnected");
        } else {
            printf("Client socket: %d\n", client_socket);
            printf("message: %s\n", message);
            //perror("Failed to notify client");
        }
    }
}
void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        shop_open = 0;
        // unlock order_cond and delivery_cond
        pthread_cond_broadcast(&order_cond);
        pthread_cond_broadcast(&delivery_cond);
        printf("Server shutting down WAIT 3 seconds\n");
        log_event("Server shutting down");

        // Notify all connected clients about the shutdown
        for (int i = 0; i < order_count; ++i) {
            if ( !orders[i].arrived)  {

                orders[i].cancelled = 1;
            }
        }
        sleep(3); // Wait for clients to receive the cancellation message
        fclose(log_file);
        close(server_socket);
    }
}


void *cook_thread(void *arg) {
    int cook_id = *((int *)arg);
    free(arg);

    while (shop_open) {
        Order *order = NULL;

        // Manager assigns an order to the cook
        pthread_mutex_lock(&order_mutex);   
        while (shop_open) {         // if shop is closed, no need to check the orders
            for (int i = 0; i < order_count; ++i) {
                if (!orders[i].prepared && !orders[i].cancelled) {
                    order = &orders[i];
                    order->prepared = 1; // Mark as being prepared
                    break;
                }
            }
            if (order) break;       

            pthread_cond_wait(&order_cond, &order_mutex);
            if (!shop_open) {
                pthread_mutex_unlock(&order_mutex);
                return NULL;
            }
        }
        pthread_mutex_unlock(&order_mutex);

        if (order && !order->cancelled) {
            char log_msg[256];
            snprintf(log_msg, sizeof(log_msg), "Cook %d is preparing order %d", cook_id, order->order_id);
            log_event(log_msg);
            snprintf(log_msg, sizeof(log_msg), "You are Client id %d , your order is prepared by cook %d", order->client_socket, cook_id);
            notify_client(order->client_socket, log_msg);

            // Preparation time
            psodo_inverse();
            psodo_inverse();
//            sleep(2); // Preparation time
            if (order->cancelled) {
                if (client_cancel_all) {    // clients is disconnected no need to notify
                    snprintf(log_msg, sizeof(log_msg), "Order %d was cancelled during preparation by cook %d", order->order_id, cook_id);
                    log_event(log_msg);
                }
                else{   // server is closing, notify the client
                    notify_client(order->client_socket, "Your order was cancelled during preparation.");
                }

                pthread_cond_signal(&order_cond);
                continue;
            }

            snprintf(log_msg, sizeof(log_msg), "Order %d is prepared by cook %d", order->order_id, cook_id);
            log_event(log_msg);

            // Simulate oven usage
            pthread_mutex_lock(&oven_mutex);
            while (available_oven_aparatus <= 0 || available_oven_capacity <= 0) {
                pthread_cond_wait(&oven_cond, &oven_mutex);
            }
            available_oven_aparatus--;
            available_oven_capacity--;
            pthread_mutex_unlock(&oven_mutex);
            
            // Cooking time
            psodo_inverse();

            if (order->cancelled) {
                snprintf(log_msg, sizeof(log_msg), "Order %d was cancelled during cooking by cook %d", order->order_id, cook_id);
                log_event(log_msg);
                snprintf(log_msg, sizeof(log_msg), "You are Client id %d , your order was cancelled during cooking by cook %d", order->client_socket, cook_id);
                notify_client(order->client_socket, log_msg);
                pthread_mutex_lock(&oven_mutex);
                available_oven_aparatus++;
                available_oven_capacity++;
                pthread_cond_signal(&oven_cond);
                pthread_mutex_unlock(&oven_mutex);
                pthread_cond_signal(&order_cond);
                continue;
            }

            order->cooked = 1;
            amount_of_meal_per_cook[cook_id]++;    // it is for promotion
            snprintf(log_msg, sizeof(log_msg), "Order %d is cooked by cook %d", order->order_id, cook_id);
            log_event(log_msg);

            if (!client_cancel_all)  {  // clients is disconnected no need to notify
                char log_msg[256];
                snprintf(log_msg, sizeof(log_msg), "You are Client id %d , your order is cooked by cook %d", order->client_socket, cook_id);
                notify_client(order->client_socket, log_msg);
            }

            pthread_mutex_lock(&oven_mutex);
            available_oven_aparatus++;
            available_oven_capacity++;
            pthread_cond_signal(&oven_cond);
            pthread_mutex_unlock(&oven_mutex);

            pthread_cond_signal(&delivery_cond);
        }
    }

    return NULL;
}

void *delivery_thread(void *arg) {
    DeliveryPerson *dp = (DeliveryPerson *)arg;
            // get rem order_count%3
            
            
    while (shop_open) {
        int goout = 0;

        rem = order_count % 3; 

        pthread_mutex_lock(&delivery_mutex);
        while (dp->bag_capacity < MAX_DELIVERY_BAG && shop_open) {

            for (int i = 0; i < order_count- rem; ++i) {
                if (orders[i].cooked && !orders[i].delivered && !orders[i].cancelled) {
                    orders[i].delivered = 1; // Mark as being delivered to prevent other delivery personnel from picking it up
                    dp->bag[dp->bag_capacity++] = &orders[i]; // Add order to delivery person's bag
                    char log_msg[256];
                    snprintf(log_msg, sizeof(log_msg), "Order %d added to delivery person %d's bag", orders[i].order_id, dp->id);
                    log_event(log_msg);
                    if (orders[i].cancelled) {
                        snprintf(log_msg, sizeof(log_msg), "Order %d was cancelled while in delivery by delivery person %d", orders[i].order_id, dp->id);
                       
                        log_event(log_msg);
                        if (!client_cancel_all){    // clients is disconnected no need to notify
                            char log_msg[256];
                            snprintf(log_msg, sizeof(log_msg), "You are Client id %d , your order was cancelled during delivery by delivery person %d", orders[i].client_socket, dp->id);
                            notify_client(orders[i].client_socket, log_msg);
                        }           
                        continue;
                    }
                    char log_msg3[256];
                    snprintf(log_msg3, sizeof(log_msg3), "You are Client id %d , your order is out for delivery by delivery person %d", orders[i].client_socket, dp->id);
                    notify_client(orders[i].client_socket, log_msg3);
                    //printf("Your order is out for delivery. Client %d\n", orders[i].client_socket);
                    if (dp->bag_capacity == MAX_DELIVERY_BAG) break;
                }
            }
            for (int i = order_count - rem; i < order_count; ++i) {

                if (orders[i].cooked && !orders[i].delivered && !orders[i].cancelled) {                    
                    orders[i].delivered = 1; // Mark as being delivered to prevent other delivery personnel from picking it up
                    dp->bag[dp->bag_capacity++] = &orders[i]; // Add order to delivery person's bag
                    char log_msg[256];
                    snprintf(log_msg, sizeof(log_msg), "Order %d added to delivery person %d's bag", orders[i].order_id, dp->id);
                    log_event(log_msg);
                    if (orders[i].cancelled) {
                        snprintf(log_msg, sizeof(log_msg), "Order %d was cancelled while in delivery by delivery person %d", orders[i].order_id, dp->id);

                        
                        log_event(log_msg);
                        if (!client_cancel_all){    // clients is disconnected no need to notify
                            char log_msg[256];
                            snprintf(log_msg, sizeof(log_msg), "You are Client id %d , your order was cancelled during delivery by delivery person %d", orders[i].client_socket, dp->id);
                            notify_client(orders[i].client_socket, log_msg);
                        }
                        
                        continue;
                    }
                    char log_msg4[256];
                    snprintf(log_msg4, sizeof(log_msg4), "You are Client id %d , your order is out for delivery by delivery person %d", orders[i].client_socket, dp->id);
                    notify_client(orders[i].client_socket, log_msg4);
                    //printf("Your order is out for delivery. Client %d\n", orders[i].client_socket);
                     
                    if (dp->bag_capacity == MAX_DELIVERY_BAG - 1) {
                        goout = 1;
                        break;
                    }
                    if (dp->bag_capacity == MAX_DELIVERY_BAG - 2) {
                        goout = 1;
                        break;
                    }
                }
            }

            if (dp->bag_capacity == 0) pthread_cond_wait(&delivery_cond, &delivery_mutex); // wait for a signal from the manager for a new order
            if (goout) break;
        }
        pthread_mutex_unlock(&delivery_mutex);

        if (dp->bag_capacity == MAX_DELIVERY_BAG) {
            char log_msg[256];
            snprintf(log_msg, sizeof(log_msg), "Delivery person %d is delivering orders", dp->id);
            log_event(log_msg);

            for (int i = 0; i < dp->bag_capacity; ++i) {
                Order *order = dp->bag[i];
                if (order->cancelled) {
                    snprintf(log_msg, sizeof(log_msg), "Order %d was cancelled while in delivery by delivery person %d", order->order_id, dp->id);
                    if (!client_cancel_all){
                        char log_msg2[256];
                        snprintf(log_msg2, sizeof(log_msg2), "You are Client id %d , your order was cancelled during delivery by delivery person %d", order->client_socket, dp->id);
                        notify_client(order->client_socket, log_msg2);
                    }
                    log_event(log_msg);
                    continue;
                }
                int distance = sqrt(pow(order->loc.x, 2) + pow(order->loc.y, 2));
                int delivery_time = (2 * distance) / delivery_speed; // Round trip time
                sleep(delivery_time / 10); // Simulate delivery time

                pthread_mutex_lock(&order_mutex);
                notify_client(order->client_socket, "Order delivered");     // dont change 
                order->arrived = 1;
                amount_of_delivery_per_person[dp->id]++;        // it is for promotion 
                close(order->client_socket);
                pthread_mutex_unlock(&order_mutex);

                snprintf(log_msg, sizeof(log_msg), "Order %d delivered by delivery person %d", order->order_id, dp->id);
                log_event(log_msg);
            }
            dp->bag_capacity = 0; // Empty the bag

            pthread_cond_signal(&order_cond);
        }
        if ((dp->bag_capacity == MAX_DELIVERY_BAG - 1) || (dp->bag_capacity == MAX_DELIVERY_BAG - 2)) {
            char log_msg[256];
            snprintf(log_msg, sizeof(log_msg), "Delivery person %d is delivering orders", dp->id);
            log_event(log_msg);
            for (int i = 0; i < dp->bag_capacity; ++i) {
                Order *order = dp->bag[i];
                if (order->cancelled) {
                    snprintf(log_msg, sizeof(log_msg), "Order %d was cancelled while in delivery by delivery person %d", order->order_id, dp->id);
                    if (!client_cancel_all){
                        notify_client(order->client_socket, "Your order was cancelled during delivery.");
                    }
                    log_event(log_msg);
                    continue;
                }
                int distance = sqrt(pow(order->loc.x, 2) + pow(order->loc.y, 2));
                int delivery_time = (2 * distance) / delivery_speed; // Round trip time
                sleep(delivery_time / 10); // Simulate delivery time

                pthread_mutex_lock(&order_mutex);
                notify_client(order->client_socket, "Order delivered");     // dont change
                order->arrived = 1;
                amount_of_delivery_per_person[dp->id]++;        // it is for promotion 
                close(order->client_socket);
                pthread_mutex_unlock(&order_mutex);

                snprintf(log_msg, sizeof(log_msg), "Order %d delivered by delivery person %d", order->order_id, dp->id);
                log_event(log_msg);
            }
            dp->bag_capacity = 0; // Empty the bag

            pthread_cond_signal(&order_cond);
        }

    }

    // if server is closing, say goodbye to the clients
    if (!shop_open) {
        for (int i = 0; i < dp->bag_capacity; ++i) {
            Order *order = dp->bag[i];
            notify_client(order->client_socket, " Your pides are in thrash. PideShop is closing. Goodbye");
            close(order->client_socket);
        }
    }
    return NULL;
}

void *client_handler(void *arg) {
    int client_socket = *((int *)arg);
    free(arg);

    Location loc;
    if (read(client_socket, &loc, sizeof(loc)) <= 0) {
        perror("Failed to read location from client");
        close(client_socket);
        return NULL;
    }

    pthread_mutex_lock(&order_mutex);
    printf("Client connected at (%d, %d)  , client socket: %d  , order id : %d\n", loc.x, loc.y, client_socket, order_count + 1);

    if (order_count < MAX_ORDERS) {
        orders[order_count].order_id = order_count + 1;
        orders[order_count].loc = loc;
        orders[order_count].prepared = 0;
        orders[order_count].cooked = 0;
        orders[order_count].delivered = 0;
        orders[order_count].cancelled = 0;
        orders[order_count].client_socket = client_socket;
        order_count++;

        char log_msg[256];
        snprintf(log_msg, sizeof(log_msg), "Order %d received at location (%d, %d)", orders[order_count - 1].order_id, loc.x, loc.y);
        log_event(log_msg);
        snprintf(log_msg, sizeof(log_msg), "You are Client id %d , your order is received", client_socket);
        notify_client(client_socket, log_msg);

        pthread_cond_broadcast(&order_cond);
    } else {
        log_event("Max order capacity reached. Order cannot be accepted.");
        notify_client(client_socket, "Order cannot be accepted");
        close(client_socket);
    }
    pthread_mutex_unlock(&order_mutex);

    if (read(client_socket, &loc, sizeof(loc)) <= 0) {
        perror("Failed to read location from client");
        close(client_socket);
        return NULL;
    }
    
    if (loc.x < 0 && loc.y < 0) {       // I am commincaiting with loc to cancel the order. -1 -1 means cancel

        printf ("Client disconnected. client socket: %d\n", client_socket);

        for (int i = 0; i < order_count; ++i) {
            if (orders[i].client_socket == client_socket) {
                orders[i].cancelled = 1;
                client_cancel_all = 1;
                break;
            }
        }    
        close(client_socket);
        return NULL;
    }

    return NULL;
}


int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s [portnumber] [CookthreadPoolSize] [DeliveryPoolSize] [k]\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    cook_pool_size = atoi(argv[2]);
    delivery_pool_size = atoi(argv[3]);
    delivery_speed = atoi(argv[4]);

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    log_file = fopen("pide_shop_log.txt", "w");

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {      
        perror("setsockopt failed");
        close(server_socket);
        return 1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, MAX_ORDERS) < 0) {
        perror("Listen failed");
        close(server_socket);
        return 1;
    }

    pthread_t cook_threads[MAX_COOKS];
    pthread_t delivery_threads[MAX_DELIVERIES];

    for (int i = 0; i < cook_pool_size; ++i) {
        int *cook_id = malloc(sizeof(int));
        *cook_id = i + 1;
        pthread_create(&cook_threads[i], NULL, cook_thread, cook_id);
    }

    for (int i = 0; i < delivery_pool_size; ++i) {
        delivery_persons[i].id = i + 1;
        delivery_persons[i].bag_capacity = 0;
        pthread_create(&delivery_threads[i], NULL, delivery_thread, &delivery_persons[i]);
    }

    log_event("PideShop server started");

   while (shop_open) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            if (!shop_open) break;
            perror("Accept failed");
            continue;
        }

        pthread_t client_thread;
        int *client_sock_ptr = malloc(sizeof(int));
        if (client_sock_ptr == NULL) {
            perror("Failed to allocate memory for client_sock_ptr");
            close(client_socket);
            continue;
        }
        *client_sock_ptr = client_socket;
        if (pthread_create(&client_thread, NULL, client_handler, client_sock_ptr) != 0) {
            perror("Failed to create client thread");
            free(client_sock_ptr);
            close(client_socket);
        } else {
            pthread_detach(client_thread);  // Detach the thread to avoid memory leaks
        }
    }

    // Cleanup all threads
    for (int i = 0; i < cook_pool_size; ++i) {
        pthread_join(cook_threads[i], NULL);   
    }
    for (int i = 0; i < delivery_pool_size; ++i) {
        pthread_join(delivery_threads[i], NULL);
    }

    // get the promotion
    int max_meal = 0;
    int max_delivery = 0;
    int max_meal_index = 0;
    int max_delivery_index = 0;
    for (int i = 0; i < cook_pool_size; ++i) {
        if (amount_of_meal_per_cook[i] > max_meal) {
            max_meal = amount_of_meal_per_cook[i];
            max_meal_index = i;
        }
    }
    for (int i = 0; i < delivery_pool_size; ++i) {
        if (amount_of_delivery_per_person[i] > max_delivery) {
            max_delivery = amount_of_delivery_per_person[i];
            max_delivery_index = i;
        }
    }
    printf("\nThe cook who cooked the most meals: %d\n", max_meal_index );
    printf("The delivery person who delivered the most orders: %d\n", max_delivery_index );


    close(server_socket);
    return 0;
}
