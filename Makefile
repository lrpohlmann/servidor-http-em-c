CC := gcc
SOURCE := $(shell find ./src -name "*.c")
TEST_SOURCE := $(shell find ./test -name "test_*.c")
TEST_BIN := $(TEST_SOURCE:.c=.out)
CFLAGS := -ggdb -Wall -fsanitize=address -std=gnu11

servidor: $(OBJ)
	$(CC) $(CFLAGS) $(SOURCE) main.c -o main.out
print:

	for f in $(TEST_SOURCE) ; do \
		echo $$f  ; \
	done

run-test: $(TEST_BIN)
	$(foreach t,$(TEST_BIN), $(t) &)

test/%.out: test/%.c
	$(CC) $(CFLAGS) $(SOURCE) $< -o $@
