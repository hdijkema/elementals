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

/*static inline void swap_data(list_t *l, list_entry_t *a, list_entry_t *b)
{
  void *d = a->data;
  a->data = b->data;
  b->data = d;
}

#define SWAP(a,b) { void *h=a; a=b; b=h; }

static void sort_part(list_t *l, int (cmp)(list_data_t a,list_data_t b), 
                              list_entry_t *start, list_entry_t *end, 
                              int n_elems)
{
  if (start == end) {
    // we've reached the bottom
    return; 
  } else {
    if (start->next == end) { // sort tuple
      int r = cmp(start->data, end->data);
      if (r > 0) { 
        swap_data(l, start, end);
      }
    } else if (start->next->next == end) { // sort triple a, b, c
      void *a = start->data;
      void *b = start->next->data;
      void *c = end->data;

      int b_gt_c = (cmp(b,c)>0);      
      if (b_gt_c) { 
        SWAP(b,c);  // a , b > c -> a c b 
      } 

      int a_gt_c = (cmp(a,c)>0);  // a, b, c -> a b < c
      if (a_gt_c) { 
        SWAP(a,b);SWAP(b,c); // a < b < c 
      } else {
        int a_gt_b = (cmp(a,b)>0);
        if (a_gt_b) { 
          SWAP(a,b);
        }
      }

      start->data = a;
      start->next->data = b;
      end->data = c;
    } else { 
      // partition and merge sorted partitions

      // partition
      int N=n_elems/2;
      list_entry_t *mid = start;
      int i;
      for(i=1; i<N; ++i) {
        mid = mid->next;
      }
      sort_part(l, cmp, start, mid, N);
      sort_part(l, cmp, mid->next, end, n_elems-N);

        list_data_t *merge_table = (list_data_t *) mc_malloc(sizeof(list_data_t) * n_elems);
        int i=0;
        list_entry_t *s = start;      // part 1
        list_entry_t *e = mid->next;  // part 2
        while (e != end->next && s != mid->next) {
          if ( cmp(s->data, e->data) <= 0) {
            merge_table[i++] = s->data;
            s = s->next;
          } else {
            merge_table[i++] = e->data;
            e = e->next;
          }
        }
        if (e == end->next) {
          while (s != mid->next) {
            merge_table[i++] = s->data;
            s = s->next;
          }
        } else if (s == mid->next) {
          while (e != end->next) {
            merge_table[i++] = e->data;
            e = e->next;
          }
        }
        s = start;
        for (i=0; i < n_elems; ++i) {
          s->data = merge_table[i];
          s = s->next;
        }
        mc_free(merge_table);
      
    }
  }
}*/


stat void _list_sort(list_t *l,int (*cmp)(list_data_t a,list_data_t b))
{
  //sort_part(l, cmp, l->first, l->last, l->count);
  
  int listSize=1,numMerges,leftSize,rightSize;
  list_entry_t *tail, *left, *right, *next;
  list_entry_t *list = l->first;
  
  if (list == NULL  || list->next == NULL) {
    return;
  }
  
  do { // For each power of two<=list length
    numMerges=0,left=list;tail=list=0; // Start at the start
    
    while (left) { // Do this list_len/listSize times:
      numMerges++,right=left,leftSize=0,rightSize=listSize;
      // Cut list into two halves (but don't overrun)
      while (right && leftSize<listSize) leftSize++,right=right->next;
      // Run through the lists appending onto what we have so far.
      while (leftSize>0 || (rightSize>0 && right)) {
        // Left empty, take right OR Right empty, take left, OR compare.
        if (!leftSize)                  {next=right;right=right->next;rightSize--;}
        else if (!rightSize || !right)  {next=left;left=left->next;leftSize--;}
        else if (compare(left->data,right->data)<0) {next=left;left=left->next;leftSize--;}
        else                            {next=right;right=right->next;rightSize--;}
        // Update pointers to keep track of where we are:
        if (tail) tail->next=next;  else list=next;
        // Sort prev pointer
        tail=next;
      }
      // Right is now AFTER the list we just sorted, so start the next sort there.
      left=right;
    }
    // Terminate the list, double the list-sort size.
    tail->next=0,listSize<<=1;
  } while (numMerges>1); // If we only did one merge, then we just sorted the whole list.
  
  // administrative update: One sweep to go
  l->first = list;
  list_entry_t *e = l->first;
  e->previous=NULL;
  while (e->next != NULL) {
    e->next->previous = e;
    e = e->next;
  }
  l->last = e;
  l->current = l->first;
}

