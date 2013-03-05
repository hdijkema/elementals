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

#include <elementals/log.h>
#include <elementals/crc.h>
#include <elementals/hash.h>
#ifndef MEMCHECK_INTERNAL
#include <elementals/memcheck.h>
#endif
#include <string.h>

static const char *copy_key(const char *e)
{
  return mc_strdup(e);
}

static void destroy_key(const char *e) {
  mc_free((void *) e);
}

static int key_cmp(const char *a,const char *b) {
  return strcmp(a,b);
}

#ifdef MEMCHECK_INTERNAL
STATIC_IMPLEMENT_LIST(hash_key_list,const char,copy_key, destroy_key);
#else
IMPLEMENT_LIST(hash_key_list,const char,copy_key, destroy_key);
#endif

/******************************************************************/

static int is_prime(int n) {
    int j;
    for(j=2;(j*j)<n;++j) {
        if ((n%j)==0) { return 0; }
    }
    return 1;
}

static int next_prime(int n) {
    for(;!is_prime(n);++n);
    return n;
}

/******************************************************************/

#define INC(a) a=(a > 100000000) ? 0 : a+1

static void hash_put1(hash_t *h,const char *key,hash_data_t data,void (*data_destroyer)(hash_data_t));

static int check_resize(hash_t *h,void (*data_destroyer)(hash_data_t)) {
  int resize=0;

  if (h->count>(h->table_size/1.5)) {
    resize=1;
  }

  if (resize) {
    int N=h->table_size;
    struct __hash_elem__ *table=h->table;

    int nn=next_prime(N*2);
    h->table=(struct __hash_elem__ *) mc_calloc(nn,sizeof(struct __hash_elem__));
    h->count=0;
    h->table_size=nn;
    h->collisions=0;

    int i;
    for(i = 0;i < N; ++i) {
      struct __hash_elem__ *e=&table[i];
      int k;
      if (e->count == 1) {
        hash_put1(h, e->kd.key, e->kd.data, data_destroyer);
        mc_free(e->kd.key);
      } else if (e->count > 1) {
        for(k=0;k < e->count;k++) {
          hash_put1(h, e->kds.keys[k], e->kds.datas[k], data_destroyer);
          mc_free(e->kds.keys[k]);
        }
        mc_free(e->kds.keys);
        mc_free(e->kds.datas);
      }
    }
    mc_free(table);
  }

  return resize;
}


/******************************************************************/

static int eq_case_sensitive(const char *k1,const char *k2) {
  return strcmp(k1,k2)==0;
}

static int eq_case_insensitive(const char *k1,const char *k2) {
  return strcasecmp(k1,k2)==0;
}

/******************************************************************/

__hash__hod_static int _hash_count(hash_t *h) {
  log_assert(h != NULL);
  return h->count;
}

__hash__hod_static int _hash_collisions (hash_t *h) {
  log_assert(h != NULL);
  return h->collisions;
}

__hash__hod_static int _hash_table_size(hash_t *hash) {
  log_assert(hash != NULL);
  return hash->table_size;
}

/******************************************************************/

__hash__hod_static hash_t *_hash_new(int initial_table_size,int case_sensitive) {
  hash_t *h=(hash_t *) mc_malloc(sizeof(hash_t));
  if (h!=NULL) {
    int N=next_prime((initial_table_size<10) ? 10 : initial_table_size);
    h->table=(struct __hash_elem__ *) mc_calloc(N,sizeof(struct __hash_elem__));
    h->count=0;
    h->collisions=0;
    h->table_size=N;

    h->mutex=(pthread_mutex_t *) mc_malloc(sizeof(pthread_mutex_t));
    h->eq=(case_sensitive) ? eq_case_sensitive : eq_case_insensitive;
    pthread_mutex_init(h->mutex,NULL);
    return h;
  } else {
    return NULL;
  }
}

__hash__hod_static void _hash_destroy(hash_t *h,void (*data_destroyer)(hash_data_t)) {
  log_assert(h!=NULL);
  int i,N;
  for(i=0,N=h->table_size;i<N;i++) {
    struct __hash_elem__  *e=&h->table[i];
    if (e->count==0) {
      // do nothing
    } else if (e->count==1) {
      mc_free(e->kd.key);
      data_destroyer(e->kd.data);
    } else {
      int j;
      for(j=0;j<e->count;j++) {
          mc_free(e->kds.keys[j]);
          data_destroyer(e->kds.datas[j]);
      }
      mc_free(e->kds.keys);
      mc_free(e->kds.datas);
    }
  }
  pthread_mutex_destroy(h->mutex);
  mc_free(h->mutex);
  mc_free(h->table);
  mc_free(h);
}

