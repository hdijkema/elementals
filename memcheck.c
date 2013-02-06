/*
   This file is part of elementals (http://hoesterholt.github.com/elementals/).
   Copyright 2013, Hans Oesterholt <debian@oesterholt.net>

   Elementals are free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Elementals are distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with elementals.  If not, see <http://www.gnu.org/licenses/>.

   ********************************************************************
*/
#define USE_MEMCHECK

#define MEMCHECK_INTERNAL
#include "list.h"
#include "log.h"
#include "list.c"

#undef mc_malloc
#undef mc_realloc
#undef mc_calloc
#undef mc_free
#undef mc_take_over
#undef mc_take_control
#undef mc_init
#undef mc_report

typedef struct {
  size_t  size;
  const char *func;
  const char *file;
  int line;
  void   *ptr;
} mc_entry_t;

static list_data_t copy(mc_entry_t *e) {
  return e;
}

static void destroy(list_data_t e) {
  mc_entry_t *m=(mc_entry_t *) e;
  free(m->ptr);
  free(m);
}

STATIC_DECLARE_LIST(mc_list,mc_entry_t);
STATIC_IMPLEMENT_LIST(mc_list,mc_entry_t,copy,destroy);

static mc_list *MEMLIST=NULL;

#define mc_check_init()  log_assert(MEMLIST!=NULL)

void *_mc_malloc( size_t size, const char *func, const char *file, int line ) {
  mc_check_init();
  void *p=malloc(size);
  mc_entry_t *e=(mc_entry_t *) malloc(sizeof(mc_entry_t));
  e->size=size;
  e->func=func;
  e->file=file;
  e->line=line;
  e->ptr=p;
  mc_list_lock(MEMLIST);
  mc_list_start_iter(MEMLIST,LIST_FIRST);
  mc_list_prepend_iter(MEMLIST,e);
  mc_list_unlock(MEMLIST);
  return p;
}

void *_mc_take_over( void *ptr, const char *func, const char *file, int line ) {
  mc_check_init();

  mc_list_lock(MEMLIST);
  mc_entry_t *e=mc_list_start_iter(MEMLIST,LIST_FIRST);
  while (e != NULL && e->ptr != ptr) {
    e = mc_list_next_iter(MEMLIST);
  }
  if (e == NULL) {
    log_error4("Cannot take over memory at %s, %s, %d",func,file,line);
  } else {
    e->func=func;
    e->file=file;
    e->line=line;
  }
  mc_list_unlock(MEMLIST);
  return ptr;
}


void _mc_take_control( void *ptr, size_t size, const char *func, const char *file, int line ) {
  mc_check_init();
  mc_entry_t *e=(mc_entry_t *) malloc(sizeof(mc_entry_t));
  e->size=size;
  e->func=func;
  e->file=file;
  e->line=line;
  e->ptr=ptr;
  mc_list_lock(MEMLIST);
  mc_list_start_iter(MEMLIST,LIST_FIRST);
  mc_list_prepend_iter(MEMLIST,e);
  mc_list_unlock(MEMLIST);
}

void *_mc_realloc( void *ptr, size_t size, const char *func, const char *file, int line  ) {
  mc_check_init();

  if (ptr == NULL) {
    return _mc_malloc(size, func, file, line);
  }

  mc_list_lock(MEMLIST);
  mc_entry_t *e=mc_list_start_iter(MEMLIST,LIST_FIRST);
  while (e != NULL && e->ptr != ptr) {
    e = mc_list_next_iter(MEMLIST);
  }

  if (e == NULL) {
    log_error5("Reallocation of unknown pointer %s,%s,%d with size %d",func,file,line,(int) size);
    mc_list_unlock(MEMLIST);
    return ptr;
  } else {
    void *p = realloc(ptr,size);
    if (p == NULL) {
      log_error5("Realloc of pointer %s,%s,%d with size %d results in NULL",e->func,e->file,e->line,(int) size);
    } else {
      e->ptr = p;
      e->size = size;
      /*e->func=func;
      e->file=file;
      e->line=line;*/
    }
    mc_list_unlock(MEMLIST);
    return p;
  }
}

void *_mc_calloc( size_t num, size_t size, const char *func, const char *file, int line  ) {
  mc_check_init();
  return _mc_malloc(num * size, func, file, line);
}

char *_mc_strdup( const char * s, const char *func, const char *file, int line ) {
  mc_check_init();
  char *ss=_mc_malloc(strlen(s)+1, func, file, line);
  strcpy(ss,s);
  return ss;
}

void _mc_free( void * ptr, const char *func, const char *file, int line  ) {
  mc_check_init();

  if (ptr == NULL) return;

  mc_list_lock(MEMLIST);
  mc_entry_t *e=mc_list_start_iter(MEMLIST,LIST_FIRST);

  while (e != NULL && e->ptr != ptr) {
    e = mc_list_next_iter(MEMLIST);
  }

  if (e == NULL) {
    log_error4("free of pointer %s, %s, %d: not in list, double free!",func,file,line);
  } else {
    mc_list_drop_iter(MEMLIST);
  }

  mc_list_unlock(MEMLIST);
}

void mc_report(void) {
  mc_check_init();
  int count=0;
  size_t total=0;
  mc_entry_t *e=mc_list_start_iter(MEMLIST,LIST_FIRST);
  while (e != NULL) {
    count += 1;
    total += e->size;
    log_error4("not freed: pointer allocated at %s, %s, %d",e->func,e->file,e->line);
    e = mc_list_next_iter(MEMLIST);
  }

  if (count>0) {
    log_error2("MC_REPORT: count = %d", count);
    log_error2("MC_REPORT: total size = %d", (int) total);
  } else {
    log_info("MEMORY CLEAN");
  }
}

void mc_init(void) {
  MEMLIST = mc_list_new();
  atexit(mc_report);
}

