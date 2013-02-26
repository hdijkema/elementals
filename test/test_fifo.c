#include <elementals.h>
#include <stdio.h>

FILE *log_handle() 
{
  return stderr;
}

int log_this_severity(int level)
{
  return 1;
}

/************************************************************
 * fifo declaration
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

DECLARE_FIFO(ififo, int);
IMPLEMENT_FIFO(ififo, int, int_copy, int_destroy);

void pfifo(ififo *l) {
  printf("fifo count = %d\n",ififo_count(l));
  int *e = ififo_peek(l);
  if (e != NULL) { printf("peek = %d\n",*e); }
}

#define TEST(name,code,l) \
    printf("%s: ",#name);fflush(stdout); \
    { code } \
    printf(" ok\n");fflush(stdout); \
    pfifo(l);

int main() {

  mc_init();

  
  return 0;
}
