#include <stdio.h>
#include <pthread.h>
#include "game.h"
#include "input.h"
#include "network.h"

#define TOTAL_BOXES (BOX_ROWS * BOX_COLS)

typedef struct {
    int sock;
    Move move;
    int has_move;
} NetworkData;

NetworkData net_data;

static char current_player_symbol(const Game *game) {
    return (game->current_player == 0) ? 'A' : 'B';
}

void* receive_thread(void* arg) {
    NetworkData* data = (NetworkData*)arg;

    while (1) {
        int buffer[4];
        if (receive_move(data->sock, buffer) <= 0)
            break;

        data->move.row1 = buffer[0];
        data->move.col1 = buffer[1];
        data->move.row2 = buffer[2];
        data->move.col2 = buffer[3];

        data->has_move = 1;
    }

    return NULL;
}

void init_game(Game *game) {
    if (!game) return;

    init_board(&game->board);
    game->current_player = 0;
    game->scoreA = 0;
    game->scoreB = 0;
    game->claimed_boxes = 0;
}

void switch_player(Game *game) {
    game->current_player = 1 - game->current_player;
}

int is_game_over(const Game *game) {
    return game->claimed_boxes >= TOTAL_BOXES;
}

void declare_winner(const Game *game) {
    printf("\nFinal Scores:\nA: %d\nB: %d\n", game->scoreA, game->scoreB);

    if (game->scoreA > game->scoreB)
        printf("Winner: Player A\n");
    else if (game->scoreB > game->scoreA)
        printf("Winner: Player B\n");
    else
        printf("It's a tie!\n");
}

void run_game(Game *game, int sock) {
    pthread_t thread;

    net_data.sock = sock;
    net_data.has_move = 0;

    pthread_create(&thread, NULL, receive_thread, &net_data);

    while (!is_game_over(game)) {
        print_board(&game->board);

        char sym = current_player_symbol(game);

        printf("\nScores: A=%d, B=%d\n", game->scoreA, game->scoreB);

        Move move;

        if (game->current_player == 0) {
            printf("Your move:\n");

            while (!read_move(&move)) {
                printf("Invalid input, try again:\n");
            }

            int data[4] = {move.row1, move.col1, move.row2, move.col2};
            send_move(sock, data);

        } else {
            printf("Waiting for opponent...\n");

            while (!net_data.has_move);

            move = net_data.move;
            net_data.has_move = 0;

            printf("Opponent move: %d %d %d %d\n",
                   move.row1, move.col1, move.row2, move.col2);
        }

        char errbuf[128];
        if (!is_valid_move(&game->board, &move, errbuf, sizeof(errbuf))) {
            printf("Invalid move: %s\n", errbuf);
            continue;
        }

        int completed = apply_move(&game->board, &move, sym);

        if (game->current_player == 0)
            game->scoreA += completed;
        else
            game->scoreB += completed;

        game->claimed_boxes += completed;

        if (completed == 0)
            switch_player(game);
    }

    declare_winner(game);
}