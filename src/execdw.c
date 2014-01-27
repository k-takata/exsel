/*
	Execdw.c Ver.2.00	Copyright (C) 2000-2005  K.Takata
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DosID	"DOS="
#define WinID	"WIN="


#ifndef WINMAIN
void myfputs(char *str, HANDLE handle)
{
	DWORD dwWritten;
	WriteFile(handle, str, lstrlen(str), &dwWritten, NULL);
}
#endif

void message(char *str)
{
#ifndef WINMAIN
	static HANDLE hStdOut = INVALID_HANDLE_VALUE;
	
	if (hStdOut == INVALID_HANDLE_VALUE)
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	
	myfputs("execdw: ", hStdOut);
	myfputs(str, hStdOut);
	myfputs("\n", hStdOut);
#else
	MessageBox(NULL, str, "execdw", MB_OK | MB_ICONEXCLAMATION);
#endif
}

/* �����������當��������� */
char *searchstr(const char *mem, size_t n, const char *str)
{
	const char *p;
	char c = *str;
	int len;
	
	len = lstrlen(str);
	for (p = mem; p < mem + n; p++) {
		if (*p != c)
			continue;
		if (!strncmp(p, str, len))
			return (char *)p;
	}
	return NULL;
}


/* Main */
#ifndef WINMAIN
int main(int argc, char *argv[])
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int nCmdShow)
#endif
{
	char /*filename[MAX_PATH],*/ buf[2048];
	char *filename;
	char *img;
	DWORD ret, readbytes;
	unsigned char *lpszCommandLine, *p1, *p2;
//	HANDLE fp;
	PROCESS_INFORMATION pi;
	STARTUPINFO si /*= {sizeof(STARTUPINFO)}*/;
	
	
	lpszCommandLine = GetCommandLine();
	
	if (*lpszCommandLine == '"') {
		do {
			lpszCommandLine++;
		} while (*lpszCommandLine && (*lpszCommandLine != '"'));
		
		if (*lpszCommandLine == '"') {
			*lpszCommandLine++ = '\0';
		}
	} else {
		while (*lpszCommandLine > ' ') {
			lpszCommandLine++ ;
		}
	}
	
	while (*lpszCommandLine && (*lpszCommandLine <= ' ')) {
		*lpszCommandLine++ = '\0';
	}
	
	/*
	GetModuleFileName(NULL, filename, sizeof(filename));
	
	fp = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_READ,
			NULL, OPEN_EXISTING, 0, NULL);
	if (fp == INVALID_HANDLE_VALUE) {
		message("File open error.");
		return 1;
	}
	ReadFile(fp, buf, sizeof(buf), &readbytes, NULL);
	CloseHandle(fp);
	*/
	img = (char *) GetModuleHandle(NULL);
	readbytes = *(DWORD *) (img + 0x3c);	// offset to PE header
	
	
//	p1 = searchstr(buf, readbytes, DosID);
	p2 = searchstr(img, readbytes, WinID);
	if (/*(p1 == NULL) ||*/ (p2 == NULL)) {
		message("Data is broken.");
		return 1;
	}
	
//	filename[0] = '\0';
	/* Windows �N�����ɋN������v���O���������Z�b�g */
//	lstrcat(filename, p2 + lstrlen(WinID));
//	wsprintf(filename, "%s %s" + 3, p2 + lstrlen(WinID));
	
	filename = p2 + sizeof(WinID) - 1;
	
	if (*filename == '\0' || *filename == '\xff') {
		return 0;
	}
	
//	wsprintf(buf, "%s %s", filename, lpszCommandLine);
	buf[0] = '\0';
	lstrcat(buf, filename);
	lstrcat(buf, " ");
	lstrcat(buf, lpszCommandLine);
	
	GetStartupInfo(&si);
	
	ret = CreateProcess(NULL, buf, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
//	ret = CreateProcess(filename, lpszCommandLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	
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
