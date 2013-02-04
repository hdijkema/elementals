

VERSION=0.1
CC=cc
CFLAGS=-c -O2

all: btree.o list.o log.o crc.o hash.o
	ar rv libelementals.a btree.o list.o log.o crc.o hash.o
	ranlib libelementals.a

btree.o: btree.c btree.h log.o
	$(CC) $(CFLAGS) btree.c


list.o: list.c list.h log.o
	$(CC) $(CFLAGS) list.c

log.o: log.c log.h
	$(CC) $(CFLAGS) log.c

crc.o: crc.c crc.h
	$(CC) $(CFLAGS) crc.c

hash.o: hash.c hash.h log.o
	$(CC) $(CFLAGS) hash.c

testlist: test_list.o all
	$(CC) -o test_list test_list.c -L. -lelementals
	./test_list

testhash: test_hash.o all
	$(CC) -o test_hash test_hash.c -L. -lelementals
	./test_hash



clean:
	rm -f *~ *.o *.a test_list test_hash

