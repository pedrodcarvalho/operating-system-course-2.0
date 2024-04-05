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
    int len;
} item_t;

void consumidor(item_t *prox, int socket_fd)
{
    int message_length;

    prox->len = read(socket_fd, &message_length, sizeof(int));

    prox->message = (char *)malloc(message_length + 1);

    prox->len = read(socket_fd, prox->message, message_length);

    prox->message[message_length] = '\0';
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

    bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    listen(socket_fd, 5);

    socklen_t addr_len = sizeof(serv_addr);

    int new_socket_fd = accept(socket_fd, (struct sockaddr *)&serv_addr, &addr_len);

    while (1) {
        consumidor(&prox, new_socket_fd);

        if (prox.len <= 0 || prox.message[0] == '\0') {
            break;
        }

        printf("Message received: %s\n", prox.message);

        free(prox.message);
    }

    free(prox.message);
    close(socket_fd);

    return 0;
}