__hash__hod_static void _hash_put(hash_t *h, const char *key, hash_data_t data,
                            void (*data_destroyer)(hash_data_t))
{
    log_assert(h!=NULL);
    pthread_mutex_lock(h->mutex);
    hash_put1(h,key,data,data_destroyer);
    pthread_mutex_unlock(h->mutex);
}

__hash__hod_static void hash_put1(hash_t *h,const char *key,hash_data_t data,
                             void (*data_destroyer)(hash_data_t))
{

	check_resize (h, data_destroyer);

  int index=str_crc32(key) % h->table_size;
  struct __hash_elem__ *e = &h->table[index];

  if (e->count == 0) {
    e->kd.key = mc_strdup(key);
    e->kd.data = data;
    e->count += 1;
    h->count += 1;
  } else if (e->count == 1) {
    char *o_key = e->kd.key;
    hash_data_t o_data = e->kd.data;
    if (h->eq(o_key,key)) {
      mc_free(e->kd.key);
      e->kd.key = mc_strdup(key);
      data_destroyer(o_data);
      e->kd.data = data;
    } else {
      e->kds.keys = (char **) mc_malloc(sizeof(char *) * 2);
      e->kds.datas = (hash_data_t *) mc_malloc(sizeof(hash_data_t) * 2);
      e->kds.keys[0] = o_key;
      e->kds.datas[0] = o_data;
      e->kds.keys[1] = mc_strdup(key);
      e->kds.datas[1] = data;
      e->count += 1;
      h->count += 1;
      h->collisions += 1;
    }
  } else { // e->count > 1
    int i, N;
    for(i=0, N=e->count;i < N && !h->eq(e->kds.keys[i], key); ++i);
    if (i == N) {
      e->kds.keys=(char **) mc_realloc(e->kds.keys, sizeof(char *) * (N+1));
      e->kds.datas=(hash_data_t *) mc_realloc(e->kds.datas, sizeof(hash_data_t) * (N+1));
      e->kds.keys[N] = mc_strdup(key);
      e->kds.datas[N] = data;
      e->count += 1;
      h->count += 1;
      h->collisions += 1;
    } else {
      mc_free(e->kds.keys[i]);
      e->kds.keys[i] = mc_strdup(key);
      data_destroyer(e->kds.datas[i]);
      e->kds.datas[i] = data;
    }
  }
  INC(h->update_count);
}

__hash__hod_static hash_data_t _hash_get(hash_t *h, const char *key)
{
  log_assert(h!=NULL);
  pthread_mutex_lock(h->mutex);

  int index=str_crc32(key)%h->table_size;
  struct __hash_elem__ *e=&h->table[index];

  hash_data_t result;

  if (e->count == 0) {
    result = NULL;
  } else if (e->count == 1) {
    if (h->eq(e->kd.key, key)) {
      result = e->kd.data;
    } else {
      result = NULL;
    }
  } else {
    int i;
    for(i=0;i<e->count && !h->eq(e->kds.keys[i],key);i++);
    if (i==e->count) {
        result=NULL;
    } else {
        result=e->kds.datas[i];
    }
  }

  pthread_mutex_unlock(h->mutex);

  return result;
}

__hash__hod_static void _hash_del(hash_t *h, const char *key, void (*data_destroyer)(hash_data_t))
{
  log_assert(h!=NULL);
  pthread_mutex_lock(h->mutex);

  int index=str_crc32(key)%h->table_size;
  struct __hash_elem__ *e=&h->table[index];

  if (e->count == 0) {
    // nothing to delete
  } else if (e->count == 1) {
    if (h->eq(e->kd.key,key)) {
      mc_free(e->kd.key);
      data_destroyer(e->kd.data);
      e->count -= 1;
      h->count -= 1;
    } else {
      // not found, cannot delete
    }
  } else { // count > 1
    int i, N;
    for(i=0, N=e->count;i < N && !h->eq(e->kds.keys[i],key); ++i);
    if (i == N) {
      // not found, cannot delete
    } else {

      mc_free(e->kds.keys[i]);
      data_destroyer(e->kds.datas[i]);
      for(N -= 1;i < N; ++i) {
        e->kds.keys[i] = e->kds.keys[i+1];
        e->kds.datas[i] = e->kds.datas[i+1];
      }
      e->count -= 1;
      h->count -= 1;
      h->collisions -= 1;

      if (e->count == 1) {
        char *k = e->kds.keys[0];
        hash_data_t *d = e->kds.datas[0];
        mc_free(e->kds.keys);
        mc_free(e->kds.datas);
        e->kd.key = k;
        e->kd.data = d;
      }
    }
  }

  INC(h->update_count);

  pthread_mutex_unlock(h->mutex);
}

