/****************************************************************************/
/*		Executable file Selector	exsel.exe		Ver.1.20				*/
/*		Copyright (C) 1998-2000  K.Takata									*/
/****************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jstring.h>
#include <dos.h>
#include "exsel.h"								/* char execdw[]; */
#include "exselw.h"								/* char execdww[]; */


#define	ProgNameSize	256
#define	DosID			"DOS="
#define	WinID			"WIN="


int getargs(int argc, char *argv[],
		char *execname, char *dosprog, char *winprog, int *nocons);
void inputprogname(char *execname, char *dosprog, char *winprog);
char *addext(char *filename, const char *ext);
char *chgext(char *filename, const char *ext);
int cvt(const char *execname, char *dosprog, char *winprog);
int copy(const char *srcname, const char *dstname);
int exsel(const char *execname, const char *dosprog, const char *winprog,
		int nocons);
int readprogname(const char *execname, char *dosprog, char *winprog);
char *searchstr(const char *mem, size_t n, const char *str);
void usage(void);


/* メイン */
int main(int argc, char *argv[])
{
	int ret, nocons;
	static char execname[ProgNameSize],
				dosprog[ProgNameSize],
				winprog[ProgNameSize];
	
	ret = getargs(argc, argv, execname, dosprog, winprog, &nocons);
	if (ret) {
		return ret;
	}
	return exsel(execname, dosprog, winprog, nocons);
}

/* コマンドラインの解析 */
int getargs(int argc, char *argv[],
		char *execname, char *dosprog, char *winprog, int *nocons)
{
	int f_exec = 0,
		f_dos = 0,
		f_win = 0,
		f_cvt = 0;
	
	*execname = *dosprog = *winprog = '\x00';
	*nocons = 0;
	while (--argc) {
		char *p;
		
		p = *++argv;
		if (*p == '-' || *p == '/') {			/* スイッチ指定 チェック */
			switch(tolower(*++p)) {
			case 'n':
				if (strcmp(p, "nw") == 0) {		/* -nw */
					f_win = 1;
					*winprog = '\xff';
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
				*nocons = 1;		/* not open console on windows */
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
				if (f_cvt) {
					return cvt(execname, dosprog, winprog);
				}
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
	return 0;
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

/* 新バージョンへの変換（前処理） */
int cvt(const char *execname, char *dosprog, char *winprog)
{
	int ret;
	char buf[ProgNameSize];
	ret = readprogname(execname, dosprog, winprog);
	if (ret) {
		return ret;
	}
	strcpy(buf, execname);
	chgext(buf, "old");
	copy(execname, buf);
	return 0;
}

/* ファイルをコピー */
int copy(const char *srcname, const char *dstname)
{
	FILE *fpi, *fpo;
	char buf[1024];
	int len;
	unsigned date, time, attr;
	
	remove(dstname);
	fpi = fopen(srcname, "rb");
	fpo = fopen(dstname, "wb");
	if (fpi == NULL || fpo == NULL) {
		fputs("\nexsel : can't open file\n", stderr);
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
}

/* セレクタプログラムの生成 */
int exsel(const char *execname, const char *dosprog, const char *winprog,
		int nocons)
{
	FILE *fp;
	char *p1, *p2, *pexecdw;
	int size;
	
	if (nocons) {
		pexecdw = execdww;
		size = sizeof(execdww);
	} else {
		pexecdw = execdw;
		size = sizeof(execdw);
	}
	
	p1 = searchstr(pexecdw, size, DosID);
	p2 = searchstr(pexecdw, size, WinID);
	if ((p1 == NULL) || (p2 == NULL)) {
		fputs("\nexsel : data is broken\n", stderr);
		return 1;
	}
	
	/* MS-DOS 起動時に起動するプログラム名をセット */
	strcpy(p1 + strlen(DosID), dosprog);
	
	/* Windows 起動時に起動するプログラム名をセット */
	strcpy(p2 + strlen(WinID), winprog);
	
	fp = fopen(execname, "wb");
	if (fp == NULL) {
		fputs("\nexsel : can't open file\n", stderr);
		return 1;
	}
	fwrite(pexecdw, size, 1, fp);
	fclose(fp);
	return 0;
}

/* プログラム名を指定ファイルから読み込む */
int readprogname(const char *execname, char *dosprog, char *winprog)
{
	FILE *fp;
	char *p1, *p2;
	char buf[1024];
	
	fp = fopen(execname, "rb");
	if (fp == NULL) {
		fputs("\nexsel : can't open file\n", stderr);
		return 1;
	}
	fread(buf, sizeof(buf), 1, fp);
	fclose(fp);
	
	p1 = searchstr(buf, sizeof(buf), DosID);
	p2 = searchstr(buf, sizeof(buf), WinID);
	if ((p1 == NULL) || (p2 == NULL)) {
		fputs("\nexsel : data is broken\n", stderr);
		return 1;
	}
	
	/* MS-DOS 起動時に起動するプログラム名をセット */
	strcpy(dosprog, p1 + strlen(DosID));
	
	/* Windows 起動時に起動するプログラム名をセット */
	strcpy(winprog, p2 + strlen(WinID));
	
	return 0;
}

/* メモリ内から文字列を検索 */
char *searchstr(const char *mem, size_t n, const char *str)
{
	const char *p;
	int len;
	
	len = strlen(str);
	for (p = mem; p < mem + n; p++) {
		if (*p != *str)
			continue;
		if (!strncmp(p, str, len))
			return (char *)p;
	}
	return NULL;
}

/* 使用法表示 */
void usage(void)
{
	char *msg[] = {
		"Executable file Selector  exsel.exe  Ver.1.20",
		"Copyright (C) 1998-2000  K.Takata\n",
		"usage : exsel [<option>] <execname> [<dosprog> [<winprog>]]\n",
		" <option>    -nw    : Windows 起動時はプログラムを起動しない",
		"             -k     : <execname>, <dosprog>, <winprog> をキー入力する",
		"             -cvt   : <execname> を新バージョンに変換する",
		"             -w     : Windows 起動時はコンソールを開かない",
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
