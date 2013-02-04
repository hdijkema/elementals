
VERSION=0.1
CC=cc
CFLAGS=-c -O2

all: btree.o list.o log.o 
	ar rv libelementals.a btree.o list.o log.o
	ranlib libelementals.a

btree.o: btree.c btree.h log.o
	$(CC) $(CFLAGS) btree.c


list.o: list.c list.h log.o
	$(CC) $(CFLAGS) list.c

log.o: log.c log.h
	$(CC) $(CFLAGS) log.c

test: test_list.o all
	$(CC) -o test_list test_list.c -L. -lelementals

clean:
	rm -f *~ *.o *.a test_list

