#include <elementals/regexp.h>
#include <elementals/array.h>
#include <elementals/log.h>
#include <elementals/memcheck.h>
#include <pcre.h>
#include <ctype.h>

static inline hre_match_t* hre_copy_match(hre_match_t *e)
{
  hre_match_t* ee=(hre_match_t*) mc_malloc(sizeof(hre_match_t));
  ee->match = mc_strdup(e->match);
  ee->begin_offset = e->begin_offset;
  ee->end_offset = e->end_offset;
  return ee;
}

static inline void hre_destroy_match(hre_match_t *e)
{
  mc_free(e->match);
  mc_free(e);
}

IMPLEMENT_EL_ARRAY(hre_matches, hre_match_t, hre_copy_match, hre_destroy_match);

hre_t hre_compile(const char* re, const char* modifiers)
{
  int options = 0;
  int i, l;
  for(i=0, l=strlen(modifiers);i < l;++i) {
    switch (modifiers[i]) {
      case 'i' : options |= PCRE_CASELESS;
      break;
      case 's' : options |= PCRE_DOTALL;
      break;
      case 'x' : options |= PCRE_EXTENDED;
      break;
      case 'm' : options |= PCRE_MULTILINE;
      break;
      default:
      break;
    }
  }
  
  const char *errstr;
  int erroffset;
  
  pcre* cre = pcre_compile(re, options, &errstr, &erroffset, NULL);
  if (cre == NULL) {
    log_error3("pcre_compile error: %s at %d", errstr, erroffset);
  }
  
  return (hre_t) cre;
}

hre_matches hre_match(hre_t re, const char *string)
{
  int num_of_subpatterns;
  int res = pcre_fullinfo((pcre*) re, NULL, PCRE_INFO_CAPTURECOUNT, &num_of_subpatterns);
  
  if (res == 0) {
    int ovec_len = (1 + num_of_subpatterns) * 3;
    int *ovector = (int*) mc_malloc(sizeof(int) * ovec_len);
    res = pcre_exec((pcre*) re, NULL, string, strlen(string), 0, 0, ovector, ovec_len);
    if (res >= 0) {
      hre_matches m = hre_matches_new();
      int i;
      for(i = 0;i < res;++i) {
        int k=i*2;
        int begin_offset = ovector[k];
        int end_offset = ovector[k+1];
        int len = end_offset - begin_offset;
        char* mstr = mc_strndup(&string[begin_offset],len);
        hre_match_t match = { begin_offset, end_offset, mstr };
        hre_matches_append(m, &match);
        mc_free(mstr);
      }
      mc_free(ovector);
      return m;
    } else {
      mc_free(ovector);
      if (res != PCRE_ERROR_NOMATCH) {
        log_error2("pcre_exec returns error: %d", res);
      }
      return hre_matches_new();
    }
  } else {
    log_error2("pcre_fullinfo returns error: %d", res);
    return hre_matches_new();
  }
}

hre_matches hre_match_all(hre_t re, const char* string)
{
  hre_matches R = hre_matches_new();
  hre_matches m = hre_match(re, string);
  
  while (hre_matches_count(m) > 0) {
    int i, l;
    for(i = 0, l = hre_matches_count(m);i < l;++i) {
      hre_match_t *match = hre_matches_get(m, i);
      hre_matches_append(R, match);
    }
    
    hre_match_t *match = hre_matches_get(m, 0);
    int k = hre_match_end(match);
    hre_matches_destroy(m);
    const char* p = &string[k];
    m = hre_match(re, p);
    for(i = 0, l = hre_matches_count(m);i < l;++i) {
      hre_match_t *match = hre_matches_get(m, i);
      match->begin_offset += k;
      match->end_offset += k;
    }
  }
  
  hre_matches_destroy(m);
  
  return R;
}

hre_matches hre_match_all0(hre_t re, const char* string)
{
  hre_matches R = hre_matches_new();
  hre_matches m = hre_match(re, string);
  
  while (hre_matches_count(m) > 0) {
    hre_match_t *match = hre_matches_get(m, 0);
    hre_matches_append(R, match);
    int k = hre_match_end(match);
    hre_matches_destroy(m);
    const char* p = &string[k];
    m = hre_match(re, p);
    int i, l;
    for(i = 0, l = hre_matches_count(m);i < l;++i) {
      hre_match_t *match = hre_matches_get(m, i);
      match->begin_offset += k;
      match->end_offset += k;
    }
  }
  
  hre_matches_destroy(m);
  
  return R;
}

