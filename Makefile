CC := gcc
SOURCE := $(shell find ./src -name "*.c")
TEST_SOURCE := $(shell find ./test -name "test_*.c")
TEST_BIN := $(TEST_SOURCE:.c=.out)
CFLAGS := -ggdb -Wall -fsanitize=address -std=gnu11
TEST_CFLAGS := -ggdb -Wall -std=gnu11

servidor: $(OBJ)
	$(CC) $(CFLAGS) $(SOURCE) main.c -o main.out
print:

	for f in $(TEST_SOURCE) ; do \
		echo $$f  ; \
	done

.ONESHELL:
run-test: $(TEST_BIN)
	for t in $(TEST_BIN); do \
		$$t & \
	done
	@wait

test/%.out: test/%.c
	$(CC) $(TEST_CFLAGS) $(SOURCE) $< -o $@
