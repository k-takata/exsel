#
#	This is a makefile of  "Executabe file Selector."
#
#	Copyright (C)  1998-2011  K.Takata
#

CC = cl /nologo
CC16 = lcc
CFLAGS = /O1 /W4 /GF /FAsc
LDFLAGS = /merge:.rdata=.text /map

# Get the version of cl.exe.
#  1. Write the version to a work file (mscver$(_NMAKE_VER).~).
!if ![(echo _MSC_VER>mscver$(_NMAKE_VER).c) && \
	(for /f %I in ('"$(CC) /EP mscver$(_NMAKE_VER).c 2>nul"') do @echo _MSC_VER=%I> mscver$(_NMAKE_VER).~)]
#  2. Include it.
!include mscver$(_NMAKE_VER).~
#  3. Clean up.
!if [del mscver$(_NMAKE_VER).~ mscver$(_NMAKE_VER).c]
!endif
!endif


!if $(_MSC_VER) < 1500
LDFLAGS = $(LDFLAGS) /opt:nowin98
!else
LDFLAGS = $(LDFLAGS) /dynamicbase:no
!endif

!if $(_MSC_VER) >= 1400
CFLAGS = $(CFLAGS) /GS-
!endif

all : exsel.exe

#exsel.exe : exsel.c exselc.h exselg.h exselcu.h exselgu.h exsel1.h
#	$(CC16) -o $@ exsel.c -lintlib -lexpand.obj
exsel.exe : exsel.c exselc.h exselg.h exselcu.h exselgu.h exsel1.h
	$(CC) /MD /Fe$@ /O1 exsel.c /link setargv.obj $(LDFLAGS)

exselc.h : execdwc.exe mkarray.exe
	mkarray execdwc.exe execdwc > $@
exselg.h : execdwg.exe mkarray.exe
	mkarray execdwg.exe execdwg > $@
exselcu.h : execdwcu.exe mkarray.exe
	mkarray execdwcu.exe execdwcu > $@
exselgu.h : execdwgu.exe mkarray.exe
	mkarray execdwgu.exe execdwgu > $@
exsel1.h : execdw1.com mkarray.exe
	mkarray execdw1.com execdw1 > $@


# GUI version

# ANSI
execdwg.exe : execdwg.obj tnywmain.obj execdos.exe
	$(CC) /Fo$@ execdwg.obj tnywmain.obj kernel32.lib user32.lib /link $(LDFLAGS) /stub:execdos.exe

execdwg.obj : execdw.c
	$(CC) /c $(CFLAGS) /Fa$*.cod /DWINMAIN /Fo$@ execdw.c

# Unicode with INI support
execdwgu.exe : execdwgu.obj tnywmain.obj execdos.exe
	$(CC) /Fo$@ execdwgu.obj tnywmain.obj kernel32.lib user32.lib /link $(LDFLAGS) /stub:execdos.exe

execdwgu.obj : execdw.c
	$(CC) /c $(CFLAGS) /Fa$*.cod /DWINMAIN /DUNICODE /D_UNICODE /DINIFILE /Fo$@ execdw.c

tnywmain.obj : tnywmain.c
	$(CC) /c $(CFLAGS) /Fa$*.cod tnywmain.c


# CUI version

# ANSI
execdwc.exe : execdwc.obj tnymain.obj execdos.exe
	$(CC) /Fo$@ execdwc.obj tnymain.obj kernel32.lib user32.lib /link $(LDFLAGS) /stub:execdos.exe

execdwc.obj : execdw.c
	$(CC) /c $(CFLAGS) /Fa$*.cod /Fo$@ execdw.c

# Unicode with INI support
execdwcu.exe : execdwcu.obj tnymain.obj execdos.exe
	$(CC) /Fo$@ execdwcu.obj tnymain.obj kernel32.lib user32.lib /link $(LDFLAGS) /stub:execdos.exe

execdwcu.obj : execdw.c
	$(CC) /c $(CFLAGS) /Fa$*.cod /DUNICODE /D_UNICODE /DINIFILE /Fo$@ execdw.c

tnymain.obj : tnymain.c
	$(CC) /c $(CFLAGS) /Fa$*.cod tnymain.c



execdos.exe : execdos.obj
	$(CC16) -a -o $@ execdos.obj
	stbhdr -f $@
#execdos.obj : execdos.asm
#	$(CC16) -o $@ -c execdos.asm
execdos.obj : execdos.asm
	ml /Zm /c execdos.asm


execdw1.com : execdw1.obj
	$(CC16) -a -o $@ $?
execdw1.obj : execdw.asm
	ml /Zm /c /Fo$@ execdw.asm



mkarray.exe : mkarray.c
	$(CC) /O1 /MD $? /link /opt:nowin98 /merge:.rdata=.text


clean :
	del execdos.exe
	del execdos.old
	del execdwc.exe
	del execdwg.exe
	del execdwcu.exe
	del execdwgu.exe
	del execdw1.com
	del exsel.exe
	del mkarray.exe
	del execdos.obj
	del execdwc.obj
	del execdwc.cod
	del execdwg.obj
	del execdwg.cod
	del execdwcu.obj
	del execdwcu.cod
	del execdwgu.obj
	del execdwgu.cod
	del execdw1.obj
	del exsel.obj
	del mkarray.obj
	del tnymain.obj
	del tnywmain.obj
	del exselc.h
	del exselg.h
	del exselcu.h
	del exselgu.h
	del exsel1.h
