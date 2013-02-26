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
#ifndef __FIFO__HOD
#define __FIFO__HOD

#include <elementals/list.h>

#define _DECLARE_FIFO(MODIFIER, NAME, T) \
  typedef list_t NAME; \
  MODIFIER inline NAME* NAME##_new(); \
  MODIFIER inline void NAME##_destroy(NAME* fifo); \
  MODIFIER inline int  NAME##_count(NAME* fifo); \
  MODIFIER inline void NAME##_enqueue(NAME* fifo, T* e); \
  MODIFIER inline T*   NAME##_dequeue(NAME* fifo); \
  MODIFIER inline T*   NAME##_peek(NAME* fifo);
  
#define _IMPLEMENT_FIFO(MODIFIER, NAME, T, COPY, DESTROY) \
  MODIFIER inline NAME* NAME##_new() { return (NAME* ) _list_new(); } \
  MODIFIER inline void NAME##_destroy(NAME* fifo) { _list_destroy(fifo,(void (*)(list_data_t)) DESTROY); } \
  MODIFIER inline int NAME##_count(NAME* fifo) { return _list_length(fifo); } \
  MODIFIER inline void NAME##_enqueue(NAME* fifo, T* e) { _list_lock(fifo); \
                                                          _list_start_iter(fifo, LIST_LAST); \
                                                          _list_append_iter(fifo, (list_data_t) COPY(e)); \
                                                          _list_unlock(fifo); \
                                                          sem_post(fifo->sem); \
 } \
 MODIFIER inline T* NAME##_dequeue(NAME* fifo) { T* e; \
                                                 sem_wait(fifo->sem); \
                                                 _list_lock(fifo); \
                                                 e = COPY((T *) _list_start_iter(fifo, LIST_FIRST)); \
                                                 _list_drop_iter(fifo,(void (*)(list_data_t)) DESTROY); \
                                                 _list_unlock(fifo); \
                                                 return e; \
 } \
 MODIFIER inline T* NAME##_peek(NAME* fifo) { T* e = NULL; \
                                              _list_lock(fifo); \
                                              if (_list_length(fifo) > 0) \
                                                e = (T *) _list_start_iter(fifo, LIST_FIRST); \
                                              _list_unlock(fifo); \
                                              return e; \
 }
                                                 

#define DECLARE_FIFO(NAME, T) _DECLARE_FIFO(, NAME, T)
#define IMPLEMENT_FIFO(NAME, T, COPY, DESTROY) _IMPLEMENT_FIFO(, NAME, T, COPY, DESTROY)

#define STATIC_DECLARE_FIFO(NAME, T) _DECLARE_FIFO(static ,NAME, T)
#define STATIC_IMPLEMENT_FIFO(NAME, T, COPY, DESTROY) _IMPLEMENT_FIFO(static ,NAME, T, COPY, DESTROY)

#endif
