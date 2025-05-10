CC = gcc
CFLAGS = -Wall -Wextra -g

OBJS_1 = queue.o ppos_core.o test1.o
OBJS_2 = queue.o ppos_core.o test2.o

all: test1 test2

debug: CFLAGS += -DDEBUG
debug: test1 test2

test1: $(OBJS_1)
	$(CC) $(OBJS_1) $(CFLAGS) -o test1

test2: $(OBJS_2)
	$(CC) $(OBJS_2) $(CFLAGS) -o test2

test1.o: pingpong-contab.c 
	$(CC) -c pingpong-contab.c -o test1.o $(CFLAGS)

test2.o: pingpong-contab-prio.c
	$(CC) -c pingpong-contab-prio.c -o test2.o $(CFLAGS)

ppos_core.o: ppos_core.c ppos_data.h ppos.h
	$(CC) -c ppos_core.c $(CFLAGS)

queue.o: queue.c queue.h
	$(CC) -c queue.c $(CFLAGS)

clean:
	rm -rf *.o test1 test2

.PHONY: all clean setup