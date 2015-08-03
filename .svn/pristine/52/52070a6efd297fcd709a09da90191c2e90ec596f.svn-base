$MOD167              				; Define C167 mode

; Setup model-dependent Assembler controls
; July 2003 added WD support for CANtrak 2600

$CASE
$IF NOT TINY
$SEGMENTED
$ENDIF

NAME ?APP_HEAD
; CANtrak Application Header

$IF MEDIUM OR LARGE OR HLARGE
Model LIT   'FAR'
$ELSE
Model LIT   'NEAR'
$ENDIF

EXTRN main:Model

APP_HEAD_DATA SECTION CODE WORD AT 10200H
	; $10200
	APPHEAD  DB    'A','P','P','H','E','A','D',0    ;DO NOT CHANGE !!
	;(Used by Hex2BinC.exe to validate insertion of sum and size, NOT!)

	;-------------------------------------------------------------------------
	; The following four fields are written to by Hex2BinC.exe (when used)
	; $10208
	APPSIZE  DW 0000H,0000H			; Size of app in bytes (incl app_head)

	 ; $1020C
	CHKSUM2C DW 0000H   			; 2's Complement checksum

	; $1020E						; Size of checksum (1 or 2 (bytes))
	CSSIZE   DB 2
	
	; $1020F						; Hex2BinC version. (HiNibble.LowNibble)
	H2BCVER  DB 0x21
	;-------------------------------------------------------------------------
	
	; $10210						; 16 Characters (incl. Null termination)
	APPNAME  DB 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	
	; $10220						; 8 char software version
	APPVER   DB 0,0,0,0,0,0,0,0
	
	; $10228						; single byte Q: :flash data format
	FLFORMAT DB	00
	
	; $10229						; single byte eeprom data format
	EEFORMAT DB	00
	
	; $1022A						; Watchdog enable/disable , non zero=enable
	
	WDENABLE DB	0xff
	
	RESERVED DS	21					; bytes reserved
									; (Edit to (64 - Total of other data)  )
APP_HEAD_DATA ENDS

;0FCE0h-0FCFFh are used by PEC transfers.
;PEC_MEM SECTION XDATA AT 0FCE0H
;	DS 0x20
;PEC_MEM ENDS

END

