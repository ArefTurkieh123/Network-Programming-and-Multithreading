#include <stdio.h>
#include "game.h"
#include "network.h"

int main() {
    Game game;
    init_game(&game);

    int sock;
    char mode;

    printf("Server (s) or Client (c): ");
    scanf(" %c", &mode);

    if (mode == 's') {
        sock = start_server(8080);
        game.current_player = 0;
    } else {
        char ip[50];
        printf("Enter server IP: ");
        scanf("%s", ip);
        sock = connect_to_server(ip, 8080);
        game.current_player = 1;
    }

    run_game(&game, sock);

    return 0;
}