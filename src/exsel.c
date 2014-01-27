/****************************************************************************/
/*		Executable file Selector	exsel.exe		Ver.1.11				*/
/*		Copyright (C) 1998  K.Takata										*/
/****************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exsel.h"								/* char execdw[]; */


#define	ProgNameSize	256
#define	DosID			"DOS="
#define	WinID			"WIN="


void getargs(int argc, char *argv[],
				char *execname, char *dosprog, char *winprog);
void inputprogname(char *execname, char *dosprog, char *winprog);
char *addext(char *filename, const char *ext);
int exsel(char *execprog, char *dosprog, char *winprog);
char *searchstr(const char *mem, size_t n, const char *str);
void usage(void);


/* ���C�� */
int main(int argc, char *argv[])
{
	static char	execname[ProgNameSize],
				dosprog[ProgNameSize],
				winprog[ProgNameSize];
	
	getargs(argc, argv, execname, dosprog, winprog);
	exit(exsel(execname, dosprog, winprog));
}

/* �R�}���h���C���̉�� */
void getargs(int argc, char *argv[],
				char *execname, char *dosprog, char *winprog)
{
	int		f_exec = 0,
			f_dos = 0,
			f_win = 0;
	
	*execname = *dosprog = *winprog = '\x00';
	while (--argc) {
		char	*p;
		
		p = *++argv;
		if (*p == '-' || *p == '/') {			/* �X�C�b�`�w�� �`�F�b�N */
			switch(tolower(*++p)) {
			case 'n':
				if (tolower(*++p) == 'w') {		/* -nw */
					f_win = 1;
					*winprog = '\xff';
				}
				break;
			
			case 'k':							/* -k */
				inputprogname(execname, dosprog, winprog);
				f_exec = f_dos = f_win = 1;
				break;
			
			case '?':							/* -h, -? */
			case 'h':
			default:
				usage();
				break;
			}
		} else {
			if (!f_exec) {
				f_exec = 1;
				strcpy(execname, p);
				addext(execname, "com");
			} else if (!f_dos) {
				f_dos = 1;
				strcpy(dosprog, p);
			} else if (!f_win) {
				f_win = 1;
				strcpy(winprog, p);
			}
		}
	}
	if (!f_dos)
		usage();
}

/* �v���O����������͂��� */
void inputprogname(char *execname, char *dosprog, char *winprog)
{
	if (*execname == '\x00') {
		fputs("Input launcher program name : ", stdout);
		gets(execname);
		addext(execname, "com");
	}
	if (*dosprog == '\x00') {
		fputs("Input MS-DOS program name : ", stdout);
		gets(dosprog);
	}
	if (*winprog == '\x00') {
		fputs("Input Windows program name : ", stdout);
		gets(winprog);
	}
}

/* �t�@�C�����Ɋg���q��t������ */
char *addext(char *filename, const char *ext)
{
	char	*p;
	
	p = strrchr(filename, '\\');
	if (p == NULL)
		p = filename;
	if (strchr(p, '.') == NULL)
		strcat(strcat(filename, "."), ext);
	
	return filename;
}

/* �Z���N�^�v���O�����̐��� */
int exsel(char *execprog, char *dosprog, char *winprog)
{
	FILE	*fp;
	char	*p1, *p2;
	
	p1 = searchstr(execdw, sizeof(execdw), DosID);
	p2 = searchstr(execdw, sizeof(execdw), WinID);
	if ((p1 == NULL) || (p2 == NULL)) {
		fputs("\nexsel : data is broken\n", stderr);
		return 1;
	}
	
	/* MS-DOS �N�����ɋN������v���O���������Z�b�g */
	strcpy(p1 + strlen(DosID), dosprog);
	
	/* Windows �N�����ɋN������v���O���������Z�b�g */
	strcpy(p2 + strlen(WinID), winprog);
	
	fp = fopen(execprog, "wb");
	if (fp == NULL) {
		fputs("\nexsel : can't open file\n", stderr);
		return 1;
	}
	fwrite(execdw, sizeof(execdw), 1, fp);
	fclose(fp);
	return 0;
}

/* �����������當��������� */
char *searchstr(const char *mem, size_t n, const char *str)
{
	const char	*p;
	int		len;
	
	len = strlen(str);
	for (p = mem; p < mem + n; p++) {
		if (*p != *str)
			continue;
		if (!strncmp(p, str, len))
			return (char *)p;
	}
	return NULL;
}

/* �g�p�@�\�� */
void usage(void)
{
	char	*msg[] = {
		"Executable file Selector  exsel.exe  Ver.1.11",
		"Copyright (C) 1998  K.Takata\n",
		"usage : exsel [<option>] <execname> <dosprog> [<winprog>]\n",
		" <option>    -nw    : Windows �N�����̓v���O�������N�����Ȃ�",
		"             -k     : <execname>, <dosprog>, <winprog> ���L�[���͂���",
		"             -h, -? : ���̃w���v��\������\n",
		" <execname>  ��������Z���N�^�v���O������",
		" <dosprog>   MS-DOS �N�����ɋN������v���O������",
		" <winprog>   Windows �N�����ɋN������v���O������",
		""
	};
	char	**pmsg = msg;
	
	while (**pmsg)
		puts(*pmsg++);
	exit(1);
}
