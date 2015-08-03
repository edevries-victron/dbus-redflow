$MOD167              ; Define C167 mode
;
;------------------------------------------------------------------------------
;  This file is part of the C166 Compiler package
;  Copyright KEIL ELEKTRONIK GmbH 1993-1999
;  Version 4.00
;------------------------------------------------------------------------------
;  START167.A66:  This code is executed after processor reset and provides the
;                 startup sequence for the extended 166 architecture CPU's.
;                 (i.e. C167/C165/C164/C163/C161, ST10-262 ect.)
;
;  To translate this file use A166 with the following invocation:
;
;     A166 START167.A66 SET (<model>)
;
;     <model> determines the memory model and can be one of the following:
;         TINY, SMALL, COMPACT, HCOMPACT, MEDIUM, LARGE or HLARGE
;
;  Example:  A166 START167.A66 SET (SMALL)
;
;  To link the modified START167.OBJ file to your application use the following
;  L166 invocation:
;
;     L166 <your object file list>, START167.OBJ <controls>
;
;------------------------------------------------------------------------------
; Setup model-dependend Assembler controls
$CASE
$IF NOT TINY
$SEGMENTED
$ENDIF
;------------------------------------------------------------------------------
;
; Definitions for BUSCON0 and SYSCON Register:
; --------------------------------------------
;
; MCTC0: Memory Cycle Time (BUSCON0.0 .. BUSCON0.3):
; Note: if RDYEN0 == 1 a maximum number of 7 waitstates can be selected
_MCTC0    EQU  0  ; Memory wait states is 1 (MCTC0 field = 0EH).
;        ; (Reset Value = 15 additional state times)
;
; RWDC0: Read/Write Signal Delay (BUSCON0.4):
_RWDC0    EQU    1; 0 = Delay Time     0.5 States (Reset Value)
;        ; 1 = No Delay Time  0   States
;
; MTTC0: Memory Tri-state Time (BUSCON0.5):
_MTTC0    EQU  1  ; 0 = Delay Time     0.5 States (Reset Value)
;        ; 1 = No Delay Time  0   States
;
$SET (BTYP_ENABLE = 1)  ; 0 = BTYP0 and BUSACT0 is set according to the level
                        ;     at pins P0L.6 and P0L.7 during reset.
         ; 1 = the following _BTYP0 and _BUSACT0 values are
         ;     written to BTYP0 and BUSACT0
;
; BTYP0: External Bus Configuration Control (BUSCON0.6 .. BUSCON0.7):
_BTYP0    EQU  1  ; 0 = 8 Bit Non Multiplexed
;        ; 1 = 8 Bit Multiplexed
;        ; 2 = 16 Bit Non Multiplexed
;        ; 3 = 16 Bit Multiplexed
;
; ALECTL0: ALE Lengthening Control Bit (BUSCON0.9):
_ALECTL0 EQU   0  ; see data sheet for description
;
; BUSACT0: Bus Active Control Bit (BUSCON0.10):
_BUSACT0 EQU   1  ; = 0 external bus disabled
         ; = 1 external bus enabled
;
; RDYEN0: READY# Input Enable control bit (BUSCON0.12):
_RDYEN0  EQU   0  ; 0 = READY# function disabled  (Reset Value)
;        ; 1 = READY# function enabled
;
; RDY_AS0: Synchronous / Asynchronous READY# Input (BUSCON0.3):
; Note: This bit is only valid if _RDYEN0 == 1.
_RDY_AS0 EQU   0  ; 0 = synchronous READY# input
;        ; 1 = asynchronous READY# input
;
; CSREN0: Read Chip Select Enable bit (BUSCON0.14, only in some devices):
_CSREN0  EQU   0  ; 0 = CS0# is independent of read command (RD#)
;        ; 1 = CS0# is generated for the duration of read
;
; CSWEN0: Write Chip Select Enable bit (BUSCON0.15, only in some devices):
_CSWEN0  EQU   0  ; 0 = CS0# is independent of write command (WR#)
;        ; 1 = CS0# is generated for the duration of write
;
; XPERSHARE: XBUS Peripheral Share Mode Control (SYSCON.0)
_XPERSHARE EQU 0        ; 0 = External accesses to XBUS peripherals disabled
;                       ; 1 = XBUS accessible via external BUS in hold mode
;
; VISIBLE: Visible Mode Control (SYSCON.1)
_VISIBLE EQU 0          ; 0 = Accesses to XBUS are done internally
;                       ; 1 = XBUS accesses are made visible on external pins
;
; XPEN: XRAM & XBUS Peripheral Enable Control Bit (SYSCON.2)
_XPEN EQU   1  ; 0 = access to on-chip XRAM & XBUS disable => EXT.BUS
;                       ; 1 = on-chip XRAM & XBUS is accessed
;
; BDRSTEN: Bidirectional Reset Enable Bit (SYSCON.3, only in some devices)
_BDRSTEN EQU    1       ; 0 = Pin RSTIN# is an input only
;                       ; 1 = RSTIN# is pulled low during internal reset
;
$SET (OWDDIS_ENABLE = 0); 0 = OWDDIS is set according to the inverted level
;                       ;     at pin RD\ duirng reset.
;                       ; 1 = the following _OWDDIS value is
;                       ;     written to OWDDIS in the SYSCON register
; OWDDIS: Oscillator Watchdog Disable Bit (SYSCON.4, only in some devices)
_OWDDIS EQU     0       ; 0 = the on-chip oscillator watchdog is enabled
;                       ; 1 = the on-chip oscillator watchdog is disabled
;
; PWDCFG: Power Down Mode Configuration Bit (SYSCON.5, only in some devices)
_PWDCFG EQU     0       ; 0 = Power Down mode can be left via reset
;                       ; 1 = Power Down mode left via ext. or RTC interrupt
;
; CSCFG: Chip Select Configuration Control (SYSCON.6, only in some devices)
_CSCFG  EQU     0       ; 0 = Latched CS mode; CS signals are latch internally
;                       ; 1 = Unlatched CS mode
;
$SET (WRCFG_ENABLE = 0) ; 0 = WRCFG is set according to the level at
;                       ;     pin P0H.0 during reset.
;                       ; 1 = the following _WRCFG value is
;                       ;     written to WRCFG in the SYSCON register
; WRCFG: Write Configuration Control Bit (SYSCON.7):
_WRCFG    EQU  0  ; 0 = Normal configuration of WR# and BHE#
;        ; 1 = WR# pin acts as WRL#, BHE# pin acts as WRH#
;
; CLKEN: System Clock Output Enable bit (SYSCON.8):
_CLKEN   EQU   0  ; 0 = disabled    (Reset Value)
;        ; 1 = enabled
;
; BYTDIS: Byte High Enable pin control bit (SYSCON.9):
_BYTDIS  EQU   0  ; 0 = enabled     (Reset Value)
;        ; 1 = disabled
;
; ROMEN: Internal ROM Access Enable is read only (SYSCON.10):
_ROMEN   EQU   0  ; 0 = Internal ROM disabled
         ; 1 = Internal ROM enabled
