#ifndef __PSEM__H
#define __PSEM__H

#include <pthread.h>

#define USE_SEM_T

#ifdef USE_SEM_T
#include <semaphore.h>
#endif

#ifdef USE_SEM_T
typedef sem_t psem_t;
#else
typedef struct {
  pthread_mutex_t* mut;
  pthread_cond_t*  cond;
  int count;
} psem_t;
#endif

typedef enum {
  PSEM_OK = 0,
  PSEM_TIMEOUT = 1,
  PSEM_ERR = 2
} psem_code_t;

psem_t* psem_new(int initial_count);
void psem_init(psem_t* psem, int initial_count);
psem_code_t psem_wait(psem_t* sem);   
psem_code_t psem_wait_timeout(psem_t* sem, int timeout_ms);
psem_code_t psem_post(psem_t* sem);
void psem_destroy(psem_t* sem);


#endif
