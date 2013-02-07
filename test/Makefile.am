## Process this file with automake to produce Makefile.in


bin_PROGRAMS =  \
	test_hash

test_hash_SOURCES =  \
	test_hash.c

test_hash_CPPFLAGS = -I../src \
	-DUSE_MEMCHECK 

test_hash_CFLAGS = -I../src \
	-DUSE_MEMCHECK 

test_hash_CXXFLAGS = -I../src 

test_hash_LDADD = -lelementals 

test_hashEXTRA_DIST = ../src/libelementals.la 

test_hash_LDFLAGS = -L../src 
