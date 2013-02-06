#ifndef _HASH_HOD_
#define _HASH_HOD_

#include <pthread.h>

typedef void * hash_data_t;

struct __hash_elem__ {
    char       **keys;
    hash_data_t *data;
    int          count;
};

typedef struct {
    struct __hash_elem__ *table;
  int            table_size;
  int            count;
  int (*eq)(const char *k1,const char *k2);
  pthread_mutex_t *mutex;
} hash_t;

typedef struct {
    hash_t      *hash;
    const char *key;
    int          index,eindex;
} hash_iter_t;

#define HASH_CASE_SENSITIVE 1
#define HASH_CASE_INSENSITIVE 0

hash_t *    hash_new(int initial_table_size,int case_sensitive);
void        hash_destroy(hash_t *hash,void (*data_destroyer)(hash_data_t));

void        hash_put(hash_t *hash,const char *key,hash_data_t data,void (*data_destroyer)(hash_data_t));
int         hash_exists(hash_t *hash,const char *key);
hash_data_t hash_get(hash_t *hash,const char *key);
void        hash_del(hash_t *hash,const char *key,void (*data_destroyer)(hash_data_t));

hash_iter_t hash_iter(hash_t *hash);
hash_iter_t hash_iter_next(hash_iter_t it);
int         hash_iter_end(hash_iter_t it);
const char *hash_iter_key(hash_iter_t it);
int         hash_iter_index(hash_iter_t it);
int         hash_iter_element(hash_iter_t it);

int         hash_count(hash_t *hash);
double      hash_collision_stat(hash_t *hash);
int         hash_table_size(hash_t *hash);

#define DECLARE_HASH(NAME,T) \
  typedef hash_t  NAME; \
  NAME *        NAME##_new(int size,int case_sensitive); \
  void          NAME##_destroy(NAME *h); \
  void          NAME##_put(NAME *h,const char *key,T *e); \
  void          NAME##_del(NAME *h,const char *key); \
  T *           NAME##_get(NAME *h,const char *key); \
  int           NAME##_exists(NAME *h,const char *key); \
  hash_iter_t   NAME##_iter(NAME *h); \
  int           NAME##_count(NAME *h); \
  int           NAME##_table_size(NAME *h);

#define IMPLEMENT_HASH(NAME,T,COPY,DESTROY) \
  NAME *        NAME##_new(int size,int case_sensitive) { return (NAME *) hash_new(size,case_sensitive); } \
  void          NAME##_destroy(NAME *h) { hash_destroy((hash_t *) h,DESTROY); } \
  void          NAME##_put(NAME *h,const char *key,T *e) { hash_put((hash_t *) h,key,COPY(e),DESTROY); } \
  void          NAME##_del(NAME *h,const char *key) { hash_del((hash_t *) h,key,DESTROY); } \
  T *           NAME##_get(NAME *h,const char *key) { return (T *) hash_get((hash_t *) h,key); } \
  int           NAME##_exists(NAME *h,const char *key) { return hash_exists((hash_t *) h,key); } \
  hash_iter_t   NAME##_iter(NAME *h) { return hash_iter((hash_t *) h); } \
  int           NAME##_count(NAME *h) { return hash_count((hash_t *) h); } \
  int           NAME##_table_size(NAME *h) { return hash_table_size((hash_t *) h); }

#define STATIC_DECLARE_HASH(NAME,T) \
  typedef hash_t  NAME; \
  static NAME *        NAME##_new(int size,int case_sensitive); \
  static void          NAME##_destroy(NAME *h); \
  static void          NAME##_put(NAME *h,const char *key,T *e); \
  static void          NAME##_del(NAME *h,const char *key); \
  static T *           NAME##_get(NAME *h,const char *key); \
  static int           NAME##_exists(NAME *h,const char *key); \
  static hash_iter_t   NAME##_iter(NAME *h); \
  static int           NAME##_count(NAME *h); \
  static int           NAME##_table_size(NAME *h);


#define STATIC_IMPLEMENT_HASH(NAME,T,COPY,DESTROY) \
  static NAME *        NAME##_new(int size,int case_sensitive) { return (NAME *) hash_new(size,case_sensitive); } \
  static void          NAME##_destroy(NAME *h) { hash_destroy((hash_t *) h,DESTROY); } \
  static void          NAME##_put(NAME *h,const char *key,T *e) { hash_put((hash_t *) h,key,COPY(e),DESTROY); } \
  static void          NAME##_del(NAME *h,const char *key) { hash_del((hash_t *) h,key,DESTROY); } \
  static T *           NAME##_get(NAME *h,const char *key) { return (T *) hash_get((hash_t *) h,key); } \
  static int           NAME##_exists(NAME *h,const char *key) { return hash_exists((hash_t *) h,key); } \
  static hash_iter_t   NAME##_iter(NAME *h) { return hash_iter((hash_t *) h); } \
  static int           NAME##_count(NAME *h) { return hash_count((hash_t *) h); } \
  static int           NAME##_table_size(NAME *h) { return hash_table_size((hash_t *) h); }


#endif

