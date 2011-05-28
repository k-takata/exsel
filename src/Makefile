#
#	This is a makefile of  "Executabe file Selector."
#
#	Copyright (C)  1998-2011  K.Takata
#

CC = cl /nologo
CC16 = lcc


all : exsel.exe

#exsel.exe : exsel.c exsel.h exselw.h exsel1.h
#	lcc -o $@ exsel.c -lintlib -lexpand.obj
exsel.exe : exsel.c exsel.h exselw.h exsel1.h
	$(CC) /MD /Fe$@ /O1 exsel.c /link setargv.obj /opt:nowin98 /merge:.rdata=.text

exsel.h : execdw.exe mkarray.exe
	mkarray execdw.exe execdw > $@
exselw.h : execdww.exe mkarray.exe
	mkarray execdww.exe execdww > $@
exsel1.h : execdw1.com mkarray.exe
	mkarray execdw1.com execdw1 > $@



execdww.exe : execdww.obj tnywmain.obj execdos.exe
	$(CC) /Fo$@ execdww.obj tnywmain.obj kernel32.lib user32.lib /link /opt:nowin98 /stub:execdos.exe /merge:.rdata=.text

execdww.obj : execdw.c
	$(CC) /c /O1 /W4 /GF /FAsc /DWINMAIN /Fo$@ execdw.c

tnywmain.obj : tnywmain.c
	$(CC) /c /O1 /W4 /GF /FAsc tnywmain.c


execdw.exe : execdw.obj tnymain.obj execdos.exe
	$(CC) /Fo$@ execdw.obj tnymain.obj kernel32.lib user32.lib /link /opt:nowin98 /stub:execdos.exe /merge:.rdata=.text

execdw.obj : execdw.c
	$(CC) /c /O1 /W4 /GF /FAsc execdw.c

tnymain.obj : tnymain.c
	$(CC) /c /O1 /W4 /GF /FAsc tnymain.c



execdos.exe : execdos.obj
	lcc -a -o $@ execdos.obj
	stbhdr -f $@
#execdos.obj : execdos.asm
#	lcc -o $@ -c execdos.asm
execdos.obj : execdos.asm
	ml /Zm /c execdos.asm


execdw1.com : execdw1.obj
	lcc -a -o $@ $?
execdw1.obj : execdw.asm
	ml /Zm /c /Fo$@ execdw.asm



mkarray.exe : mkarray.c
	$(CC) /O1 /MD $? /link /opt:nowin98 /merge:.rdata=.text


clean :
	del execdos.exe
	del execdos.old
	del execdw.exe
	del execdww.exe
	del execdw1.com
	del exsel.exe
	del mkarray.exe
	del execdos.obj
	del execdw.obj
	del execdw.cod
	del execdww.obj
	del execdww.cod
	del execdw1.obj
	del exsel.obj
	del mkarray.obj
	del tnymain.obj
	del tnywmain.obj
	del exsel.h
	del exselw.h
	del exsel1.h
