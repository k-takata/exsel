/*
	tnymain.c
*/

#include <windows.h>

extern int main(int argc, char *argv[]);

void __cdecl mainCRTStartup(void)
{
	ExitProcess(main(0, NULL));
}
