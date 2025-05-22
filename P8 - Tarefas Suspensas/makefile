CC = gcc
CFLAGS = -Wall -Wextra -g

OBJS_1 = queue.o ppos_core.o ppos.o
OBJS_2 = queue.o ppos_core.o ppos2.o

all: ppos ppos2

debug: CFLAGS += -DDEBUG
debug: ppos ppos2

ppos: $(OBJS_1)
	$(CC) $(OBJS_1) $(CFLAGS) -o ppos

ppos2: $(OBJS_2)
	$(CC) $(OBJS_2) $(CFLAGS) -o ppos2

ppos.o: main.c 
	$(CC) -c main.c -o ppos.o $(CFLAGS)

ppos2.o: main2.c
	$(CC) -c main2.c -o ppos2.o $(CFLAGS)

ppos_core.o: ppos_core.c ppos_data.h ppos.h
	$(CC) -c ppos_core.c $(CFLAGS)

queue.o: queue.c queue.h
	$(CC) -c queue.c $(CFLAGS)

clean:
	rm -rf *.o ppos ppos2

.PHONY: all clean setup