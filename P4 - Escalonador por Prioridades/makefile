# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Source and object files
SRCS = pingpong-scheduler.c ppos_core.c queue.c
OBJS = queue.o ppos_core.o test.o

# Default target
all: test

# Build the target executable
test: $(OBJS) $(SRCS)
	$(CC) $(OBJS) $(CFLAGS) -o test

test.o: $(SRCS)
	$(CC) -c pingpong-scheduler.c -o test.o $(CFLAGS)

ppos_core.o: ppos_core.c ppos_data.h ppos.h
	$(CC) -c ppos_core.c -o ppos_core.o $(CFLAGS)

queue.o: queue.c queue.h
	$(CC) -c queue.c -o queue.o $(CFLAGS)


debug: CFLAGS += -DDEBUG
debug: test

# Clean up build files
clean:
	rm -rf *.o test

# Phony targets
.PHONY: all clean setup