__hash__hod_static int _hash_exists(hash_t *hash,const char *key) {
  log_assert(hash != NULL);
  return _hash_get(hash,key)!=NULL;
}

__hash__hod_static hash_iter_t _hash_iter(hash_t *hash) {
  log_assert(hash != NULL);
  pthread_mutex_lock(hash->mutex);

  int i,N;
  hash_iter_t iter;
  for(i=0,N=hash->table_size;i<N && hash->table[i].count==0;++i);
  if (i==N) {
    hash_iter_t itr={hash, NULL, NULL, i, 0, hash->update_count};
    iter = itr;
  } else {
    struct __hash_elem__ *e = &hash->table[i];
    if (e->count == 1) {
      hash_iter_t itr={hash, hash->table[i].kd.key, hash->table[i].kd.data, i, 0, hash->update_count};
      iter = itr;
    } else {
      hash_iter_t itr={hash, hash->table[i].kds.keys[0], hash->table[i].kds.datas[0], i, 0, hash->update_count};
      iter = itr;
    }
  }
  pthread_mutex_unlock(hash->mutex);
  return iter;
}

__hash__hod_static int _hash_iter_end(hash_iter_t it) {
  if (it.hash->update_count != it.update_count) { log_fatal("hash has been invalidated"); }
  return it.key==NULL;
}

__hash__hod_static const char *_hash_iter_key(hash_iter_t it) {
  if (it.hash->update_count != it.update_count) { log_fatal("hash has been invalidated"); }
  return it.key;
}

__hash__hod_static hash_data_t _hash_iter_data(hash_iter_t it) {
  if (it.hash->update_count != it.update_count) { log_fatal("hash has been invalidated"); }
  return it.data;
}

__hash__hod_static void _hash_iter_set_data(hash_iter_t it, hash_data_t d, void (*destroyer) (hash_data_t)) {
  if (it.hash->update_count != it.update_count) { log_fatal("hash has been invalidated"); }
  hash_t *h = it.hash;
  pthread_mutex_lock(h->mutex);
  destroyer(it.data);
  struct __hash_elem__ *e=&h->table[it.index];
  if (e->count == 1 && it.eindex == 0) {
    e->kd.data = d;
  } else {
    e->kds.datas[it.eindex] = d;
  }
  pthread_mutex_unlock(h->mutex);
}

__hash__hod_static hash_iter_t _hash_iter_next(hash_iter_t it) {
  if (it.hash->update_count != it.update_count) { log_fatal("hash has been invalidated"); }
  hash_t *h=it.hash;
  pthread_mutex_lock(h->mutex);

  struct __hash_elem__ *e=&h->table[it.index];
  it.eindex += 1;
  hash_iter_t iter;

  if (it.eindex >= e->count) {
    int i;
    int N = h->table_size;
    for(i = it.index+1 ;i < N && h->table[i].count == 0; ++i);
    if (i == N) {
      hash_iter_t itr={h, NULL, NULL, i, 0, h->update_count};
      iter=itr;
    } else {
      struct __hash_elem__ *e = &h->table[i];
      if (e->count == 1) {
        hash_iter_t itr={h, h->table[i].kd.key, h->table[i].kd.data, i, 0, h->update_count};
        iter=itr;
      } else {
        hash_iter_t itr={h, h->table[i].kds.keys[0], h->table[i].kds.datas[0], i, 0, h->update_count};
        iter=itr;
      }
    }
  } else {
    it.key  = e->kds.keys[it.eindex];
    it.data = e->kds.datas[it.eindex];
    iter = it;
  }
  pthread_mutex_unlock(h->mutex);
  return iter;
}

__hash__hod_static hash_key_list *_hash_keys(hash_t *h) {
  pthread_mutex_lock(h->mutex);
  hash_key_list *l=hash_key_list_new();
  int i,N;
  hash_key_list_start_iter(l,LIST_LAST);
  for(i=0,N=h->table_size;i<N;++i) {
    struct __hash_elem__ *e=&h->table[i];
    if (e->count==1) {
      hash_key_list_append_iter(l,e->kd.key);
    } else if (e->count>1) {
      int j;
      for(j=0;j<e->count;j++) {
        hash_key_list_append_iter(l,e->kds.keys[j]);
      }
    }
  }
  pthread_mutex_unlock(h->mutex);
  return l;
}

__hash__hod_static hash_key_cmp _hash_key_cmp(hash_t *h) {
  return key_cmp;
}

