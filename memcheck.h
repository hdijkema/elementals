#ifndef MEMCHECK_H_INCLUDED
#define MEMCHECK_H_INCLUDED

#include <malloc.h>
#include <string.h>

#ifdef USE_MEMCHECK
  void *_mc_malloc( size_t size, const char *func, const char *file, int line );
  void *_mc_realloc( void *ptr, size_t size, const char *func, const char *file, int line );
  void *_mc_calloc( size_t num, size_t size, const char *func, const char *file, int line );
  char *_mc_strdup(const char *, const char *func, const char *file, int line );
  void  _mc_free( void * ptr, const char *func, const char *file, int line );
  void  _mc_take_control( void * ptr, size_t size, const char *func, const char *file, int line);
  void *_mc_take_over( void * ptr, const char *func, const char *file, int line);

  #define mc_malloc(s) _mc_malloc(s,__FUNCTION__,__FILE__,__LINE__)
  #define mc_realloc(p,s) _mc_realloc(p,s,__FUNCTION__,__FILE__,__LINE__)
  #define mc_calloc(n,s) _mc_calloc(n,s,__FUNCTION__,__FILE__,__LINE__)
  #define mc_strdup(s) _mc_strdup(s,__FUNCTION__,__FILE__,__LINE__)
  #define mc_free(p) _mc_free(p,__FUNCTION__,__FILE__,__LINE__)
  #define mc_take_control(p,s) _mc_take_control(p,s,__FUNCTION__,__FILE__,__LINE__)
  #define mc_take_over(p) _mc_take_over(p,__FUNCTION__,__FILE__,__LINE__)

  void  mc_init(void);
  void  mc_report(void);
#else
  #define mc_init()
  #define mc_malloc(s) malloc(s)
  #define mc_realloc(p,s) realloc(p,s)
  #define mc_calloc(n,s) calloc(n,s)
  #define mc_free(p) free(p)
  #define mc_report()
  #define mc_strdup(a)
  #define mc_take_control(p,s)
  #define mc_take_over(p)
#endif


#endif // MEMCHECK_H_INCLUDED
