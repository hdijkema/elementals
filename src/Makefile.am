## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\"

AM_CFLAGS =\
	 -Wall\
	 -g

lib_LTLIBRARIES = libelementals.la


libelementals_la_SOURCES =  \
	crc.c \
	hash.c \
	list.c \
	log.c \
	memcheck.c

libelementals_la_LDFLAGS = 

libelementals_la_LIBADD = 

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = elementals-0.1.pc

EXTRA_DIST = \
	elementals-0.1.pc.in

include_HEADERS = elementals.h 
	
otherincludedir = $(includedir)/elementals
otherinclude_HEADERS = elementals/hash.h \
	elementals/crc.h \
	elementals/list.h \
	elementals/log.h \
	elementals/memcheck.h