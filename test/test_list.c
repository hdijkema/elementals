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

#include "../src/list.c"

/************************************************************
 * log support
 ***********************************************************/

FILE *log_handle() 
{
  return stderr;
}

int log_this_severity() 
{
  return 1;
}

/************************************************************
 * list declaration
 ***********************************************************/

int * int_copy(int *e) 
{
  int *ee=(int *) mc_malloc(sizeof(int));
  *ee=*e;
  return (list_data_t) ee;
}

void int_destroy(int * e) 
{
  int *ee=(int *) e;
  mc_free(ee);
}

DECLARE_LIST(ilist,int);
IMPLEMENT_LIST(ilist,int,int_copy,int_destroy);

/************************************************************
 * testing
 ***********************************************************/

int int_reverse_cmp(int *a, int *b) 
{
  return ((*a>*b) ? -1 : ((*a<*b) ? 1 : 0));
}

int int_cmp(int *a, int *b) {
  return ((*a<*b) ? -1 : ((*a>*b) ? 1 : 0));
}

void plist(ilist *l) {
  printf("plist[%d]: -> ",ilist_count(l));
  int *e=ilist_start_iter(l,LIST_FIRST);
  while(e!=NULL) {
    printf("%d ",*e);fflush(stdout);
    e=ilist_next_iter(l);
  }
  printf("\n");
  printf("plist[%d]: <- ",ilist_count(l));
  e=ilist_start_iter(l,LIST_LAST);
  while(e!=NULL) {
    printf("%d ",*e);fflush(stdout);
    e=ilist_prev_iter(l);
  }
  printf("\n");
}

#define TEST(name,code,l) \
    printf("%s: ",#name);fflush(stdout); \
    { code } \
    printf(" ok\n");fflush(stdout); \
    plist(l);


int main() {

  mc_init();

  list_t *l=ilist_new();
  log_debug2("l=%p",l);

  int i,*e;

  TEST(t1b,i=0;
       ilist_append_iter(l,&i);
      ,l);

  TEST(t1a,for(i=1;i<10;i++) {
         ilist_append_iter(l,&i);
       }
      ,l);

  TEST(t1,ilist_lock(l);
          e=ilist_start_iter(l,LIST_FIRST);
      while (e!=NULL) {
        printf("%d ",*e);
        e=ilist_next_iter(l);
      }
      ilist_unlock(l);
       ,l);

  TEST(t2,e=ilist_iter_at(l,5);
      printf("%d ",*e);
       ,l);


  TEST(t3,
      while(e!=NULL) {
        printf("%d ",*e);
        e=ilist_prev_iter(l);
      }
       ,l);

  TEST(t4,
      ilist_iter_at(l,5);
      i=100;
      ilist_prepend_iter(l,&i);
       ,l);

  TEST(t5,
      ilist_iter_at(l,6);
      i=98;
      ilist_append_iter(l,&i);
       ,l);

  TEST(t6,
      ilist_iter_at(l,6);
      ilist_drop_iter(l);
       ,l);

  TEST(t7,
      ilist_start_iter(l,LIST_FIRST);
      ilist_drop_iter(l);
       ,l);

  TEST(t8,
      ilist_start_iter(l,LIST_LAST);
      ilist_drop_iter(l);
       ,l);

  TEST(t9,
      ilist_iter_at(l,5);
      ilist_drop_iter(l);
      ,l);

  TEST(t20,
      ilist_start_iter(l,LIST_FIRST);
      i=2000;
      ilist_prepend_iter(l,&i);
       ,l);

  TEST(t21,
      ilist_start_iter(l,LIST_FIRST);
      i=2001;
      ilist_append_iter(l,&i);
       ,l);

  TEST(t22,
      ilist_start_iter(l,LIST_LAST);
      i=3001;
      ilist_prepend_iter(l,&i);
      ,l);

  TEST(t23,
      ilist_start_iter(l,LIST_LAST);
      i=3000;
      ilist_append_iter(l,&i);
      ,l);

  TEST(sort,
      ilist_sort(l,int_cmp);
    ,l);

  TEST(revsort,
      ilist_sort(l,int_reverse_cmp);
    ,l);

  TEST(t22,
      ilist_start_iter(l,LIST_LAST);
      i=8000;
      ilist_prepend_iter(l,&i);
      ,l);
  
  TEST(sort,
      ilist_sort(l,int_cmp);
    ,l);
  
  TEST(t30,
      ilist_iter_at(l,4);
      ilist_move_iter(l,LIST_FIRST);
      ,l);

  TEST(t31,
      ilist_iter_at(l,4);
      ilist_move_iter(l,LIST_LAST);
      ,l);

  TEST(t32,
      ilist_start_iter(l,LIST_LAST);
      ilist_move_iter(l,LIST_FIRST);
      ,l);

  TEST(t33,
      ilist_start_iter(l,LIST_LAST);
      ilist_move_iter(l,LIST_LAST);
      ,l);

  TEST(t34,
      ilist_start_iter(l,LIST_FIRST);
      ilist_move_iter(l,LIST_FIRST);
      ,l);

  TEST(t35,
      ilist_start_iter(l,LIST_FIRST);
      ilist_move_iter(l,LIST_LAST);
      ,l);

  TEST(t99,
      while (ilist_start_iter(l,LIST_FIRST)!=NULL) {
        ilist_drop_iter(l);
      }
       ,l);

    ilist_destroy(l);

    return 0;
}
