#include <elementals.h>
#include <sys/time.h>

psem_t* psem_new(int initial_count)
{
  psem_t* s = (psem_t*) malloc(sizeof(psem_t));
#ifdef USE_SEM_T
  sem_init((sem_t*) s, 0, initial_count);
#else 
  s->mut = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(s->mut, NULL);
  s->cond = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(s->cond, NULL);
  s->count = initial_count;
#endif 
  return s;
}

void psem_destroy(psem_t* sem)
{
#ifdef USE_SEM_T
  sem_destroy((sem_t*) sem);
#else 
  pthread_mutex_destroy(sem->mut);
  pthread_cond_destroy(sem->cond);
#endif  
  free(sem);
}

psem_code_t psem_wait(psem_t* sem)
{
#ifdef USE_SEM_T
  if (sem_wait((sem_t*) sem) == 0) {
    return PSEM_OK;
  } else {
    return PSEM_ERR;
  }
#else
  pthread_mutex_lock(sem->mut);
  while(sem->count == 0) {
    pthread_cond_wait(sem->cond, sem->mut);
  }
  sem->count -= 1;
  pthread_mutex_unlock(sem->mut);
#endif  
  return PSEM_OK;
}

psem_code_t psem_wait_timeout(psem_t* sem, int timeout_ms)
{
  struct timeval tm;
  gettimeofday(&tm, NULL);
  long long l = tm.tv_sec * 1000000000 + tm.tv_usec * 1000;
  l += timeout_ms * 1000000;
  struct timespec timeout = { l / 1000000000, l % 1000000000 };
#ifdef USE_SEM_T
  int retcode = sem_timedwait((sem_t*) sem, &timeout);
  if (retcode == ETIMEDOUT) {
    return PSEM_TIMEOUT;
  } else if (retcode == 0) {
    return PSEM_OK;
  } else {
    return PSEM_ERR;
  }
#else
  pthread_mutex_lock(sem->mut);
  int retcode = 0;
  while(sem->count == 0 && retcode != ETIMEDOUT) {
    retcode = pthread_cond_timedwait(sem->cond, sem->mut, &timeout);
  }
  if (retcode == ETIMEDOUT) {
    pthread_mutex_unlock(sem->mut);
    return PSEM_TIMEOUT;
  } else {
    sem->count -= 1;
    pthread_mutex_unlock(sem->mut);
    return PSEM_OK;
  }
#endif  
}

psem_code_t psem_post(psem_t* sem)
{
#ifdef USE_SEM_T
  if (sem_post((sem_t*) sem) == 0) {
    return PSEM_OK; 
  } else {
    return PSEM_ERR;
  }
#else
  pthread_mutex_lock(sem->mut);
  sem->count += 1;
  pthread_cond_broadcast(sem->cond);
  pthread_mutex_unlock(sem->mut);
  return PSEM_OK;
#endif  
}

void psem_init(psem_t* sem, int initial_count)
{
#ifdef USE_SEM_T
  sem_init((sem_t*) sem, 0, initial_count);
#else 
  pthread_mutex_lock(sem->mut);
  sem->count = initial_count;
  pthread_mutex_unlock(sem->mut);
#endif  
}

