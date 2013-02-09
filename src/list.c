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

#include <elementals/log.h>
#include <elementals/list.h>
#include <elementals/memcheck.h>
#include <assert.h>

stat list_t *  _list_new()  {
  list_t *l=(list_t *) mc_malloc(sizeof(list_t));
  l->first=NULL;
  l->last=NULL;
  l->current=NULL;
  l->mutex=(pthread_mutex_t *) mc_malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(l->mutex,NULL);
  return l;
}

stat void _list_destroy(list_t *l,void (*data_destroyer)(list_data_t v)) {
  log_assert(l!=NULL);
  list_entry_t *e=l->first;
  while (e!=NULL) {
    data_destroyer(e->data);
    list_entry_t *next=e->next;
    mc_free(e);
    e=next;
  }
  pthread_mutex_destroy(l->mutex);
  mc_free(l->mutex);
  mc_free(l);
}

stat void _list_lock(list_t *l) {
  log_assert(l!=NULL);
  pthread_mutex_lock(l->mutex);
}

stat void _list_unlock(list_t *l) {
  log_assert(l!=NULL);
  pthread_mutex_unlock(l->mutex);
}

stat int _list_length(list_t *l) {
  log_assert(l!=NULL);
  return l->count;
}

stat list_data_t _list_start_iter(list_t *l,list_pos_t pos) {
  log_assert(l!=NULL);
  if (pos==LIST_FIRST) {
    l->current=l->first;
  } else {
    l->current=l->last;
  }
  if (l->current==NULL) {
    return NULL;
  } else {
    return l->current->data;
  }
}

stat list_data_t _list_iter_at(list_t *l,int i) {
  log_assert(l!=NULL);
  if (i>=l->count) {
    return NULL;
  } else if (i<0) {
    return NULL;
  } else {
    l->current=l->first;
    while(i>0) { l->current=l->current->next;i-=1; }
    return l->current->data;
  }
}

stat list_data_t _list_next_iter(list_t *l) {
  log_assert(l!=NULL);
  if (l->current==NULL) {
    return NULL;
  } else {
    l->current=l->current->next;
    if (l->current==NULL) {
      return NULL;
    } else {
      return l->current->data;
    }
  }
}

stat list_data_t _list_prev_iter(list_t *l) {
  log_assert(l!=NULL);
  if (l->current==NULL) {
    return NULL;
  } else {
    l->current=l->current->previous;
    if (l->current==NULL) {
      return NULL;
    } else {
      return l->current->data;
    }
  }
}

stat void _list_drop_iter(list_t *l,void (*data_destroyer)(list_data_t v)) {
  log_assert(l!=NULL);
  if (l->current!=NULL) {
    list_entry_t *e=l->current;
    if (e==l->last && e==l->first) {
      l->current=NULL;
      l->first=NULL;
      l->last=NULL;
    } else if (e==l->last) {
      l->current=NULL;
      l->last=l->last->previous;
      l->last->next=NULL;
    } else if (e==l->first) {
      l->first=e->next;
      l->first->previous=NULL;
      e->previous=NULL;
      l->current=e->next;
    } else {
      e->previous->next=e->next;
      e->next->previous=e->previous;
      l->current=e->next;
    }
    l->count-=1;
    data_destroyer(e->data);
    mc_free(e);
  }
}

stat void _list_prepend_iter(list_t *l ,list_data_t data) {
  log_assert(l!=NULL);
  if (l->current==NULL) {
    if (l->first==NULL && l->last==NULL) { // first entry in the list
      list_entry_t *e=(list_entry_t *) mc_malloc(sizeof(list_entry_t));
      e->next=NULL;
      e->previous=NULL;
      e->data=data;
      l->first=e;
      l->last=e;
      l->current=e;
      l->count=1;
    } else { // prepend before the list
      list_entry_t *e=(list_entry_t *) mc_malloc(sizeof(list_entry_t));
      e->next=l->first;
      l->first->previous=e;
      e->previous=NULL;
      e->data=data;
      l->first=e;
      l->current=e;
      l->count+=1;
    }
  } else {
    if (l->current==l->first) {
      l->current=NULL;
      _list_prepend_iter(l,data);
    } else if (l->current==l->last) {
      list_entry_t *e=(list_entry_t *) mc_malloc(sizeof(list_entry_t));
      e->previous=l->last;
      l->last->next=e;
      e->next=NULL;
      e->data=data;
      l->last=e;
      l->current=e;
      l->count+=1;
    } else {
      list_entry_t *e=(list_entry_t *) mc_malloc(sizeof(list_entry_t));
      e->previous=l->current->previous;
      e->next=l->current;
      e->previous->next=e;
      e->next->previous=e;
      e->data=data;
      l->current=e;
      l->count+=1;
    }
  }
}

