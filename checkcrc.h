#ifndef CHECKCRC_H_
#define CHECKCRC_H_

void usage();
int crc32_match(const char *string, char *match);
char *basename(char *name);
int file_exists(char *file);
int main(int argc, char **argv);

#endif /*CHECKCRC_H_*/
