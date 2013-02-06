#ifndef _HASH_HOD
#define _HASH_HOD

#include "crc.h"
#include "hash.h"
#include "log.h"
#include <string.h>
#include <malloc.h>

#include "memcheck.h"

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

static void phash(hash_t *h) {
    printf("phash[%d,%d,%.2lf]: ",hash_count(h),hash_table_size(h),hash_collision_stat(h));fflush(stdout);
	hash_iter_t it=hash_iter(h);
	while (!hash_iter_end(it)) {
	    printf("%s (%d), ",hash_iter_key(it),hash_iter_index(it));
	    it=hash_iter_next(it);
	}
	printf("\n");
}

/******************************************************************/

static void hash_put1(hash_t *h,const char *key,hash_data_t data,void (*data_destroyer)(hash_data_t));

static int check_resize(hash_t *h,void (*data_destroyer)(hash_data_t)) {
    int resize=0;

    if (h->count>(h->table_size/1.5)) {
        resize=1;
    }

    if (resize) {
        //log_debug2("check_resize: resize=%d",resize);
        int N=h->table_size;
        struct __hash_elem__ *table=h->table;

        int nn=next_prime(N*2);
        h->table=(struct __hash_elem__ *) mc_malloc(sizeof(struct __hash_elem__)*nn);
        int i;
        for(i=0;i<nn;i++) { h->table[i].count=0;h->table[i].keys=NULL;h->table[i].data=NULL; }
        h->count=0;
        h->table_size=nn;

        //log_debug("copying over");
        for(i=0;i<N;i++) {
            struct __hash_elem__ *e=&table[i];
            int k;
            for(k=0;k<e->count;k++) {
                hash_put1(h,e->keys[k],e->data[k],data_destroyer);
                mc_free(e->keys[k]);
            }
            mc_free(e->keys);
            mc_free(e->data);
        }
        mc_free(table);

        //phash(h);

        //log_debug("ready");

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

hash_t *hash_new(int initial_table_size,int case_sensitive) {
    hash_t *h=(hash_t *) mc_malloc(sizeof(hash_t));
    if (h!=NULL) {

        int N=next_prime((initial_table_size<10) ? 10 : initial_table_size);
        h->table=(struct __hash_elem__ *) mc_malloc(sizeof(struct __hash_elem__)*N);
        int i;
        for(i=0;i<N;i++) { h->table[i].count=0;h->table[i].keys=NULL;h->table[i].data=NULL; }
        h->count=0;
        h->table_size=N;

        h->mutex=(pthread_mutex_t *) mc_malloc(sizeof(pthread_mutex_t));
        h->eq=(case_sensitive) ? eq_case_sensitive : eq_case_insensitive;
        pthread_mutex_init(h->mutex,NULL);
        return h;
    } else {
        return NULL;
    }
}

void hash_destroy(hash_t *h,void (*data_destroyer)(hash_data_t)) {
    log_assert(h!=NULL);
    int i,N;
    for(i=0,N=h->table_size;i<N;i++) {
        struct __hash_elem__  *e=&h->table[i];
        int j;
        for(j=0;j<e->count;j++) {
            mc_free(e->keys[j]);
            data_destroyer(e->data[j]);
        }
        mc_free(e->keys);
        mc_free(e->data);
    }
    pthread_mutex_destroy(h->mutex);
    mc_free(h->mutex);
    mc_free(h->table);
    mc_free(h);
}

void hash_put(hash_t *h,const char *key,hash_data_t data,void (*data_destroyer)(hash_data_t)) {
    log_assert(h!=NULL);
    pthread_mutex_lock(h->mutex);
    hash_put1(h,key,data,data_destroyer);
    pthread_mutex_unlock(h->mutex);
}

void hash_put1(hash_t *h,const char *key,hash_data_t data,void (*data_destroyer)(hash_data_t)) {

    int index=str_crc32(key)%h->table_size;
    struct __hash_elem__ *e=&h->table[index];

    int i;
    for(i=0;i<e->count && !h->eq(e->keys[i],key);i++);
    if (i==e->count) {
        e->count+=1;
        e->keys=(char **) mc_realloc(e->keys,sizeof(char *)*e->count);
        e->data=(hash_data_t *) mc_realloc(e->data,sizeof(hash_data_t)*e->count);
        e->keys[e->count-1]=mc_strdup(key);
        e->data[e->count-1]=data;
        h->count+=1;
    } else {
        mc_free(e->keys[i]);
        data_destroyer(e->data[i]);
        e->keys[i]=mc_strdup(key);
        e->data[i]=data;
    }

}

hash_data_t hash_get(hash_t *h,const char *key) {
    log_assert(h!=NULL);
    pthread_mutex_lock(h->mutex);

    int index=str_crc32(key)%h->table_size;
    struct __hash_elem__ *e=&h->table[index];

    hash_data_t result;

    int i;
    for(i=0;i<e->count && !h->eq(e->keys[i],key);i++);
    if (i==e->count) {
        result=NULL;
    } else {
        result=e->data[i];
    }

    pthread_mutex_unlock(h->mutex);

    return result;
}

void hash_del(hash_t *h,const char *key,void (*data_destroyer)(hash_data_t)) {
    log_assert(h!=NULL);
    pthread_mutex_lock(h->mutex);

    int index=str_crc32(key)%h->table_size;
    struct __hash_elem__ *e=&h->table[index];

    int i;
    for(i=0;i<e->count && !h->eq(e->keys[i],key);i++);
    if (i==e->count) {
        // nothing to delete
    } else {
        mc_free(e->keys[i]);
        data_destroyer(e->data[i]);
        for(;i<e->count-1;i++) {
            e->keys[i]=e->keys[i+1];
            e->data[i]=e->data[i+1];
        }
        e->count-=1;
    }

    h->count-=1;

    pthread_mutex_unlock(h->mutex);

}

int hash_exists(hash_t *hash,const char *key) {
    log_assert(hash!=NULL);
    return hash_get(hash,key)!=NULL;
}

int hash_count(hash_t *hash) {
    log_assert(hash!=NULL);
    return hash->count;
}

int hash_table_size(hash_t *hash) {
    log_assert(hash!=NULL);
    return hash->table_size;
}

hash_iter_t hash_iter(hash_t *hash) {
    int i,N;
    for(i=0,N=hash->table_size;i<N && hash->table[i].count==0;++i);
    if (i==N) {
        hash_iter_t iter={hash,NULL,i,0};
        return iter;
    } else {
        //log_debug4("hash_iter: found %d, count=%d, key=%s",i,hash->table[i].count,hash->table[i].keys[0]);
        hash_iter_t iter={hash,hash->table[i].keys[0],i,0};
        return iter;
    }
}

int hash_iter_end(hash_iter_t it) {
    return it.key==NULL;
}

const char *hash_iter_key(hash_iter_t it) {
    return it.key;
}

int hash_iter_index(hash_iter_t it) {
    return it.index;
}

int hash_iter_element(hash_iter_t it) {
    return it.eindex;
}

hash_iter_t hash_iter_next(hash_iter_t it) {
    hash_t *h=it.hash;
    struct __hash_elem__ *e=&h->table[it.index];
    it.eindex+=1;
    if (it.eindex>=e->count) {
        int i;
        int N=h->table_size;
        for(i=it.index+1;i<N && h->table[i].count==0;++i);
        if (i==N) {
            hash_iter_t iter={h,NULL,i,0};
            return iter;
        } else {
            hash_iter_t iter={h,h->table[i].keys[0],i,0};
            return iter;
        }
    } else {
        it.key=e->keys[it.eindex];
        return it;
    }
}

#endif

