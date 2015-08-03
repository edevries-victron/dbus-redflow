FlashErase:
; see Errata: SE133-FLASH : Unexpected Flash Block Protection Errors
STA ,X ;latch the unprotected address from H:X
NOP ;brief delay to allow the command state machine to start
STA ,X ;intentionally cause an access error to abort this command
psha ;adjust sp for DoOnStack entry
lda #(mFPVIOL+mFACCERR) ;mask
sta FSTAT ;abort any command and clear errors
lda #mPageErase ;mask pattern for page erase command
bsr DoOnStack ;finish command from stack-based sub
ais #1 ;deallocate data location from stack
rts
