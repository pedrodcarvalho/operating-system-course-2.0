/*
 * A ordem na qual os processos filhos são impressos é embaralhada devido à natureza da execução simultânea.
 * Quando você 'fork' vários processos, não há garantia sobre a ordem em que eles serão executados.
 * O sistema operacional agenda processos com base em vários fatores, como recursos disponíveis, prioridades de processos e algoritmo de agendamento.
 * Como resultado, os processos filhos podem ser executados em qualquer ordem, levando à saída embaralhada.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 5

int main(int argc, char *argv[])
{
    pid_t pid;
    int status;

    for (int i = 0; i < N; i++) {
        pid = fork();

        if (pid < 0) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            char *message;
            asprintf(&message, "eu sou o processo %d", i);

            char exec_child[50];
            snprintf(exec_child, sizeof(exec_child), "./01_msg \"%s\"", message);

            system(exec_child);

            return 0;
        }
    }

    for (int i = 0; i < N; i++) {
        wait(&status);
    }

    return 0;
}
