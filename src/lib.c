/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * lib.c
shell-init: fout tijdens bepalen van huidige map: getwd(): kan geen geen toegang verkrijgen tot bovenliggende mappen: Bestand of map bestaat niet
chdir: fout tijdens bepalen van huidige map: getwd(): kan geen geen toegang verkrijgen tot bovenliggende mappen: Bestand of map bestaat niet
 * Copyright (C) 2013 Hans Oesterholt <debian@oesterholt.net>
 * 
 * elementals is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * elementals is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>

int elementals_func(void)
{
	printf("Hello world\n");
	return (0);
}
