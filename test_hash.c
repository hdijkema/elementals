#include "hash.h"
#include <stdio.h>
#include <malloc.h>
#include "log.h"
#include "memcheck.h"

FILE *log_handle() {
	return stderr;
}

int log_this_severity() {
  return 1;
}

hash_data_t int_copy(int *e) {
    //log_debug("copy");
	int *ee=(int *) mc_malloc(sizeof(int));
	*ee=*e;
	return (hash_data_t) ee;
}

void int_destroy(hash_data_t e) {
	int *ee=(int *) e;
	mc_free(ee);
}

DECLARE_HASH(ihash,int);
IMPLEMENT_HASH(ihash,int,int_copy,int_destroy);

void phash(ihash *h) {
	printf("phash[%d,%d]: ",ihash_count(h),ihash_table_size(h));fflush(stdout);
	hash_iter_t it=ihash_iter(h);
	while (!hash_iter_end(it)) {
	    printf("%s (%d), ",hash_iter_key(it),hash_iter_index(it));
	    it=hash_iter_next(it);
	}
	printf("\n");
}

#define TEST(name,code,h) \
    printf("%s: ",#name);fflush(stdout); \
    { code } \
    printf(" ok\n");fflush(stdout); \
    phash(h);

int main() {
    mc_init();
    ihash *h=ihash_new(10,HASH_CASE_SENSITIVE);

    TEST(new,,h);

    int i=1;
    char *key="one";
    TEST(put,ihash_put(h,key,&i);,h);

    log_debug("one12");
    key="one12";
    i+=1;
    TEST(put,ihash_put(h,key,&i);,h);

    TEST(putloop,
            for(i=8;i<12;i++) {
                char I[20];
                sprintf(I,"%03d",i);
                int k=i*2;
                ihash_put(h,I,&k);
            }
        ,h);

    TEST(del,ihash_del(h,key);,h);

    TEST(putloop2,
            for(i=100;i<200;i++) {
                char I[20];
                sprintf(I,"%03d",i);
                int k=i*2;
                ihash_put(h,I,&k);
            }
        ,h);

    TEST(get,printf("get[150]=%d",*ihash_get(h,"150"));,h);

    int k=393939;
    TEST(put3,ihash_put(h,"150",&k);,h);

    TEST(get,printf("get[150]=%d",*ihash_get(h,"150"));,h);

    TEST(del,
         for(i=100;i<200;i++) {
             char I[20];
             sprintf(I,"%03d",i);
             ihash_del(h,I);
         }
         ,h);

    ihash_destroy(h);

    return 0;
}
