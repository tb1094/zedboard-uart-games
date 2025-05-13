# Simple Makefile for the snake game

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lncurses

SRC = dialog.c effects.c events.c fruits.c game.c glog.c highscore.c main.c snake.c status-display.c time-helpers.c
OBJ = $(SRC:.c=.o)
TARGET = snake

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
