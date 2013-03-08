#include <elementals/set.h>
#include <elementals/hash.h>
#include <elementals/memcheck.h>

static inline void* _set_copy(void* p) 
{
  return p;
}

static inline void _set_destroy(void* p) 
{
}

DECLARE_STATIC_HASH(set_hash, void);
IMPLEMENT_STATIC_HASH(set_hash, void, _set_copy, _set_destroy);

static const char *copy_key(const char *e)
{
  return mc_strdup(e);
}

static void destroy_key(const char *e) {
  mc_free((void *) e);
}

IMPLEMENT_LIST(set_key_list, const char, copy_key, destroy_key);

set_t* set_new(set_enum kind)
{
  return (set_t*) set_hash_new(10, (int) kind);
}

void set_destroy(set_t* set)
{
  set_hash_destroy((set_hash*) set);
}

int set_count(set_t* set)
{
  return set_hash_count((set_hash*) set);
}

void set_clear(set_t* set)
{
  set_hash_clear((set_hash*) set);
}

void set_put(set_t* set, const char* key)
{
  if (!set_contains(set, key)) 
    set_hash_put((set_hash*) set, key, (void*) set);
}

el_bool set_contains(set_t* set, const char* key)
{
  return set_hash_exists((set_hash*) set, key);
}

void set_del(set_t* set, const char* key)
{
  set_hash_del((set_hash*) set, key);
}

set_key_list* set_keys(set_t* set)
{
  return (set_key_list*) set_hash_keys((set_hash*) set);
}