el_bool hre_has_match(hre_t re, const char* string)
{
  hre_matches m = hre_match(re, string);
  if (hre_matches_count(m) == 0) {
    hre_matches_destroy(m);
    return el_false;
  } else {
    hre_matches_destroy(m);
    return el_true;
  }
}

char* hre_replace(hre_t re, const char* string, const char* replacement)
{
  hre_matches m = hre_match(re, string);
  if (hre_matches_count(m) > 0) {
    hre_match_t *match = hre_matches_get(m, 0);
    char* s1 = hre_substr(string, 0, match->begin_offset);
    char* s3 = hre_substr(string, match->end_offset, -1); 
    char* r = hre_concat3(s1, replacement, s3);
    mc_free(s1);
    mc_free(s3);
    hre_matches_destroy(m);
    return r;
  } else {
    return mc_strdup(string);
  }
}

char* hre_replace_all(hre_t re,const char* string, const char* replacement)
{
  hre_matches m = hre_match_all0(re, string);
  int i, l = hre_matches_count(m);
  char **s = (char**) mc_malloc(sizeof(char*) * ((l+1)*2));
  int n, b = 0;
  for(n = 0, i = 0 ; i < l; ++i) {
    printf("%d\n",i);
    hre_match_t *match = hre_matches_get(m, i);
    int el = match->begin_offset - b;
    printf("%d - %d\n",b,el);
    s[n] = hre_substr(string, b, el);
    printf("'%s'\n",s[n]);
    //el = match->end_offset - match->begin_offset;
    s[n+1] = mc_strdup(replacement);//hre_substr(string, match->begin_offset, el);
    printf("'%s'\n",s[n+1]);
    n += 2;
    b = match->end_offset;
  }
  s[n] = hre_substr(string, b, -1);
  n += 1;
  hre_matches_destroy(m);
  
  char* q = mc_strdup("");
  for(i=0; i < n; i++) {
    printf("%d = %s\n",i,s[i]);
    char* r = hre_concat(q, s[i]);
    mc_free(q);
    mc_free(s[i]);
    q = r;
  }
  mc_free(s);
  
  return q;
}

void hre_destroy(hre_t re)
{
  pcre_free((pcre*) re);
}


char* hre_concat(const char* s1, const char* s2) 
{
  int l = strlen(s1) + strlen(s2) + 1;
  char* s=(char*) mc_malloc(l*sizeof(char));
  strcpy(s, s1);
  strcat(s, s2);
  return s;
}

char* hre_concat3(const char* s1, const char* s2, const char* s3)
{
  int l = strlen(s1) + strlen(s2) + strlen(s3) + 1;
  char* s=(char*) mc_malloc(l*sizeof(char));
  strcpy(s, s1);
  strcat(s, s2);
  strcat(s, s3);
  return s;
}

char* hre_substr(const char* string, int offset, int len)
{
  int n = strlen(string);
  if (offset >= n) {
    return mc_strdup("");
  } else {
    int nn;
    if (len >= 0) {
      int l = offset + len;
      if (l > n) { l = n; }
      nn = l - offset;
    } else {
      nn = n - offset;
    }
    return mc_strndup(&string[offset], nn);
  }
}

char* hre_left(const char* string, int len)
{
  return hre_substr(string, 0, len);
}

char* hre_right(const char* string, int len)
{
  int offset = strlen(string) - len;
  if (offset < 0) { offset = 0; }
  return hre_substr(string, offset, len);
}


void hre_trim(char* string)
{
  char* p=(char*) string;
  while(p[0] != '\0' && isspace(p[0])) { ++p; }
  if (p[0] != '\0') {
    int i, l;
    for(l = strlen(p),i = l - 1;i >= 0 && isspace(p[i]);--i);
    strncpy(string, p, i+2);
    string[i+1]='\0';
  }  
}

int hre_match_begin(hre_match_t *m) {
  return m->begin_offset;
}

int hre_match_end(hre_match_t *m) {
  return m->end_offset;
}

int hre_match_len(hre_match_t *m) {
  return m->end_offset - m->begin_offset;
}

const char* hre_match_string(hre_match_t *m) {
  return m->match;
}

const char* hre_match_str(hre_match_t* m) {
  return m->match;
}

void hre_match_destroy(hre_match_t* m) {
  hre_destroy_match(m);
}
