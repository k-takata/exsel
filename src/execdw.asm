;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Execdw.com	Ver.1.11					;
;	Copyright (C) 1998  K.Takata					;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.186


;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 定数 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LF	equ	0ah
CR	equ	0dh

STKSIZ	equ	80h


;;;;;;;;;;;;;;;;;;;;;;;;;;;;; マクロ ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;真のDOSのバージョンを得る(dosfunc 3306h)
;Ret	bl:major version
;	bh:minor version
;	dl:revision
;	dh:version flags
GetTrueDosVer	macro
	mov	ax,3306h
	int	21h
endm

;メモリの割り当て(dosfunc 48h)
;Call	size:メモリサイズ(para.)
;Ret	ax:割り当てられたメモリのセグメントアドレス
;	cf=1:エラー(ax:エラーコード)
AllocMem macro	size
 ifdif <size>,<bx>
	mov	bx,size
 endif
	mov	ah,48h
	int	21h
endm

;メモリの開放(dosfunc 49h)
;Call	seg:開放するセグメントアドレス
;Ret	cf=1:エラー(ax:エラーコード)
FreeMem macro	seg
 ifdif <seg>,<es>
	push	seg
	pop	es
 endif
	mov	ah,49h
	int	21h
endm

;メモリブロックサイズの変更(dosfunc 4ah)
;Call	seg:変更するセグメントアドレス,size:新しいサイズ(para.)
;Ret	cf=1:エラー(ax:エラーコード,bx:使用可能な最大のサイズ)
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

;プロセスの終了(dosfunc 4ch)
;Call	code:終了コード
Exit	macro	code
 ifnb <code>
  if .type code+ax		; code が即値のとき（怪しい判定法）
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

;子プロセスのリターンコードの取得(dosfunc 4dh)
;Ret	al:リターンコード , ah:終了状態
GetRetCode	macro
	mov	ah,4dh
	int	21h
endm

;ＰＳＰアドレスの設定(dosfunc 50h)
;Call	seg:新しいPSPのセグメント
;Ret	cf=1:エラー(ax:エラーコード)
SetPSP	macro	seg
 ifdif <seg>,<bx>
	mov	bx,seg
 endif
	mov	ah,50h
	int	21h
endm

;ＰＳＰアドレスの取得(dosfunc 51h)
;Ret	bx:PSPアドレス（注）
GetPSP	macro
	mov	ah,51h
	int	21h
endm

;ストラテジの取得(dosfunc 5800h)
;Ret	ax:ストラテジ , cf=1:エラー(ax:エラーコード)
GetStrategy	macro
	mov	ax,5800h
	int	21h
endm

;ストラテジの設定(dosfunc 5801h)
;Call	strategy:ストラテジ
;Ret	cf=1:エラー(ax:エラーコード)
SetStrategy	macro	strategy
 ifdif <strategy>,<bx>
	mov	bx,strategy
 endif
	mov	ax,5801h
	int	21h
endm

;MS-DOS Ver 7.00 以上かどうか(int 2fh, ax=4a33h)
;Ret	ax=0:MS-DOS Ver 7.00 以上, ax<>0:それ以外
;	si,dx:破壊？
IsUpperThanDosVer7	macro
	push	ds
	mov	ax,4a33h
	int	2fh
	pop	ds
endm


;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 構造体 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;Program Segment Prefix
PSP_t	struc
		db	2 dup (?)	; 00h	プログラムの終了命令(int 20h)
		dw	?		; 02h	割り当てられたメモリブロックの
					;	直後のセグメントアドレス
		db	?		; 04h	予約
		db	5 dup (?)	; 05h	DOS 機能を FAR コールするための
					;	5 バイト
PSPint22h	dw	?,?		; 0ah	プログラム終了アドレス(int 22h)
PSPint23h	dw	?,?		; 0eh	<Ctrl+C> ハンドラのアドレス
					;	(int 23h)
PSPint24h	dw	?,?		; 12h	致命的エラーハンドラのアドレス
					;	(int 24h)
PSPparseg	dw	?		; 16h	親プロセスの PSP セグメント
		db	14h dup (?)	; 18h	ファイルテーブル
PSPenvseg	dw	?		; 2ch	環境変数領域のセグメント
		dw	?		; 2eh	SP 退避領域
		dw	?		; 30h	SS 退避領域
		dw	?		; 32h	ファイルテーブルの数
PSPpfiletbl	dw	?,?		; 34h	ファイルテーブルへのポインタ
		dw	?,?		; 38h	手前の PSP へのポインタ
		db	4 dup (?)	; 3ch	予約
		dw	?		; 40h	MS-DOS バージョン番号
		db	0eh dup (?)	; 42h	予約
		db	3 dup (?)	; 50h	INT 21H:RETF
		db	9 dup (?)	; 53h	予約
PSPFCB1		db	10h dup (?)	; 5ch	デフォルト FCB#1
PSPFCB2		db	10h dup (?)	; 6ch	デフォルト FCB#2
		db	4 dup (?)	; 7ch	予約
PSPargc		db	?		; 80h	コマンドラインの文字数
PSPargv		db	7fh dup (?)	; 81h	コマンドライン
PSP_t	ends

