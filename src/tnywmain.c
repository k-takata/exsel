/*
	tnywmain.c
*/

#include <windows.h>

extern int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int nCmdShow);

void __cdecl WinMainCRTStartup(void)
{
	ExitProcess(WinMain(GetModuleHandle(NULL), NULL, NULL, 0));
}
