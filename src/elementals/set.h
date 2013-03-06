#ifndef SET_HOD
#define SET_HOD

#include <elementals/types.h>

typedef void set_t;

typedef enum {
  SET_CASE_SENSITIVE = 1,
  SET_CASE_INSENSITIVE = 0
} set_enum;

set_t* set_new(set_enum kind);
void set_destroy(set_t* set);
void set_put(set_t* set, const char* key);
el_bool set_contains(set_t* set, const char* key);
void set_del(set_t* set, const char* key);

#endif
