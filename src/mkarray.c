/****************************************************************************/
/*		Mkarray.exe		Ver.1.20											*/
/*		Copyright (C) 1998-2000  K.Takata									*/
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
	char	hex;
	
	i = 0;
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fputs("mkarray : can't open file\n", stderr);
		return 1;
	}
	
	/* "char arrayname[] = {" */
	fputs("char ", stdout);
	fputs(arrayname, stdout);
	fputs("[] = {", stdout);
	
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
			fputs("\n\t", stdout);
		
		/* "0x%02x, " */
		fputs("0x", stdout);
		hex = (chr / 16) % 16;
		hex += (hex < 10) ? '0' : 'a' - 10;
		putchar(hex);
		hex = chr % 16;
		hex += (hex < 10) ? '0' : 'a' - 10;
		putchar(hex);
		fputs(", ", stdout);
	}
	fputs("\n};\n", stdout);
	fclose(fp);
	
	return 0;
}

void usage(void)
{
	fputs("usage : mkarray <filename> <arrayname>\n", stdout);
	exit(1);
}
