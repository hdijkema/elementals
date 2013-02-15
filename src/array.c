#include <elementals/array.h>
#include <elementals/log.h>
#include <elementals/memcheck.h>


el_array_t *_el_array_new(void)
{
  el_array_t *a = (el_array_t *) malloc(sizeof(el_array_t));
  if (a == NULL) {
    return a;
  }
  a->array = NULL;
  a->count = 0;
  a->size = 0;
  return a;
}

el_array_code _el_array_insert(el_array_t *a, int index, void *type_elem)
{
  log_assert(a != NULL);
  if (index >= a->size) {
    a->array = (void **) mc_realloc(a->array, (index+1) * sizeof(void *));
    log_assert(a->array != NULL);
    int i;
    for(i=a->size; i < index+1; ++i) {
      a->array[i]=NULL;
    }
    a->size  = index+1;
    a->count = index+1;
    a->array[index] = type_elem;
    return EL_ARRAY_OK;
  } else {
    if (index < 0) {
      return EL_ARRAY_EINDEX;
    } else {
      if (a->count == a->size) {
        a->array = (void **) mc_realloc(a->array, (a->size+1) * sizeof(void *));
        log_assert(a->array != NULL);
        a->size  += 1;
        a->count += 1;
      }
      int i;
      for(i = a->count-2; i >= index; --i) {
        a->array[i+1] = a->array[i];
      }
      a->array[index] = type_elem;
    }
    return EL_ARRAY_OK;
  }
}

el_array_code _el_array_delete(el_array_t *a, int index, void (*destroyer)(void *))
{
  log_assert(a != NULL);
  if (index < 0) {
    return EL_ARRAY_EINDEX;
  } else if (index >= a->count) {
    return EL_ARRAY_EINDEX;
  } else {
    if (a->array[index] != NULL) {
      destroyer(a->array[index]);
    }
    int i;
    for(i=index+1;i < a->count-1; ++i) {
      a->array[i] = a->array[i+1];
    }
    a->array[a->count-1] = NULL;
    a->count -= 1;
    return EL_ARRAY_OK;
  }
}

el_array_code _el_array_set(el_array_t *a, int index, void *type_elem, void (*destroyer) (void *))
{
  log_assert(a != NULL);
  if (index < 0) {
    return EL_ARRAY_EINDEX;
  } else if (index == a->count) {
    return _el_array_insert(a, index, type_elem);
  } else if (index > a->count) {
    return EL_ARRAY_EINDEX;
  } else {
    if (a->array[index] != NULL) {
      destroyer(a->array[index]);
    }
    a->array[index]=type_elem;
    return EL_ARRAY_OK;
  }
}

void * _el_array_get(el_array_t *a, int index)
{
  log_assert(a != NULL);
  if (index < 0) {
    return NULL;
  } else if (index >= a->count) {
    return NULL;
  } else {
    return a->array[index];
  }
}

int _el_array_count(el_array_t *a)
{
  log_assert(a != NULL);
  return a->count;
}


