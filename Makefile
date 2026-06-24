CC = gcc
CFLAGS = -Wall -Wextra -I native/include -O2
LDFLAGS =

SRC_DIR = native/src
TEST_DIR = native/tests

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)

TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(patsubst $(TEST_DIR)/%.c,%,$(TEST_SRCS))

.PHONY: all clean test

all: $(OBJS) $(TEST_BINS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%: $(TEST_DIR)/%.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test: $(TEST_BINS)
	@for test in $(TEST_BINS); do \
		echo "Running $$test..."; \
		./$$test; \
	done

clean:
	rm -f $(OBJS) $(TEST_BINS)
