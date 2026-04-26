#include <stdio.h>
#include <string.h>
#include "board.h"

typedef enum {
    MOVE_KIND_INVALID = 0,
    MOVE_KIND_HORIZONTAL,
    MOVE_KIND_VERTICAL
} MoveKind;

static int in_range(int v, int lo, int hi) {
    return (v >= lo && v <= hi);
}

static int abs_int(int x) {
    return (x < 0) ? -x : x;
}


static MoveKind classify_move(const Move *m, int *edge_row, int *edge_col) {
    if (m == NULL) {
        return MOVE_KIND_INVALID;
    }

    if ((m->row1 == m->row2) && (abs_int(m->col1 - m->col2) == 1)) {
        if (edge_row != NULL) *edge_row = m->row1;
        if (edge_col != NULL) *edge_col = (m->col1 < m->col2) ? m->col1 : m->col2;
        return MOVE_KIND_HORIZONTAL;
    }

    if ((m->col1 == m->col2) && (abs_int(m->row1 - m->row2) == 1)) {
        if (edge_row != NULL) *edge_row = (m->row1 < m->row2) ? m->row1 : m->row2;
        if (edge_col != NULL) *edge_col = m->col1;
        return MOVE_KIND_VERTICAL;
    }

    return MOVE_KIND_INVALID;
}

void init_board(Board *board) {
    if (board == NULL) return;

    memset(board->horizontal, 0, sizeof(board->horizontal));
    memset(board->vertical, 0, sizeof(board->vertical));
    for (int r = 0; r < BOX_ROWS; r++) {
        for (int c = 0; c < BOX_COLS; c++) {
            board->owners[r][c] = '\0';
        }
    }
}

int is_horizontal_move(const Move *m) {
    return classify_move(m, NULL, NULL) == MOVE_KIND_HORIZONTAL;
}

int is_vertical_move(const Move *m) {
    return classify_move(m, NULL, NULL) == MOVE_KIND_VERTICAL;
}

static int is_box_complete(const Board *board, int box_r, int box_c) {
    return board->horizontal[box_r][box_c] &&       /* top */
           board->horizontal[box_r + 1][box_c] &&   /* bottom */
           board->vertical[box_r][box_c] &&         /* left */
           board->vertical[box_r][box_c + 1];       /* right */
}

int is_valid_move(const Board *board, const Move *m, char *errbuf, size_t errsz) {
    int edge_row = 0;
    int edge_col = 0;

    if (board == NULL || m == NULL) return 0;
    if (errbuf != NULL && errsz > 0) errbuf[0] = '\0';

    if (!in_range(m->row1, 0, DOT_ROWS - 1) || !in_range(m->row2, 0, DOT_ROWS - 1) ||
        !in_range(m->col1, 0, DOT_COLS - 1) || !in_range(m->col2, 0, DOT_COLS - 1)) {
        if (errbuf != NULL && errsz > 0) {
            snprintf(errbuf, errsz, "coordinates out of range");
        }
        return 0;
    }

    if (m->row1 == m->row2 && m->col1 == m->col2) {
        if (errbuf != NULL && errsz > 0) {
            snprintf(errbuf, errsz, "same dot used twice");
        }
        return 0;
    }

    MoveKind kind = classify_move(m, &edge_row, &edge_col);
    if (kind == MOVE_KIND_INVALID) {
        if (errbuf != NULL && errsz > 0) {
            snprintf(errbuf, errsz, "dots are not adjacent horizontally/vertically");
        }
        return 0;
    }

    if (kind == MOVE_KIND_HORIZONTAL) {
        if (board->horizontal[edge_row][edge_col]) {
            if (errbuf != NULL && errsz > 0) {
                snprintf(errbuf, errsz, "that horizontal edge is already drawn");
            }
            return 0;
        }
    } else { /* vertical */
        if (board->vertical[edge_row][edge_col]) {
            if (errbuf != NULL && errsz > 0) {
                snprintf(errbuf, errsz, "that vertical edge is already drawn");
            }
            return 0;
        }
    }

    return 1;
}

static int check_completed_boxes_for_edge(Board *board, MoveKind kind, int edge_row, int edge_col, char player_symbol) {
    int completed = 0;

    if (kind == MOVE_KIND_HORIZONTAL) {
        int r = edge_row;
        int c = edge_col;

        /* Bottom edge of box above (r-1,c) */
        if (r > 0) {
            int br = r - 1;
            int bc = c;
            if (board->owners[br][bc] == '\0' && is_box_complete(board, br, bc)) {
                board->owners[br][bc] = player_symbol;
                completed++;
            }
        }

        /* Top edge of box below (r,c) */
        if (r < BOX_ROWS) {
            int br = r;
            int bc = c;
            if (board->owners[br][bc] == '\0' && is_box_complete(board, br, bc)) {
                board->owners[br][bc] = player_symbol;
                completed++;
            }
        }
    } else if (kind == MOVE_KIND_VERTICAL) {
        int r = edge_row;
        int c = edge_col;

        /* Right edge of box on the left (r,c-1) */
        if (c > 0) {
            int br = r;
            int bc = c - 1;
            if (board->owners[br][bc] == '\0' && is_box_complete(board, br, bc)) {
                board->owners[br][bc] = player_symbol;
                completed++;
            }
        }

        /* Left edge of box on the right (r,c) */
        if (c < BOX_COLS) {
            int br = r;
            int bc = c;
            if (board->owners[br][bc] == '\0' && is_box_complete(board, br, bc)) {
                board->owners[br][bc] = player_symbol;
                completed++;
            }
        }
    }

    return completed;
}

int apply_move(Board *board, const Move *m, char player_symbol) {
    int edge_row = 0;
    int edge_col = 0;

    if (board == NULL || m == NULL) return 0;

    MoveKind kind = classify_move(m, &edge_row, &edge_col);
    if (kind == MOVE_KIND_INVALID) {
        return 0;
    }

    if (kind == MOVE_KIND_HORIZONTAL) {
        board->horizontal[edge_row][edge_col] = 1;
    } else { /* vertical */
        board->vertical[edge_row][edge_col] = 1;
    }

    return check_completed_boxes_for_edge(board, kind, edge_row, edge_col, player_symbol);
}

int all_boxes_claimed(const Board *board) {
    if (board == NULL) return 1;
    for (int r = 0; r < BOX_ROWS; r++) {
        for (int c = 0; c < BOX_COLS; c++) {
            if (board->owners[r][c] == '\0') return 0;
        }
    }
    return 1;
}

void print_board(const Board *board) {
    if (board == NULL) return;

    printf("\n    ");
    for (int c = 0; c < DOT_COLS; c++) {
        printf("%d", c);
        if (c < DOT_COLS - 1) printf("  ");
    }
    printf("\n");

    for (int r = 0; r < DOT_ROWS; r++) {
        printf("%d |", r);
        for (int c = 0; c < DOT_COLS; c++) {
            printf(".");
            if (c < DOT_COLS - 1) {
                if (board->horizontal[r][c]) {
                    printf("---");
                } else {
                    printf("   ");
                }
            }
        }
        printf("|\n");

        if (r < BOX_ROWS) {
            printf("  |");
            for (int bc = 0; bc < BOX_COLS; bc++) {
                if (board->vertical[r][bc]) {
                    printf("|");
                } else {
                    printf(" ");
                }

                char owner = board->owners[r][bc];
                if (owner == '\0') owner = ' ';
                printf(" %c ", owner);
            }

            if (board->vertical[r][DOT_COLS - 1]) {
                printf("|");
            } else {
                printf(" ");
            }
            printf("|\n");
        }
    }
}
