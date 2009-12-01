#ifndef CRC32_H_
#define CRC32_H_

#define CRC32POLY 0xEDB88320 /* CRC-32 Polynom */
#define BUFSIZE 16384 /* Read buffer */

int crc32(char *file, uint32_t *crc32);
void crc32_genTab(void);

#endif /*CRC32_H_*/
