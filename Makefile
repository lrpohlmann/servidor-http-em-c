CC = gcc
CFLAGS = -ggdb -Wall -fsanitize=address

servidor:
	$(CC) $(CFLAGS) libs/stb_ds.h main.c -o main.out
