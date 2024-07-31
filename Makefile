CC = gcc
CFLAGS = -ggdb -Wall -fsanitize=address

servidor:
	$(CC) $(CFLAGS) main.c -o main.out
