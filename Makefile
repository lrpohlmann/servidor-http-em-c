CC := gcc
SOURCE := $(shell find ./src -name "*.c")
TEST_SOURCE := $(shell find ./test -name "*.c")
TEST_BIN := $(TEST_SOURCE:.c=.out)
CFLAGS := -ggdb -Wall -fsanitize=address -std=gnu11
TEST_CFLAGS := -ggdb -Wall -std=gnu11

servidor: $(OBJ)
	$(CC) $(CFLAGS) $(SOURCE) main.c -o main.out

test: $(TEST_BIN)
	$(foreach file,$(TEST_SOURCE),echo Processing $(file);)

test/%.out: test/%.c
	$(CC) $(TEST_CFLAGS) $(SOURCE) $< -o $@
