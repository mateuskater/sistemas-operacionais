CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = pingpong-scheduler.c ppos_core.c queue.c
OBJS = queue.o ppos_core.o test.o

all: test

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

clean:
	rm -rf *.o test

.PHONY: all clean setup