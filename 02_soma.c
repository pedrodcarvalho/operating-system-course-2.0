#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 10

int array[N];

void *fill_array(void *arg)
{
    for (int i = 0; i < N; i++) {
        array[i] = rand() % 21 - 10;
    }

    pthread_exit(NULL);
}

void *sum_array(void *arg)
{
    int sum = 0;

    for (int i = 0; i < N; i++) {
        sum += array[i];
    }

    printf("Sum: %d\n", sum);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t tid1, tid2;
    srand(time(NULL));

    pthread_create(&tid1, NULL, fill_array, NULL);
    pthread_join(tid1, NULL);

    pthread_create(&tid2, NULL, sum_array, NULL);
    pthread_join(tid2, NULL);

    return 0;
}

// Compile: gcc 02_soma.c -o 02_soma -lpthread