;
; SGTDIS: Segmentation Disable control bit (SYSCON.11):
$IF TINY
_SGTDIS  EQU   1  ; disable segmented mode for TINY model
$ELSE
_SGTDIS  EQU   0  ; enable segmented mode (Reset Value)
$ENDIF
;
; ROMS1: ROM Segment Mapping Control Bit (SYSCON.12):
_ROMS1  EQU 0  ; _ROMS1 = 0 Internal ROM mapped to segment 0
;        ; _ROMS1 = 1 Internal ROM mapped to segment 1
;
; STKSZ: Maximum System Stack Size selection  (SYSCON.13 .. SYSCON.15)
;  Defines the system stack space which is used by CALL/RET and PUSH/POP
;  instructions.  The system stack space must be adjusted according the
;  actual requirements of the application.
$SET (STK_SIZE = 4)
;     System stack sizes:
;       0 = 256 words (Reset Value)
;       1 = 128 words
;       2 =  64 words
;       3 =  32 words
;       4 = 512 words
;       5 = not implemented
;       6 = not implemented
;       7 = no wrapping (entire internal RAM use as STACK, set size with SYSSZ)
; If you have selected 7 for STK_SIZE, you can set the actual system stack size
; with the following SSTSZ statement.
SSTSZ EQU   400H  ; set System Stack Size to 200H Bytes
;
; USTSZ: User Stack Size Definition
;  Defines the user stack space available for automatics.  This stack space is
;  accessed by R0.  The user stack space must be adjusted according the actual
;  requirements of the application.
USTSZ EQU   1000H   ; set User Stack Size to 1000H Bytes.
;
; WATCHDOG: Disable Hardware Watchdog
; --- Set WATCHDOG = 1 to enable the Hardware watchdog
$SET (WATCHDOG = 0)
;
;
; CLR_MEMORY: Disable Memory Zero Initialization of RAM area
; --- Set CLR_MEMORY = 0 to disable memory zero initilization
$SET (CLR_MEMORY = 1)
;
; INIT_VARS: Disable Variable Initialization
; --- Set INIT_VARS = 0 to disable variable initilization
$SET (INIT_VARS = 1)
;
; DPPUSE:  Re-assign DPP registers
; --- Set DPPUSE = 0 to reduce the code size of the startup code, if you
;                    are not using the L166 DPPUSE directive.
$SET (DPPUSE = 1)
;
;------------------------------------------------------------------------------
; Initialization for XPERCON register (available on some derivatives only
;
; INIT_XPERCON: Init XPERCON register available on some devices
; --- Set INIT_XPERCON = 1 to initilize the XPERCON register
$SET (INIT_XPERCON = 0)
;
; --- XPERCON values
;
; V_CAN1: make CAN1 address range 0xEF00 .. 0xEFFF visible (XPERCON.0)
V_CAN1     EQU     1       ; 0 = CAN1 is not visible on the XBUS
;                          ; 1 = CAN1 is visible on the XBUS (default)
;
; V_CAN2: make CAN2 address range 0xEE00 .. 0xEEFF visible (XPERCON.1)
V_CAN2     EQU     0       ; 0 = CAN2 is not visible on the XBUS (default)
;                          ; 1 = CAN2 is visible on the XBUS
;
; V_XRAM2: make 2KB XRAM address range 0xE000 .. 0xE7FF visible (XPERCON.10)
V_XRAM2    EQU     1       ; 0 = 2KB XRAM is not visible on the XBUS
;                          ; 1 = 2KB XRAM is visible on the XBUS (default)
;
; V_XRAM6: make 6KB XRAM address range 0xC000 .. 0xD7FF visible (XPERCON.11)
V_XRAM6    EQU     0       ; 0 = 6KB XRAM is not visible on the XBUS (default)
;                          ; 1 = 6KB XRAM is visible on the XBUS
;
; V_XFLASH: make 4KB XFLASH address range 0x8000 .. 0x8FFF visible (XPERCON.14)
V_XFLASH   EQU     0       ; 0 = 4KB XFLASH is not visible on the XBUS (default)
;                          ; 1 = 4KB XFLASH is visible on the XBUS
;
;------------------------------------------------------------------------------
;
; Initialization for SYSCON2 and SYSCON3 (available on some derivatives only)
; Note: The SYSCON2 and SYSCON3 bits may be different in some derivatives.
;       Check the values carefully!
;
; ADVANCED_SYSCON: Init SYSCON2 and SYSCON3 register available on some devices
; --- Set ADVANCE_SYSCON = 1 to initilize SYSCON2 and SYSCON3
$SET (ADVANCED_SYSCON = 0)
;
; --- SYSCON2 values
;
; PDCON: Power Down Control (during power down mode) (SYSCON2.4 .. SYSCON2.5)
PDCON   EQU     3       ; 0 = RTC On,  Ports On  (default after Reset)
;                       ; 1 = RTC On,  Ports Off
;                       ; 2 = RTC Off, Ports On
;                       ; 3 = RTC Off, Ports Off
;
; RTS: RTC Clock Source (not affected by a reset) (SYSCON2.6)
RTS     EQU     0 ; 0 = Main oscillator
;                       ; 1 = Auxiliary oscillator (if available)
;
; SCS: SDD Clock Source (not affected by a reset) (SYSCON2.7)
SCS     EQU     0 ; 0 = Main oscillator
;                       ; 1 = Auxiliary oscillator (if available)
;
; CLKCON: Clock State Control (SYSCON2.8 .. SYSCON2.9)
CLKCON  EQU     0 ; 0 = Running on configured basic frequency
;                       ; 1 = Running on slow down frequency, PLL ON
;                       ; 2 = Running on slow down frequency, PLL OFF
;                       ; 3 = reserved
;
; CLKREL: Reload Counter Value for Slowdown Devider (SYSCON2.10 .. SYSCON2.14)
CLKREL  EQU     0       ; possible values are 0 .. 31
;
;
; --- SYSCON3 values: disable on-chip peripherals
;
ADCDIS  EQU     0  ; 1 = disable Analog/Digital Converter    (SYSCON3.0)
ASC0DIS EQU 	0  ; 1 = disable UART ASC0                   (SYSCON3.1)
SSCDIS  EQU   	0  ; 1 = disable Synchronus Serial Cnl SSC   (SYSCON3.2)
GPTDIS  EQU   	0  ; 1 = disable Timer Block GPT             (SYSCON3.3)
                  ; reserved                                (SYSCON3.4)
FMDIS   EQU   	1  ; 1 = disable on-chip Flash Memory Module (SYSCON3.5)
CC1DIS  EQU   	0  ; 1 = disable CAPCOM Unit 1               (SYSCON3.6)
CC2DIS  EQU   	0  ; 1 = disable CAPCOM Unit 2               (SYSCON3.7)
CC6DIS  EQU   	0  ; 1 = disable CAPCOM Unit 6               (SYSCON3.8)
PWMDIS  EQU   	0  ; 1 = disable Pulse Width Modulation Unit (SYSCON3.9)
ASC1DIS EQU 	0  ; 1 = disable UART ASC1                   (SYSCON3.10)
I2CDIS  EQU     0  ; 1 = disable I2C Bus Module              (SYSCON3.11)
;                       ; reserved                                (SYSCON3.12)
CAN1DIS EQU     0  ; 1 = disable on-chip CAN Module 1        (SYSCON3.13)
CAN2DIS EQU     1  ; 1 = disable on-chip CAN Module 2        (SYSCON3.14)
PCDDIS  EQU     0  ; 1 = disable Peripheral Clock Driver     (SYSCON3.15)
;
;------------------------------------------------------------------------------
;
; BUSCON1/ADDRSEL1 .. BUSCON4/ADDRSEL4 Initialization
; ===================================================
;
;
; BUSCON1/ADDRSEL1
; --- Set BUSCON1 = 1 to initialize the BUSCON1/ADDRSEL1 registers
$SET (BUSCON1 = 0)
;
; Define the start address and the address range of Chip Select 1 (CS1#)
; This values are used to set the ADDRSEL1 register
%DEFINE (ADDRESS1) (080000H)     ; Set CS1# Start Address (default 100000H)
%DEFINE (RANGE1)   (256K)       ; Set CS1# Range (default 1024K = 1MB)
;
; MCTC1: Memory Cycle Time (BUSCON1.0 .. BUSCON1.3):
; Note: if RDYEN1 == 1 a maximum number of 7 waitstates can be selected
_MCTC1    EQU  0  ; Memory wait states is 1 (MCTC1 field = 0EH).
;
; RWDC1: Read/Write Signal Delay (BUSCON1.4):
_RWDC1    EQU  1  ; 0 = Delay Time     0.5 States
;        ; 1 = No Delay Time  0   States
;
; MTTC1: Memory Tri-state Time (BUSCON1.5):
_MTTC1    EQU  1  ; 0 = Delay Time     0.5 States
;        ; 1 = No Delay Time  0   States
;
; BTYP1: External Bus Configuration Control (BUSCON1.6 .. BUSCON1.7):
_BTYP1    EQU  1  ; 0 = 8 Bit Non Multiplexed
;        ; 1 = 8 Bit Multiplexed
;        ; 2 = 16 Bit Non Multiplexed
;        ; 3 = 16 Bit Multiplexed
;
; ALECTL1: ALE Lengthening Control Bit (BUSCON1.9):
_ALECTL1 EQU   0  ; see data sheet for description
;
; BUSACT1: Bus Active Control Bit (BUSCON1.10):
_BUSACT1 EQU   1  ; = 0 external (ADDRSEL1) bus disabled
         ; = 1 external (ADDRSEL1) bus enabled
;
; RDYEN1: READY# Input Enable control bit (BUSCON1.12):
_RDYEN1  EQU   0  ; 0 = READY# function disabled
;        ; 1 = READY# function enabled
;
; RDY_AS1: Synchronous / Asynchronous READY# Input (BUSCON1.3):
; Note: This bit is only valid if _RDYEN1 == 1.
_RDY_AS1 EQU   0  ; 0 = synchronous READY# input
;        ; 1 = asynchronous READY# input
;
; CSREN1: Read Chip Select Enable bit (BUSCON1.14):
_CSREN1  EQU   0  ; 0 = CS1# is independent of read command (RD#)
;        ; 1 = CS1# is generated for the duration of read
;
; CSWEN1: Write Chip Select Enable bit (BUSCON1.15):
_CSWEN1  EQU   0  ; 0 = CS1# is independent of write command (WR#)
;        ; 1 = CS1# is generated for the duration of write
;
;
; BUSCON2/ADDRSEL2
; --- Set BUSCON2 = 1 to initialize the BUSCON2/ADDRSEL2 registers
$SET (BUSCON2 = 0)
;
; Define the start address and the address range of Chip Select 2 (CS2#)
; This values are used to set the ADDRSEL2 register
%DEFINE (ADDRESS2) (0C0000H)     ; Set CS2# Start Address (default 200000H)
%DEFINE (RANGE2)   (256K)       ; Set CS2# Range (default 1024K = 1MB)
;
; MCTC2: Memory Cycle Time (BUSCON2.0 .. BUSCON2.3):
; Note: if RDYEN2 == 1 a maximum number of 7 waitstates can be selected
_MCTC2    EQU  0  ; Memory wait states is 1 (MCTC2 field = 0EH).
;
; RWDC2: Read/Write Signal Delay (BUSCON2.4):
_RWDC2    EQU  1  ; 0 = Delay Time     0.5 States
;        ; 1 = No Delay Time  0   States
;
; MTTC2: Memory Tri-state Time (BUSCON2.5):
_MTTC2    EQU  1  ; 0 = Delay Time     0.5 States
;        ; 1 = No Delay Time  0   States
;
; BTYP2: External Bus Configuration Control (BUSCON2.6 .. BUSCON2.7):
_BTYP2    EQU  1  ; 0 = 8 Bit Non Multiplexed
;        ; 1 = 8 Bit Multiplexed
;        ; 2 = 16 Bit Non Multiplexed
;        ; 3 = 16 Bit Multiplexed
;
; ALECTL2: ALE Lengthening Control Bit (BUSCON2.9):
_ALECTL2 EQU   0  ; see data sheet for description
;
; BUSACT2: Bus Active Control Bit (BUSCON2.10):
_BUSACT2 EQU   1  ; = 0 external (ADDRSEL2) bus disabled
         ; = 1 external (ADDRSEL2) bus enabled
;
; RDYEN2: READY# Input Enable control bit (BUSCON2.12):
_RDYEN2  EQU   0  ; 0 = READY# function disabled
;        ; 1 = READY# function enabled
;
; RDY_AS2: Synchronous / Asynchronous READY# Input (BUSCON2.3):
; Note: This bit is only valid if _RDYEN2 == 1.
_RDY_AS2 EQU   0  ; 0 = synchronous READY# input
;        ; 1 = asynchronous READY# input
;
; CSREN2: Read Chip Select Enable bit (BUSCON2.14):
_CSREN2  EQU   0  ; 0 = CS2# is independent of read command (RD#)
;        ; 1 = CS2# is generated for the duration of read
;
; CSWEN2: Write Chip Select Enable bit (BUSCON2.15):
_CSWEN2  EQU   0  ; 0 = CS2# is independent of write command (WR#)
;        ; 1 = CS2# is generated for the duration of write
;
;
; BUSCON3/ADDRSEL3
; --- Set BUSCON3 = 1 to initialize the BUSCON3/ADDRSEL3 registers
$SET (BUSCON3 = 0)
;
; Define the start address and the address range of Chip Select 3 (CS3#)
; This values are used to set the ADDRSEL3 register
%DEFINE (ADDRESS3) (300000H)     ; Set CS3# Start Address (default 300000H)
%DEFINE (RANGE3)   (1024K)       ; Set CS3# Range (default 1024K = 1MB)
;
; MCTC3: Memory Cycle Time (BUSCON3.0 .. BUSCON3.3):
; Note: if RDYEN3 == 1 a maximum number of 7 waitstates can be selected
_MCTC3    EQU  1  ; Memory wait states is 1 (MCTC3 field = 0EH).
;
; RWDC3: Read/Write Signal Delay (BUSCON3.4):
_RWDC3    EQU  0  ; 0 = Delay Time     0.5 States
;        ; 1 = No Delay Time  0   States
;
; MTTC3: Memory Tri-state Time (BUSCON3.5):
_MTTC3    EQU  1  ; 0 = Delay Time     0.5 States
;        ; 1 = No Delay Time  0   States
;
; BTYP3: External Bus Configuration Control (BUSCON3.6 .. BUSCON3.7):
_BTYP3    EQU  1  ; 0 = 8 Bit Non Multiplexed
;        ; 1 = 8 Bit Multiplexed
;        ; 2 = 16 Bit Non Multiplexed
;        ; 3 = 16 Bit Multiplexed
;
; ALECTL3: ALE Lengthening Control Bit (BUSCON3.9):
_ALECTL3 EQU   0  ; see data sheet for description
;
; BUSACT3: Bus Active Control Bit (BUSCON3.10):
_BUSACT3 EQU   1  ; = 0 external (ADDRSEL3) bus disabled
         ; = 1 external (ADDRSEL3) bus enabled
;
; RDYEN3: READY# Input Enable control bit (BUSCON3.12):
_RDYEN3  EQU   0  ; 0 = READY# function disabled
;        ; 1 = READY# function enabled
;
; RDY_AS3: Synchronous / Asynchronous READY# Input (BUSCON3.3):
; Note: This bit is only valid if _RDYEN3 == 1.
_RDY_AS3 EQU   0  ; 0 = synchronous READY# input
;        ; 1 = asynchronous READY# input
;
; CSREN3: Read Chip Select Enable bit (BUSCON3.14):
_CSREN3  EQU   0  ; 0 = CS3# is independent of read command (RD#)
;        ; 1 = CS3# is generated for the duration of read
;
; CSWEN3: Write Chip Select Enable bit (BUSCON3.15):
_CSWEN3  EQU   0  ; 0 = CS3# is independent of write command (WR#)
;        ; 1 = CS3# is generated for the duration of write
;
;
; BUSCON4/ADDRSEL4
; --- Set BUSCON4 = 1 to initialize the BUSCON4/ADDRSEL4 registers
$SET (BUSCON4 = 0)
;
; Define the start address and the address range of Chip Select 4 (CS4#)
; This values are used to set the ADDRSEL4 register
%DEFINE (ADDRESS4) (400000H)     ; Set CS4# Start Address (default 400000H)
%DEFINE (RANGE4)   (1024K)       ; Set CS4# Range (default 1024K = 1MB)
;
; MCTC4: Memory Cycle Time (BUSCON4.0 .. BUSCON4.3):
; Note: if RDYEN4 == 1 a maximum number of 7 waitstates can be selected
_MCTC4    EQU  1  ; Memory wait states is 1 (MCTC4 field = 0EH).
;
; RWDC4: Read/Write Signal Delay (BUSCON4.4):
_RWDC4    EQU  0  ; 0 = Delay Time     0.5 States
;        ; 1 = No Delay Time  0   States
;
; MTTC4: Memory Tri-state Time (BUSCON4.5):
_MTTC4    EQU  1  ; 0 = Delay Time     0.5 States
;        ; 1 = No Delay Time  0   States
;
; BTYP4: External Bus Configuration Control (BUSCON4.6 .. BUSCON4.7):
_BTYP4    EQU  1  ; 0 = 8 Bit Non Multiplexed
;        ; 1 = 8 Bit Multiplexed
;        ; 2 = 16 Bit Non Multiplexed
;        ; 3 = 16 Bit Multiplexed
;
; ALECTL4: ALE Lengthening Control Bit (BUSCON4.9):
_ALECTL4 EQU   0  ; see data sheet for description
;
; BUSACT4: Bus Active Control Bit (BUSCON4.10):
_BUSACT4 EQU   1  ; = 0 external (ADDRSEL4) bus disabled
         ; = 1 external (ADDRSEL4) bus enabled
;
; RDYEN4: READY# Input Enable control bit (BUSCON4.12):
_RDYEN4  EQU   0  ; 0 = READY# function disabled
;        ; 1 = READY# function enabled
;
; RDY_AS4: Synchronous / Asynchronous READY# Input (BUSCON4.3):
; Note: This bit is only valid if _RDYEN4 == 1.
_RDY_AS4 EQU   0  ; 0 = synchronous READY# input
;        ; 1 = asynchronous READY# input
;
; CSREN4: Read Chip Select Enable bit (BUSCON4.14):
_CSREN4  EQU   0  ; 0 = CS4# is independent of read command (RD#)
;        ; 1 = CS4# is generated for the duration of read
;
; CSWEN4: Write Chip Select Enable bit (BUSCON4.15):
_CSWEN4  EQU   0  ; 0 = CS4# is independent of write command (WR#)
;        ; 1 = CS4# is generated for the duration of write
;
;------------------------------------------------------------------------------

$IF TINY
$SET (DPPUSE = 0)
$ENDIF

_STKSZ      SET   0
_STKSZ1     SET   0     ; size is 512 Words
$IF (STK_SIZE = 0)
_STKSZ1     SET   1     ; size is 256 Words
$ENDIF
$IF (STK_SIZE = 1)
_STKSZ      SET   1
_STKSZ1     SET   2     ; size is 128 Words
$ENDIF
$IF (STK_SIZE = 2)
_STKSZ      SET   2
_STKSZ1     SET   3     ; size is  64 Words
$ENDIF
$IF (STK_SIZE = 3)
_STKSZ      SET   3
_STKSZ1     SET   4     ; size is  32 Words
$ENDIF
$IF (STK_SIZE = 4)
_STKSZ      SET   4
$ENDIF
$IF (STK_SIZE = 5)
_STKSZ      SET   5
$ENDIF
$IF (STK_SIZE = 6)
_STKSZ      SET   6
$ENDIF
$IF (STK_SIZE = 7)
_STKSZ      SET   7
$ENDIF


$IF NOT TINY
ASSUME   DPP3:SYSTEM
ASSUME  DPP2:NDATA
$ENDIF

NAME  ?C_STARTUP
PUBLIC   ?C_STARTUP

$IF MEDIUM OR LARGE OR HLARGE
Model LIT   'FAR'
$ELSE
Model LIT   'NEAR'
$ENDIF

EXTRN main:Model

PUBLIC      ?C_USRSTKBOT

?C_USERSTACK   SECTION  DATA PUBLIC 'NDATA'
$IF NOT TINY
NDATA    DGROUP   ?C_USERSTACK
$ENDIF
?C_USRSTKBOT:
      DS USTSZ    ; Size of User Stack
?C_USERSTKTOP:
?C_USERSTACK   ENDS

?C_MAINREGISTERS  REGDEF   R0 - R15

$IF (STK_SIZE = 7)

?C_SYSSTACK SECTION  DATA PUBLIC 'IDATA'
$IF NOT TINY
SDATA    DGROUP   ?C_SYSSTACK
$ENDIF
_BOS:             ; bottom of system stack
      DS SSTSZ    ; Size of User Stack
_TOS:             ; top of system stack
?C_SYSSTACK ENDS

$ELSE
; Setup Stack Overflow
_TOS  EQU   0FC00H                 ; top of system stack
_BOS  EQU   _TOS - (1024 >> _STKSZ1)   ; bottom of system stack
$ENDIF

PUBLIC      ?C_SYSSTKBOT
?C_SYSSTKBOT   EQU   _BOS

      SSKDEF   _STKSZ      ; System stack size

?C_STARTUP_CODE   SECTION  CODE  'ICODE'


;------------------------------------------------------------------------------

; Special Function Register Addresses
SYSCON   DEFR   0FF12H
BUSCON0  DEFR   0FF0CH
SP       DEFR  0FE12H
STKOV    DEFR   0FE14H
STKUN    DEFR  0FE16H
P3       DEFR  0FFC4H
DP3      DEFR  0FFC6H
BUSCON1  DEFR   0FF14H
BUSCON2  DEFR   0FF16H
BUSCON3  DEFR   0FF18H
BUSCON4  DEFR   0FF1AH
ADDRSEL1 DEFR   0FE18H
ADDRSEL2 DEFR   0FE1AH
ADDRSEL3 DEFR   0FE1CH
ADDRSEL4 DEFR   0FE1EH
SYSCON2  DEFR   0F1D0H
SYSCON3  DEFR   0F1D4H

DP1H	 DEFR	0F106H
DP1L	 DEFR	0F104H
P1H	 DEFR	0FF06H
P1L	 DEFR	0FF04H

%*DEFINE (ADDR (Val, Start, Range)) (
%SET (adr, %SUBSTR(%Start,1,(%LEN(%Start)- 3))%SUBSTR(%Start,%LEN(%Start),1))
%IF (%EQS (%Range,4K))     THEN (%SET (adr, (%adr AND 0FFF0H) + 0)) FI
%IF (%EQS (%Range,8K))     THEN (%SET (adr, (%adr AND 0FFE0H) + 1)) FI
%IF (%EQS (%Range,16K))    THEN (%SET (adr, (%adr AND 0FFC0H) + 2)) FI
%IF (%EQS (%Range,32K))    THEN (%SET (adr, (%adr AND 0FF80H) + 3)) FI
%IF (%EQS (%Range,64K))    THEN (%SET (adr, (%adr AND 0FF00H) + 4)) FI
%IF (%EQS (%Range,128K))   THEN (%SET (adr, (%adr AND 0FE00H) + 5)) FI
%IF (%EQS (%Range,256K))   THEN (%SET (adr, (%adr AND 0FC00H) + 6)) FI
%IF (%EQS (%Range,512K))   THEN (%SET (adr, (%adr AND 0F800H) + 7)) FI
%IF (%EQS (%Range,1024K))  THEN (%SET (adr, (%adr AND 0F000H) + 8)) FI
%IF (%EQS (%Range,1M))     THEN (%SET (adr, (%adr AND 0F000H) + 8)) FI
%IF (%EQS (%Range,2048K))  THEN (%SET (adr, (%adr AND 0E000H) + 9)) FI
%IF (%EQS (%Range,2M))     THEN (%SET (adr, (%adr AND 0E000H) + 9)) FI
%IF (%EQS (%Range,4096K))  THEN (%SET (adr, (%adr AND 0C000H) +10)) FI
%IF (%EQS (%Range,4M))     THEN (%SET (adr, (%adr AND 0C000H) +10)) FI
%IF (%EQS (%Range,8192K))  THEN (%SET (adr, (%adr AND 08000H) +11)) FI
%IF (%EQS (%Range,8M))     THEN (%SET (adr, (%adr AND 08000H) +11)) FI
%Val  EQU   %adr
)

?C_RESET    PROC TASK C_STARTUP INTNO RESET = 0
?C_STARTUP:     LABEL   Model

$IF (WATCHDOG = 0)
      DISWDT         ; Disable watchdog timer		
$ENDIF


$IF (INIT_XPERCON = 1)
; Improtant XPERCON must be set before SYSCON.XPEN is enabled
XPERCON  DEFR 0F024H
V_XPERCON       SET     V_CAN1 OR (V_CAN2 << 1) OR (V_XRAM2 << 10)
V_XPERCON   SET   V_XPERCON OR (V_XRAM6 << 11) OR (V_XFLASH << 14)
      EXTR  #1
      MOV   XPERCON,#V_XPERCON
$ENDIF

BCON0L      SET     (_MTTC0 << 5) OR (_RWDC0 << 4)
BCON0L      SET   BCON0L OR ((NOT _MCTC0) AND 0FH)
BCON0L      SET   BCON0L AND (NOT (_RDYEN0 << 3))
BCON0L      SET   BCON0L OR (_RDY_AS0 << 3)
BCON0H      SET   (_ALECTL0 << 1) OR (_RDYEN0 << 4)
BCON0H      SET   BCON0H OR (_CSREN0 << 6) OR (_CSWEN0 << 7)
$IF (BTYP_ENABLE == 1)
BCON0L      SET   BCON0L OR (_BTYP0 << 6)
BCON0H      SET   BCON0H OR (_BUSACT0 << 2)
$ENDIF

$IF (BTYP_ENABLE == 0)
      BFLDL BUSCON0,#03FH,#BCON0L
      BFLDH BUSCON0,#0D2H,#BCON0H
$ELSE
      BFLDL BUSCON0,#0FFH,#BCON0L
      BFLDH BUSCON0,#0D6H,#BCON0H
$ENDIF
SYS_BITS SET   0FF6FH

SYS_H    SET   (_STKSZ << 5) OR (_ROMS1 << 4) OR (_SGTDIS << 3)
SYS_H    SET   SYS_H OR (_ROMEN << 2) OR (_BYTDIS << 1) OR _CLKEN
SYS_L    SET   _XPERSHARE OR (_VISIBLE << 1) OR (_XPEN << 2)
SYS_L           SET     SYS_L OR (_BDRSTEN << 3)
SYS_L           SET     SYS_L OR (_PWDCFG << 5) OR (_CSCFG << 6)
$IF (WRCFG_ENABLE == 1)
SYS_L    SET   SYS_L OR (_WRCFG << 7)
SYS_BITS SET   SYS_BITS OR 00080H
$ENDIF
$IF (OWDDIS_ENABLE == 1)
SYS_L    SET   SYS_L OR (_OWDDIS << 4)
SYS_BITS SET   SYS_BITS OR 00010H
$ENDIF
; Setup SYSCON Register

      BFLDH SYSCON,#HIGH SYS_BITS,#SYS_H
      BFLDL SYSCON,#LOW  SYS_BITS,#SYS_L
;
$IF (ADVANCED_SYSCON = 1)
SYS_2           SET     (PDCON << 4) OR (RTS << 6) OR (SCS << 7)
SYS_2           SET     SYS_2 OR (CLKCON << 8) OR (CLKREL << 10)

SYS_3           SET     ADCDIS OR (ASC0DIS << 1) OR (SSCDIS << 2)
SYS_3           SET     SYS_3 OR (GPTDIS << 3)
SYS_3           SET     SYS_3 OR (FMDIS << 5) OR (CC1DIS << 6) OR (CC2DIS << 7)
SYS_3           SET     SYS_3 OR (CC6DIS << 8) OR (PWMDIS << 9)
SYS_3           SET     SYS_3 OR (ASC1DIS << 10) OR (I2CDIS << 11)
SYS_3           SET     SYS_3 OR (CAN1DIS << 13) OR (CAN2DIS << 14)
SYS_3           SET     SYS_3 OR (PCDDIS << 15)

      EXTR  #2
      MOV   SYSCON2,#SYS_2
      MOV   SYSCON3,#SYS_3
$ENDIF
;

$IF (BUSCON1 = 1)
BCON1    SET     (_MTTC1 << 5) OR (_RWDC1 << 4)
BCON1    SET   BCON1 OR ((NOT _MCTC1) AND 0FH)
BCON1    SET   BCON1 AND (NOT (_RDYEN1 << 3))
BCON1       SET   BCON1 OR (_RDY_AS1 << 3)  OR (_BTYP1 << 6)
BCON1       SET   BCON1 OR (_ALECTL1 << 9) OR (_BUSACT1 << 10)
BCON1    SET   BCON1 OR (_RDYEN1 << 12) OR (_CSREN1 << 14)
BCON1           SET     BCON1 OR (_CSWEN1 << 15)

%ADDR (ADDR1,%ADDRESS1,%RANGE1)
      MOV   ADDRSEL1,#ADDR1
      MOV   BUSCON1,#BCON1
$ENDIF

$IF (BUSCON2 = 1)
BCON2    SET     (_MTTC2 << 5) OR (_RWDC2 << 4)
BCON2    SET   BCON2 OR ((NOT _MCTC2) AND 0FH)
BCON2    SET   BCON2 AND (NOT (_RDYEN2 << 3))
BCON2       SET   BCON2 OR (_RDY_AS2 << 3)  OR (_BTYP2 << 6)
BCON2       SET   BCON2 OR (_ALECTL2 << 9) OR (_BUSACT2 << 10)
BCON2    SET   BCON2 OR (_RDYEN2 << 12) OR (_CSREN2 << 14)
BCON2           SET     BCON2 OR (_CSWEN2 << 15)

%ADDR (ADDR2,%ADDRESS2,%RANGE2)
      MOV   ADDRSEL2,#ADDR2
      MOV   BUSCON2,#BCON2
$ENDIF

$IF (BUSCON3 = 1)
BCON3    SET     (_MTTC3 << 5) OR (_RWDC3 << 4)
BCON3    SET   BCON3 OR ((NOT _MCTC3) AND 0FH)
BCON3    SET   BCON3 AND (NOT (_RDYEN3 << 3))
BCON3       SET   BCON3 OR (_RDY_AS3 << 3)  OR (_BTYP3 << 6)
BCON3       SET   BCON3 OR (_ALECTL3 << 9) OR (_BUSACT3 << 10)
BCON3    SET   BCON3 OR (_RDYEN3 << 12) OR (_CSREN3 << 14)
BCON3           SET     BCON3 OR (_CSWEN3 << 15)

%ADDR (ADDR3,%ADDRESS3,%RANGE3)
      MOV   ADDRSEL3,#ADDR3
      MOV   BUSCON3,#BCON3
$ENDIF

$IF (BUSCON4 = 1)
BCON4    SET     (_MTTC4 << 5) OR (_RWDC4 << 4)
BCON4    SET   BCON4 OR ((NOT _MCTC4) AND 0FH)
BCON4    SET   BCON4 AND (NOT (_RDYEN4 << 3))
BCON4       SET   BCON4 OR (_RDY_AS4 << 3)  OR (_BTYP4 << 6)
BCON4       SET   BCON4 OR (_ALECTL4 << 9) OR (_BUSACT4 << 10)
BCON4    SET   BCON4 OR (_RDYEN4 << 12) OR (_CSREN4 << 14)
BCON4           SET     BCON4 OR (_CSWEN4 << 15)

%ADDR (ADDR4,%ADDRESS4,%RANGE4)
      MOV   ADDRSEL4,#ADDR4
      MOV   BUSCON4,#BCON4
$ENDIF



$IF (STK_SIZE = 7)
      MOV   STKUN,#0FFFEH     ; AVOID STKUN TRAP
      MOV   STKOV,#0H      ; AVOID STKOV TRAP
      MOV   SP,#DPP3:_TOS     ; INITIALIZE STACK POINTER
      MOV   STKUN,#DPP3:_TOS  ; INITIALIZE STACK UNFL REGISTER
      MOV   STKOV,#DPP3:(_BOS+6*2)  ; INITIALIZE STACK OVFL REGISTER
$ELSE
      MOV   STKOV,#(_BOS+6*2) ; INITIALIZE STACK OVFL REGISTER
$ENDIF

$IF NOT TINY

      EXTRN ?C_PAGEDPP0 : DATA16
      EXTRN ?C_PAGEDPP1 : DATA16
      EXTRN ?C_PAGEDPP2 : DATA16

$IF (DPPUSE = 1)
      MOV   DPP0,#?C_PAGEDPP0       ; INIT DPP0 VALUE
$ENDIF
      MOV   DPP1,#?C_PAGEDPP1 ; DEFAULT NEAR CONST PAGE
      MOV   DPP2,#?C_PAGEDPP2 ; DEFAULT NEAR DATA  PAGE

$ENDIF

      MOV   CP,#?C_MAINREGISTERS
      EINIT

$IF NOT TINY
         MOV   R0,#DPP2:?C_USERSTKTOP
$ELSE
            MOV   R0,#?C_USERSTKTOP
$ENDIF

;------------------------------------------------------------------------------
;
; The following code is necessary to set RAM variables to 0 at start-up
; (RESET) of the C application program.
;


; enable external ram

  EXTR      #01H
  MOV       DP1L,#0C1H
                                           ; SOURCE LINE # 170
                                           ; SOURCE LINE # 171
  EXTR      #01H
  MOV       DP1H,#07fH

  MOV       P1L,#01H                      ;set ramenable high


;------------------------------------------------------------------------------


$IF (CLR_MEMORY = 1)

      EXTRN ?C_CLRMEMSECSTART : WORD
Clr_Memory:
$IF TINY
      MOV   R8,#?C_CLRMEMSECSTART
      JMPR  cc_Z,EndClear

RepeatClear:
$IF (WATCHDOG = 1)
      SRVWDT            ; SERVICE WATCHDOG
$ENDIF
      MOV   R5,#0
      MOV   R2,[R8+]    ; Count
      JMPR  cc_Z,EndClear
      MOV     R3,R2
      MOV     R4,[R8+]
      JBC     R2.14,ClearNear

ClearBit:   MOV   R3,R4
      SHR   R3,#3
      BCLR  R3.0
      ADD   R3,#0FD00H     ; START OF BIT SPACE
      MOV   R5,#1
      SHL   R5,R4
      CPL   R5
      AND   R5,[R3]
      MOV   [R3],R5
      ADD   R4,#1
      SUB   R2,#1
      JMPR  cc_NZ,ClearBit
      JMPR  cc_UC,RepeatClear

ClearNear:
$IF (WATCHDOG = 1)
      SRVWDT            ; SERVICE WATCHDOG
$ENDIF
      MOVB  [R4],RL5
      ADD   R4,#1
      SUB   R2,#1
      JMPR  cc_NN,ClearNear
      JMPR  cc_UC,RepeatClear

$ELSE
      MOV   R9,#SEG (?C_CLRMEMSECSTART)
      MOV   R8,#SOF (?C_CLRMEMSECSTART)
      MOV   R1,R8
           OR  R1,R9
      JMPR  cc_Z,EndClear

RepeatClear:
$IF (WATCHDOG = 1)
      SRVWDT            ; SERVICE WATCHDOG
$ENDIF
      MOV   R5,#0
                EXTS    R9,#1
      MOV   R2,[R8+]    ; Count
      JMPR  cc_Z,EndClear
      MOV     R3,R2
                EXTS R9,#1
      MOV     R4,[R8+]
      JBC     R2.14,ClearNear
      JBC     R2.15,ClearFar

ClearBit:   MOV   R3,R4
      SHR   R3,#3
      BCLR  R3.0
      ADD   R3,#0FD00H     ; START OF BIT SPACE
      MOV   R5,#1
      SHL   R5,R4
      CPL   R5
      AND   R5,[R3]
      MOV   [R3],R5
      ADD   R4,#1
      SUB   R2,#1
      JMPR  cc_NZ,ClearBit
      JMPR  cc_UC,RepeatClear

ClearFar:   EXTS  R9,#1
                MOV  R3,[R8+]
RepClearFar:
$IF (WATCHDOG = 1)
      SRVWDT            ; SERVICE WATCHDOG
$ENDIF
      EXTP  R4,#1
      MOVB  [R3],RL5
      ADD   R3,#1
      ADDC  R4,#0
      OR R3,#0C000H
      SUB   R2,#1
      JMPR  cc_NN,RepClearFar
      JMPR  cc_UC,RepeatClear

ClearNear:
$IF (WATCHDOG = 1)
      SRVWDT            ; SERVICE WATCHDOG
$ENDIF
      MOVB  [R4],RL5
      ADD   R4,#1
      SUB   R2,#1
      JMPR  cc_NN,ClearNear
      JMPR  cc_UC,RepeatClear
$ENDIF

EndClear:

$ENDIF

;------------------------------------------------------------------------------
;
;  The following code is necessary, if the application program contains
;  initialized variables at file level.
;

$IF (INIT_VARS = 1)

      EXTRN ?C_INITSECSTART : WORD
Init_Vars:
$IF TINY
      MOV   R8,#?C_INITSECSTART
RepeatInit:
$IF (WATCHDOG = 1)
      SRVWDT            ; SERVICE WATCHDOG
$ENDIF
      MOV   R2,[R8+]
      JMPR  cc_Z,EndInit
      JBC   R2.15,InitBit
      MOV   R4,R2
      AND   R2,#3FFFH
      JMPR  cc_NZ,LenLoad
      MOV   R4,#0
      MOV   R2,[R8+]
LenLoad: MOV   R3,[R8+]
CopyInitVal:   MOVB  [R3],[R8+]
$IF (WATCHDOG = 1)
      SRVWDT            ; SERVICE WATCHDOG
$ENDIF
      ADD   R3,#1
      SUB   R2,#1
      JMPR  cc_NZ,CopyInitVal
      JNB   R8.0,RepeatInit
      ADD   R8,#1
      JMPR  cc_UC,RepeatInit

InitBit: MOVBZ R3,RL2
      SHL   R3,#1
      OR    R3,#0FD00H     ; START OF BIT SPACE
      MOV   R4,#1
      MOVB  RL5,RH2
      SHL   R4,R5       ; CALCULATE BIT MASK
      JB R2.7,SetBit
      CPL   R4       ; CLEAR BIT
      AND   R4,[R3]
      JMPR  cc_UC,StoreBit
SetBit:     OR R4,[R3]        ; SET BIT
StoreBit:   MOV   [R3],R4
      JMPR  cc_UC,RepeatInit

EndInit:

$ELSE
      MOV   R9,#SEG (?C_INITSECSTART)
      MOV   R8,#SOF (?C_INITSECSTART)
RepeatInit:
$IF (WATCHDOG = 1)
      SRVWDT            ; SERVICE WATCHDOG
$ENDIF
      MOV   DPP3,#3
      EXTS  R9,#1
      MOV   R2,[R8]
      JMPR  cc_Z,EndInit
      ADD   R8,#2
      ADDC  R9,#0
      JBC   R2.15,InitBit
      MOV   R4,R2
      AND   R2,#3FFFH
      JMPR  cc_NZ,LenLoad
      MOV   R4,#0
      EXTS  R9,#1
      MOV   R2,[R8]
      ADD   R8,#2
      ADDC  R9,#0
LenLoad: EXTS  R9,#1
      MOV   R3,[R8]
      ADD   R8,#2
      ADDC  R9,#0
      JB R4.14,CopyInitVal
      MOV   DPP3,R3
      EXTS  R9,#1
      MOV   R3,[R8]
      ADD   R8,#2
      ADDC  R9,#0
CopyInitVal:   EXTS  R9,#1
      MOVB  RL5,[R8]
      ADD   R8,#1
      ADDC  R9,#0
      MOVB  [R3],RL5
      ADD   R3,#1
      JB R4.14,NoDPP3Adj
      JB R3.14,NoDPP3Adj
      ADD   DPP3,#1
      OR R3,#0C000H     ; SET DPP3 Bits
NoDPP3Adj:
$IF (WATCHDOG = 1)
      SRVWDT            ; SERVICE WATCHDOG
$ENDIF
      SUB   R2,#1
      JMPR  cc_NZ,CopyInitVal
      JNB   R8.0,RepeatInit
      ADD   R8,#1
      JMPR  cc_UC,RepeatInit

InitBit: MOVBZ R3,RL2
      SHL   R3,#1
      OR    R3,#0FD00H     ; START OF BIT SPACE
      MOV   R4,#1
      MOVB  RL5,RH2
      SHL   R4,R5       ; CALCULATE BIT MASK
      JB R2.7,SetBit
      CPL   R4       ; CLEAR BIT
      AND   R4,[R3]
      JMPR  cc_UC,StoreBit
SetBit:     OR R4,[R3]        ; SET BIT
StoreBit:   MOV   [R3],R4
      JMPR  cc_UC,RepeatInit

EndInit:

$ENDIF

$ENDIF

;------------------------------------------------------------------------------

$IF TINY
      JMP   main
$ELSE
      JMP   FAR main
$ENDIF

?C_RESET ENDP
?C_STARTUP_CODE   ENDS

$IF (INIT_VARS = 1)
EXTERN   ?C_ENDINIT:WORD
$ENDIF

      END
