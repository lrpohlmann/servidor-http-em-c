CC = gcc
SOURCE = $(shell find ./src -name "*.c")
CFLAGS = -ggdb -Wall -fsanitize=address -std=gnu11

servidor: $(OBJ)
	$(CC) $(CFLAGS) $(SOURCE) -o main.out

print:
	for f in $(SOURCE) ; do \
		echo $$f ; \
	done
