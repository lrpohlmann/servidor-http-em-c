CC = gcc
CFLAGS = -ggdb -Wall -fsanitize=address -std=gnu11

servidor:
	$(CC) $(CFLAGS) alloc/arena.c http/request.c main.c -o main.out
