UTILS_DIR := ../../utils

SRCS := example_allocator.c \
        $(UTILS_DIR)/allocator.c \

OBJS := $(notdir $(SRCS:.c=.o))

CC := gcc
CFLAGS := -I../../.

allocator_example: $(OBJS)
	$(CC) $(OBJS) -o allocator_example

%.o: $(UTILS_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: $(TOOLS_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) allocator_example
