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
#include <elementals/crc.h>

static unsigned long crc_table[256];

static void make_crc_table(void) {
  unsigned long i;
    for (i = 0; i < 256; i++) {
      unsigned long c = i;
      int j;
        for (j = 0; j < 8; j++) {
          c = (c & 1) ? (0xEDB88320 ^ (c >> 1)) : (c >> 1);
      }
      crc_table[i] = c;
    }
}

unsigned long str_crc32(const char *buf) {
  static int init=1;
  if (init) { make_crc_table();init=0; }

    unsigned long c = 0xFFFFFFFF;
    for (;*buf!='\0';buf++) {
      unsigned char b=(unsigned char) *buf;
        c = crc_table[(c ^ b) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFF;
}


