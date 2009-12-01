/**
 * checkcrc - crc32 checksum comparing of checksums found in file names
 * Copyright (C) 2009  Daniel Triendl <daniel@pew.cc>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <inttypes.h>

#include "crc32.h"

uint32_t crcTab[256];

/* Calculate the crc32 value of a file */
int crc32(char *file, uint32_t *crc32)
{
	FILE *f;
	char buf[BUFSIZE]; /* File read buffer */
	char *p;
	int i, n;
	uint32_t crc = 0xFFFFFFFF; /* Start value */
	
	if ((f=fopen(file, "rb"))) {
		/* Read BUFSIZE from file and calculate CRC32 */
		while ((n = fread(buf, 1, sizeof(buf), f))) {
			p = buf;
			for (i = 0; i < n; i++)
				crc = (crc >> 8) ^ crcTab[(crc ^ *p++) & 0xFF];
		}
		
		/* Invert result and return */
		*crc32 = ~crc;
		return 1;
	} else {
		return 0;
	}
}

/* Calculates a CRC32 Table, must be done prior to calling crc32() */
void crc32_genTab()
{
	int i, j;
	uint32_t crc;
	
	for (i=0; i<256; i++) {
		crc = i;
		for (j = 8; j > 0; j--) {
			if (crc & 1) {
				crc = (crc >> 1) ^ CRC32POLY;
			} else {
				crc >>= 1;
			}
		}
		crcTab[i] = crc;
	}
}
