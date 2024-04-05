/*
 *
 * a) Na teoria, quando o Produtor está em execução e produzindo itens e o Consumidor não está em execução, os itens produzidos pelo Produtor ficariam
 * na fila de mensagens esperando para serem consumidas em implementações mais avançadas de IPC por Sockets. Porém no caso deste código, já que apenas o
 * Consumidor é quem inicia a conexão, o Produtor apenas ira finalizar e não conseguira enviar nada. E no caso inverso, se o Consumidor estiver em
 * execução e o Produtor não estiver, o Consumidor ficará bloqueado na função de leitura esperando eternamente pois não haverá nenhuma mensagem para
 * ser lida. Este é o comportamento esperado em conexões Socket pois a natureza dele é de comunicação síncrona em que ambos os lados devem estar em
 * execução para que a comunicação ocorra.
 *
 * b) A implementação do código não permite que mais de uma instância de processos Produtor e/ou Consumidor estejam executando simultaneamente, pois
 * o Consumidor é quem abre a conexão e o Produtor apenas se conecta a ele. Assim, o código não suporta múltiplas conexões ou processos concorrentes.
 * Para permitir a execução concorrente de mais instâncias de Produtor e/ou Consumidor, seria necessário que o Produtor também fosse capaz de abrir uma
 * conexão, e também suporte para múltiplas conexões deveria ser desenvolvido. (Possivelmente threads para lidar com cada conexão de forma independente).
 * No entanto, coordenação e sincronização adequadas seriam necessárias para garantir a integridade dos dados e evitar Race Conditions, como por exemplo
 * o uso de semáforos ou mutexes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUMBER 8080

typedef struct {
    char *message;
    char *buffer;
    int len;
} item_t;

void produtor(item_t *prox, int socket_fd)
{
    prox->len = strlen(prox->buffer);

    prox->message = (char *)malloc(prox->len + 1);

    strcpy(prox->message, prox->buffer);

    // This first write sends the size of the message
    write(socket_fd, &(prox->len), sizeof(int));

    // This second write sends the message itself
    write(socket_fd, prox->message, prox->len);

    free(prox->message);

    if (prox->len == 0) {
        int zero_len = 0;

        // This write sends a zero length message to signal the end of the communication
        write(socket_fd, &zero_len, sizeof(int));
    }
}

int main()
{
    item_t prox;

    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT_NUMBER);

    int is_socket_open = connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (is_socket_open < 0) {
        printf("Error! Consumer must start the connection first.\n");
        return 1;
    }

    while (1) {
        printf("Enter a message (\"sair\" to exit): ");

        int i = 0;
        while (1) {
            char c = getchar();
            prox.buffer = (char *)realloc(prox.buffer, i + 1);

            if (c == '\n') {
                prox.buffer[i] = '\0';
                break;
            }

            prox.buffer[i] = c;
            i++;
        }

        if (strcmp(prox.buffer, "sair") == 0) {
            break;
        }

        produtor(&prox, socket_fd);
    }

    close(socket_fd);

    return 0;
}
