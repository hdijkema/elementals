/*
   This file is part of elementals (http://hoesterholt.github.com/elementals/).
   Copyright 2013, Hans Oesterholt <debian@oesterholt.net>

   Elementals are free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Elementals are distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with elementals.  If not, see <http://www.gnu.org/licenses/>.

   ********************************************************************
*/
#ifndef MEMCHECK_H_INCLUDED
#define MEMCHECK_H_INCLUDED

#include <elementals/os.h>
#ifdef EL_OSX
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif
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
  #define mc_malloc(s) malloc(s)
  #define mc_realloc(p,s) realloc(p,s)
  #define mc_calloc(n,s) calloc(n,s)
  #define mc_free(p) free(p)
  #define mc_strdup(a)  strdup(a)
  #define mc_take_control(p,s) p
  #define mc_take_over(p) p

  #define mc_init()
  #define mc_report()
#endif


#endif // MEMCHECK_H_INCLUDED
