#ifndef BOARD_H
#define BOARD_H

#include <stddef.h>

#define DOT_ROWS 5  /* rows of dots: 0..4 */
#define DOT_COLS 6  /* cols of dots: 0..5 */

#define BOX_ROWS 4  /* rows of boxes: 0..3 */
#define BOX_COLS 5  /* cols of boxes: 0..4 */

/* edges:
   - horizontal[r][c] is the edge between (r,c) and (r,c+1)
     for r in 0..4 and c in 0..4 => [5][5]
   - vertical[r][c] is the edge between (r,c) and (r+1,c)
     for r in 0..3 and c in 0..5 => [4][6]
*/
typedef struct {
    unsigned char horizontal[DOT_ROWS][DOT_COLS - 1]; /* [5][5] */
    unsigned char vertical[BOX_ROWS][DOT_COLS];       /* [4][6] */
    char owners[BOX_ROWS][BOX_COLS];                  /* '\0', 'A', 'B' */
} Board;

typedef struct {
    int row1, col1;
    int row2, col2;
} Move;

void init_board(Board *board);
void print_board(const Board *board);

int is_horizontal_move(const Move *m);
int is_vertical_move(const Move *m);

int is_valid_move(const Board *board, const Move *m, char *errbuf, size_t errsz);

int apply_move(Board *board, const Move *m, char player_symbol);

int all_boxes_claimed(const Board *board);

#endif 
