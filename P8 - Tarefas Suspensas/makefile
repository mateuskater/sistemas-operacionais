CC = gcc
CFLAGS = -Wall -Wextra -g

OBJS_1 = queue.o ppos_core.o ppos.o
OBJS_2 = queue.o ppos_core.o ppos2.o

all: ppos

debug: CFLAGS += -DDEBUG
debug: ppos

ppos: $(OBJS_1)
	$(CC) $(OBJS_1) $(CFLAGS) -o ppos

ppos.o: main.c 
	$(CC) -c main.c -o ppos.o $(CFLAGS)

ppos_core.o: ppos_core.c ppos_data.h ppos.h
	$(CC) -c ppos_core.c $(CFLAGS)

queue.o: queue.c queue.h
	$(CC) -c queue.c $(CFLAGS)

clean:
	rm -rf *.o ppos

.PHONY: all clean setup