/*
 *	tnymain.c
 */

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOIME
#include <windows.h>

#ifndef MAXARGS
#define MAXARGS	64
#endif /* MAXARGS */

#ifdef USE_CMDLINE
#define NAKED
#else
#define NAKED	__declspec(naked)
#endif


extern int __cdecl main(int argc, char *argv[]);

NAKED void /*__cdecl*/ mainCRTStartup(void)
{
#ifdef USE_CMDLINE
	/*static*/ char *argv[MAXARGS];
	int argc;
	unsigned char c, *p;
	unsigned char *s = GetCommandLine();
	
	argc = 0;
	do {
		while ((c = *s) == ' ' || c == '\t')
			*s++;
		if (c == '\0')
			break;
		if (c == '\"') {
			p = ++s;
			while ((c = *s) && c != '\"') {
				if (c == '\t')
					*s = ' ';
				s++;
			}
		} else {
			p = s;
			while (*s > ' ')
				s++;
		}
		*s++ = '\0';
		argv[argc++] = p;
	} while (argc < MAXARGS);
	argv[argc] = NULL;
	
	ExitProcess(main(argc, (char **) &argv));
#else /* USE_CMDLINE */
	ExitProcess(main(0, NULL));
#endif /* USE_CMDLINE */
}
