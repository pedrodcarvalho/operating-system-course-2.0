/*
 *
 * a) Quando o Produtor estiver executando e produzindo itens enquanto o Consumidor não estiver, o Produtor continuará adicionando itens ao buffer
 * compartilhado. Porém, como o Consumidor não está consumindo nenhum item, o buffer compartilhado acabará ficando cheio, fazendo com que o Produtor
 * bloqueie ao tentar adicionar mais itens. Esse comportamento é esperado porque o Produtor foi projetado para aguardar até que haja espaço disponível
 * no buffer antes de adicionar mais itens.
 *
 * Por outro lado, quando o Consumidor estiver executando e consumindo itens enquanto o Produtor não estiver, o Consumidor continuará a remover itens
 * do buffer compartilhado. Se o buffer ficar vazio, o Consumidor bloqueará até que o Produtor adicione mais itens. Esse comportamento também é esperado
 * porque o Consumidor foi projetado para esperar até que haja itens disponíveis no buffer antes de consumi-los.
 *
 * b) O buffer compartilhado pode armazenar até BUFFER_SIZE (que é definido como 16) itens. Este número representa a capacidade máxima do buffer.
 * Sim, este é o número esperado porque corresponde ao valor definido pela constante BUFFER_SIZE.
 *
 * c) Sim, a implementação permite que mais de uma instância de processos Produtor e/ou Consumidor seja executada simultaneamente.
 * Isso ocorre porque a região de memória compartilhada criada por shm_open e mmap é acessível a vários processos.
 * Cada processo pode mapear a região de memória compartilhada em seu espaço de endereço e acessar o buffer compartilhado por meio do ponteiro
 * shared_memory.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "shared.h"

void consumidor(item_t *prox, shared_t *shared)
{
    *prox = shared->buffer[shared->out];
    shared->out = (shared->out + 1) % BUFFER_SIZE;
}

int main()
{
    const char *name = "/buffer_shared_memory";
    const int SIZE = sizeof(shared_t);

    int shm_fd;
    shared_t *shared_memory;

    shm_fd = shm_open(name, O_RDWR, 0666);

    if (shm_fd == -1) {
        perror("Error when opening shared memory!\n");
        exit(1);
    }

    shared_memory = (shared_t *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shared_memory == MAP_FAILED) {
        perror("Error when mapping shared memory!\n");
        exit(1);
    }

    item_t prox;

    while (1) {
        while (shared_memory->in == shared_memory->out) {
            usleep(1000);
        }

        consumidor(&prox, shared_memory);

        if (prox.n == -1) {
            break;
        }

        printf("OUT '%d' to the buffer.\n", prox.n);
    }

    return 0;
}
