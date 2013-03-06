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
#ifndef _HASH_HOD_
#define _HASH_HOD_

#include <pthread.h>
#include <elementals/list.h>
#ifndef MEMCHECK_INTERNAL
  #define __hash__hod_static
#else
  #define __hash__hod_static static
  #ifdef USE_MEMCHECK
    #undef USE_MEMCHECK
  #endif
#endif


typedef void * hash_data_t;

struct __hash_elem__ {
  union {
    struct {
      char **keys;
      hash_data_t *datas;
    } kds;
    struct {
      char *key;
      hash_data_t data;
    } kd;
  };
  int count;
};

typedef struct {
  struct __hash_elem__ *table;
  int table_size;
  int count;
  int collisions;
  int update_count;
  int (*eq)(const char *k1,const char *k2);
  unsigned long (*crc)(const char* key);
  pthread_mutex_t *mutex;
} hash_t;

typedef struct {
  hash_t *hash;
  const char *key;
  hash_data_t data;
  int index;
  int eindex;
  int update_count;
} hash_iter_t;

#define HASH_CASE_SENSITIVE 1
#define HASH_CASE_INSENSITIVE 0

#ifdef MEMCHECK_INTERNAL
STATIC_DECLARE_LIST(hash_key_list,const char);
#else
DECLARE_LIST(hash_key_list,const char);
#endif

typedef int (*hash_key_cmp)(const char *, const char *);

__hash__hod_static hash_t *    _hash_new(int initial_table_size, int case_sensitive);
__hash__hod_static void        _hash_destroy(hash_t *hash, void (*data_destroyer)(hash_data_t));

__hash__hod_static void        _hash_clear(hash_t* hash, void (*data_destroyer)(hash_data_t));

__hash__hod_static void        _hash_put(hash_t *hash, const char *key, hash_data_t data,void (*data_destroyer)(hash_data_t));
__hash__hod_static int         _hash_exists(hash_t *hash, const char *key);
__hash__hod_static hash_data_t _hash_get(hash_t *hash, const char *key);
__hash__hod_static void        _hash_del(hash_t *hash, const char *key, void (*data_destroyer)(hash_data_t));

__hash__hod_static hash_iter_t _hash_iter(hash_t *hash);
__hash__hod_static hash_iter_t _hash_iter_next(hash_iter_t it);
__hash__hod_static int         _hash_iter_end(hash_iter_t it);
__hash__hod_static const char *_hash_iter_key(hash_iter_t it);
__hash__hod_static hash_data_t _hash_iter_data(hash_iter_t it);
__hash__hod_static void        _hash_iter_set_data(hash_iter_t it, hash_data_t data, void (*data_destroyer)(hash_data_t));

__hash__hod_static int         _hash_count(hash_t *hash);
__hash__hod_static int         _hash_table_size(hash_t *hash);
__hash__hod_static int         _hash_collisions(hash_t *hash);

__hash__hod_static hash_key_list * _hash_keys(hash_t *hash);
__hash__hod_static hash_key_cmp    _hash_key_cmp();


#define __DECLARE_HASH(MODIFIER,NAME,T) \
  typedef hash_t  NAME; \
  MODIFIER inline NAME *          NAME##_new(int size,int case_sensitive); \
  MODIFIER inline void            NAME##_destroy(NAME *h); \
  MODIFIER inline void            NAME##_clear(NAME* h); \
  MODIFIER inline void            NAME##_put(NAME *h,const char *key,T *e); \
  MODIFIER inline void            NAME##_del(NAME *h,const char *key); \
  MODIFIER inline T *             NAME##_get(NAME *h,const char *key); \
  MODIFIER inline int             NAME##_exists(NAME *h,const char *key); \
  MODIFIER inline hash_key_list * NAME##_keys(NAME *h); \
  MODIFIER inline hash_key_cmp    NAME##_key_cmp(NAME *h); \
  MODIFIER inline hash_iter_t     NAME##_iter(NAME *h); \
  MODIFIER inline hash_iter_t     NAME##_iter_next(hash_iter_t it); \
  MODIFIER inline const char *    NAME##_iter_key(hash_iter_t it); \
  MODIFIER inline T *             NAME##_iter_data(hash_iter_t it); \
  MODIFIER inline void            NAME##_iter_set_data(hash_iter_t it, T * data); \
  MODIFIER inline int             NAME##_iter_end(hash_iter_t it); \
  MODIFIER inline int             NAME##_count(NAME *h); \
  MODIFIER inline int             NAME##_table_size(NAME *h); \
  MODIFIER inline int             NAME##_collisions(NAME *h);

