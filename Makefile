

VERSION=0.1
CC=cc
CFLAGS=-c -O2

all: btree.o list.o log.o crc.o hash.o memcheck.o
	ar rv libelementals.a btree.o list.o log.o crc.o hash.o memcheck.o
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

memcheck.o: memcheck.c memcheck.h log.o
	$(CC) $(CFLAGS) memcheck.c

testlist: test_list.c list.c list.h log.o memcheck.c
	$(CC) $(CFLAGS) -DUSE_MEMCHECK memcheck.c
	$(CC) $(CFLAGS) -DUSE_MEMCHECK list.c
	$(CC) $(CFLAGS) -DUSE_MEMCHECK test_list.c
	$(CC) -o test_list test_list.o list.o log.o memcheck.o
	./test_list

testhash: test_hash.o hash.c hash.h log.o memcheck.o list.o crc.o
	$(CC) $(CFLAGS) -DUSE_MEMCHECK hash.c
	$(CC) $(CFLAGS) -DUSE_MEMCHECK test_hash.c
	$(CC) -o test_hash test_hash.o hash.o log.o memcheck.o list.o crc.o
	./test_hash

clean:
	rm -f *~ *.o *.a test_list test_hash

