#include <stdio.h>
#include "input.h"

static void clear_input_buffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* discard */
    }
}

int read_move(Move *out_move) {
    if (out_move == NULL) return 0;

    int r1, c1, r2, c2;
    int matched = scanf("%d %d %d %d", &r1, &c1, &r2, &c2);

    if (matched != 4) {
        printf("Invalid input format. Expected: row1 col1 row2 col2\n");
        clear_input_buffer();
        return 0;
    }

    out_move->row1 = r1;
    out_move->col1 = c1;
    out_move->row2 = r2;
    out_move->col2 = c2;

    return 1;
}
