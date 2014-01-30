/****************************************************************************/
/*		Executable file Selector	exsel.exe		Ver.2.01				*/
/*		Copyright (C) 1998-2011  K.Takata									*/
/****************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exsel.h"								/* char execdw[]; */
#include "exselw.h"								/* char execdww[]; */
#include "exsel1.h"								/* char execdw1[]; */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mbstring.h>
#define jstrrchr(a, b)	_mbsrchr(a, b)
#else /* _WIN32 */
#include <jstring.h>
#include <dos.h>
#endif /* _WIN32 */

#define	ProgNameSize	260
#define	DosID			"DOS="
#define	WinID			"WIN="


void inputprogname(char *execname, char *dosprog, char *winprog);
char *addext(char *filename, const char *ext);
char *chgext(char *filename, const char *ext);
int cvt(const char *execname, char *dosprog, char *winprog, int progtype);
int copy(const char *srcname, const char *dstname);
int exsel(const char *execname, const char *dosprog, const char *winprog,
		int progtype);
int readprogname(const char *execname, char *dosprog, char *winprog,
		int *f_cons);
char *searchstr(const char *mem, size_t n, const char *str);
void usage(void);


/* メイン */
int main(int argc, char *argv[])
{
	int progtype = -1;
	int f_exec = 0, f_dos = 0, f_win = 0, f_cvt = 0;
	static char execname[ProgNameSize],
				dosprog[ProgNameSize],
				winprog[ProgNameSize];
	
	execname[0] = dosprog[0] = winprog[0] = '\x00';
	while (--argc) {
		char *p;
		
		p = *++argv;
		if (*p == '-' || *p == '/') {			/* スイッチ指定 チェック */
			switch (tolower(*++p)) {
			case 'n':
				if (strcmp(p, "nw") == 0) {			/* -nw */
					f_win = 1;
					winprog[0] = '\xff';
					winprog[1] = '\x00';
				} else if (strcmp(p, "nd") == 0) {	/* -nd */
					f_dos = 1;
					dosprog[0] = '\xff';
					dosprog[1] = '\x00';
				}
				break;
			
			case 'k':							/* -k */
				inputprogname(execname, dosprog, winprog);
				f_exec = f_dos = f_win = 1;
				break;
			
			case 'c':
				if (strcmp(p, "cvt") == 0) {	/* -cvt */
					f_cvt = 1;
				}
				break;
			
			case 'w':							/* -w */
				progtype = 2;		/* not open console on windows */
				break;
			
			case 't':							/* -t */
				switch (tolower(*++p)) {
				case 'd':	/* -td: DOS */
					progtype = 0;
					break;
				case 'c':	/* -tc: Win32 Console */
					progtype = 1;
					break;
				case 'w':	/* -tw: Win32 GUI */
					progtype = 2;
					break;
				}
				break;
			
			case '?':							/* -h, -? */
			case 'h':
			default:
				usage();
				break;
			}
		} else {
			if (f_cvt) {
				f_dos = 1;
				strcpy(execname, p);
				addext(execname, "com");
				cvt(execname, dosprog, winprog, progtype);
			} else if (!f_exec) {
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
	if (f_cvt)
		return 0;
	if (progtype < 0)
		progtype = 1;	/* Default: Win32 Console */
	return exsel(execname, dosprog, winprog, progtype);
}

/* プログラム名を入力する */
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

/* ファイル名に拡張子を付加する */
char *addext(char *filename, const char *ext)
{
	char *p;
	
	p = jstrrchr(filename, '\\');
	if (p == NULL)
		p = filename;
	if (strchr(p, '.') == NULL) {
		strcat(filename, ".");
		strcat(filename, ext);
	}
	return filename;
}

/* 拡張子を付け替える */
char *chgext(char *filename, const char *ext)
{
	char *p;
	
	p = jstrrchr(filename, '\\');
	if (p == NULL)
		p = filename;
	p = strchr(p, '.');
	if (p != NULL) {
		*p = '\0';
	}
	strcat(filename, ".");
	strcat(filename, ext);
	
	return filename;
}

/* 新バージョンへの変換 */
int cvt(const char *execname, char *dosprog, char *winprog, int progtype)
{
	int ret, pt;
	char buf[ProgNameSize];
	
	printf("converting %s\n", execname);
	ret = readprogname(execname, dosprog, winprog, &pt);
	if (ret) {
		return ret;
	}
	strcpy(buf, execname);
	chgext(buf, "old");
	if (copy(execname, buf)) {	/* backup */
		fputs("exsel : can't backup\n", stderr);
		return 1;
	}
	
	if (progtype < 0) {
		if (pt == 0)	/* DOS ? */
			progtype = 1;	/* Win32 Console */
		else
			progtype = pt;	/* Win32 Console or GUI */
	}
	return exsel(execname, dosprog, winprog, progtype);
}

/* ファイルをコピー */
int copy(const char *srcname, const char *dstname)
{
#ifdef _WIN32
	return !CopyFile(srcname, dstname, FALSE);
#else /* _WIN32 */
	FILE *fpi, *fpo;
	char buf[1024];
	int len;
	unsigned date, time, attr;
	
	remove(dstname);
	fpi = fopen(srcname, "rb");
	fpo = fopen(dstname, "wb");
	if (fpi == NULL || fpo == NULL) {
		fputs("exsel : can't open file\n", stderr);
		if (fpi)
			fclose(fpi);
		if (fpo)
			fclose(fpo);
		return 1;
	}
	while ((len = fread(buf, 1, sizeof(buf), fpi)) != 0) {
		fwrite(buf, 1, len, fpo);
	}
	_dos_getftime(fileno(fpi), &date, &time);
	_dos_setftime(fileno(fpo), date, time);
	fclose(fpo);
	fclose(fpi);
	_dos_getfileattr(srcname, &attr);
	_dos_setfileattr(dstname, attr);
	return 0;
#endif /* _WIN32 */
}

/* セレクタプログラムの生成 */
int exsel(const char *execname, const char *dosprog, const char *winprog,
		int progtype)
{
	FILE *fp;
	char *p1, *p2, *pexecdw;
	int size;
	
	switch (progtype) {
	case 0:
		pexecdw = execdw1;
		size = sizeof(execdw1);
		break;
	case 1:
		pexecdw = execdw;
		size = sizeof(execdw);
		break;
	case 2:
		pexecdw = execdww;
		size = sizeof(execdww);
		break;
	}
	
	p1 = searchstr(pexecdw, size, DosID);
	p2 = searchstr(pexecdw, size, WinID);
	if ((p1 == NULL) || (p2 == NULL)) {
		fputs("exsel : data is broken\n", stderr);
		return 1;
	}
	
	memset(p1 + strlen(DosID), 0, ProgNameSize);
	memset(p2 + strlen(WinID), 0, ProgNameSize);
	
	/* MS-DOS 起動時に起動するプログラム名をセット */
	strcpy(p1 + strlen(DosID), dosprog);
	
	/* Windows 起動時に起動するプログラム名をセット */
	strcpy(p2 + strlen(WinID), winprog);
	
	fp = fopen(execname, "wb");
	if (fp == NULL) {
		fputs("exsel : can't open file\n", stderr);
		return 1;
	}
	fwrite(pexecdw, size, 1, fp);
	fclose(fp);
	return 0;
}

/* プログラム名を指定ファイルから読み込む */
int readprogname(const char *execname, char *dosprog, char *winprog,
		int *progtype)
{
	FILE *fp;
	char *p1, *p2;
	static char buf[2048];
	
	fp = fopen(execname, "rb");
	if (fp == NULL) {
		fputs("exsel : can't open file\n", stderr);
		return 1;
	}
	fread(buf, sizeof(buf), 1, fp);
	fclose(fp);
	
	p1 = searchstr(buf, sizeof(buf), DosID);
	p2 = searchstr(buf, sizeof(buf), WinID);
	if ((p1 == NULL) || (p2 == NULL)) {
		fputs("exsel : data is broken\n", stderr);
		return 1;
	}
	
	/* MS-DOS 起動時に起動するプログラム名をセット */
	strcpy(dosprog, p1 + strlen(DosID));
	
	/* Windows 起動時に起動するプログラム名をセット */
	strcpy(winprog, p2 + strlen(WinID));
	
	*progtype = 0;
	if (buf[0] == 'M' && buf[1] == 'Z') {
		char *p = buf + *(unsigned long *) (buf + 0x3c);
		if (p[0] == 'P' && p[1] == 'E' && p[2] == '\0' && p[3] == '\0') {
			/* PE */
			if (*(unsigned short *) (p + 0x5c) == 2) {
				/* Subsystem: Windows GUI */
				*progtype = 2;
			} else {
				*progtype = 1;
			}
		}
	}
	
	return 0;
}

/* メモリ内から文字列を検索 */
char *searchstr(const char *mem, size_t n, const char *str)
{
	const char *p, *q, *r;
	
	for (p = mem; p < mem + n; p++) {
		for (q = p, r = str;
				(q < mem + n) && (*r != '\0') && (*q == *r);
				q++, r++)
			;
		if (*r == '\0')
			return (char *)p;
	}
	return NULL;
}

/* 使用法表示 */
void usage(void)
{
	char *msg[] = {
		"Executable file Selector  exsel.exe  Ver.2.01",
		"Copyright (C) 1998-2011  K.Takata\n",
		"usage : exsel [<option>] <execname> [<dosprog> [<winprog>]]\n",
		" <option>    -nw    : Windows 起動時はプログラムを起動しない",
		"             -nd    : DOS 起動時はプログラムを起動しない",
		"             -k     : <execname>, <dosprog>, <winprog> をキー入力する",
		"             -cvt   : <execname> を新バージョンに変換する",
		"             -td    : DOS 形式のセレクタプログラムを生成する",
		"             -tc    : Win32 CUI 形式のセレクタプログラムを生成する",
		"             -tw,-w : Win32 GUI 形式のセレクタプログラムを生成する",
		"             -h, -? : このヘルプを表示する\n",
		" <execname>  生成するセレクタプログラム名",
		" <dosprog>   MS-DOS 起動時に起動するプログラム名",
		" <winprog>   Windows 起動時に起動するプログラム名",
		""
	};
	char **pmsg = msg;
	
	while (**pmsg)
		puts(*pmsg++);
	exit(1);
}