;Memory Control Block
MCB_t	struc
MCBsign		db	?		; メモリ管理用識別子 'M' / 'Z'
MCBowner	dw	?		; Owner ID
MCBsize		dw	?		; メモリブロックのパラグラフ数
		db	3 dup (?)
MCBname		db	8 dup (?)	; Owner ファイル名
MCB_t	ends

;パラメータブロック(dosfunc 4b00h,4b01h)
PRM_t	struc
PRMenvseg	dw	?		; 環境変数領域のセグメントアドレス
PRMcmdline	dw	?,?		; コマンドラインへのポインタ
PRMFCB1		dw	?,?		; デフォルト FBC#1 へのポインタ
PRMFCB2		dw	?,?		; デフォルト FBC#2 へのポインタ
PRM_SP		dw	?		; SP の初期値 - 2 (dosfunc 4b01h)
PRM_SS		dw	?		; SS の初期値     (dosfunc 4b01h)
PRM_IP		dw	?		; IP の初期値     (dosfunc 4b01h)
PRM_CS		dw	?		; CS の初期値     (dosfunc 4b01h)
PRM_t	ends


;;;;;;;;;;;;;;;;;;;;;;;;;;;;; コード ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

cseg	segment				; セグメント cseg の開始
	assume	cs:cseg,ds:cseg,ss:cseg	; 各セグメントレジスタの宣言

public	DosID, dosprog, WinID, winprog, params
public	endexecwin, newadr, endofprog


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	org	100h
start:
	jmp	main


Version	db	"Execdw.com Ver.1.11 Copyright (C) 1998  K.Takata",CR,LF


	org	140h

DosID	db	"DOS="
dosprog	db	256 dup (0)		; DOS 環境時に起動するプログラム
WinID	db	"WIN="
winprog	db	256 dup (0)		; Win 環境時に起動するプログラム


; パラメータブロック
params	dw	0
	dw	PSPargc,0
	dw	PSPFCB1,0
	dw	PSPFCB2,0
;	dw	?,?		; 以下のプログラムのうち８バイトをデータ領域
;	dw	?,?		; として利用（プログラムの一部が破壊されるが
				; 実行された後なので問題ない）

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	main
;;
	public	main
main	proc	near
;	mov	sp,offset stktop	; スタック領域の設定
;
	GetTrueDosVer			; Ret : BX,DX
	cmp	bx,5 + 50 * 100h	; Ver == 5.50 (Windows NT)
	je	execwin
	cmp	bl,7			; Ver < 7.00
	jb	DOS

	IsUpperThanDosVer7		; MS-DOS 7.00 (Windows 95) 以上か
	test	ax,ax			; MS-DOS 7.00 以上  =>  AX = 0
	jne	DOS

	xor	cx,cx			; バッファサイズ = 0
	xor	di,di			; ダミーのバッファアドレス
	mov	dx,3
	mov	ax,168eh
	mov	bx,ax			; AX の値を BX に保存
	int	2fh			; 仮想マシンタイトルの取得
	cmp	ax,bx
	jne	execwin		; この機能をサポートしていれば Windows 95
				; （プロテクトモード）である。

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
	je	execdos			; Win 用のプログラムがセットされて
					; いなければ DOS 用を起動する。

	cmp	al,0ffh
	jne	lbl000			; 最初の１文字が 0ffh ならば
		Exit	;0		; プログラムは起動しない。
lbl000:
	mov	bx,offset endexecwin + STKSIZ + 0fh
	and	bx,0fff0h
	mov	sp,bx			; スタックの設定
	shr	bx,4
	ReallocMem	es,bx		; 不要なメモリを解放

	; プログラムの起動
	mov	bx,offset params
;	mov	[bx].PRMenvseg,0		; 初期化済み
;	mov	[bx].PRMcmdline,offset PSPargc	; 初期化済み
	mov	[bx].(PRMcmdline+2),ds
;	mov	[bx].PRMFCB1,offset PSPFCB1	; 初期化済み
	mov	[bx].(PRMFCB1+2),ds
;	mov	[bx].PRMFCB2,offset PSPFCB2	; 初期化済み
	mov	[bx].(PRMFCB2+2),ds
	mov	dx,offset winprog
;	push	ds
;	pop	es
	mov	ax,4b00h
	int	21h			; プログラムのロードと実行
	jc	err1

	GetRetCode

err1:
	Exit	al
execwin	endp


endexecwin:		; Win 用のプログラムを起動するときは、これ以降は不要


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	execdos
;;
	public	execdos
execdos	proc	near
	mov	bx,offset endofprog + STKSIZ + 0fh
	and	bx,0fff0h
	mov	sp,bx			; スタックの設定
	shr	bx,4
	mov	cx,bx			; 現プロセスの必要パラグラフ数
	ReallocMem	es,bx		; 不要なメモリを解放

	GetStrategy			; ストラテジの取得
	jnc	lbl100
		Exit	0ffh
