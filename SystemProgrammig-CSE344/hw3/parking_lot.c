#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

// Constants
#define MAX_PICKUP_SPOTS 4
#define MAX_AUTOMOBILE_SPOTS 8
#define NUM_VEHICLES 20

// Shared memory counters
int mFree_automobile = MAX_AUTOMOBILE_SPOTS;
int mFree_pickup = MAX_PICKUP_SPOTS;

// Semaphores
sem_t newPickup;
sem_t inChargeforPickup;
sem_t newAutomobile;
sem_t inChargeforAutomobile;

// Mutex for protecting shared counters
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  

// Flag to stop attendant threads
volatile int stop = 0;

void* carOwner(void* arg) {
    int vehicle_type = *(int*)arg;

    if (vehicle_type == 0) { // Car
        pthread_mutex_lock(&mutex);
        if (mFree_automobile > 0) {
            mFree_automobile--;
            printf("Car owner arrived. Free car spots: %d\n", mFree_automobile);
            sem_post(&newAutomobile);
            pthread_mutex_unlock(&mutex);
            sem_wait(&inChargeforAutomobile);
        } else {
            printf("Car owner left. No free car spots.\n");
            pthread_mutex_unlock(&mutex);
        }
    } else { // Pickup
        pthread_mutex_lock(&mutex);
        if (mFree_pickup > 0) {
            mFree_pickup--;
            printf("Pickup owner arrived. Free pickup spots: %d\n", mFree_pickup);
            sem_post(&newPickup);
            pthread_mutex_unlock(&mutex);
            sem_wait(&inChargeforPickup);
        } else {
            printf("Pickup owner left. No free pickup spots.\n");
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

void* carAttendant(void* arg) {
    int vehicle_type = *(int*)arg;

    while (!stop) {
        if (vehicle_type == 0) { // Car
            sem_wait(&newAutomobile);
            if (stop) break;
            sleep(2); // Simulate parking time
            pthread_mutex_lock(&mutex);
            mFree_automobile++;
            printf("Car attendant parked a car. Free car spots: %d\n", mFree_automobile);
            pthread_mutex_unlock(&mutex);
            sem_post(&inChargeforAutomobile);
        } else { // Pickup
            sem_wait(&newPickup);
            if (stop) break;
            sleep(2); // Simulate parking time
            pthread_mutex_lock(&mutex);
            mFree_pickup++;
            printf("Pickup attendant parked a pickup. Free pickup spots: %d\n", mFree_pickup);
            pthread_mutex_unlock(&mutex);
            sem_post(&inChargeforPickup);
        }
    }

    return NULL;
}

int main() {
    pthread_t owner_threads[NUM_VEHICLES];  // Car and pickup owners
    pthread_t car_attendant_thread, pickup_attendant_thread;    // Car and pickup attendants
    int vehicle_types[NUM_VEHICLES];    // 0 for car, 1 for pickup
    int car_type = 0;
    int pickup_type = 1;

    // Initialize semaphores
    if (sem_init(&newPickup, 0, 0) == -1) {
        perror("Failed to initialize semaphore newPickup");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&inChargeforPickup, 0, 0) == -1) {
        perror("Failed to initialize semaphore inChargeforPickup");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&newAutomobile, 0, 0) == -1) {
        perror("Failed to initialize semaphore newAutomobile");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&inChargeforAutomobile, 0, 0) == -1) {
        perror("Failed to initialize semaphore inChargeforAutomobile");
        exit(EXIT_FAILURE);
    }

    // Create car and pickup attendant threads
    if (pthread_create(&car_attendant_thread, NULL, carAttendant, &car_type) != 0) {
        perror("Failed to create car attendant thread");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&pickup_attendant_thread, NULL, carAttendant, &pickup_type) != 0) {
        perror("Failed to create pickup attendant thread");
        exit(EXIT_FAILURE);
    }

    // Simulate vehicle arrivals
    srand(time(NULL));
    for (int i = 0; i < NUM_VEHICLES; i++) {
        vehicle_types[i] = rand() % 2;
        if (pthread_create(&owner_threads[i], NULL, carOwner, &vehicle_types[i]) != 0) {
            perror("Failed to create owner thread");
            exit(EXIT_FAILURE);
        }
        sleep(rand() % 2); // Random arrival time
    }

    // Join owner threads
    for (int i = 0; i < NUM_VEHICLES; i++) {
        if (pthread_join(owner_threads[i], NULL) != 0) {
            perror("Failed to join owner thread");
            exit(EXIT_FAILURE);
        }
    }

    // Signal attendants to stop
    stop = 1;
    sem_post(&newAutomobile); // Wake up car attendant if waiting
    sem_post(&newPickup); // Wake up pickup attendant if waiting

    // Join attendant threads
    if (pthread_join(car_attendant_thread, NULL) != 0) {
        perror("Failed to join car attendant thread");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(pickup_attendant_thread, NULL) != 0) {
        perror("Failed to join pickup attendant thread");
        exit(EXIT_FAILURE);
    }

    // Destroy semaphores
    if (sem_destroy(&newPickup) == -1) {
        perror("Failed to destroy semaphore newPickup");
    }
    if (sem_destroy(&inChargeforPickup) == -1) {
        perror("Failed to destroy semaphore inChargeforPickup");
    }
    if (sem_destroy(&newAutomobile) == -1) {
        perror("Failed to destroy semaphore newAutomobile");
    }
    if (sem_destroy(&inChargeforAutomobile) == -1) {
        perror("Failed to destroy semaphore inChargeforAutomobile");
    }

    return 0;
}