#define __IMPLEMENT_HASH(MODIFIER,NAME,T,COPY,DESTROY) \
  MODIFIER inline NAME *          NAME##_new(int size, int case_sensitive) { return (NAME *) _hash_new(size,case_sensitive); } \
  MODIFIER inline void            NAME##_destroy(NAME *h) { _hash_destroy((hash_t *) h, (void (*) (void*)) DESTROY); } \
  MODIFIER inline void            NAME##_clear(NAME* h) { _hash_clear((hash_t*) h, (void (*)(void*)) DESTROY); } \
  MODIFIER inline void            NAME##_put(NAME *h, const char *key,T *e) { _hash_put((hash_t *) h,key,COPY(e), (void (*) (void*)) DESTROY); } \
  MODIFIER inline void            NAME##_del(NAME *h, const char *key) { _hash_del((hash_t *) h,key, (void (*) (void*)) DESTROY); } \
  MODIFIER inline T *             NAME##_get(NAME *h, const char *key) { return (T *) _hash_get((hash_t *) h, key); } \
  MODIFIER inline int             NAME##_exists(NAME *h, const char *key) { return _hash_exists((hash_t *) h, key); } \
  MODIFIER inline hash_key_list * NAME##_keys(NAME *h) { return _hash_keys((hash_t *) h); } \
  MODIFIER inline hash_key_cmp    NAME##_key_cmp(NAME *h) { return _hash_key_cmp((hash_t *) h); } \
  MODIFIER inline hash_iter_t     NAME##_iter(NAME *h) { return _hash_iter((hash_t *) h); } \
  MODIFIER inline hash_iter_t     NAME##_iter_next(hash_iter_t it) { return _hash_iter_next(it); } \
  MODIFIER inline int             NAME##_iter_end(hash_iter_t it) { return _hash_iter_end(it); } \
  MODIFIER inline const char *    NAME##_iter_key(hash_iter_t it) { return _hash_iter_key(it); } \
  MODIFIER inline T *             NAME##_iter_data(hash_iter_t it) { return (T *) _hash_iter_data(it); } \
  MODIFIER inline void            NAME##_iter_set_data(hash_iter_t it, T * e) { _hash_iter_set_data(it, COPY(e), (void (*) (void*)) DESTROY); } \
  MODIFIER inline int             NAME##_count(NAME *h) { return _hash_count((hash_t *) h); } \
  MODIFIER inline int             NAME##_table_size(NAME *h) { return _hash_table_size((hash_t *) h); } \
  MODIFIER inline int             NAME##_collisions(NAME *h) { return _hash_collisions((hash_t *) h); }

#define DECLARE_HASH(NAME, T) __DECLARE_HASH( ,NAME, T)
#define IMPLEMENT_HASH(NAME, T, COPY, DESTROY) __IMPLEMENT_HASH( ,NAME, T, COPY, DESTROY)

#define STATIC_DECLARE_HASH(NAME, T) __DECLARE_HASH(static, NAME, T)
#define STATIC_IMPLEMENT_HASH(NAME, T, COPY, DESTROY) __IMPLEMENT_HASH(static, NAME, T, COPY, DESTROY)

#define DECLARE_STATIC_HASH(NAME, T) __DECLARE_HASH(static, NAME, T)
#define IMPLEMENT_STATIC_HASH(NAME, T, COPY, DESTROY) __IMPLEMENT_HASH(static, NAME, T, COPY, DESTROY)

#endif

