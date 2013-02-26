## Process this file with automake to produce Makefile.in


check_bindir =  
check_bin_PROGRAMS =  \
	test_hash \
	test_list \
	test_fifo

test_hash_SOURCES =  \
	test_hash.c

test_hash_CPPFLAGS = -I../src \
	-DUSE_MEMCHECK 

test_hash_CFLAGS = -I../src \
	-DUSE_MEMCHECK 

test_hash_CXXFLAGS = -I../src 

test_hashEXTRA_DIST = ../src/libelementals.la 

test_hash_LDFLAGS = ../src/memcheck.o \
	../src/crc.o \
	../src/list.o -pthread

#test_hashEXTRA_DIST = ../src/memcheck.o \
#	../src/crc.o 
#
#test_hashEXTRA_DIST = ../src/memcheck.o 

test_list_CFLAGS = -I../src \
	-DUSE_MEMCHECK 

test_list_SOURCES =  \
	test_list.c

test_list_CPPFLAGS = -I../src \
	-DUSE_MEMCHECK 

test_list_LDFLAGS = ../src/memcheck.o -pthread

test_fifo_CFLAGS = -I../src \
	-DUSE_MEMCHECK 

test_fifo_SOURCES =  \
	test_fifo.c

test_fifo_CPPFLAGS = -I../src \
	-DUSE_MEMCHECK 

test_fifo_LDFLAGS = ../src/memcheck.o ../src/list.o -pthread
