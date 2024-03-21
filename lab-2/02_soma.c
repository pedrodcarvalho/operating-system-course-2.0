#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

// Array size must be a multiple of the number of processes
#define N 100
#define P 4

void fillVector(int *vector, int start, int end)
{
    srand(getpid());

    for (int i = start; i < end; i++) {
        vector[i] = rand() % 21 - 10;
    }
}

int sumVector(int *vector, int start, int end)
{
    int sum = 0;

    for (int i = start; i < end; i++) {
        sum += vector[i];
    }

    return sum;
}

int main()
{
    int *vector;
    int *sum_vector;
    pid_t pid;
    int shm_fd_fill_vector, shm_fd_sum_vector;

    shm_fd_fill_vector = shm_open("/fill_vector", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd_fill_vector, N * sizeof(int));
    vector = mmap(0, N * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_fill_vector, 0);

    shm_fd_sum_vector = shm_open("/sum_vector", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd_sum_vector, (P / 2) * sizeof(int));
    sum_vector = mmap(0, (P / 2) * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_sum_vector, 0);

    for (int i = 0; i < P; i++) {
        pid = fork();

        if (pid == 0) {
            if (i < P / 2) {
                fillVector(vector, i * (N / (P / 2)), (i + 1) * (N / (P / 2)));
            }
            else {
                sum_vector[i - P / 2] = sumVector(vector, (i - P / 2) * (N / (P / 2)), (i - P / 2 + 1) * (N / (P / 2)));
            }

            exit(0);
        }
        else if (pid < 0) {
            printf("Error creating child process.\n");
            exit(1);
        }
    }

    for (int i = 0; i < P; i++) {
        wait(NULL);
    }

    int final_sum = 0;

    for (int i = 0; i < P / 2; i++) {
        final_sum += sum_vector[i];
    }

    printf("Final sum: %d\n", final_sum);

    munmap(vector, N * sizeof(int));
    munmap(sum_vector, (P / 2) * sizeof(int));
    shm_unlink("/fill_vector");
    shm_unlink("/sum_vector");

    return 0;
}
