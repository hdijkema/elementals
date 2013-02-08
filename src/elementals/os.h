#ifndef __ELEMENTALS_OS
#define __ELEMENTALS_OS

#ifdef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
  #define EL_OSX
#else
 #ifdef __linux
   #define EL_LINUX
 #endif
#endif

#endif
