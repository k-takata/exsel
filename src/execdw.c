/*
	Execdw.c Ver.2.01	Copyright (C) 2000-2011  K.Takata
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>

#ifndef lengthof
#define lengthof(arr)	(sizeof(arr) / sizeof((arr)[0]))
#endif

#define USE_STRING_API

#ifdef USE_STRING_API
#undef _tcscpy
#undef _tcsicmp
#undef _tcsncpy
#undef _tcschr
#define _tcscpy			lstrcpy
#define _tcsicmp		lstrcmpi
#define _tcsncpy(d,s,l)	lstrcpyn((d), (s), (l) + 1)
#define _tcschr			StrChr
#endif /* USE_STRING_API */

#define DosID	"DOS="
#define WinID	"WIN="


#ifndef WINMAIN
static HANDLE hStdOut;

void myfputs(char *str, HANDLE handle)
{
	DWORD dwWritten;
	WriteFile(handle, str, lstrlenA(str), &dwWritten, NULL);
}
#endif

void message(char *str)
{
#ifndef WINMAIN
	myfputs("execdw: ", hStdOut);
	myfputs(str, hStdOut);
	myfputs("\n", hStdOut);
#else
	MessageBoxA(NULL, str, "execdw", MB_OK | MB_ICONEXCLAMATION);
#endif
}

/* メモリ内から文字列を検索 */
char *searchstr(const char *mem, size_t n, const char *str)
{
	const char *p;
	int len;
	
	len = lstrlenA(str);
	for (p = mem; p + len <= mem + n; p++) {
		if (!strncmp(p, str, len))
			return (char *)p;
	}
	return NULL;
}


#ifdef INIFILE
/*
 * getenvarg
 *
 * 環境変数の引数を解析し、変数名を arg に、値を val にコピー。
 * 次の引数へのポインタを返す。次の引数がないときは NULL を返す。
 */
LPTSTR getenvarg(LPCTSTR cmdline, LPTSTR arg, size_t argsize,
		LPTSTR val, size_t valsize)
{
	PTBYTE p = (PTBYTE) cmdline;
	PTBYTE start, end;
	TBYTE quote = _T('\0');
	size_t len;
	
	start = p;
	if (*p == _T('"') || *p == _T('\'')) {
		quote = *p;
		++p;
		start = p;
		while (*p && (*p != quote) && (*p != _T('=')))
			++p;
		end = p;
		if (*p == quote) {
			++p;
			quote = _T('\0');
		}
	} else {
		while (*p != _T('='))
			++p;
		end = p;
	}
	if (arg != NULL && argsize > 0) {
		len = end - start;
		if (len > argsize - 1)
			len = argsize - 1;
		_tcsncpy(arg, (LPTSTR) start, len);
#ifndef USE_STRING_API
		arg[len] = _T('\0');
#endif
	}
	++p;		// skip '='
	start = p;
	if (quote == _T('\0') && (*p == _T('"') || *p == _T('\''))) {
		quote = *p;
		++p;
		start = p;
	}
	if (quote != _T('\0')) {
		while (*p && (*p != quote))
			++p;
		end = p;
		if (*p == quote) {
			++p;
			quote = _T('\0');
		}
	} else {
		while (*p > _T(' '))
			++p;
		end = p;
	}
	if (val != NULL && valsize > 0) {
		len = end - start;
		if (len > valsize - 1)
			len = valsize - 1;
		_tcsncpy(val, (LPTSTR) start, len);
#ifndef USE_STRING_API
		val[len] = _T('\0');
#endif
	}
	while (*p && (*p <= _T(' ')))
		++p;
	if (*p == _T('\0'))
		return NULL;
	return (LPTSTR) p;
}
#endif

#pragma warning(disable:4100)	/* 'identifier' : unreferenced formal parameter */

/* Main */
#ifndef WINMAIN
int main(int argc, char *argv[])
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int nCmdShow)
#endif
{
	TCHAR buf[2048];
#ifdef INIFILE
	TCHAR inifile[MAX_PATH];
	TCHAR execname[MAX_PATH];
	TCHAR args[MAX_PATH];
	TCHAR envs[MAX_PATH];
	LPTSTR p;
#endif
	char *filename = NULL;
	char *p2;
	IMAGE_DOS_HEADER *img;
	DWORD ret, readbytes;
	TBYTE *lpszCommandLine;	// must be unsigned for multibyte strings
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	
#ifndef WINMAIN
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	
	lpszCommandLine = (TBYTE *) GetCommandLine();
	
	if (*lpszCommandLine == _T('"')) {
		do {
			lpszCommandLine++;
		} while (*lpszCommandLine && (*lpszCommandLine != _T('"')));
		
		if (*lpszCommandLine == _T('"')) {
			*lpszCommandLine++ = _T('\0');
		}
	} else {
		while (*lpszCommandLine > _T(' ')) {
			lpszCommandLine++;
		}
	}
	
	while (*lpszCommandLine && (*lpszCommandLine <= _T(' '))) {
		*lpszCommandLine++ = _T('\0');
	}
	
#ifdef INIFILE
	GetModuleFileName(NULL, inifile, lengthof(inifile));
	p = inifile + lstrlen(inifile) - 3;
	*p++ = _T('i');
	*p++ = _T('n');
	*p++ = _T('i');

	execname[0] = _T('\0');
	args[0] = _T('\0');
	envs[0] = _T('\0');

	if (GetFileAttributes(inifile) != (DWORD) -1) {
		GetPrivateProfileString(_T("exec"), _T("name"), _T(""),
				execname, lengthof(execname), inifile);
		GetPrivateProfileString(_T("exec"), _T("args"), _T(""),
				args, lengthof(args), inifile);
		GetPrivateProfileString(_T("exec"), _T("envs"), _T(""),
				envs, lengthof(envs), inifile);
	}
	if (execname[0]) {
		if (envs[0]) {
			LPTSTR env = inifile;	// reuse the inifile buffer
			LPTSTR val = buf;
			p = envs;
			while (p) {
				p = getenvarg(p, env, MAX_PATH, val, MAX_PATH);
				SetEnvironmentVariable(env, val[0] ? val : NULL);
			}
		}
		wsprintf(buf, _T("\"%s\" %s %s"), execname, args, lpszCommandLine);
	} else
#endif
	{
		img = (IMAGE_DOS_HEADER *) GetModuleHandle(NULL);
		readbytes = img->e_lfanew;	// offset to PE header

		p2 = searchstr((char *) img, readbytes, WinID);
		if (p2 == NULL) {
			message("Data is broken.");
			return 1;
		}

		filename = p2 + sizeof(WinID) - 1;
		if (*filename == '\0' || *filename == '\xff') {
			return 0;
		}

#ifdef UNICODE
		wsprintf(buf, _T("%hs %s"), filename, lpszCommandLine);
#else
		buf[0] = _T('\0');
		lstrcat(buf, filename);
		lstrcat(buf, _T(" "));
		lstrcat(buf, (LPTSTR) lpszCommandLine);
#endif
	}
	
	GetStartupInfo(&si);
	
	ret = CreateProcess(NULL, buf, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	
	if (ret) {
#ifndef WINMAIN
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &ret);
#else
		ret = 0;
#endif
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	} else {
		message("Can't exec.");
		ret = 1;
	}
	
	return ret;
}
