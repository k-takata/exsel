#
#	This is a makefile of  "Executabe file Selector."
#
#	Copyright (C)  1998  K.Takata
#


all : exsel.exe

exsel.exe : exsel.c exsel.h
	lcc -o $@ $< -lintlib -ltinymain.obj

exsel.h : execdw.com mkarray.exe
	mkarray $< execdw > $@

execdw.com : execdw.obj
	lld -o $@ $?

execdw.obj : execdw.asm
	lcc -o $@ -c $?

mkarray.exe : mkarray.c
	lcc -o $@ $? -lintlib -ltinymain.obj
