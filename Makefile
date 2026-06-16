CC := gcc
CFLAGS := -Wall -Wpedantic -std=c17 -lc

all: main
	./$^ 2026-07-11-00:00:00

main: main.c
	$(CC) -o $@ $^ $(CFLAGS)
