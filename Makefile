CC := gcc
SOURCE := $(shell find ./src -name "*.c")
TEST_SOURCE := $(shell find ./test -name "*.c")
CFLAGS := -ggdb -Wall -fsanitize=address -std=gnu11

servidor: $(OBJ)
	$(CC) $(CFLAGS) $(SOURCE) main.c -o main.out

test/%.out: test/%.c
	$(CC) $(SOURCE) $< -o $@
