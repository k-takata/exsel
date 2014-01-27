;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Execdw.com	Ver.1.11					;
;	Copyright (C) 1998  K.Takata					;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.186


;;;;;;;;;;;;;;;;;;;;;;;;;;;;; �萔 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LF	equ	0ah
CR	equ	0dh

STKSIZ	equ	80h


;;;;;;;;;;;;;;;;;;;;;;;;;;;;; �}�N�� ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;�^��DOS�̃o�[�W�����𓾂�(dosfunc 3306h)
;Ret	bl:major version
;	bh:minor version
;	dl:revision
;	dh:version flags
GetTrueDosVer	macro
	mov	ax,3306h
	int	21h
endm

;�������̊��蓖��(dosfunc 48h)
;Call	size:�������T�C�Y(para.)
;Ret	ax:���蓖�Ă�ꂽ�������̃Z�O�����g�A�h���X
;	cf=1:�G���[(ax:�G���[�R�[�h)
AllocMem macro	size
 ifdif <size>,<bx>
	mov	bx,size
 endif
	mov	ah,48h
	int	21h
endm

;�������̊J��(dosfunc 49h)
;Call	seg:�J������Z�O�����g�A�h���X
;Ret	cf=1:�G���[(ax:�G���[�R�[�h)
FreeMem macro	seg
 ifdif <seg>,<es>
	push	seg
	pop	es
 endif
	mov	ah,49h
	int	21h
endm

;�������u���b�N�T�C�Y�̕ύX(dosfunc 4ah)
;Call	seg:�ύX����Z�O�����g�A�h���X,size:�V�����T�C�Y(para.)
;Ret	cf=1:�G���[(ax:�G���[�R�[�h,bx:�g�p�\�ȍő�̃T�C�Y)
ReallocMem	macro	seg,size
 ifdif <seg>,<es>
	push	seg
 endif
 ifdif <size>,<bx>
	mov	bx,size
 endif
 ifdif <seg>,<es>
	pop	es
 endif
	mov	ah,4ah
	int	21h
endm

;�v���Z�X�̏I��(dosfunc 4ch)
;Call	code:�I���R�[�h
Exit	macro	code
 ifnb <code>
  if .type code+ax		; code �����l�̂Ƃ��i����������@�j
	mov	ax,4c00h+code
  else
   ifdif <code>,<al>
	mov	al,code
   endif
	mov	ah,4ch
  endif
 else
	mov	ax,4c00h
 endif
	int	21h
endm

;�q�v���Z�X�̃��^�[���R�[�h�̎擾(dosfunc 4dh)
;Ret	al:���^�[���R�[�h , ah:�I�����
GetRetCode	macro
	mov	ah,4dh
	int	21h
endm

;�o�r�o�A�h���X�̐ݒ�(dosfunc 50h)
;Call	seg:�V����PSP�̃Z�O�����g
;Ret	cf=1:�G���[(ax:�G���[�R�[�h)
SetPSP	macro	seg
 ifdif <seg>,<bx>
	mov	bx,seg
 endif
	mov	ah,50h
	int	21h
endm

;�o�r�o�A�h���X�̎擾(dosfunc 51h)
;Ret	bx:PSP�A�h���X�i���j
GetPSP	macro
	mov	ah,51h
	int	21h
endm

;�X�g���e�W�̎擾(dosfunc 5800h)
;Ret	ax:�X�g���e�W , cf=1:�G���[(ax:�G���[�R�[�h)
GetStrategy	macro
	mov	ax,5800h
	int	21h
endm

;�X�g���e�W�̐ݒ�(dosfunc 5801h)
;Call	strategy:�X�g���e�W
;Ret	cf=1:�G���[(ax:�G���[�R�[�h)
SetStrategy	macro	strategy
 ifdif <strategy>,<bx>
	mov	bx,strategy
 endif
	mov	ax,5801h
	int	21h
endm

;MS-DOS Ver 7.00 �ȏォ�ǂ���(int 2fh, ax=4a33h)
;Ret	ax=0:MS-DOS Ver 7.00 �ȏ�, ax<>0:����ȊO
;	si,dx:�j��H
IsUpperThanDosVer7	macro
	push	ds
	mov	ax,4a33h
	int	2fh
	pop	ds
endm


;;;;;;;;;;;;;;;;;;;;;;;;;;;;; �\���� ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;Program Segment Prefix
PSP_t	struc
		db	2 dup (?)	; 00h	�v���O�����̏I������(int 20h)
		dw	?		; 02h	���蓖�Ă�ꂽ�������u���b�N��
					;	����̃Z�O�����g�A�h���X
		db	?		; 04h	�\��
		db	5 dup (?)	; 05h	DOS �@�\�� FAR �R�[�����邽�߂�
					;	5 �o�C�g
PSPint22h	dw	?,?		; 0ah	�v���O�����I���A�h���X(int 22h)
PSPint23h	dw	?,?		; 0eh	<Ctrl+C> �n���h���̃A�h���X
					;	(int 23h)
PSPint24h	dw	?,?		; 12h	�v���I�G���[�n���h���̃A�h���X
					;	(int 24h)
PSPparseg	dw	?		; 16h	�e�v���Z�X�� PSP �Z�O�����g
		db	14h dup (?)	; 18h	�t�@�C���e�[�u��
PSPenvseg	dw	?		; 2ch	���ϐ��̈�̃Z�O�����g
		dw	?		; 2eh	SP �ޔ�̈�
		dw	?		; 30h	SS �ޔ�̈�
		dw	?		; 32h	�t�@�C���e�[�u���̐�
PSPpfiletbl	dw	?,?		; 34h	�t�@�C���e�[�u���ւ̃|�C���^
		dw	?,?		; 38h	��O�� PSP �ւ̃|�C���^
		db	4 dup (?)	; 3ch	�\��
		dw	?		; 40h	MS-DOS �o�[�W�����ԍ�
		db	0eh dup (?)	; 42h	�\��
		db	3 dup (?)	; 50h	INT 21H:RETF
		db	9 dup (?)	; 53h	�\��
PSPFCB1		db	10h dup (?)	; 5ch	�f�t�H���g FCB#1
PSPFCB2		db	10h dup (?)	; 6ch	�f�t�H���g FCB#2
		db	4 dup (?)	; 7ch	�\��
PSPargc		db	?		; 80h	�R�}���h���C���̕�����
PSPargv		db	7fh dup (?)	; 81h	�R�}���h���C��
PSP_t	ends

;Memory Control Block
MCB_t	struc
MCBsign		db	?		; �������Ǘ��p���ʎq 'M' / 'Z'
MCBowner	dw	?		; Owner ID
MCBsize		dw	?		; �������u���b�N�̃p���O���t��
		db	3 dup (?)
MCBname		db	8 dup (?)	; Owner �t�@�C����
MCB_t	ends

;�p�����[�^�u���b�N(dosfunc 4b00h,4b01h)
PRM_t	struc
PRMenvseg	dw	?		; ���ϐ��̈�̃Z�O�����g�A�h���X
PRMcmdline	dw	?,?		; �R�}���h���C���ւ̃|�C���^
PRMFCB1		dw	?,?		; �f�t�H���g FBC#1 �ւ̃|�C���^
PRMFCB2		dw	?,?		; �f�t�H���g FBC#2 �ւ̃|�C���^
PRM_SP		dw	?		; SP �̏����l - 2 (dosfunc 4b01h)
PRM_SS		dw	?		; SS �̏����l     (dosfunc 4b01h)
PRM_IP		dw	?		; IP �̏����l     (dosfunc 4b01h)
PRM_CS		dw	?		; CS �̏����l     (dosfunc 4b01h)
PRM_t	ends


;;;;;;;;;;;;;;;;;;;;;;;;;;;;; �R�[�h ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

cseg	segment				; �Z�O�����g cseg �̊J�n
	assume	cs:cseg,ds:cseg,ss:cseg	; �e�Z�O�����g���W�X�^�̐錾

public	DosID, dosprog, WinID, winprog, params
public	endexecwin, newadr, endofprog


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	org	100h
start:
	jmp	main


Version	db	"Execdw.com Ver.1.11 Copyright (C) 1998  K.Takata",CR,LF


	org	140h

DosID	db	"DOS="
dosprog	db	256 dup (0)		; DOS �����ɋN������v���O����
WinID	db	"WIN="
winprog	db	256 dup (0)		; Win �����ɋN������v���O����


; �p�����[�^�u���b�N
params	dw	0
	dw	PSPargc,0
	dw	PSPFCB1,0
	dw	PSPFCB2,0
;	dw	?,?		; �ȉ��̃v���O�����̂����W�o�C�g���f�[�^�̈�
;	dw	?,?		; �Ƃ��ė��p�i�v���O�����̈ꕔ���j�󂳂�邪
				; ���s���ꂽ��Ȃ̂Ŗ��Ȃ��j

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	main
;;
	public	main
main	proc	near
;	mov	sp,offset stktop	; �X�^�b�N�̈�̐ݒ�
;
	GetTrueDosVer			; Ret : BX,DX
	cmp	bx,5 + 50 * 100h	; Ver == 5.50 (Windows NT)
	je	execwin
	cmp	bl,7			; Ver < 7.00
	jb	DOS

	IsUpperThanDosVer7		; MS-DOS 7.00 (Windows 95) �ȏォ
	test	ax,ax			; MS-DOS 7.00 �ȏ�  =>  AX = 0
	jne	DOS

	xor	cx,cx			; �o�b�t�@�T�C�Y = 0
	xor	di,di			; �_�~�[�̃o�b�t�@�A�h���X
	mov	dx,3
	mov	ax,168eh
	mov	bx,ax			; AX �̒l�� BX �ɕۑ�
	int	2fh			; ���z�}�V���^�C�g���̎擾
	cmp	ax,bx
	jne	execwin		; ���̋@�\���T�|�[�g���Ă���� Windows 95
				; �i�v���e�N�g���[�h�j�ł���B

DOS:
	jmp	short execdos

main	endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	execwin
;;
	public	execwin
execwin	proc	near
	mov	al,byte ptr [winprog]
	cmp	al,0
	je	execdos			; Win �p�̃v���O�������Z�b�g�����
					; ���Ȃ���� DOS �p���N������B

	cmp	al,0ffh
	jne	lbl000			; �ŏ��̂P������ 0ffh �Ȃ��
		Exit	;0		; �v���O�����͋N�����Ȃ��B
lbl000:
	mov	bx,offset endexecwin + STKSIZ + 0fh
	and	bx,0fff0h
	mov	sp,bx			; �X�^�b�N�̐ݒ�
	shr	bx,4
	ReallocMem	es,bx		; �s�v�ȃ����������

	; �v���O�����̋N��
	mov	bx,offset params
;	mov	[bx].PRMenvseg,0		; �������ς�
;	mov	[bx].PRMcmdline,offset PSPargc	; �������ς�
	mov	[bx].(PRMcmdline+2),ds
;	mov	[bx].PRMFCB1,offset PSPFCB1	; �������ς�
	mov	[bx].(PRMFCB1+2),ds
;	mov	[bx].PRMFCB2,offset PSPFCB2	; �������ς�
	mov	[bx].(PRMFCB2+2),ds
	mov	dx,offset winprog
;	push	ds
;	pop	es
	mov	ax,4b00h
	int	21h			; �v���O�����̃��[�h�Ǝ��s
	jc	err1

	GetRetCode

err1:
	Exit	al
execwin	endp


endexecwin:		; Win �p�̃v���O�������N������Ƃ��́A����ȍ~�͕s�v


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	execdos
;;
	public	execdos
execdos	proc	near
	mov	bx,offset endofprog + STKSIZ + 0fh
	and	bx,0fff0h
	mov	sp,bx			; �X�^�b�N�̐ݒ�
	shr	bx,4
	mov	cx,bx			; ���v���Z�X�̕K�v�p���O���t��
	ReallocMem	es,bx		; �s�v�ȃ����������

	GetStrategy			; �X�g���e�W�̎擾
	jnc	lbl100
		Exit	0ffh
lbl100:
	push	ax			; ���݂̃X�g���e�W��ۑ�
	mov	bx,2			; ��ʃ������u���b�N���犄�蓖��
	cmp	ax,bx			; oldstrategy == 2
	jne	lbl101
		xor	bx,bx	; ���ʃ������u���b�N���犄�蓖�� (bx = 0)
lbl101:
	SetStrategy	bx		; �X�g���e�W�̐ݒ�

	AllocMem	cx		; �V�v���O�����̈�̊m��
	jc	allocmemerr

	cld
	; �V�̈�Ƀv���O�������R�s�[
	shl	cx,3
	mov	es,ax			; �V�v���O�����̈�̃Z�O�����g
	xor	di,di
	xor	si,si
	rep	movsw

	mov	dx,ds:[PSPenvseg]
	mov	ax,dx
	dec	ax
	mov	ds,ax
	mov	cx,ds:[MCBsize]		; ���ϐ��̈�̕K�v�p���O���t��

	AllocMem	cx		; �V���ϐ��̈�̊m��
	jc	allocmemerr

	; �V�̈�Ɋ��ϐ����R�s�[
	shl	cx,3
	push	es
	mov	es,ax			; �V���ϐ��̈�̃Z�O�����g
	mov	ds,dx
	xor	di,di
	xor	si,si
	rep	movsw
	pop	si			; <=  push es

	mov	di,ax			; �V���ϐ��̈�̃Z�O�����g

	jmp	short lbl102

allocmemerr:
;	mov	cl,1
lbl102:
	pop	bx
	SetStrategy	bx		; �X�g���e�W�����ɖ߂�
	jcxz	lbl102_1		; �G���[���Ȃ���� CX = 0 �ł���
		Exit	0ffh		; �������s��
lbl102_1:
	FreeMEM	dx			; �����ϐ��̈�̉��

	mov	ds,si
	mov	ds:[PSPenvseg],di	; �V���ϐ��̈�̃Z�O�����g
	mov	ds:[PSPpfiletbl+2],si	; �t�@�C���e�[�u���ւ̃|�C���^
	dec	di
	mov	ds,di			; �V���ϐ��̈�� MCB �Z�O�����g
	mov	di,offset MCBowner
	mov	ds:[di],si		; Owner ID �ύX
	mov	bx,si			; SI �� BX �ɃR�s�[
	dec	si
	mov	ds,si			; �V�̈�� MCB �Z�O�����g
	mov	ds:[di],bx		; Owner ID �ύX
;	mov	ax,ss
;	dec	ax
;	mov	ds,ax			; ���̈�� MCB �Z�O�����g
;	mov	ds:[di],bx		; Owner ID �ύX

	SetPSP	bx			; �V PSP �A�h���X�̃Z�b�g

	push	ss
	pop	es		; ���̈�̃Z�O�����g�A�h���X�� ES �ɕۑ�
	mov	ds,bx			; �V ds
	mov	ss,bx			; �V ss
	push	bx			; �V cs
	mov	bx,offset newadr	; �V ip
	push	bx
db	0cbh		;retf		; �V�̈�ɃW�����v

newadr:
	FreeMem	es			; ���������u���b�N�̉��
	jnc	lbl103
		Exit	0ffh
lbl103:
	; �v���O�����̃��[�h
	mov	bx,offset params
;	mov	[bx].PRMenvseg,0		; �������ς�
;	mov	[bx].PRMcmdline,offset PSPargc	; �������ς�
	mov	[bx].(PRMcmdline+2),ds
;	mov	[bx].PRMFCB1,offset PSPFCB1	; �������ς�
	mov	[bx].(PRMFCB1+2),ds
;	mov	[bx].PRMFCB2,offset PSPFCB2	; �������ς�
	mov	[bx].(PRMFCB2+2),ds
	mov	dx,offset dosprog
	push	ds
	pop	es
	mov	ax,4b01h
	int	21h			; �v���O�����̃��[�h�Ɣ���s
	jnc lbl104
		Exit	al
lbl104:
	mov	es,ds:[PSPenvseg]
	FreeMem	es			; �����ϐ��̈�̉��

	GetPSP			; �N���v���O������ PSP �Z�O�����g => BX

	; �e�v���Z�X�� PSP �Z�O�����g�A�h���X���R�s�[���A
	; ���荞�݃x�N�^ 22h, 23h, 24h �����ɖ߂�
	mov	si,PSPint22h
	push	si
	mov	es,bx			; �N���v���O������ PSP �Z�O�����g
	mov	di,si
	mov	cx,7			; PSPint22h .. PSPparseg
	rep	movsw

	mov	cl,3			; mov cx,3 �Ɠ���(CX = 0 �ł���)
	pop	bp			; PSPint22h
	mov	ax,2500h+22h
	push	ds
lbl105:
	lds	dx,dword ptr [bp]
	int	21h			; ���荞�݃x�N�^�̐ݒ�
	add	bp,4
	inc	ax
	loop	lbl105
	pop	ds

	mov	si,offset params
	mov	ss,[si].PRM_SS		; SS �̏����l
	mov	sp,[si].PRM_SP		; SP �̏����l - 2
	pop	ax			; �N������ AX ���W�X�^�̏����l
	pushf
	push	[si].PRM_CS		; CS �̏����l
	push	[si].PRM_IP		; IP �̏����l
	mov	dx,ds			; ���̈�� PSP �Z�O�����g
	dec	dx
	mov	ds,dx			; ���̈�� MCB �Z�O�����g
	cli				; ���荞�݋֎~
	mov	ds:[MCBowner],0		; ���̈�̉��
	push	es
	pop	ds			; �N���v���O������ PSP �Z�O�����g
	iret				; �q�v���Z�X�ɃW�����v�i���荞�݋��j
execdos	endp


endofprog:				; �v���O�����̍Ō�

cseg	ends
	end
