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
#include <elementals.h>
#include <stdio.h>

#include "../src/hash.c"

FILE *log_handle() {
  return stderr;
}

int log_this_severity(int level) {
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
  while (!ihash_iter_end(it)) {
      printf("%s=%d (%d,%d), ",ihash_iter_key(it),*ihash_iter_data(it),it.index,it.eindex);
      it=ihash_iter_next(it);
  }
  printf("\n");
  printf("Statistics: elements: %d, collisions: %d, table size: %d\n",
              ihash_count(h), ihash_collisions(h), ihash_table_size(h) );
}

void plist(hash_key_list *l) {
  printf("plist[%d]: -> ",hash_key_list_count(l));
  const char *e=hash_key_list_start_iter(l,LIST_FIRST);
  while(e!=NULL) {
    printf("%s ",e);fflush(stdout);
    e=hash_key_list_next_iter(l);
  }
  printf("\n");
  printf("plist[%d]: <- ",hash_key_list_count(l));
  e=hash_key_list_start_iter(l,LIST_LAST);
  while(e!=NULL) {
    printf("%s ",e);fflush(stdout);
    e=hash_key_list_prev_iter(l);
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

    TEST(putloop1,
            for(i=3000;i<4020;i++) {
                char I[20];
                sprintf(I,"%03d",i);
                int k=i*2;
                ihash_put(h,I,&k);
            }
        ,h);

    TEST(del,ihash_del(h,key);,h);

    TEST(del0,
         for(i=3000;i<4016;i++) {
             char I[20];
             sprintf(I,"%03d",i);
             ihash_del(h,I);
         }
         ,h);


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

    TEST(del1, ihash_del(h,"one");, h);

    hash_key_list *l;
    TEST(keys,
         {
           l=ihash_keys(h);
           plist(l);
         }
        ,h);

    TEST(skeys,
         {
           hash_key_list_sort(l,ihash_key_cmp(h));
           plist(l);
         }
         ,h);
  
    hash_key_list_destroy(l);

    ihash_destroy(h);

    return 0;
}
