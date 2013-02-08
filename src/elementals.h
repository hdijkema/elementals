#ifndef _ELEMENTALS_H
#define _ELEMENTALS_H

#ifdef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
  #define EL_OSX
#else
 #ifdef __linux
   #define EL_LINUX
 #endif
#endif

#include <elementals/log.h>
#include <elementals/memcheck.h>
#include <elementals/list.h>
#include <elementals/hash.h>
#include <elementals/crc.h>

#endif
