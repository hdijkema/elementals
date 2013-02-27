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

#include "../src/array.c"

/************************************************************
 * log support
 ***********************************************************/

FILE *log_handle() 
{
  return stderr;
}

int log_this_severity(int level) 
{
  return 1;
}

/************************************************************
 * list declaration
 ***********************************************************/

int* int_copy(int *e) 
{
  int *ee=(int *) mc_malloc(sizeof(int));
  *ee=*e;
  return ee;
}

void int_destroy(int * e) 
{
  mc_free(e);
}

DECLARE_EL_ARRAY(iarray, int);
IMPLEMENT_EL_ARRAY(iarray, int, int_copy, int_destroy);

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

void parray(iarray l) {
  printf("parray[%d]: -> ",iarray_count(l));
  int i, n;
  for(i=0, n=iarray_count(l);i < n; ++i) {
    printf("%d ",*iarray_get(l, i));fflush(stdout);
  }
  printf("\n");
}

#define TEST(name,code,l) \
    printf("%s: ",#name);fflush(stdout); \
    { code } \
    printf(" ok\n");fflush(stdout); \
    parray(l);


int main() {

  mc_init();

  iarray l=iarray_new();
  log_debug2("l=%p",l);

  int i,*e;

  TEST(t1b,i=0;
       iarray_append(l,&i);
      ,l);

  TEST(t1a,for(i=1;i<10;i++) {
         iarray_append(l,&i);
       }
      ,l);
  
  TEST(t2,i=100;iarray_insert(l,0,&i);,l);
  TEST(t3,i=200;iarray_insert(l,5,&i);,l);

  TEST(t4,iarray_delete(l,iarray_count(l)-1);,l);
  TEST(t5,iarray_delete(l,iarray_count(l));,l);
  TEST(t6,iarray_delete(l,0);,l);
  TEST(t7,iarray_delete(l,5);,l);
  TEST(t8,i=4242;iarray_set(l,5,&i);,l);

  iarray_destroy(l);

  return 0;
}
