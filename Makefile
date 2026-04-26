CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -g
LDFLAGS = -pthread

OBJS = main.o game.o board.o input.o network.o

dots_and_boxes: $(OBJS)
	$(CC) $(CFLAGS) -o dots_and_boxes $(OBJS) $(LDFLAGS)

main.o: main.c game.h board.h input.h network.h
	$(CC) $(CFLAGS) -c main.c

game.o: game.c game.h board.h input.h network.h
	$(CC) $(CFLAGS) -c game.c

board.o: board.c board.h
	$(CC) $(CFLAGS) -c board.c

input.o: input.c input.h board.h
	$(CC) $(CFLAGS) -c input.c

network.o: network.c network.h
	$(CC) $(CFLAGS) -c network.c

.PHONY: run clean

run: dots_and_boxes
	./dots_and_boxes

clean:
	rm -f $(OBJS) dots_and_boxes