CC := gcc
CFLAGS := -Wall -Wpedantic -std=c17 -lc

all: main
	./$^ test_file.txt 

main: main.c
	$(CC) -o $@ $^ $(CFLAGS)
