CC = gcc
CFLAGS = -Wall -O2

.PHONY: all clean

# Targets
all: myuart_run gameselection

myuart_run: myuart_run.c
	$(CC) $(CFLAGS) -o $@ $<

gameselection: gameselection.c
	$(CC) $(CFLAGS) -o $@ $< -lncurses

run: myuart_run gameselection
	./myuart_run ./gameselection

clean:
	rm -f myuart_run gameselection
