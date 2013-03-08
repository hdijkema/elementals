#ifndef __ELEMENTALS_ARRAY
#define __ELEMENTALS_ARRAY

typedef struct {
  int    count;
  int    size;
  void **array;
} el_array_t;

typedef enum {
  EL_ARRAY_OK = 1,
  EL_ARRAY_EOMEM = -1,
  EL_ARRAY_EINDEX = -2
} el_array_code;

el_array_t   *_el_array_new(void);
el_array_code _el_array_insert(el_array_t* a, int index, void *type_elem);
el_array_code _el_array_delete(el_array_t* a, int index, void (*destroyer)(void *));
el_array_code _el_array_set(el_array_t* a, int index, void *type_elem, void (*destroyer) (void *));
void        * _el_array_get(el_array_t* a, int index);
int           _el_array_count(el_array_t* a);
void          _el_array_destroy(el_array_t* a, void (*destroyer)(void *));
void          _el_array_sort(el_array_t* a, int (*cmp)(void*, void*));
void          _el_array_sort1(el_array_t* a, void* data, int (*cmp)(void* data, void* r1, void * r2));
el_array_t   *_el_array_copy(el_array_t* source, void* (*copy)(void*));
void          _el_array_clear(el_array_t* a, void (*destroyer)(void*));


#define _DECLARE_EL_ARRAY(MODIFIER, NAME, T) \
  typedef el_array_t * NAME; \
  MODIFIER inline NAME NAME##_new(void); \
  MODIFIER inline el_array_code NAME##_insert(NAME a, int index, T *elem); \
  MODIFIER inline el_array_code NAME##_append(NAME a, T *elem); \
  MODIFIER inline el_array_code NAME##_delete(NAME a, int index); \
  MODIFIER inline el_array_code NAME##_set(NAME a,int index, T *elem); \
  MODIFIER inline T* NAME##_get(NAME a,int index); \
  MODIFIER inline void NAME##_sort(NAME a, int (*cmp)(T* a, T* b)); \
  MODIFIER inline void NAME##_sort1(NAME a, void* data, int (*cmp)(void* data, T* a, T* b)); \
  MODIFIER inline NAME NAME##_copy(NAME a); \
  MODIFIER inline void NAME##_clear(NAME a); \
  MODIFIER int NAME##_count(NAME a); \
  MODIFIER inline void NAME##_destroy(NAME a);

#define _IMPLEMENT_EL_ARRAY(MODIFIER, NAME, T, COPY, DESTROY) \
  MODIFIER inline NAME NAME##_new(void)  { return (NAME) _el_array_new(); } \
  MODIFIER inline el_array_code NAME##_insert(NAME a, int index, T *elem) { return _el_array_insert( a, index, (void *) COPY(elem)); } \
  MODIFIER inline el_array_code NAME##_append(NAME a, T *elem) { return _el_array_insert(a, _el_array_count(a), (void*) COPY(elem)); } \
  MODIFIER inline el_array_code NAME##_delete(NAME a, int index) { return _el_array_delete(a, index, (void (*)(void*)) DESTROY); } \
  MODIFIER inline el_array_code NAME##_set(NAME a,int index, T *elem) { return _el_array_set(a, index, (void*) COPY(elem), (void (*)(void*)) DESTROY); } \
  MODIFIER inline T * NAME##_get(NAME a,int index) { return (T *) _el_array_get(a, index); } \
  MODIFIER inline void NAME##_sort(NAME a, int (*cmp)(T* a, T* b)) { _el_array_sort(a, (int (*)(void*,void*)) cmp); } \
  MODIFIER inline void NAME##_sort1(NAME a, void* data, int (*cmp)(void* data, T* a, T* b)) { _el_array_sort1(a, data, (int (*)(void*, void*,void*)) cmp); } \
  MODIFIER inline NAME NAME##_copy(NAME a) { return _el_array_copy(a, (void* (*)(void*)) COPY); } \
  MODIFIER inline void NAME##_clear(NAME a) { _el_array_clear(a, (void (*)(void*)) DESTROY ); } \
  MODIFIER inline int NAME##_count(NAME a) { return _el_array_count(a); } \
  MODIFIER inline void NAME##_destroy(NAME a) { _el_array_destroy(a,(void (*)(void*)) DESTROY); }

#define DECLARE_EL_ARRAY(NAME, T) _DECLARE_EL_ARRAY(, NAME, T)
#define IMPLEMENT_EL_ARRAY(NAME, T, COPY, DESTROY) _IMPLEMENT_EL_ARRAY(, NAME, T, COPY, DESTROY)
#define DECLARE_STATIC_EL_ARRAY(NAME, T) _DECLARE_EL_ARRAY(static, NAME, T)
#define IMPLEMENT_STATIC_EL_ARRAY(NAME, T, COPY, DESTROY) _IMPLEMENT_EL_ARRAY(static, NAME, T, COPY, DESTROY)

  
  
#endif
