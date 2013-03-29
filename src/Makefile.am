## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(LIBPCRE_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

lib_LTLIBRARIES = libelementals.la


libelementals_la_SOURCES =  \
	crc.c \
	hash.c \
	set.c \
	list.c \
	log.c \
	memblock.c \
	array.c \
	memcheck.c \
	fileinfo.c \
	psem.c \
	regexp.c

libelementals_la_LDFLAGS = -pthread

libelementals_la_LIBADD =  \
	$(LIBPCRE_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = elementals.pc

EXTRA_DIST = \
	elementals.pc.in

include_HEADERS = elementals.h 
	
otherincludedir = $(includedir)/elementals
otherinclude_HEADERS = elementals/hash.h \
	elementals/set.h \
	elementals/crc.h \
	elementals/list.h \
	elementals/fifo.h \
	elementals/log.h \
	elementals/memcheck.h \
	elementals/memblock.h \
	elementals/array.h \
	elementals/os.h \
	elementals/psem.h \
	elementals/fileinfo.h \
	elementals/types.h \
	elementals/regexp.h

