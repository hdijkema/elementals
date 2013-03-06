#include <elementals/set.h>
#include <elementals/hash.h>

static inline void* _set_copy(void* p) 
{
  return p;
}

static inline void _set_destroy(void* p) 
{
}

DECLARE_STATIC_HASH(set_hash, void);
IMPLEMENT_STATIC_HASH(set_hash, void, _set_copy, _set_destroy);


set_t* set_new(set_enum kind)
{
  return (set_t*) set_hash_new(10, (int) kind);
}

void set_destroy(set_t* set)
{
  set_hash_destroy((set_hash*) set);
}

void set_put(set_t* set, const char* key)
{
  set_hash_put((set_hash*) set, key, NULL);
}

el_bool set_contains(set_t* set, const char* key)
{
  return set_hash_exists((set_hash*) set, key);
}

void set_del(set_t* set, const char* key)
{
  set_hash_del((set_hash*) set, key);
}


