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

#exsel.exe : exsel.c exsel.h exselw.h exsel1.h
#	$(CC16) -o $@ exsel.c -lintlib -lexpand.obj
exsel.exe : exsel.c exsel.h exselw.h exsel1.h
	$(CC) /MD /Fe$@ /O1 exsel.c /link setargv.obj $(LDFLAGS)

exsel.h : execdw.exe mkarray.exe
	mkarray execdw.exe execdw > $@
exselw.h : execdww.exe mkarray.exe
	mkarray execdww.exe execdww > $@
exsel1.h : execdw1.com mkarray.exe
	mkarray execdw1.com execdw1 > $@


# GUI version

# ANSI
execdww.exe : execdww.obj tnywmain.obj execdos.exe
	$(CC) /Fo$@ execdww.obj tnywmain.obj kernel32.lib user32.lib /link $(LDFLAGS) /stub:execdos.exe

execdww.obj : execdw.c
	$(CC) /c /O1 /W4 /GF /FAsc /DWINMAIN /Fo$@ execdw.c

# Unicode with INI support
execdwwu.exe : execdwwu.obj tnywmain.obj execdos.exe
	$(CC) /Fo$@ execdwwu.obj tnywmain.obj kernel32.lib user32.lib /link $(LDFLAGS) /stub:execdos.exe

execdwwu.obj : execdw.c
	$(CC) /c /O1 /W4 /GF /FAsc /DWINMAIN /DUNICODE /D_UNICODE /DINIFILE /Fo$@ execdw.c

tnywmain.obj : tnywmain.c
	$(CC) /c /O1 /W4 /GF /FAsc tnywmain.c


# CUI version

# ANSI
execdw.exe : execdw.obj tnymain.obj execdos.exe
	$(CC) /Fo$@ execdw.obj tnymain.obj kernel32.lib user32.lib /link $(LDFLAGS) /stub:execdos.exe

execdw.obj : execdw.c
	$(CC) /c /O1 /W4 /GF /FAsc execdw.c

# Unicode with INI support
execdwu.exe : execdwu.obj tnymain.obj execdos.exe
	$(CC) /Fo$@ execdwu.obj tnymain.obj kernel32.lib user32.lib /link $(LDFLAGS) /stub:execdos.exe

execdwu.obj : execdw.c
	$(CC) /c /O1 /W4 /GF /FAsc /DUNICODE /D_UNICODE /DINIFILE /Fo$@ execdw.c

tnymain.obj : tnymain.c
	$(CC) /c /O1 /W4 /GF /FAsc tnymain.c



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
	del execdw.exe
	del execdww.exe
	del execdwu.exe
	del execdwwu.exe
	del execdw1.com
	del exsel.exe
	del mkarray.exe
	del execdos.obj
	del execdw.obj
	del execdw.cod
	del execdww.obj
	del execdww.cod
	del execdwu.obj
	del execdwu.cod
	del execdwwu.obj
	del execdwwu.cod
	del execdw1.obj
	del exsel.obj
	del mkarray.obj
	del tnymain.obj
	del tnywmain.obj
	del exsel.h
	del exselw.h
	del exsel1.h
