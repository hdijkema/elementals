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

int* int_copy(int* e) 
{
  int *ee=(int *) mc_malloc(sizeof(int));
  *ee=*e;
  return ee;
}

void int_destroy(int* e) 
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
    
void *producer(void* data) {
  sleep(1);
  ififo* f=(ififo* ) data;
  int i;
  for(i = 0;i < 100; ++i) {
    printf("producing %d\n",i);
    ififo_enqueue(f,&i);
    printf("producing %d\n",i);
  }
  i=-1;
  ififo_enqueue(f,&i);
  return NULL;
}

void *consumer(void* data) {
  ififo* f=(ififo* ) data;
  int *e;
  printf("consuming\n");
  e=ififo_dequeue(f);
  while (*e!=-1) {
    printf("consuming %d\n",*e);
    mc_free(e);
    e=ififo_dequeue(f);
  }
  if (*e == -1) 
    mc_free(e);
  return NULL;
}

int main() {

  mc_init();
  
  pthread_t consumer_, producer_;
  
  int r1, r2;
  
  ififo *fifo = ififo_new();
  
  r1 = pthread_create(&producer_, NULL, producer, fifo);
  r2 = pthread_create(&consumer_, NULL, consumer, fifo);

  pthread_join(producer_, NULL);
  pthread_join(consumer_, NULL);
  
  ififo_destroy(fifo);

  return 0;
}
