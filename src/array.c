#include <elementals/array.h>
#include <elementals/log.h>
#include <elementals/memcheck.h>


el_array_t *_el_array_new(void)
{
  el_array_t *a = (el_array_t *) mc_malloc(sizeof(el_array_t));
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
    for(i=index;i < a->count-1; ++i) {
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

void _el_array_destroy(el_array_t* a, void (*destroyer)(void *))
{
  log_assert(a != NULL);
  _el_array_clear(a, destroyer);
  mc_free(a->array);
  mc_free(a);
}

void _el_array_clear(el_array_t* a, void (*destroyer)(void *))
{
  log_assert(a != NULL);
  while (a->count > 0) {
    _el_array_delete(a, a->count - 1, destroyer);
  }
}

el_array_t* _el_array_copy(el_array_t* source, void* (*copy)(void*)) {
  el_array_t* a = _el_array_new();
  int i, N;
  for(i = 0, N = _el_array_count(source); i < N; ++i) {
    _el_array_insert(a, i, copy(_el_array_get(source, i)));
  }
  return a;
}

static void mergeSort(void* arr[],int low, int mid, int high, void* cmp, el_bool dt, void* data)
{
  int (*cmp1)(void*,void*) = (int (*)(void*, void*)) cmp;
  int (*cmp2)(void*,void*,void*) = (int (*)(void*,void*,void*)) cmp;
  
  int i,m,k,l;
  void** temp = (void**) mc_malloc((high+1) * sizeof(void*));

  l=low;
  i=low;
  m=mid+1;
  while((l <= mid) && (m <= high)) {
    int cr = (dt) ? cmp2(data, arr[l], arr[m]) : cmp1(arr[l], arr[m]);
    if (cr <= 0) {
      temp[i]=arr[l];
      ++l;
    } else {
      temp[i] = arr[m];
      ++m;
    }
    ++i;
  }

  if (l > mid){
    for(k = m;k <= high; ++k){
      temp[i] = arr[k];
      ++i;
    }
  } else {
    for(k = l;k <= mid; ++k){
      temp[i] = arr[k];
      ++i;
    }
  }
 
  for(k = low;k <= high; ++k){
    arr[k] = temp[k];
  }
  
  mc_free(temp);
}

static void partition(void *arr[],int low,int high, void* cmp, el_bool dt, void* data)
{
  int mid;
  if (low < high) {
    mid = (low + high) / 2;
    partition(arr, low, mid, cmp, dt, data);
    partition(arr, mid+1, high, cmp, dt, data);
    mergeSort(arr, low, mid, high, cmp, dt, data);
  }
}

void _el_array_sort(el_array_t* a, int (*cmp)(void* a, void* b))
{
  partition(a->array, 0, a->count - 1, cmp, el_false, NULL);
}


void _el_array_sort1(el_array_t* a, void* data, int(*cmp)(void* data, void* a, void* b))
{
  partition(a->array, 0, a->count - 1, cmp, el_true, data);
}
