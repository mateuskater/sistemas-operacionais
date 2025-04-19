# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Target executable
TARGET = test

# Source and object files
SRCS = /*.c
OBJS = /*.c, /*.o, $(SRCS)

# Default target
all: $(TARGET)

# Build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

debug: CFLAGS += -DDEBUG

# Clean up build files
clean:
	rm -rf *.o $(TARGET)

# Phony targets
.PHONY: all clean setup