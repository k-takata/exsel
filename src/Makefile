#
#	This is a makefile of  "Executabe file Selector."
#
#	Copyright (C)  1998-2000  K.Takata
#


all : exsel2.exe

exsel2.exe : exsel.c exsel.h exselw.h
	lcc -o $@ exsel.c -lintlib -ltinymain.obj

exsel.h : execdw.exe mkarray.exe
	mkarray execdw.exe execdw > $@

exselw.h : execdww.exe mkarray.exe
	mkarray execdww.exe execdww > $@



execdww.exe : execdww.obj tnywmain.obj execdos.exe
	cl /Fo$@ execdww.obj tnywmain.obj kernel32.lib user32.lib /link /opt:nowin98 /stub:execdos.exe /merge:.data=.text /merge:.rdata=.text /section:.text,erw

execdww.obj : execdw.c
	cl /c /O1 /W4 /DWINMAIN /Fo$@ execdw.c

tnywmain.obj : tnywmain.c
	cl /c /O1 /W4 tnywmain.c


execdw.exe : execdw.obj tnymain.obj execdos.exe
	cl /Fo$@ execdw.obj tnymain.obj kernel32.lib user32.lib /link /opt:nowin98 /stub:execdos.exe /merge:.data=.text /merge:.rdata=.text /section:.text,erw

execdw.obj : execdw.c
	cl /c /O1 /W4 execdw.c

tnymain.obj : tnymain.c
	cl /c /O1 /W4 tnymain.c



execdos.exe : execdos.obj
	lld -o $@ execdos.obj
	stbhdr -f $@

execdos.obj : execdos.asm
	lcc -o $@ -c execdos.asm



mkarray.exe : mkarray.c
	lcc -o $@ $? -lintlib -ltinymain.obj
