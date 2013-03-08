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

#ifndef __LIST__HOD
#define __LIST__HOD

#ifndef MEMCHECK_INTERNAL
  #define __list__hod_static
#else
  #define __list__hod_static static
  #ifdef USE_MEMCHECK
    #undef USE_MEMCHECK
  #endif
#endif

#include <pthread.h>
#include <semaphore.h>

typedef void * list_data_t;

typedef struct __list_entry__ {
  struct __list_entry__ *next;
  struct __list_entry__ *previous;
  void * data;
} list_entry_t;

typedef struct {
  pthread_mutex_t *mutex;
  sem_t           *sem;
  list_entry_t    *first;
  list_entry_t    *last;
  list_entry_t    *current;
  int count;
} list_t;

typedef enum { 
  LIST_LAST, 
  LIST_FIRST 
} list_pos_t;

__list__hod_static list_t *  _list_new();
__list__hod_static void      _list_destroy(list_t *,void (*data_destroyer)(list_data_t v));
__list__hod_static int       _list_length(list_t *);
__list__hod_static void      _list_lock(list_t *);
__list__hod_static void      _list_unlock(list_t *);
__list__hod_static void      _list_sort(list_t *, int (*cmp) (list_data_t a, list_data_t b));

__list__hod_static list_data_t _list_start_iter(list_t *,list_pos_t pos);
__list__hod_static list_data_t _list_iter_at(list_t *,int i);
__list__hod_static list_data_t _list_next_iter(list_t *);
__list__hod_static list_data_t _list_prev_iter(list_t *);

__list__hod_static void         _list_drop_iter(list_t *,void (*data_destroyer)(list_data_t v));
__list__hod_static void         _list_prepend_iter(list_t *,list_data_t data);
__list__hod_static void         _list_append_iter(list_t *,list_data_t data);
__list__hod_static void         _list_move_iter(list_t *,list_pos_t pos);

#define _DECLARE_LIST(MODIFIER, NAME,T) \
  typedef list_t  NAME; \
  MODIFIER NAME * NAME##_new(); \
  MODIFIER inline void     NAME##_destroy(NAME *l); \
  MODIFIER inline int      NAME##_length(NAME *l); \
  MODIFIER inline int      NAME##_count(NAME *l); \
  MODIFIER inline void     NAME##_sort(NAME *l,int (*cmp)(T *a,T *b)); \
  MODIFIER inline T *      NAME##_start_iter(NAME *l,list_pos_t p); \
  MODIFIER inline T *      NAME##_next_iter(NAME *l); \
  MODIFIER inline T *      NAME##_prev_iter(NAME *l); \
  MODIFIER inline void     NAME##_drop_iter(NAME *l); \
  MODIFIER inline void     NAME##_prepend_iter(NAME *l,T *e); \
  MODIFIER inline void     NAME##_append_iter(NAME *l, T *e); \
  MODIFIER inline T *      NAME##_iter_at(NAME *l, int i); \
  MODIFIER inline void     NAME##_move_iter(NAME *l,list_pos_t pos); \
  MODIFIER inline void     NAME##_lock(NAME *l); \
  MODIFIER inline void     NAME##_unlock(NAME *l);

#define _IMPLEMENT_LIST(MODIFIER, NAME, T, COPY, DESTROY) \
  MODIFIER inline NAME * NAME##_new() { return _list_new(); } \
  MODIFIER inline void     NAME##_destroy(NAME *l) { _list_destroy(l,(void (*)(list_data_t)) DESTROY); } \
  MODIFIER inline int      NAME##_length(NAME *l) { return _list_length(l); } \
  MODIFIER inline int      NAME##_count(NAME *l) { return _list_length(l); } \
  MODIFIER inline T *      NAME##_start_iter(NAME *l,list_pos_t p) { return (T *) _list_start_iter(l,p); } \
  MODIFIER inline T *      NAME##_next_iter(NAME *l) { return (T *) _list_next_iter(l); } \
  MODIFIER inline T *      NAME##_prev_iter(NAME *l) { return (T *) _list_prev_iter(l); } \
  MODIFIER inline void     NAME##_sort(NAME *l, int (*cmp)(T *a,T *b)) { \
                                    _list_sort(l, (int (*)(list_data_t,list_data_t)) cmp); \
  } \
  MODIFIER inline void     NAME##_drop_iter(NAME *l) { _list_drop_iter(l,(void (*)(list_data_t)) DESTROY); } \
  MODIFIER inline void     NAME##_prepend_iter(NAME *l,T *e) { _list_prepend_iter(l,(list_data_t) COPY(e)); } \
  MODIFIER inline void     NAME##_append_iter(NAME *l, T *e) { _list_append_iter(l,(list_data_t) COPY(e)); } \
  MODIFIER inline T *      NAME##_iter_at(NAME *l, int i) { return (T *) _list_iter_at(l,i); } \
  MODIFIER inline void     NAME##_move_iter(NAME *l,list_pos_t pos) { _list_move_iter(l,pos); } \
  MODIFIER inline void     NAME##_lock(NAME *l) { _list_lock(l); } \
  MODIFIER inline void     NAME##_unlock(NAME *l) { _list_unlock(l); }


#define DECLARE_LIST(NAME, T) _DECLARE_LIST( ,NAME, T)
#define IMPLEMENT_LIST(NAME, T, COPY, DESTROY) _IMPLEMENT_LIST( ,NAME, T, COPY, DESTROY)

#define STATIC_DECLARE_LIST(NAME, T) _DECLARE_LIST(static ,NAME, T)
#define STATIC_IMPLEMENT_LIST(NAME, T, COPY, DESTROY) _IMPLEMENT_LIST(static ,NAME, T, COPY, DESTROY)


#endif
