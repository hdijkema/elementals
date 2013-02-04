#include "crc.h"

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


