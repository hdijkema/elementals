dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(elementals, 0.46)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC




dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************

LT_INIT


#PKG_CHECK_MODULES(LIBPCRE,libpcre)
AX_PATH_LIB_PCRE

AC_OUTPUT([
Makefile
src/elementals.pc
src/Makefile
test/Makefile
pod/Makefile])
