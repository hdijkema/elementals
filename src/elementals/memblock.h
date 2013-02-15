//
//  memblock.h
//  
//
//  Created by fam. Oesterholt on 15-02-13.
//
//

#ifndef _memblock_h
#define _memblock_h

#include <stdio.h>

typedef struct {
  void *block;
  size_t size;
  off_t pos;
} memblock_t;

memblock_t *memblock_new();
void        memblock_write(memblock_t *blk, void *bytes, size_t size);
size_t      memblock_read(memblock_t *blk, void *buf, size_t size);
off_t       memblock_seek(memblock_t *blk, off_t pos);
void        memblock_destroy(memblock_t *blk);
size_t      memblock_size(memblock_t *blk);
off_t       memblock_pos(memblock_t *blk);

#endif
