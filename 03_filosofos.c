#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define N 5

pthread_mutex_t forks[N];

void *philosopher(void *arg)
{
    int id = *((int *)arg);
    int left = id;
    int right = (id + 1) % N;

    while (1) {
        printf("Philosopher %d is thinking\n", id);
        sleep(rand() % 3 + 1);

        printf("Philosopher %d is hungry\n", id);
        pthread_mutex_lock(&forks[left]);
        pthread_mutex_lock(&forks[right]);

        printf("Philosopher %d is eating\n", id);
        sleep(rand() % 3 + 1);

        pthread_mutex_unlock(&forks[left]);
        pthread_mutex_unlock(&forks[right]);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t philosophers[N];
    int ids[N];
    srand(time(NULL));

    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(philosophers[i], NULL);
    }

    return 0;
}

// Compile: 03_filosofos.c -o 03_filosofos -lpthread
