#ifndef SET_HOD
#define SET_HOD

#include <elementals/types.h>
#include <elementals/list.h>

typedef void set_t;

typedef enum {
  SET_CASE_SENSITIVE = 1,
  SET_CASE_INSENSITIVE = 0
} set_enum;

DECLARE_LIST(set_key_list, const char);

set_t* set_new(set_enum kind);
void set_destroy(set_t* set);
void set_put(set_t* set, const char* key);
el_bool set_contains(set_t* set, const char* key);
void set_del(set_t* set, const char* key);
void set_clear(set_t* set);
int set_count(set_t* set);
set_key_list* set_keys(set_t* set);

#endif
