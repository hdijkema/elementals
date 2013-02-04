/* 
   This file is part of elementals.
   Copyright 2013, Hans Oesterholt <debian@oesterholt.net>

   Elementals are free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with elementals.  If not, see <http://www.gnu.org/licenses/>.

   ********************************************************************
*/

/*
 * log.c
 *
 *  Created on: 3 okt. 2011
 *      Author: hans
 */

#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *f=NULL;

// TODO: Deze code moet robuuster!
FILE *log_handle(void) {
	if (f==NULL) {
		char fn[8192];
		sprintf(fn,"%s/.syncfuse/syncfuse.log",getenv("HOME"));
		f=fopen(fn,"wt");
	}
	return f;
}

void reset_log_handle_to_space(const char *space) {
	if (f!=NULL) {
		fclose(f);
	}
	char fn[8192];
	sprintf(fn,"%s/.syncfuse/%s.log",getenv("HOME"),space);
	f=fopen(fn,"wt");
}
