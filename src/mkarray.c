/****************************************************************************/
/*		Mkarray.exe		Ver.1.11											*/
/*		Copyright (C) 1998  K.Takata										*/
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>


int mkarray(char *filename, char *arrayname);
void usage(void);

int main(int argc, char *argv[])
{
	if (argc < 3)
		usage();
	exit(mkarray(argv[1], argv[2]));
}

int mkarray(char *filename, char *arrayname)
{
	FILE	*fp;
	int		chr, i;

	i = 0;
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fputs("mkarray : can't open file\n", stderr);
		return 1;
	}
	printf("char %s[] = {", arrayname);
	while (1) {
		chr = fgetc(fp);
		if (chr == EOF) {
			if (feof(fp)) {
				break;
			} else if (ferror(fp)) {
				fputs("mkarray : can't read file\n", stderr);
				fclose(fp);
				return 1;
			}
		}
		if ((i++ % 10) == 0)
			printf("\n\t");
		printf("0x%02x, ", chr);
	}
	printf("\n};\n");
	fclose(fp);
	return 0;
}

void usage(void)
{
	fputs("usage : mkarray <filename> <arrayname>\n", stdout);
	exit(1);
}
