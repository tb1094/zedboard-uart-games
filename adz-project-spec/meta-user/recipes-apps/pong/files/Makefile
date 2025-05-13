CC = gcc
CFLAGS = -std=c99
LDFLAGS = -lm -lncurses
PONG_OBJS = pong.c

.PHONY: all clean

all:
	$(CC) $(CFLAGS) `pkg-config --cflags ncurses` -o pong $(PONG_OBJS) $(LDFLAGS)

clean:
	rm -f pong
