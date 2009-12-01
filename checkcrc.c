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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <inttypes.h>
#include <regex.h>
#include <unistd.h>

#include "checkcrc.h"
#include "crc32.h"

/* Print file usage */
void usage()
{
	printf("Usage: %s [-cvh] [file ...]\n"
			"\n"
			"\t-c\tOnly calculate checksums, don't try to find a checksum string to compare\n"
			"\t-v\tDisplay version number\n"
			"\t-g\tDisplay this help\n", PACKAGE);
}

/* Try to find a CRC32-like hex number in a string */
int crc32_match(const char *string, char *match) {
	int status;
	regex_t re;
	regmatch_t rm[1];

	/* Try to complie the regex */
	if(regcomp(&re, "[a-fA-F0-9]{8}", REG_EXTENDED) != 0) {
		return 0;
	}
	
	/* Run the regex against the string */
	status = regexec(&re, string, 1, rm, 0);

	regfree(&re);
	
	if(status != 0) {
		return 0;
	}
	
	/* If we got a match, copy matching string to match */
	strncpy(match, string + rm[0].rm_so, 8);
	return 1;
}

/* Test if the file exists */
int file_exists(char *file)
{
	FILE *f;
	
	if((f = fopen(file, "r"))) {
		fclose(f);
		return 1;
	}
	return 0;
}

/* Gets the last component of a path *
char *basename(char *name)
{
	char *np;
	
	for(np = name; *name; name++) {
		if(*name == '/') np = name + 1;
	}
	
	return (char *)np;
}*/

/* The main routine */
int main(int argc, char **argv)
{
	int i, iexit = 0;
	uint32_t crc_found, crc_calc;
	char result[9] = "";
	char ch, mode = 0;
	
	if(argc <= 1) {
		usage(argv[0]);
		exit(1);
	}
	
	while ((ch = getopt(argc, argv, "chv")) != -1)
	switch (ch) {
		case 'h':
			usage();
			exit(0);
			break;
		case 'c':
			mode = 1;
			break;
		case 'v':
			printf("%s: Version %s\n", PACKAGE, VERSION);
			exit(0);
			break;
	}
	
	/* Generate the CRC32 table */
	crc32_genTab();
	
	/* Loop through the files */
	for(i=optind; i<argc; i++) {
		printf("%s: ", argv[i]);
		if(file_exists(argv[i])) {
			if(mode == 0) {
				/* Searching for CRC32 checksum */
				if (crc32_match(basename(argv[i]), result)) {
					/*
					 * If we found something that looks like a CRC checksume
					 * calculate the CRC32 value of the file
					 */
					crc_found = strtoll(result, NULL, 16);
					if (crc32(argv[i], &crc_calc)) {
						printf("%08X | %08X: ", crc_found, crc_calc);
						if (crc_found == crc_calc) {
							printf("OK\n");
						} else {
							printf("CRC ERROR!\n");
							iexit = 1;
						}
					} else {
						printf( "Error calculating CRC32\n");
					}
				} else {
					printf("CRC not found\n");
				}
			} else if(mode==1) {
				if(crc32(argv[i], &crc_calc)) {
					printf("%08X\n", crc_calc);
				} else {
					printf("CRC ERROR!\n");
					iexit = 1;
				}
			}
		} else {
			printf("not found\n");
		}
	}
	
	exit(iexit);
}