stat void _list_append_iter(list_t *l,list_data_t data) {
  log_assert(l!=NULL);
  if (l->current==NULL) {
    if (l->first==NULL && l->last==NULL) {
      _list_prepend_iter(l,data);
    } else { // append at the end of list
      list_entry_t *e=(list_entry_t *) mc_malloc(sizeof(list_entry_t));
      e->next=NULL;
      e->previous=l->last;
      e->data=data;
      l->last->next=e;
      l->last=e;
      l->current=e;
      l->count+=1;
    }
  } else {
    if (l->current==l->first && l->current==l->last) {
      list_entry_t *e=(list_entry_t *) mc_malloc(sizeof(list_entry_t));
      e->previous=l->first;
      e->next=l->first->next;
      l->first->next=e;
      l->last=e;
      e->data=data;
      l->current=e;
      l->count+=1;
    } else if (l->current==l->first) {
      list_entry_t *e=(list_entry_t *) mc_malloc(sizeof(list_entry_t));
      e->previous=l->first;
      e->next=l->first->next;
      l->first->next=e;
      e->next->previous=e;
      e->data=data;
      l->current=e;
      l->count+=1;
    } else if (l->current==l->last) {
      list_entry_t *e=(list_entry_t *) mc_malloc(sizeof(list_entry_t));
      e->previous=l->last;
      e->next=NULL;
      e->previous->next=e;
      e->data=data;
      l->last=e;
      l->current=e;
      l->count+=1;
    } else {
      list_entry_t *e=(list_entry_t *) mc_malloc(sizeof(list_entry_t));
      e->previous=l->current;
      e->next=l->current->next;
      e->previous->next=e;
      e->next->previous=e;
      e->data=data;
      l->current=e;
      l->count+=1;
    }
  }
}

stat void _list_move_iter(list_t *l,list_pos_t pos) {
  log_assert(l!=NULL);
  if (l->current==NULL) {
    // does nothing
  } else {
    if (l->current==l->first) {
      if (pos==LIST_LAST) {
        list_entry_t *e;
        e=l->first;
        l->first=e->next;
        l->first->previous=NULL;
        l->last->next=e;
        e->previous=l->last;
        e->next=NULL;
        l->last=e;
        l->current=e;
      }
    } else if (l->current==l->last) {
      if (pos==LIST_FIRST) {
        list_entry_t *e;
        e=l->last;
        l->last=e->previous;
        l->last->next=NULL;
        l->first->previous=e;
        e->next=l->first;
        e->previous=NULL;
        l->first=e;
        l->current=e;
      }
    } else {
      if (pos==LIST_LAST) {
        list_entry_t *e=l->current;
        e->previous->next=e->next;
        e->next->previous=e->previous;
        l->last->next=e;
        e->previous=l->last;
        e->next=NULL;
        l->last=e;
        l->current=e;
      } else {
        list_entry_t *e=l->current;
        e->previous->next=e->next;
        e->next->previous=e->previous;
        l->first->previous=e;
        e->next=l->first;
        e->previous=NULL;
        l->first=e;
        l->current=e;
      }
    }
  }
}


static inline list_entry_t *_get_middle(list_entry_t *head)
{
  if (head == NULL) { return head; }
  list_entry_t *slow, *fast;
  slow = fast = head;
  while (fast->next != NULL && fast->next->next != NULL) {
    slow = slow->next;
    fast = fast->next->next;
  }
  return slow;
}

static list_entry_t *_merge(list_entry_t *a, list_entry_t *b,
                                     int (*cmp)(list_data_t a,list_data_t b))
{
  list_entry_t h = { NULL, NULL, NULL };
  list_entry_t *current = &h;
  while (a != NULL && b != NULL) {
    if ( cmp(a->data, b->data) <0 ) { current->next = a; a = a->next; }
    else { current->next = b; b = b->next; }
    current = current->next;
  }
  current->next = (a == NULL) ? b : a;
  return h.next;
}

static list_entry_t *_merge_sort(list_entry_t *head, int (*cmp)(list_data_t a,list_data_t b))
{
  if (head == NULL || head->next == NULL) {
    return head;
  }
  list_entry_t *middle = _get_middle(head);
  list_entry_t *s_half = middle->next;
  middle->next = NULL;
  return _merge(_merge_sort(head,cmp), _merge_sort(s_half, cmp), cmp);
}

stat void _list_sort(list_t *l,int (*cmp)(list_data_t a,list_data_t b))
{
  l->first = _merge_sort(l->first, cmp);
  
  // administrative update: One sweep to go
  list_entry_t *e = l->first;
  e->previous=NULL;
  while (e->next != NULL) {
    e->next->previous = e;
    e = e->next;
  }
  l->last = e;
  l->current = l->first;
}

