#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "network.h"

int start_server(int port) {
    int server_fd, client_fd;
    struct sockaddr_in addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 1);

    printf("Waiting for player...\n");
    client_fd = accept(server_fd, NULL, NULL);
    printf("Player connected!\n");

    close(server_fd);
    return client_fd;
}

int connect_to_server(const char *ip, int port) {
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    printf("Connected to server!\n");

    return sock;
}

int send_move(int sock, const int move[4]) {
    return write(sock, move, sizeof(int) * 4);
}

int receive_move(int sock, int move[4]) {
    return read(sock, move, sizeof(int) * 4);
}