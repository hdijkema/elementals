//
//  memblock.c
//  
//
//  Created by fam. Oesterholt on 15-02-13.
//
//

#include <stdio.h>
#include <elementals/memblock.h>
#include <elementals/memcheck.h>
#include <elementals/log.h>

memblock_t *memblock_new() {
  memblock_t *e=(memblock_t *) mc_malloc(sizeof(memblock_t));
  e->block=NULL;
  e->size=0;
  e->pos=0;
  return e;
}

void memblock_clear(memblock_t *blk) {
  mc_free(blk->block);
  blk->block=NULL;
  blk->size=0;
  blk->pos=0;
}

void memblock_write(memblock_t *blk, void *bytes, size_t s) {
  if ((blk->size - blk->pos) < s) {
    size_t e = s - (blk->size - blk->pos);
    size_t t = e + blk->size;
    blk->block = (void *) mc_realloc(blk->block, t);
    blk->size = t;
    log_assert(blk->block != NULL);
  }
  memcpy(blk->block+blk->pos, bytes, s);
  blk->pos += s;
}

size_t memblock_read(memblock_t *blk, void *buf, size_t s) {
  int t = blk->size - blk->pos;
  if (t < s) { s=t; }
  memcpy(buf, blk->block+blk->pos, s);
  blk->pos += s;
  return s;
}

off_t memblock_seek(memblock_t *blk, off_t seek) {
  if (seek > blk->size) {
    seek = blk->size;
  }
  blk->pos = seek;
  return blk->pos;
}

void memblock_destroy(memblock_t *blk) {
  mc_free(blk->block);
  mc_free(blk);
}

size_t memblock_size(memblock_t *blk) {
  return blk->size;
}

off_t memblock_pos(memblock_t *blk) {
  return blk->pos;
}