lbl100:
	push	ax			; 現在のストラテジを保存
	mov	bx,2			; 上位メモリブロックから割り当て
	cmp	ax,bx			; oldstrategy == 2
	jne	lbl101
		xor	bx,bx	; 下位メモリブロックから割り当て (bx = 0)
lbl101:
	SetStrategy	bx		; ストラテジの設定

	AllocMem	cx		; 新プログラム領域の確保
	jc	allocmemerr

	cld
	; 新領域にプログラムをコピー
	shl	cx,3
	mov	es,ax			; 新プログラム領域のセグメント
	xor	di,di
	xor	si,si
	rep	movsw

	mov	dx,ds:[PSPenvseg]
	mov	ax,dx
	dec	ax
	mov	ds,ax
	mov	cx,ds:[MCBsize]		; 環境変数領域の必要パラグラフ数

	AllocMem	cx		; 新環境変数領域の確保
	jc	allocmemerr

	; 新領域に環境変数をコピー
	shl	cx,3
	push	es
	mov	es,ax			; 新環境変数領域のセグメント
	mov	ds,dx
	xor	di,di
	xor	si,si
	rep	movsw
	pop	si			; <=  push es

	mov	di,ax			; 新環境変数領域のセグメント

	jmp	short lbl102

allocmemerr:
;	mov	cl,1
lbl102:
	pop	bx
	SetStrategy	bx		; ストラテジを元に戻す
	jcxz	lbl102_1		; エラーがなければ CX = 0 である
		Exit	0ffh		; メモリ不足
lbl102_1:
	FreeMEM	dx			; 現環境変数領域の解放

	mov	ds,si
	mov	ds:[PSPenvseg],di	; 新環境変数領域のセグメント
	mov	ds:[PSPpfiletbl+2],si	; ファイルテーブルへのポインタ
	dec	di
	mov	ds,di			; 新環境変数領域の MCB セグメント
	mov	di,offset MCBowner
	mov	ds:[di],si		; Owner ID 変更
	mov	bx,si			; SI を BX にコピー
	dec	si
	mov	ds,si			; 新領域の MCB セグメント
	mov	ds:[di],bx		; Owner ID 変更
;	mov	ax,ss
;	dec	ax
;	mov	ds,ax			; 現領域の MCB セグメント
;	mov	ds:[di],bx		; Owner ID 変更

	SetPSP	bx			; 新 PSP アドレスのセット

	push	ss
	pop	es		; 現領域のセグメントアドレスを ES に保存
	mov	ds,bx			; 新 ds
	mov	ss,bx			; 新 ss
	push	bx			; 新 cs
	mov	bx,offset newadr	; 新 ip
	push	bx
db	0cbh		;retf		; 新領域にジャンプ

newadr:
	FreeMem	es			; 旧メモリブロックの解放
	jnc	lbl103
		Exit	0ffh
lbl103:
	; プログラムのロード
	mov	bx,offset params
;	mov	[bx].PRMenvseg,0		; 初期化済み
;	mov	[bx].PRMcmdline,offset PSPargc	; 初期化済み
	mov	[bx].(PRMcmdline+2),ds
;	mov	[bx].PRMFCB1,offset PSPFCB1	; 初期化済み
	mov	[bx].(PRMFCB1+2),ds
;	mov	[bx].PRMFCB2,offset PSPFCB2	; 初期化済み
	mov	[bx].(PRMFCB2+2),ds
	mov	dx,offset dosprog
	push	ds
	pop	es
	mov	ax,4b01h
	int	21h			; プログラムのロードと非実行
	jnc lbl104
		Exit	al
lbl104:
	mov	es,ds:[PSPenvseg]
	FreeMem	es			; 現環境変数領域の解放

	GetPSP			; 起動プログラムの PSP セグメント => BX

	; 親プロセスの PSP セグメントアドレスをコピーし、
	; 割り込みベクタ 22h, 23h, 24h を元に戻す
	mov	si,PSPint22h
	push	si
	mov	es,bx			; 起動プログラムの PSP セグメント
	mov	di,si
	mov	cx,7			; PSPint22h .. PSPparseg
	rep	movsw

	mov	cl,3			; mov cx,3 と同じ(CX = 0 である)
	pop	bp			; PSPint22h
	mov	ax,2500h+22h
	push	ds
lbl105:
	lds	dx,dword ptr [bp]
	int	21h			; 割り込みベクタの設定
	add	bp,4
	inc	ax
	loop	lbl105
	pop	ds

	mov	si,offset params
	mov	ss,[si].PRM_SS		; SS の初期値
	mov	sp,[si].PRM_SP		; SP の初期値 - 2
	pop	ax			; 起動時の AX レジスタの初期値
	pushf
	push	[si].PRM_CS		; CS の初期値
	push	[si].PRM_IP		; IP の初期値
	mov	dx,ds			; 現領域の PSP セグメント
	dec	dx
	mov	ds,dx			; 現領域の MCB セグメント
	cli				; 割り込み禁止
	mov	ds:[MCBowner],0		; 現領域の解放
	push	es
	pop	ds			; 起動プログラムの PSP セグメント
	iret				; 子プロセスにジャンプ（割り込み許可）
execdos	endp


endofprog:				; プログラムの最後

cseg	ends
	end
