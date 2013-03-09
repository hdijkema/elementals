#ifndef __REGEXP__HOD
#define __REGEXP__HOD

#include <elementals/array.h>
#include <elementals/types.h>

typedef struct {
  int begin_offset;
  int end_offset;
  char *match;  
} hre_match_t;

int hre_match_begin(hre_match_t *m);
int hre_match_len(hre_match_t *m);
int hre_match_end(hre_match_t *m);
const char* hre_match_string(hre_match_t *m);
const char* hre_match_str(hre_match_t *m);
void hre_match_destroy(hre_match_t *m);

typedef void* hre_t;

DECLARE_EL_ARRAY(hre_matches, hre_match_t);

hre_t hre_compile(const char *re, const char* modifiers);

hre_matches hre_match(hre_t re, const char* string);
hre_matches hre_match_all(hre_t re, const char* string);
el_bool hre_has_match(hre_t re, const char* string);

char* hre_replace(hre_t re, const char* string, const char* replacement);
char* hre_replace_all(hre_t re, const char* string, const char* replacement);

void hre_destroy(hre_t re);

void hre_trim(char* string);
void hre_lc(char* string);
void hre_uc(char* string);

char* hre_left(const char* string, int l);
char* hre_substr(const char* string, int offset, int len);
char* hre_right(const char* string, int l);
char* hre_concat(const char* s1, const char *s2);
char* hre_concat3(const char* s1, const char* s2, const char* s3);



#endif
