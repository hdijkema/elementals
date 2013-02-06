#ifndef MEMCHECK_H_INCLUDED
#define MEMCHECK_H_INCLUDED

#include <malloc.h>

#ifdef USE_MEMCHECK
  void  mc_init(void);
  void *mc_malloc( size_t size );
  void *mc_realloc( void *ptr, size_t size );
  void *mc_calloc( size_t num, size_t size );
  void  mc_free( void * ptr );
  void  mc_report(void);
#else
  #define mc_init()
  #define mc_malloc(s) malloc(s)
  #define mc_realloc(p,s) realloc(p,s)
  #define mc_calloc(n,s) calloc(n,s)
  #define mc_free(p) free(p)
  #define mc_report()
#endif


#endif // MEMCHECK_H_INCLUDED
