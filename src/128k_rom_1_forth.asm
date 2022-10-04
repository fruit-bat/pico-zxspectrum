; Disassembly of the file "C:\ACE\JupiterAce.rom"
; to adapt for spectrum architecture 16 kB ROM

;==================================================
; SPECTRUM FORTH ROM (IO CONVERSION IN PROGRESS)
;==================================================

; TODO Adapt IO

#define DEFB    .BYTE
#define DEFW    .WORD
#define DEFM    .TEXT
#define EQU     .EQU
#define ORG     .ORG

        ORG     $0000

; -------------------
; THE 'START' RESTART
; -------------------

L0000:  DI                              ; disable interrupts.
        LD      HL,$3C00                ; start of 'User' RAM
        LD      A,$FC                   ; a test byte and 1K masking byte.
        JR      L0028                   ; forward to continue at Part 2.

; -------------------
; THE 'PRINT' RESTART
; -------------------

L0008:  EXX                             ; preserve main registers.
        BIT     3,(IX+$3E)              ; test FLAGS for print destination.
        JP      L03EE                   ; forward to

; ---------------------------
; THE 'STACK WORD DE' RESTART
; ---------------------------

L0010:  LD      HL,($3C3B)              ; SPARE
        LD      (HL),E
        INC     HL
        JP      L085F                   ;

; -------------------------
; THE 'POP WORD DE' RESTART
; -------------------------


L0018:  LD      HL,($3C3B)              ; SPARE
        DEC     HL
        LD      D,(HL)
        JP      L0859                   ;

; -------------------
; THE 'ERROR' RESTART
; -------------------

L0020:  POP     HL
        LD      A,(HL)
        LD      ($3C3D),A               ; ERR_NO
        JP      L00AD                   ;

; ------------------------------------
; THE 'INITIALIZATION ROUTINE' Part 2.
; ------------------------------------

L0028:  INC     H                       ; increase high byte
        LD      (HL),A                  ; insert A value
        CP      (HL)                    ; compare to expected
        JR      Z,L0028                 ; loop back while RAM is populated.

        AND     H                       ; limit to nearest 1K segment.
        LD      H,A                     ; place back in H.
        LD      ($3C18),HL              ; set system variable RAMTOP.
        LD      SP,HL                   ; initialize the stack pointer.

; the Z80 instructions CALL, PUSH and POP can now be used.

        LD      HL,L010D                ; prepare to copy the system variables
                                        ; initial state from ROM.
        JR      L003B                   ; skip past the fixed-position restart.

; -----------------------
; THE 'INTERRUPT' RESTART
; -----------------------

L0038:  JP      L013A                   ; jump to somewhere more convenient.

;------------------------------------------------------------------------------
;
; MEMORY MAP
;
; $0000 +======================================================+
;       |                                                      |
;       |                   ROM 8K                             |
;       |                                     v $2300          |
; $2000 +======================================================+ - - - - - -
;       |       copy of $2400                 |0|<  cassette  >|
; $2400 +-------------------------------------+-+--------------+
;       |       VIDEO MEMORY 768 bytes        |0| PAD 254 bytes| 1K RAM
; $2800 +-------------------------------------+-+--------------+
;       |       copy of $2c00                 ^ $2700          |
; $2C00 +------------------------------------------------------+
;       |       CHARACTER SET - Write-Only                     | 1K RAM
; $3000 +------------------------------------------------------+
;       |       copy of $3c00                                  |
; $3400 +------------------------------------------------------+
;       |       copy of $3c00                                  |
; $3800 +------------------------------------------------------+
;       |       copy of $3c00                                  |
; $3C00 +-------+----------------------------------------------+
;       |SYSVARS| DICT {12} DATA STACK ->         <- RET STACK | 1K RAM
; $4000 +=======+==============================================+ - - - - - -
;       |                                                      |
;                       48K AVAILABLE FOR EXPANSION.
;       |                                                      |
; $FFFF +======================================================+
;
; The Ace had an 8K ROM and was sold with 3K of RAM each byte of which had
; at least two addresses and sometimes four addresses so the mapping of the
; 3K of RAM was as above.
; The 768 bytes of video memory is accessed by the ROM using addresses
; $2400 - $26FF. This gives priority to the video circuitry which also needs
; this information to build the TV picture. The byte at $2700 is set to zero
; so that it is easy for the ROM to detect when it is at the end of the screen.
; The 254 bytes remaining are the PAD - the workspace used by FORTH.
; This same area is used by the tape recorder routines to assemble the tape
; header information but since, for accurate tape timing, the FORTH ROM needs
; priority over the video circuitry, then the ROM uses addresses $2301 - $23FF.
;
; Similarly the Character Set is written to by the ROM (and User) at the 1K
; section starting at $2C00. The video circuitry accesses this using addresses
; $2800 - $2BFF to build the TV picture. It is not possible for the ROM or User
; to read back the information from either address so this precludes the saving
; of character sets and writing a driver for a device like the ZX Printer.
;
; The final 1K or RAM has four addresses although it is normal to use addresses
; $3C00 - $3FFF. The first sixty three bytes are the System Variables which
; hold information like the number BASE and CONTEXT, and even the plotting
; coordinates should the user wish to develop a word like DRAW to draw lines.
;
; Then comes the User Dictionary, the first word of which is "FORTH" which links
; to the Dictionary in ROM. Next a gap of 12 bytes to allow for Data Stack
; underflow and then the Data Stack itself which grows upwards.
; At the opposite end of free memory is the Return Stack (machine stack) which
; grows downwards.

; ------------------------------------
; THE 'INITIALIZATION ROUTINE' Part 3.
; ------------------------------------

L003B:  LD      DE,$3C24                ; destination system variable L_HALF
        LD      BC,$002D                ; number of bytes.
        LDIR                            ; copy initial state from ROM to RAM.

        LD      IX,$3C00                ; set IX to index the system variables.
        LD      IY,L04C8                ; set IY to the SLOW return address.

L004B:  CALL    L0A24                   ; routine CLS.

        XOR     A                       ; clear accumulator.

        LD      ($2700),A               ; make location after screen zero.

; There are 128 bit-mapped 8x8 characters.
; Define the 8 Battenberg graphics ($10 to $17) from low byte of address.
; This routine also sets the other characters $00 to $0F and $18 to $1F
; to copies of this range. The inverse form of character $17 is used as the
; normal cursor - character $97.

L0052:  LD      HL,$2C00                ; point to the start of the 1K write-
                                        ; only Character Set RAM.

L0055:  LD      A,L                     ; set A to low byte of address
        AND     $BF                     ; AND %10111111
        RRCA                            ; rotate
        RRCA                            ; three times
        RRCA                            ; to test bit 2
        JR      NC,L005F                ; forward if not set.

        RRCA                            ; else rotate
        RRCA                            ; twice more.

L005F:  RRCA                            ; set carry from bit (3) or (6)

        LD      B,A

        SBC     A,A                     ; $00 or $FF
        RR      B
        LD      B,A
        SBC     A,A
        XOR     B
        AND     $F0
        XOR     B
        LD      (HL),A                  ; insert the byte.
        INC     L                       ; increment low byte of address
        JR      NZ,L0055                ; loop back until the first 256 bytes
                                        ; have been filled with 32 repeating
                                        ; characters.

; Now copy the bit patterns at the end of this ROM to the last 768 bytes of
; the Character RAM, filling in some blank bytes omitted to save ROM space.
; This process starts at high memory and works downwards.

L006E:  LD      DE,$2FFF                ; top of destination.
        LD      HL,L1FFB                ; end of copyright character.
        LD      BC,$0008                ; 8 characters

        LDDR                            ; copy the  Â©  character

        EX      DE,HL                   ; switch pointers.

        LD      A,$5F                   ; set character counter to ninety five.
                                        ; i.e. %0101 1111
                                        ; bit 5 shows which 32-character sector
                                        ; we are in.

; enter a loop for the remaining characters supplying zero bytes as required.

L007C:  LD      C,$07                   ; set byte counter to seven.

        BIT     5,A                     ; test bit 5 of the counter.
        JR      Z,L0085                 ; forward if not in middle section
                                        ; which includes "[A-Z]"

        LD      (HL),B                  ; else insert a zero byte.
        DEC     HL                      ; decrement the destination address.
        DEC     C                       ; and the byte counter.

L0085:  EX      DE,HL                   ; switch pointers.

        LDDR                            ; copy the 5 or 6 characters.

        EX      DE,HL                   ; switch pointers.

        LD      (HL),B                  ; always insert the blank top byte.
        DEC     HL                      ; decrement the address.

        DEC     A                       ; decrement the character counter.

        JR      NZ,L007C                ; back for all 95 characters.

        IM      1                       ; Select Interrupt Mode 1

        JR      L009B                   ; and then jump into the code for the
                                        ; QUIT word.


; ---------------
; THE 'QUIT' WORD
; ---------------
; (  --  )
; Clears return stack, empties input buffer and returns control to the
; keyboard.

L0092:  DEFM    "QUI"                   ; 'name field'
        DEFB    'T' + $80

L0096:  DEFW    $0000                   ; 'link field' - end of linked list.

L0098:  DEFB    $04                     ; 'name length field'

L0099:  DEFW    L009B                   ; 'code field'
                                        ; address of machine code for routine.

; ---

L009B:  LD      SP,($3C18)              ; set stack-pointer to RAMTOP.

        EI                              ; Enable Interrupts.

        JP      L04F2                   ; jump forward to the main execution
                                        ; loop.

; ----------------
; THE 'ABORT' WORD
; ----------------
; Clears the data and return stacks, deletes any incomplete definition
; left in the dictionary, prints 'ERROR' and the byte from address $3C3D
; if the byte is non-negative, empties the input buffer, and returns
; control to the keyboard.


L00A3:  DEFM    "ABOR"                  ; 'name field'
        DEFB    'T' + $80

        DEFW    L0098                   ; 'link field' to previous word QUIT.

L00AA:  DEFB    $05                     ; 'name length field'

L00AB:  DEFW    L00AD                   ; 'code field'

; ---

; -> also continuation of the error restart.

L00AD:  PUSH    IY                      ; preserve current IY value slow/fast.

        LD      IY,L04B9                ; set IY to FAST
                                        ; now empty the data stack
        LD      HL,($3C37)              ; STKBOT
        LD      ($3C3B),HL              ; SPARE
        LD      HL,$3C3E                ; address FLAGS
        LD      A,(HL)                  ; fetch status from FLAGS.
        AND     $B3                     ; AND %10110011
                                        ; reset bit 2 - show definition complete
                                        ; reset bit 3 - output to screen.
                                        ; reset bit 6 - show in interpreter mode
        BIT     2,(HL)                  ; was there an incomplete definition ?
        LD      (HL),A                  ; update FLAGS
        JR      Z,L00DE                 ; forward if no incomplete word.

L00C4:  CALL    L04B9                   ; do forth

        DEFW    L0490                   ; dict          address of sv DICT
        DEFW    L08B3                   ; @             value of sv DICT (d).
        DEFW    L104B                   ; stk_data      d.         length field
        DEFB    $05                     ; five          d, 5.
        DEFW    L0DD2                   ; +             d+5.       code field
        DEFW    L086B                   ; dup           d+5, d+5.
        DEFW    L1610                   ; prvcur        d+5.
        DEFW    L15B5                   ; namefield     n.
        DEFW    L1011                   ; stackwrd      n.
        DEFW    $3C37                   ; (stkbot)      n, stkbot.
        DEFW    L08C1                   ; !             .
        DEFW    L1A0E                   ; end-forth.    .

; at this stage the system variable STKBOT holds the address of the
; obsolete name field and the system variable CURRENT points to the
; address of the previous complete word - obtained from the old link field.

L00DE:  BIT     7,(IX+$3D)              ; test ERR_NO for normal value 255.
        JR      NZ,L00FF                ; set-min then main-loop if OK.

        CALL    L1808                   ; else pr-inline

; ---

L00E7:  DEFM    "ERRO"                  ; the message "ERROR" with the last
        DEFB    'R' + $80               ; character inverted.

; ---

L00EC:  CALL    L04B9                   ; forth

        DEFW    L1011                   ; stack next word
        DEFW    $3C3D                   ; -> system variable ERR_NO
        DEFW    L0896                   ; C@            - fetch content byte
        DEFW    L09B3                   ; .             - print it
        DEFW    L0A95                   ; CR
        DEFW    L1A0E                   ; end-forth.

        LD      (IX+$3D),$FF            ; set ERR_NO to 'No Error'

L00FF:  LD      HL,($3C37)              ; fetch STKBOT
        LD      BC,$000C                ; allow twelve bytes for stack underflow
        ADD     HL,BC                   ; add the extra
        LD      ($3C3B),HL              ; set SPARE
        POP     IY                      ; restore previous state of IY

        JR      L009B                   ; rejoin main loop

; -------------------------
; THE 'DEFAULT ENVIRONMENT'
; -------------------------
; This is the default environment that is copied from ROM to RAM as part of
; the initialization process. This also contains the FORTH word FORTH definition

L010D:  DEFW    $26E0                   ; L_HALF

        DEFB    $00                     ; KEYCOD
        DEFB    $00                     ; KEYCNT copy the 32 bytes.
        DEFB    $00                     ; STATIN
        DEFW    $0000                   ; EXWRCH
        DEFB    $00                     ; FRAMES
        DEFB    $00                     ; FRAMES
        DEFB    $00                     ; FRAMES
        DEFB    $00                     ; FRAMES
        DEFB    $00                     ; XCOORD
        DEFB    $00                     ; YCOORD
        DEFW    $3C4C                   ; CURRENT
        DEFW    $3C4C                   ; CONTEXT
        DEFW    $3C4F                   ; VOCLNK
        DEFW    $3C51                   ; STKBOT
        DEFW    $3C45                   ; DICT
        DEFW    $3C5D                   ; SPARE
        DEFB    $FF                     ; ERR_NO
        DEFB    $00                     ; FLAGS
        DEFB    $0A                     ; BASE

; FORTH

        DEFM    "FORT"                  ; The 'name field'
        DEFB    'H' + $80               ; FORTH


        DEFW    $0000                   ; length field - filled when next word
                                        ; is defined.
        DEFW    L1FFF                   ; link field copied to $3C49.
        DEFB    $05                     ; name length field
        DEFW    L11B5                   ; code field
        DEFW    $3C49                   ; address of parameters
        DEFB    $00                     ; VOCLNK                        [$3C4F]
        DEFB    $00                     ; - link to next vocabulary.
        DEFB    $00                     ; last byte to be copied.    to [$3C51]

; -----------------------------------------------
; THE 'CONTINUATION OF THE Z80 INTERRUPT' ROUTINE
; -----------------------------------------------
; The destination of the jump at $0038.
; Begin by saving both accumulators and the 3 main registers.

L013A:  PUSH    AF                      ; preserve both accumulators
        EX      AF,AF'                  ;
        PUSH    AF                      ;

        PUSH    BC                      ; and main registers.
        PUSH    DE                      ;
        PUSH    HL                      ;

; Now wait for 62 * 12 clock cycles. ( To avoid flicker perhaps? ).

        LD      B,$3E                   ; delay counter.

L0142:  DJNZ    L0142                   ; self loop for delay

; Increment the 4-byte frames counter for use as a system clock.

        LD      HL,$3C2B                ; FRAMES1

L0147:  INC     (HL)                    ; increment timer.
        INC     HL                      ; next significant byte of four.
        JR      Z,L0147                 ; loop back if the value wrapped back
                                        ; to zero.

; Note. as manual points out, there is no actual check on this and if
; you leave your Ace switched on for 2.75 years it will advance to the
; following system variables although it takes several millennia to advance
; through the screen coordinates.

; Now read the keyboard and if no new key then exit after restoring the
; preserved registers.

        CALL    L0310                   ; routine KEYBOARD.

        LD      HL,$3C28                ; address system variable STATIN

        BIT     0,(HL)                  ; new key?
        JR      Z,L0176                 ; forward if not to RESTORE/EXIT

        AND     A                       ; zero key code ?
        JR      Z,L0176                 ; forward if so to EXIT.

        CP      $20                     ; compare to SPACE
        JR      C,L0170                 ; forward if less as an Editing Key.

        BIT     1,(HL)                  ; CAPS shift?
        CALL    NZ,L0807                ; routine TO_UPPER

        BIT     2,(HL)                  ; GRAPHICS mode?
        JR      Z,L0167                 ; skip forward if not

        AND     $9F                     ; convert to one of 8 mosaic characters

L0167:  BIT     3,(HL)                  ; INVERSE mode?
        JR      Z,L016D                 ; forward if not.

        OR      $80                     ; set bit 7 to make character inverse.

L016D:  CALL    L0196                   ; routine pr_buffer

L0170:  CALL    L01E6                   ; routine EDIT_KEY
        CALL    L0282                   ; routine pr_cursor

; Before exiting restore the preserved registers.

L0176:  POP     HL                      ;
        POP     DE                      ;
        POP     BC                      ;
        POP     AF                      ;
        EX      AF,AF'                  ;
        POP     AF                      ;

        EI                              ; Enable Interrupts

        RET                             ; return.

; -----------------------------------
; THE 'PRINT to LOWER SCREEN' ROUTINE
; -----------------------------------

L017E:  CP      $0D                     ; carriage return?
        JR      NZ,L0196                ; forward if not

; a carriage return to input buffer i.e. lower screen memory.

        LD      HL,$2700                ; set pointer to location after the
                                        ; input buffer.

        LD      ($3C22),HL              ; set ENDBUF - end of logical line
        LD      ($3C20),HL              ; set the CURSOR

        XOR     A                       ; clear A

        CALL    L0198                   ; print character zero.

        LD      HL,$26E0                ; left hand position of bottom line.
        LD      ($3C1E),HL              ; set INSCRN to this position.
        RET                             ; return.

; ---------------------------------------
; THE 'PRINT CHARACTER TO BUFFER' ROUTINE
; ---------------------------------------

L0196:  AND     A                       ; check for zero character
        RET     Z                       ; return if so.

; => also called from previous routine only to print a zero skipping above test.

L0198:  EX      AF,AF'                  ; preserve the output character.

        LD      HL,($3C22)              ; fetch ENDBUF end of logical line
        LD      A,(HL)                  ; fetch character from position
        AND     A                       ; is it zero ?
        JR      Z,L01A6                 ; skip forward if so.

; else lower screen scrolling is required.

        LD      DE,$D900                ; $0000 - $2700
        ADD     HL,DE                   ; test if position is within video RAM
        JR      NC,L01CE                ; forward if < $26FF

; now check that the limit of 22 lines in lower screen is not exceeded.

L01A6:  LD      DE,($3C24)              ; fetch start of buffer from L_HALF
        LD      HL,$DBA0                ; $0000 - $2460
        ADD     HL,DE                   ;
        JR      NC,L01E4                ; forward to exit if buffer full.


        LD      HL,($3C1C)              ; fetch position SCRPOS for upper screen
        LD      BC,$0020                ; allow an extra 32 characters - 1 line.
        ADD     HL,BC                   ;
        SBC     HL,DE                   ; subtract the start of input buffer
        PUSH    DE                      ; and save the L_HALF value

        CALL    NC,L0421                ; routine to scroll upper display.

        CALL    L02B0                   ; find zerobyte loc in HL

        POP     DE                      ; retrieve the L_HALF value

        CALL    L042F                   ; routine scroll and blank

; The four system variables INSCRN, CURSOR, ENDBUF and L_HALF are each
; reduced by 32 bytes a screen line.

        LD      HL,$3C1E                ; address INSCRN the left-hand location
                                        ; of the current input line.

        LD      B,$04                   ; four system variables to update

L01C9:  CALL    L0443                   ; routine SCR-PTRS

        DJNZ    L01C9                   ; repeat for all four pointers.

; ok to print

L01CE:  CALL    L0302                   ; routine find characters to EOL.

        LD      D,H                     ; HL is end of line
        LD      E,L                     ; transfer to DE register.
        INC     HL                      ; increment
        LD      ($3C22),HL              ; update ENDBUF
        DEC     HL                      ; decrement
        DEC     HL                      ; so HL = DE -1

        JR      Z,L01DD                 ; skip if BC zero.

        LDDR                            ; else move the characters.

L01DD:  EX      AF,AF'                  ; restore the output character.
        LD      (DE),A                  ; insert at screen position.
                                        ; (a zero if CR lower)
        INC     DE                      ; next character position
        LD      ($3C20),DE              ; update CURSOR

L01E4:  XOR     A                       ; ?
        RET                             ; return.

; -------------------------
; THE 'EDIT KEY' SUBROUTINE
; -------------------------

L01E6:  LD      HL,L01F0                ; address the EDIT KEYS table.

        LD      D,$00                   ; prepare to index by one byte.
        LD      E,A                     ; character code to E.
        ADD     HL,DE                   ; index into the table.

        LD      E,(HL)                  ; pick up required offset to the
                                        ; handling routine.

        ADD     HL,DE                   ; add to the current address.
        JP      (HL)                    ; exit via the routine.

; ---------------------
; THE 'EDIT KEYS' TABLE
; ---------------------

L01F0:  DEFB    $20             ; L0210         $00     - RET
L01F1:  DEFB    $13             ; L0204         $01     - LEFT
L01F2:  DEFB    $0C             ; L01FE         $02     - CAPS
L01F3:  DEFB    $1E             ; L0211         $03     - RIGHT
L01F4:  DEFB    $0A             ; L01FE         $04     - GRAPH
L01F5:  DEFB    $37             ; L022C         $05     - DEL
L01F6:  DEFB    $1A             ; L0210         $06     - RET
L01F7:  DEFB    $50             ; L0247         $07     - UP
L01F8:  DEFB    $06             ; L01FE         $08     - INV
L01F9:  DEFB    $9C             ; L0295         $09     - DOWN
L01FA:  DEFB    $C9             ; L02C3         $0A     - DEL LINE
L01FB:  DEFB    $15             ; L0210         $0B     - RET
L01FC:  DEFB    $14             ; L0210         $0C     - RET
L01FD:  DEFB    $D3             ; L02D0         $0D     - KEY-ENTER

; -------------------------------
; THE 'TOGGLE STATUS BIT' ROUTINE
; -------------------------------
; The keycodes have been cleverly mapped to individual bits of the STATIN
; system variable so this simple routine maintains all three status bits.
; KEY '2' - CAPS SHIFT, '4' - GRAPHICS, '8' - INVERSE VIDEO.

L01FE:  LD      HL,$3C28                ; system variable STATIN
        XOR     (HL)                    ; toggle the single relevant bit.
        LD      (HL),A                  ; put back.
        RET                             ; return.

; ----------------------------
; THE 'CURSOR LEFT' SUBROUTINE
; ----------------------------
; this subroutine moves the cursor to the left unless the character at that
; position is zero.

L0204:  LD      HL,($3C20)              ; fetch CURSOR.
        DEC     HL                      ; decrement value.
        LD      A,(HL)                  ; fetch character at new position.
        AND     A                       ; test for zero. (cr)
        RET     Z                       ; return if so.                  >>

        LD      ($3C20),HL              ; else update CURSOR
        INC     HL                      ; step back
        LD      (HL),A                  ; and put character that was at new
                                        ; cursor position where cursor is now.

L0210:  RET                             ; return.

; Note. various unallocated keys in the EDIT KEYS table point to the
; above RET instruction.

; -----------------------------
; THE 'CURSOR RIGHT' SUBROUTINE
; -----------------------------

L0211:  LD      HL,($3C20)              ; fetch CURSOR position
        INC     HL                      ; and increment it.

        LD      DE,($3C22)              ; fetch ENDBUF - end of current line.
        AND     A                       ; prepare to subtract.
        SBC     HL,DE                   ; test
        RET     Z                       ; return if zero - CURSOR is at ENDBUF

        ADD     HL,DE                   ; else reform the pointers.
        LD      ($3C20),HL              ; update CURSOR
        LD      A,(HL)                  ; fetch character at new position.
        DEC     HL                      ; decrement
        LD      (HL),A                  ; and insert where cursor was.
        RET                             ; ret.

; ---------------------------
; THE 'DELETE CURSOR' ROUTINE
; ---------------------------
; Moves cursor position to right and then continues into DEL-CHAR

L0225:  LD      HL,($3C20)              ; fetch CURSOR
        INC     HL                      ; increment position.
        LD      ($3C20),HL              ; update CURSOR


; ------------------------------
; THE 'DELETE CHARACTER' ROUTINE
; ------------------------------

L022C:  CALL    L0302                   ; routine finds characters to EOL.

        LD      H,D                     ; transfer CURSOR position DE to HL.
        LD      L,E                     ;
        DEC     DE                      ; decrement DE
        LD      A,(DE)                  ; fetch character to left of original
                                        ; cursor.
        AND     A                       ; test for zero.
        RET     Z                       ; return if so.                 >>

        LD      ($3C20),DE              ; else update CURSOR
        LD      A,B                     ; check for count of characters
        OR      C                       ; being zero
        JR      Z,L023F                 ; skip if so.

L023D:  LDIR                            ; else shift characters to left.

L023F:  DEC     HL                      ; decrement HL so that points to end -
                                        ; last position on the logical line.
        LD      (HL),$20                ; insert a space.
        LD      ($3C22),HL              ; set ENDBUF
        INC     C                       ; reset zero flag??
        RET                             ; return.

; -----------------------
; THE 'CURSOR UP' ROUTINE
; -----------------------
; When the cursor is moved up while editing a multi-line word definition,
; then the cursor is first moved to the left of the screen abutting the
; character zeros at the leftmost position.
; These zero characters appear as spaces but mark the beginning of each logical
; line. A logical line may, for instance if it contains a text item, extend over
; several physical screen lines.

L0247:  CALL    L0204                   ; routine CURSOR-LEFT
        JR      Z,L0254                 ; skip forward if not possible.

; else move left by thirty two positions. This may achieve a vertical move if
; attempted when a word is first being entered. Alternatively if one of the
; calls to cursor left fails having encountered a zero, then all subsequent
; calls will fail. The routine will return with the cursor adjacent to the zero.

        LD      B,$1F                   ; count 31 decimal
L024E:  CALL    L0204                   ; move cursor left thirty one times.
        DJNZ    L024E                   ; makes thirty two moves counting first

        RET                             ; return.

; ---

L0254:  LD      HL,($3C1E)              ; fetch INSCRN start of current line.
        LD      DE,($3C24)              ; fetch L_HALF start of buffer.
        AND     A                       ; reset carry for
        SBC     HL,DE                   ; true subtraction.
        RET     Z                       ; return if at beginning of input buffer

        CALL    L0225                   ; routine DEL-CURSOR

        LD      HL,($3C1E)              ; fetch INSCRN leftmost location of
                                        ; current line.
        LD      DE,$FFE0                ; make DE minus thirty two.
        XOR     A                       ; clear accumulator to zero.

L0269:  ADD     HL,DE                   ; subtract 32
        CP      (HL)                    ; compare contents to zero
                                        ; ( i.e. prev (cr) or buffer start?)
        JR      NZ,L0269                ; loop back until HL holds zero.

        LD      ($3C1E),HL              ; update INSCRN

        CALL    L02F4                   ; find endbuf

        LD      ($3C20),HL              ; set CURSOR

; ----------
; PR_CURSOR
; ----------

L0276:  LD      A,$A0                   ; inverse space - so solid square

        CALL    L017E                   ; routine PR_LOWER

        LD      HL,($3C20)              ; CURSOR
        DEC     HL
        LD      ($3C20),HL              ; CURSOR

; -> from interrupt
L0282:  LD      HL,($3C20)              ; CURSOR

        LD      A,($3C28)               ; STATIN
        RRA                             ; ignore bit 0
        LD      (HL),$97                ; pixel cursor.
        RRA                             ; test bit 1 - CAPS
        JR      NC,L0290                ; forward if no CAPS SHIFT

        LD      (HL),$C3                ; inverse [C] cursor.

L0290:  RRA                             ; test bit 2 - GRAPHICS.
        RET     NC                      ; return if not

L0292:  LD      (HL),$C7                ; inverse [G] cursor.
        RET                             ; return

; -------------------------
; THE 'CURSOR DOWN' ROUTINE
; -------------------------


L0295:  CALL    L0211                   ; routine CURSOR RIGHT
        JR      Z,L02A2                 ; forward if not possible.

        LD      B,$1F                   ; set counter to thirty one.

L029C:  CALL    L0211                   ; routine CURSOR RIGHT
        DJNZ    L029C                   ; thirty two moves altogether.
        RET                             ; return.

; ---

L02A2:  CALL    L02B0                   ; find zerobyte
        RET     PO                      ; return if    found

        PUSH    HL                      ; save position
        CALL    L0225                   ; routine DEL-CURSOR
        POP     HL                      ; retrieve position.
        CALL    L02ED                   ; set logical line
        JR      L0276                   ; back to exit via pr_cursor.

; ---
; find zerobyte
; ---
; -> called 5 times

L02B0:  LD      HL,$2700                ; this location is always zero.
                                        ; the byte following video RAM.
        LD      DE,($3C1E)              ; INSCRN        e.g. $26E0

        AND     A                       ; prepare for true subtraction

        SBC     HL,DE                   ; subtract to give number of chars

        LD      B,H                     ; transfer count to
        LD      C,L                     ; the BC register pair.

        EX      DE,HL                   ; transfer INSCR value to HL.

        INC     HL                      ; start next location
        XOR     A                       ; search for a zero character.

        CPIR                            ; at most BC locations.
                                        ; sets P/O flag if BC!=0

        DEC     HL                      ; step back to last non-zero
        RET                             ; return.

; -------------------------
; THE 'DELETE LINE' ROUTINE
; -------------------------
; CHR$ 10

L02C3:  LD      HL,($3C22)              ; ENDBUF
        DEC     HL                      ;
        LD      ($3C20),HL              ; CURSOR

L02CA:  CALL    L022C                   ; KEY-DEL
        JR      NZ,L02CA                ; repeat

        RET                             ; return.

; --------------------------
; THE 'KEY-ENTER' SUBROUTINE
; --------------------------

L02D0:  LD      HL,$3C28                ; STATIN
        SET     5,(HL)                  ; signal new key.
        RES     0,(HL)                  ; reset new key flag
        RET                             ; return.


; ------------------------
; THE 'SET BUFFER' ROUTINE
; ------------------------
; called by LIST, QUERY

L02D8:  LD      HL,$2700                ; one past end of screen.
        LD      DE,($3C24)              ; fetch start of buffer from L_HALF

        CALL    L07FA                   ; routine SPACE_FILL

        LD      HL,$26E0                ; first location of bottom line.
        LD      ($3C24),HL              ; set L_HALF

        LD      (HL),$00                ; insert a ZERO.

; -> called by retype
L02EA:  LD      HL,($3C24)              ; fetch L_HALF

; -> from cursor down
L02ED:  LD      ($3C1E),HL              ; set INSCRN
        INC     HL                      ; step past the zero
        LD      ($3C20),HL              ; set CURSOR

; => from cursor up.
L02F4:  CALL    L02B0                   ; find zerobyte

        LD      A,$20                   ; prepare a space

L02F9:  DEC     HL                      ; move to the left.
        CP      (HL)                    ; compare to space.
        JR      Z,L02F9                 ; back while spaces exist.

        INC     HL                      ; point to last space encountered.
        LD      ($3C22),HL              ; set ENDBUF - end of logical line.
        RET                             ; return.

; ----------------------------------
; THE 'COUNT TO END OF LINE' ROUTINE
; ----------------------------------
; Find the number of characters to the end of the logical line.

L0302:  LD      HL,($3C22)              ; system variable ENDBUF
        LD      DE,($3C20)              ; system variable CURSOR
        AND     A                       ; prepare to subtract.
        SBC     HL,DE                   ; subtract to give character places
        LD      B,H                     ; transfer result
        LD      C,L                     ; to the BC register pair.
        ADD     HL,DE                   ; reform the pointers.

        RET                             ; return with zero flag set if cursor
                                        ; at EOL.

; ----------------------
; THE 'KEYBOARD' ROUTINE
; ----------------------

L0310:  CALL    L0336                   ; routine KEY_SCAN

        LD      B,A                     ; save key in B

        LD      HL,($3C26)              ; load L with KEYCOD - last key pressed
                                        ; load H with KEYCNT - debounce counter

        XOR     L                       ; compare to previous key.
        JR      Z,L0325                 ; forward if a match.

        XOR     L                       ; reform original
        JR      Z,L0320                 ; forward if zero - no key.

        XOR     A                       ; else clear accumulator.

        CP      L                       ; compare with last.
        RET     NZ                      ; return if not zero.

L0320:  LD      L,B                     ; set L to original keycode
        LD      H,$20                   ; set counter to thirty two.
        JR      L0332                   ; forward to store values and exit
                                        ; returning zero.

; ---

; Key is same as previously accepted key.
; It repeats after two interrupts

L0325:  DEC     H                       ; decrement the counter.
        LD      A,H                     ; fetch counter to A.
        CP      $1E                     ; compare to thirty.
        JR      Z,L0331                 ; forward if so to return key in A.

        XOR     A                       ; clear accumulator.
        CP      H                       ; is counter zero?
        JR      NZ,L0332                ; forward if not to keep counting.

        LD      H,$04                   ; else set counter to four.

L0331:  LD      A,L                     ; pick up previous key.

L0332:  LD      ($3C26),HL              ;  update KEYCOD/KEYCNT

        RET                             ; return.

;----------------------------------------------------------------------------
;                          LOGICAL VIEW OF KEYBOARD
;
;         0     1     2     3     4 -Bits-  4     3     2     1     0
; PORT                                                                    PORT
;
; F7FE  [ 1 ] [ 2 ] [ 3 ] [ 4 ] [ 5 ]  |  [ 6 ] [ 7 ] [ 8 ] [ 9 ] [ 0 ]   EFFE
;  ^                                   |                                   v
; FBFE  [ Q ] [ W ] [ E ] [ R ] [ T ]  |  [ Y ] [ U ] [ I ] [ O ] [ P ]   DFFE
;  ^                                   |                                   v
; FDFE  [ A ] [ S ] [ D ] [ F ] [ G ]  |  [ H ] [ J ] [ K ] [ L ] [ ENT ] BFFE
;  ^                                   |                                   v
; FEFE  [SHI] [SYM] [ Z ] [ X ] [ C ]  |  [ V ] [ B ] [ N ] [ M ] [ SPC ] 7FFE
;  ^            v                                                ^         v
; Start         +------------>--------------------->-------------+        End
;
;
;----------------------------------------------------------------------------


; ----------------------------------
; THE 'KEYBOARD SCANNING' SUBROUTINE
; ----------------------------------
; This routine is called by the KEYBOARD routine 50 times a second and
; by the ACE FORTH 'INKEY' WORD.
; The above diagram shows the logical view of the Keyboard and PORTS.
; The physical view is similar except that the symbol shift key is to the
; left of the space key.


L0336:  LD      BC,$FEFE                ; port address - B is also an 8 counter

        IN      D,(C)                   ; read from port to D.
                                        ; when a key is pressed, the
                                        ; corresponding bit is reset.

        LD      E,D                     ; save in E

        SRL     D                       ; read the outer SHIFT key.

        SBC     A,A                     ; $00 if SHIFT else $FF.
        AND     $D8                     ; $00 if SHIFT else $D8.

        SRL     D                       ; read the symbol shift bit
        JR      C,L0347                 ; skip if not pressed.

        LD      A,$28                   ; load A with 40 decimal.

L0347:  ADD     A,$57                   ; gives $7F SYM, $57 SHIFT, or $2F

; Since 8 will be subtracted from the initial key value there are three
; distinct ranges 0 - 39, 40 - 79, 80 - 119.

        LD      L,A                     ; save key range value in L
        LD      A,E                     ; fetch the original port reading.
        OR      $03                     ; cancel the two shift bits.

        LD      E,$FF                   ; set a flag to detect multiple keys.

; KEY_LINE the half-row loop.

L034F:  CPL                             ; complement bits

        AND     $1F                     ; mask off the rightmost five key bits.
        LD      D,A                     ; save a copy in D.
        JR      Z,L0362                 ; forward if no keys pressed to do the
                                        ; next row.

        LD      A,L                     ; else fetch the key value
        INC     E                       ; test E for $FF
        JR      NZ,L036B                ; forward if not now zero to quit

L0359:  SUB     $08                     ; subtract 8 from key value

        SRL     D                       ; test next bit affecting zero and carry

        JR      NC,L0359                ; loop back until the set bit is found.

        LD      E,A                     ; transfer key value to E.
        JR      NZ,L036B                ; forward to abort if more than one key
                                        ; is pressed in the row.

L0362:  DEC     L                       ; decrement the key value for next row.

        RLC     B                       ; rotate the 8 counter and port address

        JR      NC,L036D                ; skip forward when all 8 rows have
                                        ; been read.

        IN      A,(C)                   ; else read the next half-row.
        JR      L034F                   ; and back to KEY_LINE.

; ---
; ABORTKEY

L036B:  LD      E,$FF                   ; signal invalid key.

; the normal exit checks if E holds a key and not $FF.

L036D:  LD      A,E                     ; fetch possible key value.
        INC     A                       ; increment
        RET     Z                       ; return if was $FF as original.

        LD      HL,L0376                ; else address KEY TABLE
        ADD     HL,DE                   ; index into table.
                                        ; (D is zero)

        LD      A,(HL)                  ; pick up character.

        RET                             ; return with translated character.



; ---------------
; THE 'KEY TABLE'
; ---------------

; -----------------------
; THE '40 UNSHIFTED KEYS'
; -----------------------

L0376:  DEFB    $76                     ; V - v
        DEFB    $68                     ; H - h
        DEFB    $79                     ; Y - y
        DEFB    $36                     ; 6 - 6
        DEFB    $35                     ; 5 - 5
        DEFB    $74                     ; T - t
        DEFB    $67                     ; G - g
        DEFB    $63                     ; C - c
        DEFB    $62                     ; B - b
        DEFB    $6A                     ; J - j
        DEFB    $75                     ; U - u
        DEFB    $37                     ; 7 - 7
        DEFB    $34                     ; 4 - 4
        DEFB    $72                     ; R - r
        DEFB    $66                     ; F - f
        DEFB    $78                     ; X - x
        DEFB    $6E                     ; N - n
        DEFB    $6B                     ; K - k
        DEFB    $69                     ; I - i
        DEFB    $38                     ; 8 - 8
        DEFB    $33                     ; 3 - 3
        DEFB    $65                     ; E - e
        DEFB    $64                     ; D - d
        DEFB    $7A                     ; Z - z
        DEFB    $6D                     ; M - m
        DEFB    $6C                     ; L - l
        DEFB    $6F                     ; O - o
        DEFB    $39                     ; 9 - 9
        DEFB    $32                     ; 2 - 2
        DEFB    $77                     ; W - w
        DEFB    $73                     ; S - s
        DEFB    $00                     ; SYMBOL
        DEFB    $20                     ; SPACE
        DEFB    $0D                     ; ENTER
        DEFB    $70                     ; P - p
        DEFB    $30                     ; 0 - 0
        DEFB    $31                     ; 1 - 1
        DEFB    $71                     ; Q - q
        DEFB    $61                     ; A - a
        DEFB    $00                     ; SHIFT

; ---------------------
; THE '40 SHIFTED KEYS'
; ---------------------

        DEFB    $56                     ; V - V
        DEFB    $48                     ; H - H
        DEFB    $59                     ; Y - Y
        DEFB    $07                     ; 6 - 7 KEY-UP
        DEFB    $01                     ; 5 - 1 KEY-LEFT
        DEFB    $54                     ;
        DEFB    $47
        DEFB    $43
        DEFB    $42
        DEFB    $4A
        DEFB    $55
        DEFB    $09                     ; 7 - 9 KEY-DOWN
        DEFB    $08                     ; 4 - 8 INV-VIDEO
        DEFB    $52
        DEFB    $46
        DEFB    $58
        DEFB    $4E
        DEFB    $4B
        DEFB    $49
        DEFB    $03                     ; 8 - 3 KEY-RIGHT
        DEFB    $33                     ; 3 - 3
        DEFB    $45
        DEFB    $44
        DEFB    $5A
        DEFB    $4D
        DEFB    $4C
        DEFB    $4F
        DEFB    $04                     ; 9 - 4 GRAPH
        DEFB    $02                     ; 2 - 2 CAPS LOCK
        DEFB    $57                     ; W - W
        DEFB    $53                     ; S - S
        DEFB    $00                     ; SYMB
        DEFB    $20                     ; SPACE
        DEFB    $0D                     ; ENTER
        DEFB    $50                     ; P - P
        DEFB    $05                     ; 0 - 5   DEL
        DEFB    $0A                     ; 1 - 0A  DEL_LINE
        DEFB    $51                     ; Q - Q
        DEFB    $41                     ; A - A
        DEFB    $00                     ; SHIFT

; --------------------------
; THE '40 SYMBOL SHIFT KEYS'
; --------------------------

        DEFB    $2F                     ; V - /
        DEFB    $5E                     ; H - ^
        DEFB    $5B                     ; Y - [
        DEFB    $26                     ; 6 - &
        DEFB    $25                     ; 5 - %
        DEFB    $3E                     ; T - >
        DEFB    $7D                     ;
        DEFB    $3F
        DEFB    $2A
        DEFB    $2D
        DEFB    $5D
        DEFB    $27
        DEFB    $24
        DEFB    $3C
        DEFB    $7B
        DEFB    $60
        DEFB    $2C
        DEFB    $2B
        DEFB    $7F
        DEFB    $28
        DEFB    $23
        DEFB    $45
        DEFB    $5C
        DEFB    $3A
        DEFB    $2E
        DEFB    $3D
        DEFB    $3B
        DEFB    $29
        DEFB    $40                     ; 2 - @
        DEFB    $57                     ; W - W
        DEFB    $7C                     ; S
        DEFB    $00                     ; SYMB
        DEFB    $20                     ; SPACE
        DEFB    $0D                     ; ENTER
        DEFB    $22                     ; P - "
        DEFB    $5F                     ; 0 - _
        DEFB    $21                     ; 1 - !
        DEFB    $51                     ; Q - Q
        DEFB    $7E                     ; A - ~
        DEFB    $00                     ; SHIFT

; end of key tables


; ---------------------------
; THE 'PRINT ROUTINE' Part 2.
; ---------------------------
; If output is not directed into the input buffer then jump forward else
; call the routine to output to lower screen.

L03EE:  JR      Z,L03F5                 ; forward to main screen print.

        CALL    L017E                   ; PR_LOWER

        EXX                             ; restore main set
        RET                             ; return.                >>

; the print output is not directed to the input buffer but first check that
; the user has not set up a vector to their own routine to print characters
; for instance to a printer.

L03F5:  LD      B,A                     ; save the character in the B register.

        LD      HL,($3C29)              ; fetch possible vector from EXWRCH
                                        ; (normally 0)
        LD      A,H                     ; test for
        OR      L                       ; the value zero.
        LD      A,B                     ; fetch the character back to A.

        JR      Z,L03FF                 ; skip forward if no user-supplied
                                        ; routine.

L03FE:  JP      (HL)                    ; else jump to user-supplied routine
                                        ; which should finish with a JP (IY)**
                                        ; ** 2022 update This is an error in the
                                        ; listing **
                                        ; The character is provided in the A register
                                        ; of the Z80.
                                        ; The output routine should preserve the
                                        ; auxiliary registers, ix and iy, and
                                        ; finish off with exx and ret.
                                        ; see page  142 of manual.
; ---
; PRINTING TO UPPER SCREEN
; ---

L03FF:  LD      HL,($3C1C)              ; SCRPOS
        LD      DE,($3C24)              ; L_HALF

        EX      DE,HL                   ; ??

        SCF                             ; inclusive byte.
        SBC     HL,DE                   ; subtract screen position+1 from
                                        ; the start of input buffer.
        EX      DE,HL                   ; hl=scrpos

        CALL    C,L0421                 ; if no room then scroll upper display

        CP      $0D                     ; carriage return?

        JR      Z,L0416                 ; skip forward if so.

        LD      (HL),A                  ; else insert the character.

        INC     HL                      ; point to next position.
        JR      L041C                   ; forward

; ---

; a carriage return

L0416:  INC     HL                      ; increment screen address.
        LD      A,L                     ; fetch low byte of address and mask.
        AND     $1F                     ; a zero result indicates a line skip.
        JR      NZ,L0416                ; loop until a new line of 32 columns
                                        ; is started.

; both paths converge.

L041C:  LD      ($3C1C),HL              ; update SCRPOS

        EXX                             ; back to main set.

        RET                             ; return.

; -------------------------------------
; The 'UPPER DISPLAY SCROLLING' ROUTINE
; -------------------------------------

L0421:  PUSH    AF                      ; save character

        LD      HL,$3C1C                ; address the low order byte SCRPOS

        CALL    L0443                   ; routine cursor up
                                        ; i.e. SCRPOS = SCRPOS - 32

        POP     AF                      ; restore character

; now calculate the number of characters to scroll in the upper display.

        LD      HL,($3C24)              ; fetch L_HALF the start of input buffer
        LD      DE,$2420                ; second line in video display

;
; => scroll lower display enters here
L042F:  AND     A                       ; prepare for true subtraction.
        SBC     HL,DE                   ; find number of characters to scroll.

        LD      B,H                     ; result to BC
        LD      C,L

        LD      HL,$FFE0                ; set HL to -32d
        ADD     HL,DE                   ; now HL = DE -32d
        EX      DE,HL                   ; switch so DE = HL - 32

        LDIR                            ; scroll the lines up.

        LD      B,$20                   ; blank a line of 32 characters

L043D:  DEC     HL                      ; decrement screen address.
        LD      (HL),$20                ; insert a space character
        DJNZ    L043D                   ; and loop for all 32 characters

        RET                             ; return.

; --------------------------------
; THE 'SCREEN POINTERS' SUBROUTINE
; --------------------------------
;

L0443:  LD      A,(HL)                  ; fetch low byte of screen address
        SUB     $20                     ; subtract thirty two characters.
        LD      (HL),A                  ; and put back.

        INC     HL                      ; address high-order byte.
        JR      NC,L044B                ; forward if low byte did not wrap

        DEC     (HL)                    ; else decrement the high byte as the
                                        ; position has moved across a third of
                                        ; the display.

L044B:  INC     HL                      ; address following System Variable
        RET                             ; return.

; -----------------------------------
; THE 'INDEX SYSTEM VARIABLE' ROUTINE
; -----------------------------------
; This routine is used by words CONTEXT, CURRENT, BASE etc. to index and then
; stack a system variable associated with a FORTH word. See shortly.
;
; It is a bit overblown considering the eventual position of the System
; Variables and ld d,$3c; rst 10h; jp (iy) could have been used instead of
; the long-winded addition below.

L044D:  EX      DE,HL                   ; HL addresses the offset byte.
        LD      E,(HL)                  ; fetch to E register
;
        LD      D,$00                   ; prepare to add.
        LD      HL,$3C00                ; the address of start of SYSVARS
        ADD     HL,DE                   ; add the 8-bit offset
        EX      DE,HL                   ; location to DE.
        RST     10H                     ; push word DE

        JP      (IY)                    ; to 'next'.

; ---------------
; THE 'HERE' WORD
; ---------------
; ( -- address)
; Leaves the address of one past the end of the dictionary.

L0459:  DEFM    "HER"                   ; 'name field'
        DEFB    'E' + $80

        DEFW    L00AA                   ; 'link field'

L045F:  DEFB    $04                     ; 'name length field'

L0460:  DEFW    L0462                   ; 'code field'

; ---

L0462:  LD      DE,($3C37)              ; system variable STKBOT.
        RST     10H                     ; push word DE

        JP      (IY)                    ; to 'next'.

; ------------------
; THE 'CONTEXT' WORD
; ------------------
; (  -- 15411 )
; A system variable pointing to the context vocabulary.
; $3C33 CONTEXT

L0469:  DEFM    "CONTEX"                ; 'name field'
        DEFB    'T' + $80

        DEFW    L045F                   ; 'link field'

L0472:  DEFB    $07                     ; 'name length field'

L0473:  DEFW    L044D                   ; 'code field'

; ---

L0475:  DEFB    $33                     ; low byte of system variable.

; ------------------
; THE 'CURRENT' WORD
; ------------------
; (  -- 15409 )
; A system variable pointing to the current vocabulary.
; $3C31 CURRENT

L0476:  DEFM    "CURREN"                ; 'name field'
        DEFB    'T' + $80

        DEFW    L0472                   ; 'link field'

L047F:  DEFB    $07                     ; 'name length field'

L0480:  DEFW    L044D                   ; 'code field'

; ---

L0482:  DEFB    $31                     ; a single parameter low-byte of $3C31.

; ---------------
; THE 'BASE' WORD
; ---------------
; ( -- 15423)
; A one-byte variable containing the system number base.
; $3C3F BASE

L0483:  DEFM    "BAS"                   ; 'name field'
        DEFB    'E' + $80

        DEFW    L047F                   ; 'link field'

L0489:  DEFB    $04                     ; 'name length field'

L048A:  DEFW    L044D                   ; 'code field'

; ---

L048C:  DEFB    $3F                     ; low-byte of system variable BASE

; ---

; These two Internal Words are used to stack the value of FLAGS and DICT.

; -------------------------
; The 'flags' Internal Word
; -------------------------

L048D:  DEFW    L044D                   ; headerless 'code field'

; ---

L048F:  DEFB    $3E                     ; low-order byte of FLAGS $3C3E

; -------------------------
; The 'dict' Internal Word
; -------------------------

L0490:  DEFW    L044D                   ; headerless 'code field'

; ---

L0492:  DEFB    $39                     ; low-order byte of DICT $3C39


; --------------
; THE 'PAD' WORD
; --------------
; (  -- 9985 )
; Stacks the address of the 254-byte workpad.
; On most FORTH systems the PAD floats about in memory but on the Ace it is
; fixed in location and size. Its definition is simply a constant.

l0493   DEFM    "PA"                    ; 'name field'
        DEFB    'D' + $80

        DEFW    L0489                   ; 'link field'

L0498:  DEFB    $03                     ; 'name length field'

L0499:  DEFW    L0FF5                   ; 'code field' - stack word

; ---

L049B:  DEFW    $2701                   ; parameter is 9985 decimal -
                                        ; work pad address

; ------------
; THE ';' WORD
; ------------
; Terminates colon, DEFINER and COMPILER definitions.

L049D:  DEFB    ';' + $80               ; 'name field'

        DEFW    L0498                   ; 'link field'

L04A0:  DEFB    $41                     ; length 1 + $40 (immediate word)

L04A1:  DEFW    L1108                   ; 'code field' - compile

; ---

L04A3:  DEFW    L04B6                   ; exit

L04A5:  DEFW    L12D8                   ; check-for
        DEFB    $0A                     ; ten                   marker byte?
        DEFW    L1A0E                   ; end-forth.

; code gels

L04AA:  LD      HL,$3C3E                ; address FLAGS
        LD      A,(HL)                  ; fetch FLAGS value.

        AND     $BB                     ; AND %10111011
                                        ; reset bit 2 - show definition complete
                                        ; reset bit 6 - show in interpreter mode

        LD      (HL),A                  ; update FLAGS value.

        JP      (IY)                    ; to 'next'.

; ----
; Note. these backward links to the beginning of words will probably be less
; of a mystery when the syntax checking and listing modules are more fully
; explored. A value of $FFFF sometimes occurs.

x04b3   DEFB    $00                     ;;

x04b4   DEFB    $E8                     ;;
x04b5   DEFB    $FF                     ;; 04b5 + ffe8 = 049d  = ';'

; ----------------------------------
; THE 'ADDRESS' INTERPRETER ROUTINES
; ----------------------------------

; ------------------------
; The 'Exit' Internal Word
; ------------------------
; Drops the 'Next Word' pointer from the Return Stack thereby ending a
; subroutine and returning to next word in calling thread.

L04B6:  DEFW    L04B8                   ; headerless 'code field'

; ---

L04B8:  POP     HL                      ; discard the next word pointer.

; ------------------------------
; THE 'ADDRESS INTERPRETER' LOOP
; ------------------------------
; Sometimes known as the Sequencer.
;
; iy_fast

L04B9:  POP     HL                      ; word pointer.

; =====> from DOCOLON and BRANCH

L04BA:  LD      E,(HL)
        INC     HL
        LD      D,(HL)
        INC     HL

        PUSH    HL                      ; word pointer.

; ==>
;
L04BF:  EX      DE,HL
        LD      E,(HL)
        INC     HL
        LD      D,(HL)
        INC     HL
        EX      DE,HL

        JP      (HL)                    ; jump to machine code (4 clock cycles)
                                        ; which will terminate with a JP (IY)
                                        ; instruction (8 clock cycles).



; --------------------------------
; The 'Memory Check' Internal Word
; --------------------------------
; This internal word which also checks the BREAK key is only used from the
; start of the LINE definition. However the machine code entry point is the
; normal value of the IY register and so this code is executed at the end of
; every word.

L04C6:  DEFW    L04C8                   ; headerless 'code field'

; iy_slow

L04C8:  LD      BC,$000B                ; allow overhead of eleven bytes
        LD      DE,($3C3B)              ; SPARE
        LD      HL,($3C37)              ; STKBOT
        ADD     HL,BC                   ; add the overhead
        SBC     HL,DE                   ; subtract the SPARE value
        JR      C,L04D9                 ; forward if the original 12 byte gap
                                        ; remains.

; else stack underflow has occurred.

L04D7:  RST     20H                     ; Error 2
        DEFB    $02                     ; Data stack underflow.

; ---

L04D9:  LD      BC,$0000                ; allow no overhead.

        CALL    L0F8C                   ; check free memory
        CALL    L04E4                   ; check BREAK key.
        JR      L04B9                   ; back to iy_fast

; ------------------------------------
; THE 'CHECK FOR BREAK KEY' SUBROUTINE
; ------------------------------------
; Check for the key combination SHIFT/SPACE.

L04E4:  LD      A,$FE                   ; read port $FEFE -
        IN      A,($FE)                 ; keys SPACE, SYMSHIFT, M, N, B.

        RRA                             ; test bit for outermost key
        RET     C                       ; return if not pressed.

        LD      A,$7F                   ; read port $7FFE -
        IN      A,($FE)                 ; keys SHIFT, Z, X, C, V.

        RRA                             ; test bit for outermost key
        RET     C                       ; return if not pressed.

L04F0:  RST     20H                     ; Error 3.
        DEFB    $03                     ; BREAK pressed.

; -------------------------
; THE 'MAIN EXECUTION' LOOP
; -------------------------
; The final part of the QUIT definition, as in all FORTH implementations,
; just loops through two FORTH words.

; The first call - to the Address Interpreter - does not return.
; The return address is the next word QUERY which the interpreter pops off
; the Return Stack and then before executing puts the address of the next word
; on Return Stack. The default action of the Address Interpreter is to execute
; words in turn until some word, such as branch, alters this default behaviour.

L04F2:  CALL    L04B9                   ; forth.

L04F5:  DEFW    L058C                   ; QUERY         - input buffer
        DEFW    L0506                   ; LINE          - interpret buffer
        DEFW    L0536                   ; prOK          - print OK
        DEFW    L1276                   ; branch        - relative jump

L04FD:  DEFW    $FFF7                   ; back to L04F5

; ---
; the first high-level interpreted word.
; ---

; ---------------
; THE 'LINE' WORD
; ---------------
; Interprets input buffer as a normal FORTH line.

L04FF:  DEFM    "LIN"                   ; 'name field'
        DEFB    'E' + $80

        DEFW    L04A0                   ; 'link field'

L0505:  DEFB    $04                     ; 'name length field'

L0506:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0508:  DEFW    L04C6                   ; check mem each time through loop
                                        ; as dictionary could be expanding.

        DEFW    L063D                   ; FIND          - search the dictionary
        DEFW    L08EE                   ; ?DUP          - duplicate if found
        DEFW    L1283                   ; ?branch       - forward if not a
L0510:  DEFW    $0007                   ; to L0518      - word.

        DEFW    L054F                   ; test and stack??
        DEFW    L1276                   ; branch
L0516:  DEFW    $FFF1                   ; back to L0508

L0518:  DEFW    L06A9                   ; NUMBER
        DEFW    L08EE                   ; ?DUP
        DEFW    L1283                   ; ?branch       - forward if not a
L051E:  DEFW    $0007                   ; to L0526      - number.

        DEFW    L0564                   ; pop de with test
        DEFW    L1276                   ; branch
L0524:  DEFW    $FFE3                   ; loop back to L0508

L0526:  DEFW    L061B                   ; stack-length
        DEFW    L0C1A                   ; 0=
        DEFW    L1283                   ; ?branch       - forward with anything
L052C:  DEFW    $0003                   ; to L0530      - else

L052E:  DEFW    L04B6                   ; EXIT                          >>>

; ---

L0530:  DEFW    L0578                   ; RETYPE        - [?] at relevant place
        DEFW    L1276                   ; branch        - once corrected back
L0534:  DEFW    $FFD3                   ; to L0508      - to the loop.

; ----------------------------
; The 'Print OK' Internal Word
; ----------------------------
; prints the OK message after successful execution.

L0536:  DEFW    L0538                   ; headerless 'code field'

L0538:  LD      A,($3C3E)               ; fetch system variable FLAGS

        BIT     6,A                     ; test for 'COMPILER' mode.
        JR      NZ,L054D                ; forward if so.

        BIT     4,A                     ; test for 'INVIS' mode.
        JR      NZ,L054D                ; forward if so.

        CALL    L1808                   ; else print the inline string.

; ---

        DEFM    " OK"                   ; the OK message between two spaces.
        DEFB    ' ' + $80               ; last one inverted.

; ---

L054A:  LD      A,$0D                   ; prepare a carriage return.
        RST     08H                     ; and PRINT also.

L054D:  JP      (IY)                    ; to 'next'.

; ------------------------------
; The 'XXXXXXXXXX' Internal Word
; ------------------------------
; to handle a Word from LINE

L054F:  DEFW    L0551                   ; headerless 'code field'

; ---

L0551:  RST     18H                     ; pop address from Data Stack to DE

        DEC     DE                      ; point to the 'name length field'

        LD      A,(DE)                  ; fetch contents of the address.

        CPL                             ; complement.

        AND     (IX+$3E)                ; FLAGS

        AND     $40                     ; isolate BIT 6 of FLAGS, set if in
                                        ; compiler mode.

        INC     DE                      ; increment address to 'code field'

        JR      Z,L0561                 ; forward if not in compiling mode

        RST     10H                     ; push word DE          - add to dict
        LD      DE,L0F4E                ; ','                   - enclose

L0561:  JP      L04BF                   ; next word.

; -----------------------
; The '???' Internal Word
; -----------------------
; after handling a number from LINE

L0564:  DEFW    L0566                   ; headerless 'code field'

; ---

L0566:  RST     18H                     ; pop word DE

        BIT     6,(IX+$3E)              ; test FLAGS - compiler mode ?

        JR      NZ,L0561                ; loop back while in compiler mode.

        JP      (IY)                    ; to 'next'.

; -----------------
; THE 'RETYPE' WORD
; -----------------
; Allows user to edit the input line. Turns cursor to [?].

L056F:  DEFM    "RETYP"                 ; 'name field'
        DEFB    'E' + $80

        DEFW    L058B                   ; 'link field'

L0577:  DEFB    $06                     ; 'name length field'

L0578:  DEFW    L057A                   ; 'code field'

; ---

L057A:  CALL    L02EA                   ; routine sets logical line.

        CALL    L0276                   ; routine pr_cursor

        LD      (HL),$BF                ; the inverse [?] character

        JR      L0594                   ; forward to join the QUERY routine.

; ----------------
; THE 'QUERY' WORD
; ----------------
; Clears input buffer, then accepts characters until ENTER pressed.
; Buffer can be edited as usual and is limited to 22 lines.

L0584:  DEFM    "QUER"                  ; 'name field'
        DEFB    'Y' + $80

        DEFW    L0505                   ; 'link field'

L058B:  DEFB    $05                     ; 'name length field'

L058C:  DEFW    L058E                   ; 'code field'

; ---

L058E:  CALL    L02D8                   ; routine SETBUF

        CALL    L0276                   ; routine pr_cursor

; ->
L0594:  LD      HL,$3C28                ; fetch STATIN
        SET     0,(HL)                  ;
        RES     5,(HL)                  ; (bit 5 set by interrupt when the user
                                        ; presses the ENTER key)

L059B:  BIT     5,(HL)                  ; wait for interrupt to set the bit.
        JR      Z,L059B                 ; loop until.

        CALL    L0225                   ; routine DEL-CURSOR
        JP      (IY)                    ; to 'next'.

; ---------------
; THE 'WORD' WORD
; ---------------
; WORD text
; ( delimiter -- address )
; Takes text out of the input buffer up as far as a delimiter, and copies it
; to pad, starting at the second byte there. Puts the length (not including
; the delimiter) in the first byte of the pad, and stacks the address of the
; first byte of the pad.
; At most 253 characters are taken from the input buffer. If there are more
; left before the delimiter, then the first byte of the pad shows 254.
; Initial delimiters are ignored.

L05A4:  DEFM    "WOR"                   ; 'name field'
        DEFB    'D' + $80

        DEFW    L0577                   ; 'link field'

L05AA:  DEFB    $04                     ; 'name length field'

L05AB:  DEFW    L05AD                   ; 'code field'

; ---

L05AD:  RST     18H                     ; pop word DE
        LD      HL,$27FE                ; set HL to penultimate byte of 'pad'.
        LD      B,$FD                   ; the count is 253.

L05B3:  LD      (HL),$20                ; insert a space in pad.
        DEC     HL                      ; decrement the address.
        DJNZ    L05B3                   ; repeat for the 253 locations.

        PUSH    DE                      ; save the delimiter.
        EX      DE,HL                   ; save in HL also, DE is start of pad.

        RST     10H                     ; stack data word DE
        POP     DE                      ; retrieve the delimiter.

        CALL    L05E1                   ;

        INC     B
        DEC     B
        JR      Z,L05C6                 ;

        LD      BC,$00FF

L05C6:  LD      HL,$2701
        LD      (HL),C
        INC     HL
        LD      A,$FC
        CP      C
        JR      NC,L05D1                ;

        LD      C,A

L05D1:  INC     C
        PUSH    DE
        PUSH    BC
        EX      DE,HL
        LDIR
        POP     BC
        POP     DE
        DEC     C
        CALL    L07DA                   ;
        JP      (IY)                    ; to 'next'.

; --------------------------------
; THE 'GET BUFFER TEXT' SUBROUTINE
; --------------------------------
; Called from FIND, NUMBER and XXXXX. Word may have leading spaces and is
; terminated by a space or newline (zero).
; It is also used to find the end of a comment delimited by ')'.
;
; =>
L05DF:  LD      E,$20                   ; set a space as the skip character.

; =>called with E holding delimiter.
;
L05E1:  LD      HL,($3C24)              ; fetch L_HALF - start of screen buffer.
        LD      ($3C1E),HL              ; make INSCRN start of logical line the
                                        ; same.

        LD      BC,$0000                ; initialize letter count to zero.

; -> loop
L05EA:  INC     HL                      ; increment screen address.
        LD      A,(HL)                  ; fetch character to A.
        CP      E                       ; compare to character in E.
        JR      Z,L05EA                 ; loop while character matches.

        AND     A                       ; test for zero (at $2700?)
        JR      Z,L0600                 ; forward if so.

; a word has been found on the screen line.

        PUSH    HL                      ; save pointer to start of word.

L05F3:  INC     BC                      ; increment the letter count.
        INC     HL                      ; increment the screen pointer.

        LD      A,(HL)                  ; fetch new character
        AND     A                       ; test for zero.
        JR      Z,L05FC                 ; skip forward as at end of word.

        CP      E                       ; compare to the skip character.
        JR      NZ,L05F3                ; loop back if still within a word.

L05FC:  POP     DE                      ; retrieve pointer to start of word.

        XOR     A                       ;; clear A
        CP      B                       ;; compare to B zero

        RET                             ; return. with carry reset for success.

; ---

L0600:  PUSH    DE                      ; save delimiter

        CALL    L02B0                   ; routine find zerobyte
        JP      PO,L0614                ; jump if found to exit failure

        LD      DE,($3C24)              ; else set DE from L_HALF
        CALL    L07FA                   ; routine SPACE_FILL (DE-HL)
        LD      ($3C24),HL              ; set L_HALF to next line

        POP     DE                      ; restore delimiter

        JR      L05E1                   ; loop back using new line.

; ---

; branch here if a word not found.

L0614:  EX      DE,HL                   ; DE addresses cursor.
        POP     BC                      ; discard saved delimiter
        LD      BC,$0000                ; set BC, to zero
        SCF                             ; signal not found
        RET                             ; return.

; --------------------------------
; The 'stack length' Internal Word
; --------------------------------
; used once only from LINE to check for any extraneous text that is not a Word
; or a Number.

L061B:  DEFW    L061D                   ; headerless 'code field'

; ---

L061D:  CALL    L05DF                   ; get buffer

        LD      D,B                     ; transfer length of word
        LD      E,C                     ; from BC to DE
        RST     10H                     ; push word DE
        JP      (IY)                    ; to 'next'.


; ----------------
; THE 'VLIST' WORD
; ----------------
; List dictionary to screen, including words in ROM.
; (no pause after 18 lines)

L0625:  DEFM    "VLIS"                  ; 'name field'
        DEFB    'T' + $80

        DEFW    L05AA                   ; 'link field'

L062C:  DEFB    $05                     ; 'name length field'

L062D:  DEFW    L062F                   ; 'code field'

; ---

L062F:  LD      A,$0D                   ; prepare a newline

        RST     08H                     ; print it.

        LD      C,$00                   ; set a flag for 'do all names'.

        JR      L0644                   ; forward to FIND.


; ---------------
; THE 'FIND' WORD
; ---------------
; ( -- compilation address )
; Leaves compilation address of first word in input buffer, if defined in
; context vocabulary; else 0.

L0636:  DEFM    "FIN"                   ; 'name field'
        DEFB    'D' + $80

        DEFW    L062C                   ; 'link field'

L063C:  DEFB    $04                     ; 'name length field'

L063D:  DEFW    L063F                   ; 'code field'

; ---

L063F:  CALL    L05DF                   ; get buffer word, gets length in C.

        JR      C,L068A                 ; back if null to stack word zero

; ->

L0644:  LD      HL,($3C33)              ; fetch value of system variable CONTEXT
        LD      A,(HL)                  ; extract low byte of address.
        INC     HL                      ; increment pointer.
        LD      H,(HL)                  ; extract high byte of address.
        LD      L,A                     ; address now in HL.

; The address points to the 'name length field' of the most recent word in the
; Dictionary.


L064B:  LD      A,(HL)                  ; fetch addressed byte.
        AND     $3F                     ; discount bit 6, the immediate word
                                        ; indicator, to give length 1-31

        JR      Z,L067F                 ; a 'zero' length indicates this is a
                                        ; link like the example at the end of
                                        ; this ROM.

        XOR     C                       ; match against C.
        JR      Z,L0657                 ; skip forward if lengths match.

        LD      A,C                     ; test flag C
        AND     A                       ; for value zero.
        JR      NZ,L067F                ; forward if C not zero.

; else a name that matches the search length or all names are required - VLIST.


L0657:  PUSH    DE                      ; preserve DE
        PUSH    HL                      ; preserve 'name length field' pointer.

        CALL    L15E8                   ; routine WORDSTART finds start of name.
                                        ; A is returned as zero.

        OR      C                       ; test C for zero
        JR      Z,L0676                 ; branch forward to print if in VLIST.

; else the search is for a specific word and a word with same length, at least,
; has been found.

        LD      B,C                     ; copy the length to counter B.

L0660:  LD      A,(DE)                  ; fetch first letter of match word.

        CALL    L0807                   ; routine UPPERCASE

        INC     DE                      ; update pointer (in lower screen)
        XOR     (HL)                    ; match against letter (in dictionary).
        AND     $7F                     ; disregard any inverted bit.
        INC     HL                      ; increment dictionary pointer.

        JR      NZ,L067D                ; exit loop to try next link if no match

        DJNZ    L0660                   ; else loop back for all letters.

; Oh Frabjous day - a match.

        POP     DE                      ; pop 'name length field' pointer.
        INC     DE                      ; increment to point to compilation
                                        ; address.
        RST     10H                     ; stack date word DE.

; the remaining task is to clean up the input buffer in the lower screen.

        POP     DE                      ; pop the DE - screen pointer.

        CALL    L07DA                   ; clean up - backfill with spaces.

        JP      (IY)                    ; to 'next'.

; -----------------------
; THE 'PRINT NAME' BRANCH
; -----------------------
; This branch is taken from the above loop when all found words are to be
; printed by VLIST. It takes its time as if the user has expanded the
; dictionary then the list will scroll off the top of the screen. By waiting
; for an interrupt each time, it ensures that a standard listing takes about
; three seconds and there is ample opportunity to press BREAK to stop at a
; certain point.

L0676:  CALL    L17FB                   ; routine print string and space

        HALT                            ; wait for an interrupt.

        CALL    L04E4                   ; routine checks BREAK key.

L067D:  POP     HL                      ; restore 'name length field' pointer
        POP     DE                      ; restore DE

L067F:  DEC     HL                      ; point to high byte of 'link field'
        LD      A,(HL)                  ; hold it in A.
        DEC     HL                      ; point to low byte of 'link field'
        LD      L,(HL)                  ; transfer address of the new
        LD      H,A                     ; 'name length field' to HL pointer.

        OR      L                       ; test if address is zero - for the
                                        ; last entry in the linked list.

        JR      NZ,L064B                ; loop back while this is not the
                                        ; last entry in the vocabulary.

L0687:  DEFB    $C3                     ; A JP instruction i.e. JP L068A

; Note. The intention is to jump past the headerless code word for the internal
; word stk_zero. Since the word that would follow the first byte of the jump
; instruction would be identical to the word it is jumping over then the word
; can be omitted. Only saves one byte but this is back in 1983.

; ----------------------------
; The 'stk-zero' Internal Word
; ----------------------------
; (  -- 0 )

L0688:  DEFW    L068A                   ; headerless 'code field'

; ---

L068A:  LD      DE,$0000                ; load DE with the value zero.
        RST     10H                     ; stack Data Word DE

        JP      (IY)                    ; to 'next'.

; ------------------
; THE 'EXECUTE' WORD
; ------------------
; ( compilation address --  )
; Executes the word with the given compilation address.

L0690:  DEFM    "EXECUT"                ; 'name field'
        DEFB    'E' + $80

        DEFW    L063C                   ; 'link field'

L0699:  DEFB    $07                     ; 'name length field'

L069A:  DEFW    L069C                   ; 'code field'

; ---

L069C:  RST     18H

        JP      L04BF                   ;

; -----------------
; THE 'NUMBER' WORD
; -----------------
; Takes a number from the start of the input buffer. Leaves the number and
; a non-zero address on the stack. (The address is the compilation address
; of a literal compiler, so that if you then say EXECUTE, the literal compiler
; compiles the number into the dictionary as a literal - for an integer it
; is 4102, for a floating point number it is 4181).
; If no valid number then leaves just 0 on the stack.

L06A0:  DEFM    "NUMBE"                 ; 'name field'
        DEFB    'R' + $80

        DEFW    L0699                   ; 'link field'

L06A8:  DEFB    $06                     ; 'name length field'

L06A9:  DEFW    L06AB                   ; 'code field'

; ---

L06AB:  CALL    L05DF                   ; get buffer

        JR      C,L068A                 ; if empty stack word zero.

        PUSH    BC
        PUSH    DE

        CALL    L074C                   ;

        JR      NZ,L06BC                ;

        LD      DE,$1006                ; addr literal?
        JR      L0714                   ;

; ---

L06BC:  RST     18H                     ; pop word DE
        LD      DE,$0000
        RST     10H                     ; push word DE
        LD      DE,$4500
        POP     BC
        PUSH    BC
        LD      A,(BC)
        CP      $2D                     ; is it '-' ?
        JR      NZ,L06CE                ;

        LD      D,$C5
        INC     BC
L06CE:  RST     10H                     ; push word DE
        LD      D,B
        LD      E,C
        DEC     HL
        DEC     HL

L06D3:  CALL    L0723                   ; routine GET_DECIMAL

        INC     HL
        INC     (HL)
        DEC     HL
        JR      NC,L06D3                ;

        CP      $FE
        JR      NZ,L071C                ;

L06DF:  CALL    L0723                   ; routine GET_DECIMAL

        JR      NC,L06DF                ;

        ADD     A,$30                   ; add '0' converting to letter.
        CALL    L077B                   ;
        JR      NZ,L06EF                ;

        LD      E,$00
        JR      L06FD                   ;

L06EF:  AND     $DF                     ;

        CP      $45                     ; is it 'E' - extended format?
        JR      NZ,L071C                ;

        PUSH    HL

        CALL    L074C                   ;

        RST     18H                     ; pop word DE
        POP     HL
        JR      NZ,L071C                ;

L06FD:  CALL    L0740                   ;
        JR      Z,L0711                 ;

        INC     HL
        LD      A,(HL)
        AND     $7F
        ADD     A,E

        JP      M,L071C                 ; forward +->

        JR      Z,L071C                 ; forward +->

        XOR     (HL)
        AND     $7F
        XOR     (HL)
        LD      (HL),A
L0711:  LD      DE,L1055                ; stk_fp
L0714:  RST     10H                     ; push word DE
        POP     DE
        POP     BC
        CALL    L07DA                   ;
        JP      (IY)                    ; to 'next'.

; ---

; +->
L071C:  POP     HL
        POP     HL
        RST     18H                     ; pop word DE
        RST     18H                     ; pop word DE
        JP      L068A                   ;

; ----------------------------
; THE 'GET DECIMAL' SUBROUTINE
; ----------------------------
; Fetch character and return with carry set if after conversion is not in
; range 0 to 9.

L0723:  LD      A,(DE)
        INC     DE
        SUB     $30                     ; subtract '0'
        RET     C                       ; return if was less than '0'

        CP      $0A                     ; compare to ten.
        CCF                             ; complement
        RET     C                       ; return - with carry set if over 9.

; ---------
; normalize?
; ---------
; => from below only.
L072C:  LD      C,A
        LD      A,(HL)
        AND     $F0
        RET     NZ

        LD      A,C

; => (int/print_fp)
L0732:  DEC     HL
        DEC     HL
        LD      C,$03

L0736:  RLD                             ;  A = xxxx3210  <--   7654<-3210 (HL)

        INC     HL                      ;
        DEC     C
        JR      NZ,L0736                ;

        DEC     (HL)                    ; decrement exponent
        DEC     HL                      ; point to start of BCD nibbles
        CP      A
        RET

; ---

; from ufloat to normalize 6-nibble mantissa

L0740:  LD      B,$06                   ; six nibbles

L0742:  XOR     A

        CALL    L072C                   ;

        RET     NZ

        DJNZ    L0742                   ;

        INC     HL
        LD      (HL),B

        RET

; ---------------------------
; THE 'GET NUMBER' SUBROUTINE
; ---------------------------
; can be called twice by the above code for the word 'NUMBER'.
; Once to get the first number encountered and sometimes, if in extended
; format, the exponent as well.

L074C:  RST     10H                     ; push word DE

        CALL    L04B9                   ; forth

L0750:  DEFW    L086B                   ; dup
        DEFW    L0896                   ; C@
        DEFW    L104B                   ; stk-data
        DEFB    $2D                     ;  chr '-'
        DEFW    L0C4A                   ; =
        DEFW    L086B                   ; dup
        DEFW    L0DA9                   ; negate
        DEFW    L08D2                   ; >R
        DEFW    L0DD2                   ; +
        DEFW    L0E1F                   ; 1-
        DEFW    L0688                   ; stk-zero
        DEFW    L0688                   ; stk-zero
        DEFW    L08FF                   ; rot
L0769:  DEFW    L078A                   ; convert
        DEFW    L08FF                   ; rot
        DEFW    L08DF                   ; R>
        DEFW    L0D94                   ; pos
        DEFW    L08FF                   ; rot
        DEFW    L0879                   ; drop
        DEFW    L0885                   ; swap
        DEFW    L1A0E                   ; end-forth.

L0779:  RST     18H                     ; pop word DE
        LD      A,(DE)

L077B:  CP      $20
        RET     Z

        AND     A
        RET

; ------------------
; THE 'CONVERT' WORD
; ------------------
; (  ud1, addr1 -- ud2, addr2  )
: Accumulates digits from text into an unsigned double length
; number ud1: for each digit, the double length accumulator is
; multiplied by the system number base and the digit (converted
; from ASCII) is added on. The text starts at addr1 + 1. addr2 is
; the address of the first unconvertible character, ud2 is the
; final value of the accumulator.

L0780:  DEFM    "CONVER"                ; 'name field'
        DEFB    'T' + $80

        DEFW    L06A8                   ; 'link field'

L0789:  DEFB    $07                     ; 'name length field'

L078A:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L078C:  DEFW    L0E09                   ; 1+
L078E:  DEFW    L086B                   ; dup
L0790:  DEFW    L08D2                   ; >R
L0792:  DEFW    L0896                   ; C@
L0794:  DEFW    L07B8                   ; stk_digit
L0796:  DEFW    L1283                   ; ?branch
L0798:  DEFW    $001B                   ; to 0799 + 1B = $07B4

L079A:  DEFW    L0885                   ; swap
L079C:  DEFW    L048A                   ; get base
L079E:  DEFW    L0896                   ; C@
L07A0:  DEFW    L0CA8                   ; u*
L07A2:  DEFW    L0879                   ; drop
L07A4:  DEFW    L08FF                   ; rot
L07A6:  DEFW    L048A                   ; get base
L07A8:  DEFW    L0896                   ; C@
L07AA:  DEFW    L0CA8                   ; U*
L07AC:  DEFW    L0DEE                   ; D+
L07AE:  DEFW    L08DF                   ; R>
L07B0:  DEFW    L1276                   ; branch
L07B2:  DEFW    $FFD9                   ; loop back to L078C

L07B4:  DEFW    L08DF                   ; R>
L07B6:  DEFW    L04B6                   ; exit

; -----------------------------
; The 'stk_digit' Internal Word
; -----------------------------

L07B8:  DEFW    L07BA                   ; headerless 'code field'

; ---

L07BA:  RST     18H                     ; pop word DE

        LD      A,E                     ; character to A

        CALL    L0807                   ; to_upper

        ADD     A,$D0                   ; add to give carry with '0' and more.

        JR      NC,L07D7                ; if less than '0' push byte 0 false.

        CP      $0A                     ; compare to ten.
        JR      C,L07CD                 ; forward to stack bytes 0 - 9.

        ADD     A,$EF                   ;
        JR      NC,L07D7                ; push word false 0.

        ADD     A,$0A

L07CD:  CP      (IX+$3F)                ; compare to BASE
        JR      NC,L07D7                ; push word false 0.

; else digit is within range of number base

        LD      D,$00
        LD      E,A
        RST     10H                     ; push word DE
        SCF                             ; set carry to signal true

L07D7:  JP      L0C21                   ; push word 1 or 0

; ---
;       ??
; ---

L07DA:  LD      H,D
        LD      L,E
        INC     BC
        ADD     HL,BC
        PUSH    HL
        BIT     4,(IX+$3E)              ; FLAGS
        CALL    Z,L097F                 ; pr_string

        CALL    L02B0                   ; curs?

        POP     DE
        AND     A
        SBC     HL,DE
        LD      B,H
        LD      C,L
        LD      HL,($3C1E)              ; INSCRN
        INC     HL
        EX      DE,HL
        JR      C,L07FB                 ;

        JR      Z,L07FA                 ; forward to SPACE_FILL.

        LDIR

; ------------------------
; The 'SPACE FILL' routine
; ------------------------
; -> from cls

L07FA:  AND     A                       ; prepare to subtract two screen
                                        ; pointers.

L07FB:  SBC     HL,DE                   ; number of bytes in HL.
        EX      DE,HL                   ; now in DE, HL = start of area.

L07FE:  LD      A,D                     ; check if the
        OR      E                       ; counter is zero.
        RET     Z                       ; return if so.                 >>

        LD      (HL),$20                ; insert a space character.
        INC     HL                      ; next address.
        DEC     DE                      ; decrement byte counter.
        JR      L07FE                   ; loop back to exit on zero.

; --------------------------
; THE 'UPPERCASE' SUBROUTINE
; --------------------------
; converts characters to uppercase.

L0807:  AND     $7F                     ; ignore inverse bit 7
        CP      $61                     ; compare to 'a'
        RET     C                       ; return if lower

        CP      $7B                     ; compare to 'z' + 1
        RET     NC                      ; return if higher than 'z'

        AND     $5F                     ; make uppercase
        RET                             ; return.

; --------------
; THE 'VIS' WORD
; --------------
; Allows copy-up mechanism and 'OK'.

L0812:  DEFM    "VI"                    ; 'name field'
        DEFB    'S' + $80

        DEFW    L0789                   ; 'link field'

L0817:  DEFB    $03                     ; 'name length field'

L0818:  DEFW    L081A                   ; 'code field'

; ---

L081A:  RES     4,(IX+$3E)              ; update FLAGS signal visible mode.
        JP      (IY)                    ; to 'next'.

; ----------------
; THE 'INVIS' WORD
; ----------------
; Suppresses copy-up mechanism and 'OK'.

L0820:  DEFM    "INVI"                  ; 'name field'
        DEFB    'S' + $80

        DEFW    L0817                   ; 'link field'

L0827:  DEFB    $05                     ; 'name length field'

L0828:  DEFW    L082A                   ; 'code field'

; ---

L082A:  SET     4,(IX+$3E)              ; update FLAGS signal invisible mode.

        JP      (IY)                    ; to 'next'.


; ---------------
; THE 'FAST' WORD
; ---------------
; Fast mode - runs without error checks.
; Debugged programs run 25% faster.

L0830:  DEFM    "FAS"                   ; 'name field'
        DEFB    'T' + $80

        DEFW    L0827                   ; 'link field'

L0836:  DEFB    $04                     ; 'name length field'

L0837:  DEFW    L0839                   ; 'code field'

; ---

L0839:  LD      IY,L04B9                ; miss memory checks on return

        JP      (IY)                    ; to 'next'.

; ---------------
; THE 'SLOW' WORD
; ---------------
; ( -- )
; Slow mode with error checking.
; Make IY point to a return routine that performs housekeeping.


L083F:  DEFM    "SLO"                   ; 'name field'
        DEFB    'W' + $80

        DEFW    L0836                   ; 'link field'

L0845:  DEFB    $04                     ; 'name length field'


L0846:  DEFW    L0848                   ; 'code field'

; ---

L0848:  LD      IY,L04C8                ; set vector to memory checks each pass

        JP      (IY)                    ; to 'next'.

; ---------------------------------
; THE 'DATA STACK TO BC' SUBROUTINE
; ---------------------------------
; Called on twenty occasions to fetch a word from the Data Stack into the
; BC register pair. Very similar to RST 18H which does the same thing with the
; DE register pair as the destination on 73 occasions.
; In fact, as two Z80 restarts are unused, then 40 bytes of ROM code could have
; been saved by making this a restart also.

L084E:  LD      HL,($3C3B)              ; fetch SPARE - start of Spare Memory.
        DEC     HL                      ; decrement to point to last stack item
        LD      B,(HL)                  ; load high byte to B.
        DEC     HL                      ; address low byte of word.
        LD      C,(HL)                  ; and load to C.
        LD      ($3C3B),HL              ; update the system variable SPARE to
                                        ; a location two bytes less than it was.
        RET                             ; return.

; -----------------------------------------
; THE 'CONTINUATION OF THE RST 18H' RESTART
; -----------------------------------------
; complete the operation of popping a word to DE from the data stack.

L0859:  DEC     HL                      ;
        LD      E,(HL)                  ;
        LD      ($3C3B),HL              ; update SPARE
        RET                             ; return.

; -----------------------------------------
; THE 'CONTINUATION OF THE RST 10H' RESTART
; -----------------------------------------
; complete the operation of pushing a word in DE to the data stack.

L085F:  LD      (HL),D                  ;
        INC     HL                      ;
        LD      ($3C3B),HL              ; update SPARE
        RET                             ; return.

; --------------
; THE 'DUP' WORD
; --------------
; ( n -- n, n )
; Duplicates the top of the stack.

L0865:  DEFM    "DU"                    ; 'name field'
        DEFB    'P' + $80

        DEFW    L0845                   ; 'link field'

L086A:  DEFB    $03                     ; 'name length field'

L086B:  DEFW    L086D                   ; 'code field'

; ---

L086D:  RST     18H                     ; unstack Data Word DE
        RST     10H                     ; stack Data Word DE
        RST     10H                     ; stack Data Word DE

        JP      (IY)                    ; to 'next'.

; ---------------
; THE 'DROP' WORD
; ---------------
; ( n -- )
; Throws away the top of the stack.

L0872:  DEFM    "DRO"                   ; 'name field'
        DEFB    'P' + $80

        DEFW    L086A                   ; 'link field'

L0878:  DEFB    $04                     ; 'name length field'

L0879:  DEFW    L087B                   ; 'code field'

; ---

L087B:  RST     18H                     ; unstack Data Word DE
        JP      (IY)                    ; to 'next'.

; ---------------
; THE 'SWAP' WORD
; ---------------
; (n1, n2 -- n2, n1)

L087E:  DEFM    "SWA"                   ; 'name field'
        DEFB    'P' + $80

        DEFW    L0878                   ; 'link field'

L0884:  DEFB    $04                     ; 'name length field'

L0885:  DEFW    L0887                   ; 'code field'

; ---

L0887:  RST     18H                     ; pop word DE
        CALL    L084E                   ; stk_to_bc
        RST     10H                     ; push word DE
        LD      D,B                     ;
        LD      E,C                     ;
        RST     10H                     ; push word DE

        JP      (IY)                    ; to 'next'.

; -------------
; THE 'C@' WORD
; -------------
; (address -- byte)
; Fetches the contents of a given address.

L0891:  DEFB    'C'                     ; 'name field'
        DEFB    '@' + $80

        DEFW    L0884                   ; 'link field'

L0895:  DEFB    $02                     ; 'name length field'

L0896:  DEFW    L0898                   ; 'code field'

; ---

L0898:  RST     18H                     ; pop word DE
        LD      A,(DE)
        LD      E,A
        LD      D,$00

        RST     10H                     ; push word DE

        JP      (IY)                    ; to 'next'.

; -------------
; THE 'C!' WORD
; -------------
; (n, address -- )
; Stores the less significant byte on n at a given address.

L08A0:  DEFB    'C'                     ; 'name field'
        DEFB    '!' + $80

        DEFW    L0895                   ; 'link field'

L08A4:  DEFB    $02                     ; 'name length field'

L08A5:  DEFW    L08A7                   ; 'code field'

; ---

L08A7:  RST     18H                     ; pop word DE
        CALL    L084E                   ; stk_to_bc
        LD      A,C
        LD      (DE),A

        JP      (IY)                    ; to 'next'.

; ------------
; THE '@' WORD
; ------------
; (address -- n)
; Leaves on stack the single length integer at the given address.

L08AF:  DEFB    '@' + $80               ; 'name field'

        DEFW    L08A4                   ; 'link field'

L08B2:  DEFB    $01                     ; 'name length field'

L08B3:  DEFW    L08B5                   ; 'code field'

; ---

L08B5:  RST     18H                     ; pop word DE

        EX      DE,HL
        LD      E,(HL)
        INC     HL
        LD      D,(HL)

        RST     10H                     ; push word DE

        JP      (IY)                    ; to 'next'.

; ------------
; THE '!' WORD
; ------------
; (n,address --)
; Stores the single-length integer n at the given address in memory.

L08BD:  DEFB    '!' + $80               ; 'name field'

        DEFW    L08B2                   ; 'link field'

L08C0:  DEFB    $01                     ; 'name length field'

L08C1:  DEFW    L08C3                   ; 'code field'

; ---

L08C3:  RST     18H                     ; pop word DE
        CALL    L084E                   ; stk_to_bc
        EX      DE,HL
        LD      (HL),C
        INC     HL
        LD      (HL),B

        JP      (IY)                    ; to 'next'.

; -------------
; THE '>R' WORD
; -------------
; (n -- )
; Transfers top entry on data stack to return stack.
; It can be copied back using 'I'.

L08CD:  DEFB    '>'                     ; 'name field'
        DEFB    'R' + $80

        DEFW    L08C0                   ; 'link field'

L08D1:  DEFB    $02                     ; 'name length field'

L08D2:  DEFW    L08D4                   ; 'code field'

; ---

L08D4:  RST     18H
        POP     BC
        PUSH    DE
        PUSH    BC
        JP      (IY)                    ; to 'next'.

; -------------
; THE 'R>' WORD
; -------------
; ( -- entry from return stack)
; Transfers top entry on return stack to data stack.

L08DA:  DEFB    'R'                     ; 'name field'
        DEFB    '>' + $80

        DEFW    L08D1                   ; 'link field'

L08DE:  DEFB    $02                     ; 'name length field'

L08DF:  DEFW    L08E1                   ; 'code field'

; ---

L08E1:  POP     BC
        POP     DE
        PUSH    BC
        RST     10H                     ; push word DE
        JP      (IY)                    ; to 'next'.

; ---------------
; THE '?DUP' WORD
; ---------------
; (n -- n, n)    if n!=0.
; (n -- n)       if n=0.

L08E7:  DEFM    "?DU"                   ; 'name field'
        DEFB    'P' + $80

        DEFW    L08DE                   ; 'link field'

L08ED:  DEFB    $04                     ; 'name length field'

L08EE:  DEFW    L08F0                   ; 'code field'

; ---


L08F0:  RST     18H                     ; fetch word DE
        RST     10H                     ; push it back
        LD      A,D                     ; test if fetched
        OR      E                       ; word is zero
        CALL    NZ,L0010                ; push word DE if non-zero
        JP      (IY)                    ; to 'next'.

; --------------
; THE 'ROT' WORD
; --------------
; (n1, n2, n3 -- n2, n3, n1)

L08F9:  DEFM    "RO"                    ; 'name field'
        DEFB    'T' + $80

        DEFW    L08ED                   ; 'link field'

L08FE:  DEFB    $03                     ; 'name length field'

L08FF:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0901:  DEFW    L08D2                   ; >R
L0903:  DEFW    L0885                   ; swap
L0905:  DEFW    L08DF                   ; R>
L0907:  DEFW    L0885                   ; swap
L0909:  DEFW    L04B6                   ; exit

; ---------------
; THE 'OVER' WORD
; ---------------
; (n1, n2 -- n1, n2, n1)

L090B:  DEFM    "OVE"                   ; 'name field'
        DEFB    'R' + $80

        DEFW    L08FE                   ; 'link field'

L0911:  DEFB    $04                     ; 'name length field'

L0912:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0914:  DEFW    L08D2                   ; >R
L0916:  DEFW    L086B                   ; dup
L0918:  DEFW    L08DF                   ; R>
L091A:  DEFW    L0885                   ; swap
L091C:  DEFW    L04B6                   ; exit

; ---------------
; THE 'PICK' WORD
; ---------------
; (n1 -- n2)
; Copies the n1-th stack entry (after dropping n1 itself) to the top.
; Error 7 if n1 <= 0.

L091E:  DEFM    "PIC"                   ; 'name field'
        DEFB    'K' + $80

        DEFW    L0911                   ; 'link field'

L0924:  DEFB    $04                     ; 'name length field'

        DEFW    L0927                   ; 'code field'

; ---

L0927:  CALL    L094D                   ;
        JP      (IY)                    ; to 'next'.

; ---------------
; THE 'ROLL' WORD
; ---------------
; (n -- )
; Extracts the nth stack value to the top of the stack, after dropping n
; itself, and moves the remaining values down to fill the vacated position.
; Error 7 if n <= 0.

L092C:  DEFM    "ROL"                   ; 'name field'
        DEFB    'L' + $80

        DEFW    L0924                   ; 'link field'

L0932:  DEFB    $04                     ; 'name length field'

L0933:  DEFW    L0935                   ; 'code field'

; ---

L0935:  CALL    L094D                   ;
        EX      DE,HL
        LD      HL,($3C37)              ; STKBOT
        SBC     HL,DE
        JP      NC,L04D7                ; jump back to Error 2

        LD      H,D
        LD      L,E
        INC     HL
        INC     HL
        LDIR
        LD      ($3C3B),DE              ; SPARE
        JP      (IY)                    ; to 'next'.

; ---

L094D:  CALL    L084E                   ; stk_to_bc
        DEC     BC
        SLA     C
        RL      B
        INC     BC
        INC     BC
        JR      NC,L095B                ; skip the error routine

        RST     20H                     ; Error 7
        DEFB    $07                     ; PICK or ROLL used with operand 0
                                        ; or negative

; ---

L095B:  LD      HL,($3C3B)              ; SPARE
        SBC     HL,BC
        PUSH    HL
        LD      E,(HL)
        INC     HL
        LD      D,(HL)
        RST     10H                     ; push word DE
        POP     HL
        RET

; ---------------
; THE 'TYPE' WORD
; ---------------
; (address, n -- )
; EMITs n characters from memory starting at the address.


L0967:  DEFM    "TYP"                   ; 'name field'
        DEFB    'E' + $80

        DEFW    L0932                   ; 'link field'

L096D:  DEFB    $04                     ; 'name length field'

L096E:  DEFW    L0970                   ; 'code field'

; ---

L0970:  CALL    L084E                   ; stk_to_bc
        RST     18H                     ; pop word DE
        CALL    L097F                   ; routine pr_string (below)

        JP      (IY)                    ; to 'next'.

; --------------------------
; THE 'PRINT STRING' ROUTINE
; --------------------------
; The first entry point prints strings embedded in the Dictionary with the
; DE pointing to the preceding length word.
;
; The second entry point prints a string with length in BC and start in DE.
; It is called by TYPE above and to print comment fields.

; ->

L0979:  LD      A,(DE)
        LD      C,A
        INC     DE
        LD      A,(DE)
        LD      B,A
        INC     DE

; -->
L097F:  LD      A,B
        OR      C
        RET     Z

        LD      A,(DE)
        INC     DE
        DEC     BC
        RST     08H                     ; print_ch

        JR      L097F                   ;

; -------------
; THE '<#' WORD
; -------------
; (  --  )
; Initiates formatted output.

L0988:  DEFB    '<'                     ; 'name field'
        DEFB    '#' + $80

        DEFW    L096D                   ; 'link field'

L098C:  DEFB    $02                     ; 'name length field'

L098D:  DEFW    L098F                   ; 'code field'

; ---

L098F:  LD      HL,$27FF                ; end of pad
        LD      ($3C1A),HL              ; update system variable HLD
        JP      (IY)                    ; to 'next'.

; -------------
; THE '#>' WORD
; -------------
; (ud -- address, n)
; Finishes formatted output, leaving the address and length (n) of the
; resultant string.

L0997:  DEFB    '#'                     ; 'name field'
        DEFB    '>' + $80

        DEFW    L098C                   ; 'link field'

L099B:  DEFB    $02                     ; 'name length field'

L099C:  DEFW    L099E                   ; 'code field'

; ---

L099E:  RST     18H                     ; pop word DE
        RST     18H                     ; pop word DE
        LD      DE,($3C1A)              ; HLD
        RST     10H                     ; push word DE (address)
        LD      HL,$27FF                ; end of pad.
        AND     A                       ; prepare to subtract.
        SBC     HL,DE                   ; find length of string.
        EX      DE,HL                   ; transfer to DE
        RST     10H                     ; push word DE (n)

        JP      (IY)                    ; to 'next'.

; ------------
; THE '.' WORD
; ------------
;

L09AF:  DEFB    '.' + $80               ; 'name field'

        DEFW    L0A49                   ; 'link field'

L09B2:  DEFB    $01                     ; 'name length field'

L09B3:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L09B5:  DEFW    L098D                   ; <#
        DEFW    L086B                   ; dup
        DEFW    L0C0D                   ; abs
        DEFW    L0688                   ; stk-zero
        DEFW    L09E1                   ; #s
        DEFW    L08FF                   ; rot
        DEFW    L0A4A                   ; sign

L09C3:  DEFW    L099C                   ; #>
        DEFW    L096E                   ; type
        DEFW    L0A73                   ; space
        DEFW    L04B6                   ; exit

; -------------
; THE 'U.' WORD
; -------------
; (un -- )
; Prints the unsigned single length integer 'un' to the television screen,
; followed by a space.

L09CB:  DEFB    'U'                     ; 'name field'
        DEFB    '.' + $80

        DEFW    L09B2                   ; 'link field'

L09CF:  DEFB    $02                     ; 'name length field'

L09D0:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L09D2:  DEFW    L0688                   ; stk-zero
L09D4:  DEFW    L098D                   ; <#
L09D6:  DEFW    L09E1                   ; #S
L09D8:  DEFW    L1276                   ; branch
L09DA:  DEFW    $FFE8                   ; -> 09C3


; -------------
; THE '#S' WORD
; -------------
; (ud -- 0,0)
; Applies # repeatedly (at least once) until the double length number left
; on the stack is 0.

L09DC:  DEFB    '#'                     ; 'name field'
        DEFB    'S' + $80

        DEFW    L09CF                   ; 'link field'

L09E0:  DEFB    $02                     ; 'name length field'

L09E1:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L09E3:  DEFW    L09F7                   ; #
        DEFW    L0912                   ; over
        DEFW    L0912                   ; over
        DEFW    L0E36                   ; or
        DEFW    L0C1A                   ; 0=
        DEFW    L128D                   ; ?branch

L09EF:  DEFW    $FFF3                   ; back to L09E3

        DEFW    L04B6                   ; exit

; ------------
; THE '#' WORD
; ------------
; (ud1 -- ud2)
; used in formatted output. Generates one digit from the unsigned double
; length integer ud1 and holds it in the pad. The unsigned double length
; integer ud2 is the quotient when ud1 is divided by the number base.

L09F3:  DEFB    '#' + $80               ; 'name field'

        DEFW    L09E0                   ; 'link field'

L09F6:  DEFB    $01                     ; 'name length field'

L09F7:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L09F9:  DEFW    L048A                   ; get base
L09FB:  DEFW    L0896                   ; C@
L09FD:  DEFW    L0CC4                   ; div?
L09FF:  DEFW    L08FF                   ; rot
L0A01:  DEFW    L0A07                   ; stk-char
L0A03:  DEFW    L0A5C                   ; hold
L0A05:  DEFW    L04B6                   ; exit

; ----------------------------
; The 'stk-char' Internal Word
; ----------------------------
; used from above thread.

L0A07:  DEFW    L0A09                   ; headerless 'code field'

; ---

L0A09:  RST     18H                     ; data stack to DE
        LD      A,E                     ; character to A
        ADD     A,$30                   ; convert digit to ASCII
        CP      $3A                     ; compare to '9'
        JR      C,L0A13                 ; forward if digit
        ADD     A,$07                   ; else add for hex

L0A13:  LD      E,A                     ; back to E
        RST     10H                     ; push ASCII on data stack.
        JP      (IY)                    ; to 'next'.

; --------------
; THE 'CLS' WORD
; --------------
; ( -- )
; Clears the screen and sets the print position to the top left of
; the screen.

L0A17:  DEFM    "CL"                    ; 'name field'
        DEFB    'S' + $80

        DEFW    L09F6                   ; 'link field'

L0A1C:  DEFB    $03                     ; 'name length field'

        DEFW    L0A1F                   ; 'code field'

; ---

L0A1F:  CALL    L0A24                   ; routine CLS below.

        JP      (IY)                    ; to 'next'.


; --------------------
; THE 'CLS' SUBROUTINE
; --------------------
; Called from the 'CLS' word definition above and also from the initialization
; routine.

L0A24:  LD      DE,$26FF                ; point destination to end of video
                                        ; memory.
        LD      HL,($3C24)              ; set HL to first byte of input buffer
                                        ; from system variable L_HALF.
                                        ; (at initialization $26E0).

        LD      BC,$0020                ; set count to thirty two.

        ADD     HL,BC                   ; add to the low address.
        DEC     HL                      ; step back and
        LDDR                            ; copy the 32 bytes.

; while BC is zero, set the plotting coordinates.

        LD      ($3C2F),BC              ; set XCOORD and YCOORD to zero.

; set the screen position to the start of video memory.

        LD      HL,$2400                ; start of the 768 bytes of video RAM.
        LD      ($3C1C),HL              ; set system variable SCRPOS.

        INC     DE                      ; the byte before logical line.
        EX      DE,HL                   ; transfer to HL.
        LD      ($3C24),HL              ; set L_HALF.
        JP      L07FA                   ; jump back to fill the locations
                                        ; from DE to HL -1 with spaces.

; ---------------
; THE 'SIGN' WORD
; ---------------
; (n -- )
; In formatted output, holds a minus sign in the pad if n is negative.


L0A43:  DEFM    "SIG"                   ; 'name field'
        DEFB    'N' + $80

        DEFW    L099B                   ; 'link field'

L0A49:  DEFB    $04                     ; 'name length field'

L0A4A:  DEFW    L0A4C                   ; 'code field'

; ---

L0A4C:  RST     18H                     ; pop word DE
        RL      D                       ; test sign bit
        LD      E,$2D                   ; prepare a '-'
        JR      C,L0A5F                 ; forward if minus
        JP      (IY)                    ; to 'next'.

; ---------------
; THE 'HOLD' WORD
; ---------------
; (character -- )
; Used in formatted output to hold the character in the pad.

L0A55:  DEFM    "HOL"                   ; 'name field'
        DEFB    'D' + $80

L0A59:  DEFW    L0A1C                   ; 'link field'

L0A5B:  DEFB    $04                     ; 'name length field'

L0A5C:  DEFW    L0A5E                   ; 'code field'

; ---

L0A5E:  RST     18H                     ; data stack to DE

L0A5F:  LD      HL,($3C1A)              ; HLD
        DEC     L
        JR      Z,L0A69                 ; forward when full

        LD      ($3C1A),HL              ; update HLD
        LD      (HL),E                  ; and place character in buffer

L0A69:  JP      (IY)                    ; to 'next'.

; ----------------
; THE 'SPACE' WORD
; ----------------
; (  --  )
; EMITs a space.

L0A6B:  DEFM    "SPAC"                  ; 'name field'
        DEFB    'E' + $80

        DEFW    L0A5B                   ; 'link field'

L0A72:  DEFB    $05                     ; 'name length field'

L0A73:  DEFW    L0A75                   ; 'code field'

; ---

L0A75:  LD      A,$20                   ; load accumulator with the ASCII
                                        ; code for space.
        RST     08H                     ; print_ch

L0A78:  JP      (IY)                    ; to 'next'.

; -----------------
; THE 'SPACES' WORD
; -----------------
; (n -- )
; EMITs n spaces if n >= 1.

L0A7A:  DEFM    "SPACE"                 ; 'name field'
        DEFB    'S' + $80

        DEFW    L0A72                   ; 'link field'

L0A82:  DEFB    $06                     ; 'name length field'

        DEFW    L0A85                   ; 'code field'

; ---

L0A85:  RST     18H                     ; fetch stack data to DE

L0A86:  DEC     DE                      ; decrement the counter.
        BIT     7,D                     ; test for a negative value
        JR      NZ,L0A78                ; back to a jp iy  when done    >>

        LD      A,$20                   ; prepare a space
        RST     08H                     ; print it
        JR      L0A86                   ; loop back for more.

; -------------
; THE 'CR' WORD
; -------------
; Outputs a carriage return character to the television.

L0A90:  DEFB    'C'                     ; 'name field'
        DEFB    'R' + $80

        DEFW    L0A82                   ; 'link field'

L0A94:  DEFB    $02                     ; 'name length field'

L0A95:  DEFW    L0A97                   ; 'code field'

; ---

L0A97:  LD      A,$0D                   ; prepare a CR
        RST     08H                     ; print it.

        JP      (IY)                    ; to 'next'.

; ---------------
; THE 'EMIT' WORD
; ---------------
; (character -- )
; writes the character to the television screen.

L0A9C:  DEFM    "EMI"                   ; 'name field'
        DEFB    'T' + $80

        DEFW    L0A94                   ; 'link field'

L0AA2:  DEFB    $04                     ; 'name length field'

L0AA3:  DEFW    L0AA5                   ; 'code field'

; ---

L0AA5:  RST     18H                     ; pop de off data stack
        LD      A,E                     ; character to A
        RST     08H                     ; print it.

        JP      (IY)                    ; to 'next'.


; -------------
; THE 'F.' WORD
; -------------
; (f -- )
; print a floating point number.
; If 1.0E-4 <= f < 1.0E9, then f is printed without an exponent and with a
; decimal point in the appropriate place. If f is outside this range, then
; it is printed in standard form f'En where 0 <= f' < 10 and -64 <= n <= 62.
; Input may be either form, but only six significant digits are accepted -
; further digits are ignored.
; Floating point numbers are stored as 3 bytes of binary coded decimal
; mantissa and 1 byte for sign and decimal exponents.
;
; e.g. the number 123.456 on Data Stack would be two words, four bytes.
;
;       ^       43              01000011   bits 5 - 0 are exponent
;       |       12      BCD     ||
;       |       34      BCD     |sign of exponent 1=positive (bit 6)
;       |       56      BCD     sign of number 0=positive (bit 7)
;
; Zero 0. is a special case floating point number with all four bytes set
; to zero.


L0AAA:  DEFB    'F'                     ; 'name field'
        DEFB    '.' + $80

        DEFW    $0AA2                   ; 'link field'

L0AAE:  DEFB    $02                     ; 'name length field'

L0AAF:  DEFW    $0AB1                   ; 'code field'

; ---

L0AB1:  LD      HL,($3C3B)              ; set pointer from system variable SPARE
        DEC     HL                      ; now points to last byte of data stack.
        BIT     7,(HL)                  ; test sign of number.
        RES     7,(HL)                  ; reset the sign bit.
        JR      Z,L0ABE                 ; forward if initially positive.

        LD      A,$2D                   ; prepare  the '-' character.
        RST     08H                     ; print the minus sign.

; The E register is initialized to zero to denote not E-FORMAT

L0ABE:  LD      E,$00                   ; signal not scientific notation.

        LD      A,(HL)                  ; fetch exponent byte
        DEC     A                       ; adjust to make zero $FF

        CP      $49                     ; compare to +9   e.g.  123456000.
        JR      NC,L0ACA                ; skip forward if out of range.

        CP      $3C                     ; compare to -4   e.g  .000123456
        JR      NC,L0ACE                ; skip forward if in range.

; else E format printing will be used with decimal point after first digit.

L0ACA:  LD      (HL),$41                ; make Data Stack exponent +1
        INC     A                       ; restore true exponent byte
        LD      E,A                     ; transfer to E.

; the branch was here when within range for normal printing.

L0ACE:  LD      A,$40                   ; test value is plus zero.
        SUB     (HL)                    ; subtract signed exponent.
        JR      C,L0ADC                 ; forward if positive

; exponent is negative so decimal point comes first. e.g. .001

        LD      B,A                     ; result of subtraction to B.
        INC     B                       ; B is now one less than count of
                                        ; leading zeros.

        LD      A,$2E                   ; prepare '.'

L0AD7:  RST     08H                     ; print decimal point or zero.

        LD      A,$30                   ; prepare a zero - '0'

        DJNZ    L0AD7                   ; loop back to print leading zeros
                                        ; unless the counter was 1.

; the branch was here with positive exponent (and zero)
; now enter a loop to print each of the leading BCD digits
; the loop will end when the exponent is <= +0 and all 6 nibbles contain zero.

L0ADC:  LD      A,$40                   ; set accumulator to plus 0
        CP      (HL)                    ; compare to exponent on data stack.
        SBC     A,A                     ; $FF if more leading digits else $00.
        DEC     HL                      ; address first two nibbles.
        OR      (HL)                    ; combine.
        DEC     HL                      ; address next two nibbles.
        OR      (HL)                    ; combine.
        DEC     HL                      ; address last two nibbles.
        OR      (HL)                    ; combine.

        INC     HL                      ; adjust the pointer to
        INC     HL                      ; the start of the mantissa.

        JR      Z,L0AFC                 ; forward if all digits have been
                                        ; printed.

; else print each binary coded decimal in turn.

        XOR     A                       ; prepare to feed a zero nibble in.

        CALL    L0732                   ; routine shift_fp extracts the most
                                        ; significant nibble from the 3 bytes
                                        ; also decrementing the exponent.

        ADD     A,$30                   ; convert to ASCII
        RST     08H                     ; print digit

        INC     HL                      ; point to reduced exponent.
        LD      A,(HL)                  ; fetch to accumulator and
        CP      $40                     ; compare to zero.

        JR      NZ,L0ADC                ; loop back while more digits.

; else this is the place to print the mid or trailing decimal point.

        LD      A,$2E                   ; prepare '.'
        RST     08H                     ; print it.

        JR      L0ADC                   ; loop back for end test and any digits
                                        ; following the decimal point.

; ---

; the branch was to here when all digits of the mantissa have been printed.

L0AFC:  LD      A,E                     ; fetch the exponent format flag - from
                                        ; the E register appropriately.
        AND     A                       ; test for zero - normal format.
        JR      NZ,L0B05                ; forward to E_FORMAT if not.

        LD      A,$20                   ; else prepare a space
        RST     08H                     ; print it

        JR      L0B10                   ; forward to delete the two words from
                                        ; the data stack and exit.

; ---

; this branch deals with scientific notation. The accumulator holds the
; original exponent. $01-$3C (negative) $49-$7F (positive).

L0B05:  SUB     $41                     ; convert to signed 8-bit.
        LD      L,A                     ; low order byte to L.
        SBC     A,A                     ; $FF negative or $00 positive
        LD      H,A                     ; set the high order byte.

        LD      A,$45                   ; prepare a 'E'
        RST     08H                     ; print it

        CALL    L180E                   ; routine pr_int_hl prints the signed
                                        ; integer followed by a space.

; finally delete the floating point number from the Data Stack.


L0B10:  RST     18H                     ; unstack word DE
        RST     18H                     ; unstack word DE

        JP      (IY)                    ; to 'next'.

; -------------
; THE 'AT' WORD
; -------------
; (line, column -- )
; Sets print position to line and column numbers on the stack.
; There are 23 lines (0 to 22) and 32 columns (0 to 31). The
; column number is taken modulo 32, and ERROR 9 if trying to print
; in the input buffer at the bottom.

L0B14:  DEFB    'A'                     ; 'name field'
        DEFB    'T' + $80

        DEFW    L0AAE                   ; 'link field'

L0B18:  DEFB    $02                     ; 'name length field'

        DEFW    L0B1B                   ; 'code field'

; ---

L0B1B:  RST     18H                     ; pop word DE

        CALL    L084E                   ; stk_to_bc

        LD      A,C

        CALL    L0B28                   ;

        LD      ($3C1C),HL              ; update system variable SCRPOS

        JP      (IY)                    ; to 'next'.

; ---

; plotsub

L0B28:  ADD     A,$20
        LD      L,A
        LD      H,$01
        ADD     HL,HL
        ADD     HL,HL
        ADD     HL,HL
        ADD     HL,HL
        ADD     HL,HL
        LD      D,$00
        LD      A,E
        AND     $1F
        LD      E,A
        ADD     HL,DE
        LD      DE,($3C24)              ; fetch start of lower half from L_HALF
        SBC     HL,DE
        ADD     HL,DE
        RET     C

;

        RST     20H                     ; Error 9
        DEFB    $09                     ; Erroneous 'AT' Command.

; ---------------
; THE 'PLOT' WORD
; ---------------
; (x, y, n -- )
; Plots pixel (x, y) with plot mode n.
; n =   0       unplot
;       1       plot
;       2       move
;       3       change
; If n>3, takes value modulo 4.

L0B43:  DEFM    "PLO"                   ; 'name field'
        DEFB    'T' + $80

        DEFW    L0B18                   ; 'link field'

L0B49:  DEFB    $04                     ; 'name length field'

        DEFW    L0B4C                   ; 'code field'

; ---

L0B4C:  CALL    L084E                   ; stk_to_bc

        RST     18H                     ; pop word DE
        LD      (IX+$30),E              ; YCOORD
        SRL     E
        RL      C
        LD      A,$16                   ; 24
        SUB     E

        RST     18H                     ; pop word DE
        LD      (IX+$2F),E              ; XCOORD
        SRL     E
        RL      C

        CALL    L0B28                   ;

        LD      A,(HL)
        AND     $78                     ; 01111000
        CP      $10
        LD      A,(HL)
        JR      Z,L0B6F                 ;

        LD      A,$10

L0B6F:  LD      E,A
        LD      D,$87
        LD      A,C
        AND     $03
        LD      B,A
        JR      Z,L0B7F                 ;

        CPL

        ADD     A,$02
        ADC     A,$03
        LD      D,A
        LD      B,E
L0B7F:  LD      A,C
        RRCA
        RRCA
        RRCA
        SBC     A,A
        BIT     3,C
        JR      NZ,L0B8C                ;
        XOR     E
        RLCA
        SBC     A,A
        XOR     B

L0B8C:  AND     D
        XOR     E
        LD      (HL),A
        JP      (IY)                    ; to 'next'.

; ---------------
; THE 'BEEP' WORD
; ---------------
; ( m, n --  )
; Plays a note on the loudspeaker. 8 * m = period in microseconds,
; n = time in milliseconds.

L0B91:  DEFM    "BEE"                   ; 'name field'
        DEFB    'P' + $80

        DEFW    L0B49                   ; 'link field'

L0B97:  DEFB    $04                     ; 'name length field'

        DEFW    L0EC3                   ; 'code field'  m, n.

; ---

L0B9A:  DEFW    L0912                   ; OVER          m, n, m.
        DEFW    L104B                   ; stk-data      m, n, m, 125.
        DEFB    $7D                     ;  (125)
        DEFW    L0885                   ; SWAP          m, n, 125, m.
        DEFW    L0D7A                   ; */            m, (n*125)/m
        DEFW    L1A0E                   ; end

; ---

L0BA5:  RST     18H                     ; pop word DE

        CALL    L084E                   ; stk_to_bc

        LD      HL,$00F9                ;
        ADD     HL,BC                   ;
        INC     L                       ;

        DI                              ; Disable Interrupts.

L0BAF:  LD      A,$7F                   ; place $7FFE on address bus and read
        IN      A,($FE)                 ; from port, pushing the loudspeaker
                                        ; diaphragm in.

        RRCA                            ; test the read 'SPACE' key bit.

        JR      NC,L0BC7                ; forward if BREAK pressed.

        CALL    L0BC9                   ; routine delay_HL

        DEC     DE                      ; decrement counter.

        LD      A,D                     ; all even addresses are reserved for
                                        ; Jupiter Ace so any value does for the
                                        ; high order byte. $FE is low value.

        OUT     ($FE),A                 ; push the loudspeaker diaphragm out.

        CALL    L0BC9                   ; routine delay_HL

        OR      E                       ; test for counter DE reaching zero.
        JP      NZ,L0BAF                ; loop back if not.

        EI                              ; Enable Interrupts.

        JP      (IY)                    ; to 'next'.

; ---

L0BC7:  RST     20H                     ; Error 3
        DEFB    $03                     ; BREAK pressed.

; ---------------------------
; THE 'BEEP DELAY' SUBROUTINE
; ---------------------------
; called twice from the above BEEP routine.

L0BC9:  LD      B,L                     ; transfer the value of
        LD      C,H                     ; the HL register to BC.

L0BCB:  DJNZ    L0BCB                   ; self-loop for B times

        DEC     B                       ; set B to $FF for future loops
        DEC     C                       ; decrement outer loop counter C
        JP      NZ,L0BCB                ; JUMP back if not zero           (10)

        RET                             ; return

; ----------------
; THE 'INKEY' WORD
; ----------------
; ( -- ASCII code)
; Reads the keyboard. Puts ASCII value on the stack if a key is pressed, 0
; otherwise.


L0BD3:  DEFM    "INKE"                  ; 'name field'
        DEFB    'Y' + $80

        DEFW    L0B97                   ; 'link field'

L0BDA:  DEFB    $05                     ; 'name length field'

L0BDB:  DEFW    L0BDD                   ; 'code field'

; ---

L0BDD:  CALL    L0336                   ; routine KEY-SCAN

        LD      E,A                     ; transfer the key code to E.
        LD      D,$00                   ; make high order byte zero.

        RST     10H                     ; stack Data Word DE

        JP      (IY)                    ; to 'next'.

; -------------
; THE 'IN' WORD
; -------------
; (port address -- data byte)
; Inputs a data byte from an I/O port.

L0BE6:  DEFB    'I'                     ; 'name field'
        DEFB    'N' + $80

        DEFW    L0BDA                   ; 'link field'

L0BEA:  DEFB    $02                     ; 'name length field'

        DEFW    L0BED                   ; 'code field'

; ---

L0BED:  CALL    L084E                   ; stk_to_bc
        LD      D,$00                   ; make high order byte zero.

        IN      E,(C)                   ; read the port to E.

        RST     10H                     ; stack Data Word DE.

L0BF5:  JP      (IY)                    ; to 'next'.

; --------------
; THE 'OUT' WORD
; --------------
; (data byte, port address -- )
; Outputs a data byte to an I/O port.

L0BF7:  DEFM    "OU"                    ; 'name field'
        DEFB    'T' + $80

        DEFW    L0BEA                   ; 'link field'

L0BFC:  DEFB    $03                     ; 'name length field'

        DEFW    L0BFF                   ; 'code field'

; ---

L0BFF:  CALL    L084E                   ; stk_to_bc
                                        ; all 16 bits are placed on the
                                        ; Z80A address bus.
        RST     18H                     ; pop word DE

        OUT     (C),E                   ; output byte to port address.

        JP      (IY)                    ; to 'next'.

; --------------
; THE 'ABS' WORD
; --------------
; (n -- absolute value of n)

L0C07:  DEFM    "AB"                    ; 'name field'
        DEFB    'S' + $80

        DEFW    L0BFC                   ; 'link field'

L0C0C:  DEFB    $03                     ; 'name length field'

L0C0D:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

        DEFW    L086B                   ; DUP
        DEFW    L0D94                   ; pos
        DEFW    L04B6                   ; EXIT

; -------------
; THE '0=' WORD
; -------------
; (n -- flag)
; flag is 1 in n = 0.

L0C15:  DEFB    '0'                     ; 'name field'
        DEFB    '=' + $80

        DEFW    L0C0C                   ; 'link field'

L0C19:  DEFB    $02                     ; 'name length field'

L0C1A:  DEFW    L0C1C                   ; 'code field'

; ---

L0C1C:  RST     18H                     ; pop word DE
        LD      A,D                     ; test for
        OR      E                       ; zero
        CP      $01                     ; sets carry if word is zero

; -> zero_or_one

L0C21:  LD      A,$00                   ; make accumulator zero.
        LD      D,A                     ; set D to zero
        RLA                             ; pick up carry (1/0)
        LD      E,A                     ; set DE to one or zero
        RST     10H                     ; push word DE

        JP      (IY)                    ; to 'next'.

; -------------
; THE '0<' WORD
; -------------
; (n -- flag)
; flag is 1 if n is negative

L0C29:  DEFB    '0'                     ; 'name field'
        DEFB    '<' + $80

        DEFW    L0C19                   ; 'link field'

L0C2D:  DEFB    $02                     ; 'name length field'

L0C2E:  DEFW    L0C30                   ; 'code field'

; ---

L0C30:  RST     18H                     ; pop word DE
        RL      D                       ; test the sign bit.

        JR      L0C21                   ; back to above routine to stack the
                                        ; carry as one (true) or zero (false).

; -------------
; THE '0>' WORD
; -------------
; (n -- flag)
; flag is 1 if n is positive.


L0C35:  DEFB    '0'                     ; 'name field'
        DEFB    '>' + $80

        DEFW    L0C2D                   ; 'link field'

L0C39:  DEFB    $02                     ; 'name length field'

L0C3A:  DEFW    L0C3C                   ; 'code field'

; ---

L0C3C:  RST     18H                     ; pop word DE
        LD      A,D
        OR      E
        JR      Z,L0C21                 ; to stack word one or zero

        RL      D
        CCF
        JR      L0C21                   ; to stack word one or zero

; ------------
; THE '=' WORD
; ------------
; (n1, n2 -- flag)
; flag is 1 if n1=n2.

L0C46:  DEFB    '=' + $80               ; 'name field'

        DEFW    L0C39                   ; 'link field'

L0C49:  DEFB    $01                     ; 'name length field'

L0C4A:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0C4C:  DEFW    L0DE1                   ; -
        DEFW    L0C1A                   ; 0=
        DEFW    L04B6                   ; exit

; ------------
; THE '>' WORD
; ------------
; (n1, n2 -- flag)
; flag is 1 if n1>n2.

L0C52:  DEFB    '>' + $80               ; 'name field'

        DEFW    L0C49                   ; 'link field'

L0C55:  DEFB    $01                     ; 'name length field'

L0C56:  DEFW    L0C58                   ; 'code field'

; ---

L0C58:  RST     18H                     ; pop word DE
        PUSH    DE                      ;
        RST     18H                     ; pop word DE
        POP     HL                      ;

        CALL    L0C99                   ;

        JR      L0C21                   ; to stack word one or zero

; ------------
; THE '<' WORD
; ------------
; (n1, n2 -- flag)
; flag is 1 if n1 < n2.

L0C61:  DEFB    '<' + $80               ; 'name field'

        DEFW    L0C55                   ; 'link field'

L0C64:  DEFB    $01                     ; 'name length field'

L0C65:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

        DEFW    L0885                   ; swap
        DEFW    L0C56                   ; >
        DEFW    L04B6                   ; exit


; -------------
; THE 'U<' WORD
; -------------
; (un1, un2 -- flag)
; The flag is 1 if, of the two unsigned single length integers, un1 is less
; than un2.

L0C6D:  DEFB    'U'                     ; 'name field'
        DEFB    '<' + $80

        DEFW    L0C64                   ; 'link field'

L0C71:  DEFB    $02                     ; 'name length field'

L0C72:  DEFW    L0C74                   ; 'code field'

; ---

L0C74:  CALL    L084E                   ; stk_to_bc

L0C77:  RST     18H                     ; pop word DE
        EX      DE,HL
        AND     A
        SBC     HL,BC
        JR      L0C21                   ; to stack word one or zero

; -------------
; THE 'D<' WORD
; -------------
; (d1, d2 -- flag)
; flag is 1 if the signed double integer, d1 < d2.

L0C7E:  DEFB    'D'                     ; 'name field'
        DEFB    '<' + $80

        DEFW    L0C71                   ; 'link field'

L0C82:  DEFB    $02                     ; 'name length field'

L0C83:  DEFW    L0C85                   ; 'code field'

; ---

L0C85:  RST     18H                     ; pop word DE
        PUSH    DE
        CALL    L084E                   ; stk_to_bc
        RST     18H                     ; pop word DE
        POP     HL
        AND     A
        SBC     HL,DE
        JR      Z,L0C77                 ;

        ADD     HL,DE
        EX      DE,HL

        CALL    L0C99                   ;

        RST     18H                     ; pop word DE
        JR      L0C21                   ; to stack word one or zero

; ---
; THE 'sign?' SUBROUTINE
; ---

L0C99:  LD      A,H
        XOR     D
        JP      M,L0CA0                 ;

        SBC     HL,DE

L0CA0:  RL      H
        RET

; -------------
; THE 'U*' WORD
; -------------
; (un1, un2 -- double length(un1 * un2))
; Multiplies two unsigned single length integers to give an unsigned
; double length product.

L0CA3:  DEFB    'U'                     ; 'name field'
        DEFB    '*' + $80

        DEFW    L0C82                   ; 'link field'

L0CA7:  DEFB    $02                     ; 'name length field'

L0CA8:  DEFW    L0CAA                   ; 'code field'

; => mult

L0CAA:  RST     18H                     ; pop word DE
        CALL    L084E                   ; stk_to_bc
        LD      HL,$0000
        LD      A,$10
L0CB3:  ADD     HL,HL
        EX      DE,HL
        ADC     HL,HL
        EX      DE,HL
        JR      NC,L0CBE                ;

        ADD     HL,BC
        JR      NC,L0CBE                ;

        INC     DE

L0CBE:  DEC     A
        JR      NZ,L0CB3                ;

        EX      DE,HL
        JR      L0CF3                   ;

; ---
; The 'div?' Internal Word
; ---

L0CC4:  DEFW    L0CC6

L0CC6:  RST     18H                     ; pop word DE
        EXX
        RST     18H                     ; pop word DE
        PUSH    DE
        RST     18H                     ; pop word DE
        POP     HL
        LD      A,H
        OR      L
        LD      A,$21                   ; 33
        JR      NZ,L0CD5                ;

        EX      DE,HL
        LD      A,$11                   ; 17

L0CD5:  EXX
        LD      B,A
        XOR     A
        LD      H,A
        LD      L,A
        LD      C,A

L0CDB:  ADC     HL,HL
        SBC     A,A
        AND     A
        SBC     HL,DE
        SBC     A,C
        JR      NC,L0CE5                ;
        ADD     HL,DE

L0CE5:  CCF
        EXX
        EX      DE,HL
        ADC     HL,HL
        EX      DE,HL
        ADC     HL,HL
        EXX
        DJNZ    L0CDB                   ;

        EX      DE,HL
        RST     10H                     ; push word DE
        EXX

L0CF3:  PUSH    HL
        RST     10H                     ; push word DE
        POP     DE
        RST     10H                     ; push word DE

        JP      (IY)                    ; to 'next'.

; ---------------
; THE '/MOD' WORD
; ---------------
; (n1, n2 -- remainder, quotient of n1/n2)
; The remainder has the same sign as the dividend n1.

L0CF9:  DEFM    "/MO"                   ; 'name field'
        DEFB    'D' + $80

        DEFW    L0CA7                   ; 'link field'

L0CFF:  DEFB    $04                     ; 'name length field'

L0D00:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0D02:  DEFW    L0885                   ; swap
        DEFW    L08D2                   ; >R
        DEFW    L12E9                   ; I
        DEFW    L0C0D                   ; abs
        DEFW    L104B                   ; stk_data
        DEFB    $00                     ; zero
; ->
L0D0D:  DEFW    L08FF                   ; rot
        DEFW    L086B                   ; dup
        DEFW    L12E9                   ; I
        DEFW    L0E60                   ; xor
        DEFW    L08D2                   ; >R
        DEFW    L0C0D                   ; abs
        DEFW    L0D8C                   ; U/MOD
        DEFW    L08DF                   ; >R
        DEFW    L0D94                   ; pos
        DEFW    L0885                   ; swap
        DEFW    L08DF                   ; >R
        DEFW    L0D94                   ; pos
        DEFW    L0885                   ; swap
        DEFW    L04B6                   ; exit

; ----------------
; THE '*/MOD' WORD
; ----------------
; (n1, n2, n3 -- remainder, quotient of (n1 * n2)/n3)
; As in */, n1 * n2 is held to double length.

L0D29:  DEFM    "*/MO"                  ; 'name field'
        DEFB    'D' + $80

        DEFW    L0CFF                   ; 'link field'

L0D30:  DEFB    $05                     ; 'name length field'

L0D31:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

        DEFW    L08FF                   ; rot
        DEFW    L08D2                   ; >R
        DEFW    L12E9                   ; I
        DEFW    L0C0D                   ; abs
        DEFW    L08FF                   ; rot
        DEFW    L086B                   ; dup
        DEFW    L08DF                   ; >R
        DEFW    L0E60                   ; xor
        DEFW    L08D2                   ; >R
        DEFW    L0C0D                   ; abs
        DEFW    L0CA8                   ; u*
        DEFW    L1276                   ; branch

L0D4B:  DEFW    $FFC1                   ; back to L0D0D  (in /MOD)




; ------------
; THE '/' WORD
; ------------
; (n1, n2 -- n1/n2)
; Single length signed integer division.

L0D4D:  DEFB    '/' + $80               ; 'name field'

        DEFW    L0D30                   ; 'link field'

L0D50:  DEFB    $01                     ; 'name length field'

L0D51:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0D53:  DEFW    L0D00                   ; /MOD
        DEFW    L0885                   ; swap
        DEFW    L0879                   ; drop
        DEFW    L04B6                   ; exit

; --------------
; THE 'MOD' WORD
; --------------
; (n1, n2 -- remainder n1/n2)
; The remainder has the same sign as the dividend.

L0D5B:  DEFM    "MO"                    ; 'name field'
        DEFB    'D' + $80

        DEFW    L0D50                   ; 'link field'

L0D60:  DEFB    $03                     ; 'name length field'

L0D61:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

        DEFW    L0D00                   ; /MOD
        DEFW    L0879                   ; drop
        DEFW    L04B6                   ; exit


; ------------
; THE '*' WORD
; ------------
; (n1, n2 -- n1*n2)

L0D69:  DEFB    '*' + $80               ; 'name field'

        DEFW    L0D60                   ; 'link field'

L0D6C:  DEFB    $01                     ; 'name length field'

        DEFW    L0EC3                   ; 'code field' - docolon

; ---

        DEFW    L0CA8                   ; u*
        DEFW    L0879                   ; drop
        DEFW    L04B6                   ; exit


; -------------
; THE '*/' WORD
; -------------
; (n1, n2, n3 -- (n1*n2)/n3)
; The intermediate product n1*n2 is held to double length.

L0D75:  DEFB    '*'                     ; 'name field'
        DEFB    '/' + $80

        DEFW    L0D6C                   ; 'link field'

L0D79:  DEFB    $02                     ; 'name length field'

L0D7A:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

        DEFW    L0D31                   ; */MOD
        DEFW    L0885                   ; swap
        DEFW    L0879                   ; drop
        DEFW    L04B6                   ; exit

; --------------
; THE 'U/MOD' WORD
; --------------
; (ud1, un2 -- un3, un4)
; In unsigned arithmetic throughout, divides the double length integer ud1
; by the single length integer un2 to give a single length remainder un3
; and a single length quotient un4.

L0D84:  DEFM    "U/MO"                  ; 'name field'
        DEFB    'D' + $80

        DEFW    L0D79                   ; 'link field'

L0D8B:  DEFB    $05                     ; 'name length field'

L0D8C:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0D8E:  DEFW    L0CC4                   ; div?
        DEFW    L0879                   ; drop
        DEFW    L04B6                   ; exit

; ---

; make positive

L0D94:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0D96:  DEFW    L0C2E                   ; 0<
        DEFW    L1283                   ; ?branch               (if false)
L0D9A:  DEFW    $0003                   ; to L0D9E

        DEFW    L0DA9                   ; negate

L0D9E:  DEFW    L04B6                   ; exit

; -----------------
; THE 'NEGATE' WORD
; -----------------
; (n -- -n)


L0DA0:  DEFM    "NEGAT"                 ; 'name field'
        DEFB    'E' +$80

        DEFW    L0D8B                   ; 'link field'

L0DA8:  DEFB    $06                     ; 'name length field'

L0DA9:  DEFW    L0DAB                   ; 'code field'

; ---

L0DAB:  LD      BC,$0002                ;
        JR      L0DBF                   ;

; ------------------
; THE 'DNEGATE' WORD
; ------------------
; (d -- -d)
; Double length integer negation.

L0DB0:  DEFM    "DNEGAT"                ; 'name field'
        DEFB    'E' +$80

        DEFW    L0DA8                   ; 'link field'

L0DB9:  DEFB    $07                     ; 'name length field'

L0DBA:  DEFW    L0DBC                   ; 'code field'

; ---

L0DBC:  LD      BC,$0004

; NEGATE joins here with bc=2

L0DBF:  LD      HL,($3C3B)              ; SPARE
        AND     A
        SBC     HL,BC

L0DC5:  LD      A,B
        SBC     A,(HL)
        LD      (HL),A
        INC     HL
        DEC     C
        JR      NZ,L0DC5                ;

        JP      (IY)                    ; to 'next'.

; ------------
; THE '+' WORD
; ------------
; (n1, n2 -- n1 + n2)

L0DCE:  DEFB    '+' + $80               ; 'name field'

        DEFW    L0DB9                   ; 'link field'

L0DD1:  DEFB    $01                     ; 'name length field'

L0DD2:  DEFW    L0DD4                   ; 'code field'

; ---

L0DD4:  RST     18H                     ; pop word DE
        PUSH    DE                      ; save on machine stack
        RST     18H                     ; pop word DE
        POP     HL                      ; first number to HL

        ADD     HL,DE                   ; the actual addition

        EX      DE,HL                   ; result to DE
        RST     10H                     ; push word DE

        JP      (IY)                    ; to 'next'.

; ------------
; THE '-' WORD
; ------------
; (n1, n2 -- n1-n2)
; flip the sign and do a plus.

L0DDD:  DEFB    '-' + $80               ; 'name field'

        DEFW    L0DD1                   ; 'link field'

L0DE0:  DEFB    $01                     ; 'name length field'

L0DE1:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0DE3:  DEFW    L0DA9                   ; negate
        DEFW    L0DD2                   ; +
        DEFW    L04B6                   ; exit

; -------------
; THE 'D+' WORD
; -------------
; (d1, d2 -- d1 + d2)
; double length integer addition.

L0DE9:  DEFB    'D'                     ; 'name field'
        DEFB    '+' + $80

        DEFW    L0DE0                   ; 'link field'

L0DED:  DEFB    $02                     ; 'name length field'

L0DEE:  DEFW    L0DF0                   ; 'code field'

; ---

L0DF0:  RST     18H                     ; pop word DE

        PUSH    DE
        CALL    L084E                   ; stk_to_bc
        RST     18H                     ; pop word DE
        PUSH    DE
        RST     18H                     ; pop word DE
        EX      DE,HL
        ADD     HL,BC
        EX      DE,HL
        RST     10H                     ; push word DE
        POP     BC
        POP     HL
        ADC     HL,BC
        EX      DE,HL
        RST     10H                     ; push word DE

        JP      (IY)                    ; to 'next'.

; -------------
; THE '1+' WORD
; -------------
; (n -- n+1)

L0E04:  DEFB    '1'                     ; 'name field'
        DEFB    '+' + $80

        DEFW    L0DED                   ; 'link field'

L0E08:  DEFB    $02                     ; 'name length field'

L0E09:  DEFW    L0E0B                   ; 'code field'

; ---

L0E0B:  RST     18H                     ; get word 'n' in DE
        JR      L0E17                   ; forward to increment and stack

; -------------
; THE '2+' WORD
; -------------
; (n -- n+2)

L0E0E:  DEFB    '2'                     ; 'name field'
        DEFB    '+' + $80

        DEFW    L0E08                   ; 'link field'

L0E12:  DEFB    $02                     ; 'name length field'

L0E13:  DEFW    L0E15                   ; 'code field'

; ---

L0E15:  RST     18H                     ; get word 'n' in DE.
        INC     DE                      ; increment n                   (4)
; ->
L0E17:  INC     DE                      ; increment n                   (4)
        JR      L0E2E                   ; forward to push word DE and exit

; -------------
; THE '1-' WORD
; -------------
; (n -- n-1)


L0E1A:  DEFB    '1'                     ; 'name field'
        DEFB    '-' + $80

        DEFW    L0E12                   ; 'link field'

L0E1E:  DEFB    $02                     ; 'name length field'

L0E1F:  DEFW    L0E21                   ; 'code field'

; ---

L0E21:  RST     18H                     ;
        JR      L0E2D                   ;

; -------------
; THE '2-' WORD
; -------------
; (n -- n-2)


L0E24:  DEFB    '2'                     ; 'name field'
L0E25:  DEFB    '-' + $80

L0E26:  DEFW    L0E1E                   ; 'link field'

L0E28:  DEFB    $02                     ; 'name length field'

L0E29:  DEFW    L0E2B                   ; 'code field'

; ---

;
L0E2B:  RST     18H
        DEC     DE

; ->
L0E2D:  DEC     DE

; ->
L0E2E:  RST     10H                     ; push word DE

        JP      (IY)                    ; to 'next'.

; -------------
; THE 'OR' WORD
; -------------
; (n1, n2 -- n1 OR n2)
; Bitwise Boolean operation.


L0E31:  DEFB    'O'                     ; 'name field'
        DEFB    'R' + $80

        DEFW    L0E28                   ; 'link field'

L0E35:  DEFB    $02                     ; 'name length field'

L0E36:  DEFW    L0E38                   ; 'code field'

; ---

L0E38:  RST     18H                     ; pop word DE
        CALL    L084E                   ; stk_to_bc

        LD      A,E                     ;
        OR      C                       ; OR low order bytes
        LD      E,A                     ;

        LD      A,D                     ;
        OR      B                       ; OR high order bytes
        LD      D,A                     ;

        RST     10H                     ; push word DE

        JP      (IY)                    ; to 'next'.

; --------------
; THE 'AND' WORD
; --------------
; (n1, n2 -- n1 AND n2)
; Bitwise Boolean operation.


L0E45:  DEFM    "AN"                    ; 'name field'
        DEFB    'D' + $80

        DEFW    L0E35                   ; 'link field'

L0E4A:  DEFB    $03                     ; 'name length field'

        DEFW    L0E4D                   ; 'code field'

; ---

L0E4D:  RST     18H
        CALL    L084E                   ; stk_to_bc

        LD      A,E                     ;
        AND     C                       ;
        LD      E,A                     ;

        LD      A,D                     ;
        AND     B                       ;
        LD      D,A                     ;

        RST     10H                     ; push word DE
        JP      (IY)                    ; to 'next'.

; --------------
; THE 'XOR' WORD
; --------------
; (n1, n2 -- n1 XOR n2)
; Bitwise Boolean XOR (exclusive or)

L0E5A:  DEFM    "XO"                    ; 'name field'
        DEFB    'R' + $80

        DEFW    L0E4A                   ; 'link field'

L0E5F:  DEFB    $03                     ; 'name length field'

L0E60:  DEFW    L0E62                   ; 'code field'

; ---

L0E62:  RST     18H
        CALL    L084E                   ; stk_to_bc

        LD      A,E                     ;
        XOR     C                       ;
        LD      E,A                     ;

        LD      A,D                     ;
        XOR     B                       ;
        LD      D,A                     ;

        RST     10H                     ; push word DE
        JP      (IY)                    ; to 'next'.

; --------------
; THE 'MAX' WORD
; --------------
; (n1, n2 -- max (n1, n2))
; Calculates the larger of two numbers.

L0E72:  DEFM    "MA"                    ; 'name field'
        DEFB    'X' + $80

        DEFW    L0E5F                   ; 'link field'

L0E74:  DEFB    $03                     ; 'name length field'

L0E75:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0E77:  DEFW    L0912                   ; over
        DEFW    L0912                   ; over
        DEFW    L0C65                   ; <
        DEFW    L1271                   ; branch
L0E7F:  DEFW    $000F                   ; forward to L0E8F

; --------------
; THE 'MIN' WORD
; --------------
; (n1, n2 -- min (n1, n2))
; Calculates the smaller of two numbers.

L0E81:  DEFM    "MI"                    ; 'name field'
        DEFB    'N' + $80

        DEFW    L0E74                   ; 'link field'

L0E86:  DEFB    $03                     ; 'name length field'

        DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0E89:  DEFW    L0912                   ; over
        DEFW    L0912                   ; over
        DEFW    L0C56                   ; >
; ->
L0E8F:  DEFW    L1283                   ; ?branch
L0E91:  DEFW    $0003                   ; forward to L0995

        DEFW    L0885                   ; swap

L0995:  DEFW    L0879                   ; drop
        DEFW    L04B6                   ; exit

; ------------------
; THE 'DECIMAL' WORD
; ------------------
; (  --  )
; Sets the system number base to ten.

L0E99:  DEFM    "DECIMA"                ; 'name field'
        DEFB    'L' + $80

        DEFW    L0E86                   ; 'link field'

L0EA2:  DEFB    $07                     ; 'name length field'

        DEFW    L0EA5                   ; 'code field'

; ---

L0EA5:  LD      (IX+$3F),$0A            ; update system variable BASE to 10

        JP      (IY)                    ; to 'next'.

; ------------
; THE ':' WORD
; ------------
; Introduces colon definitions.

L0EAB:  DEFB    ':' + $80               ; 'name field'

        DEFW    L0EA2                   ; 'link field'

L0EAE:  DEFB    $01                     ; 'name length field'

L0EAF:  DEFW    L1085                   ; 'code field' - create and enclose

; ---

L0EB1:  DEFW    L0EC3                   ; do_colon

        DEFW    L104B                   ; stk_data
        DEFB    $0A                     ; ten                   marker byte?
; ->
L0EB6:  DEFW    L1A0E                   ; end_forth

L0EB8:  LD      HL,$3C3E                ; FLAGS

        LD      A,(HL)                  ; update bits 6 and 2.
        OR      $44                     ; signal in compile mode, definition
                                        ; incomplete.
        LD      (HL),A                  ; update FLAGS.

        JP      (IY)                    ; to 'next'.

; ---

x0EC1   DEFB    $E9                     ;;
x0Ec2   DEFB    $FF                     ;; 0ec2 + ffe9 =  0eab = ':'

; -------------------------------
; THE 'ENTER' or 'DOCOLON' action
; -------------------------------
;

L0EC3:  EX      DE,HL                   ;
        JP      L04BA                   ;


; -----------------
; THE 'CREATE' WORD
; -----------------
; CREATE name
; (  --  )
; Defines a new word with a header and an empty parameter field.
; When executed, the new word stacks its parameter field address.

L0EC7:  DEFM    "CREAT"                 ; 'name field'
        DEFB    'E' + $80

        DEFW    L0EAE                   ; 'link field'

L0ECF:  DEFB    $06                     ; 'name length field'

L0ED0:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0ED2:  DEFW    L104B                   ; stk_data
        DEFB    $20                     ; a space               delimiter
        DEFW    L05AB                   ; word to pad
        DEFW    L0EFB                   ; get-name              in dict
        DEFW    L0688                   ; stk-zero              link
        DEFW    L0F4E                   ; ,
        DEFW    L0480                   ; current
        DEFW    L08B3                   ; @
        DEFW    L086B                   ; dup
        DEFW    L08B3                   ; @
        DEFW    L0F4E                   ; ,
        DEFW    L0460                   ; here
        DEFW    L0885                   ; swap
        DEFW    L08C1                   ; !
        DEFW    L0499                   ; pad
        DEFW    L0896                   ; C@            fetch 1 byte
        DEFW    L0F5F                   ; C,
        DEFW    L1011                   ; stack next word
        DEFW    $0FEC                   ; ???
        DEFW    L0F4E                   ; ,
L0EF9:  DEFW    L04B6                   ; exit

; ----------------------------
; The 'get_name' Internal Word
; ----------------------------
; Used only by the above CREATE thread.

L0EFB:  DEFW    L0EFD                   ; headerless 'code field'

; ---

L0EFD:  CALL    L0F2E                   ; blank stack

        RST     18H                     ; pop word DE

        LD      A,(DE)
        DEC     A                       ; zero becomes $FF
        CP      $3F                     ; max length is 64
        JR      C,L0F09                 ; forward if n range 1 - 64.

        RST     20H                     ; Error 6
        DEFB    $06                     ; Name of new word too short or long.

; ---

L0F09:  ADD     A,$08                   ; allow for prev/len/addr 3 missing

        LD      C,A                     ;
        LD      B,$00                   ; length to BC

L0F0E:  CALL    L0F8C                   ; check free memory.

x0f11   LD      A,(DE)                  ; true length to A
        LD      C,A                     ; and BC again

        LD      HL,($3C37)              ; STKBOT

        PUSH    DE                      ;
        CALL    L0F9E                   ; routine MAKE ROOM
        POP     DE                      ;

        LD      A,(DE)                  ; length of word in pad
        LD      B,A                     ; transfer to counter.

L0F1D:  INC     DE                      ; increase source
        LD      A,(DE)                  ; fetch character

        CALL    L0807                   ; to_upper makes uppercase.

        LD      (HL),A                  ; store in dictionary
        INC     HL                      ; increase destination
        DJNZ    L0F1D                   ; loop back for all letters.

        LD      ($3C39),HL              ; store this location in SPARE
        DEC     HL                      ; step back to last letter of word.
        SET     7,(HL)                  ; and 'invert' it.
        JP      (IY)                    ; to 'next'.

; ---


L0F2E:  BIT     2,(IX+$3E)              ; test FLAGS incomplete definition ?
        JR      Z,L0F36                 ; forward if not.

        RST     20H                     ; Error 12
        DEFB    $0C                     ; Incomplete definition in dictionary.

; ---

L0F36:  LD      HL,($3C37)              ; fetch STKBOT
        LD      DE,($3C39)              ; fetch SPARE

        XOR     A                       ; clear accumulator and carry flag

        SBC     HL,DE                   ; subtract

        EX      DE,HL                   ;
        LD      (HL),E                  ; place low byte at next STACK slot.
        INC     HL                      ;
        LD      (HL),D                  ; place high byte
        LD      H,A                     ; make HL zero
        LD      L,A                     ;
        LD      ($3C39),HL              ; update system variable SPARE to zero

        RET                             ; return

; ---------------------

; ------------
; THE ',' WORD
; ------------
; ( n --   )
; Encloses the single length integer in the dictionary.

L0F4A:  DEFB    ',' + $80               ; 'name field'

        DEFW    L0ECF                   ; 'link field'

L0F4D:  DEFB    $01                     ; 'name length field'

L0F4E:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0F50:  DEFW    L0F83                   ; allot2

        DEFW    L0460                   ; here
        DEFW    L0E29                   ; 2-
        DEFW    L08C1                   ; !
        DEFW    L04B6                   ; exit


; -------------
; THE 'C,' WORD
; -------------
; ( n --   )
; Encloses the less significant byte of n in the dictionary.

L0F5A:  DEFB    'C'                     ; 'name field'
        DEFB    ',' + $80

        DEFW    L0F4D                   ; 'link field'

L0F5E:  DEFB    $02                     ; 'name length field'

L0F5F:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L0F61:  DEFW    L104B                   ; stk-data
        DEFB    $01                     ; one
        DEFW    L0F76                   ; allot

x0f66   DEFW    L0460                   ; here
        DEFW    L0E1F                   ; 1-
        DEFW    L08A5                   ; C!
        DEFW    L04B6                   ; exit

; ----------------
; THE 'ALLOT' WORD
; ----------------
; (n -- )
; Encloses n bytes in the dictionary, without initializing them.

L0F6E:  DEFM    "ALLO"                  ; 'name field'
        DEFB    'T' + $80

        DEFW    L0F5E                   ; 'link field'

L0F75:  DEFB    $05                     ; 'name length field'

L0F76:  DEFW    L0F78                   ; 'code field'

; ---

L0F78:  CALL    L084E                   ; stk_to_bc
        LD      HL,($3C37)              ; STKBOT
        CALL    L0F9E                   ; routine MAKE ROOM
        JP      (IY)                    ; to 'next'.

; --------------------------
; The 'allot2' Internal Word
; --------------------------
; Encloses 2 bytes in the dictionary, without initializing them.

L0F83:  DEFW    L0EC3                   ; headerless 'code field' - docolon

; ---

L0F85:  DEFW    L104B                   ; stk_data
        DEFB    $02                     ; two bytes required
        DEFW    L0F76                   ; allot
        DEFW    L04B6                   ; exit

; ----------------------------------
; THE 'DEFAULT MEMORY CHECK' ROUTINE
; ----------------------------------
; called each cycle in slow mode to check free memory.

L0F8C:  LD      HL,$001E                ; Allow a thirty byte overhead.

; ----------------------------------
; THE 'CHECK FREE MEMORY' SUBROUTINE
; ----------------------------------

L0F8F:  PUSH    BC                      ; save bytes to check.

        ADD     HL,BC                   ;
        LD      BC,($3C3B)              ; SPARE
        ADD     HL,BC                   ; carry indicates error - past 65535

        POP     BC                      ; restore number of bytes
        JR      C,L0F9C                 ; forward with error

        SBC     HL,SP                   ; now check against the return stack
                                        ; (machine stack)
        RET     C                       ; return if value is less

L0F9C:  RST     20H                     ; Error 1
        DEFB    $01                     ; Not enough memory

; --------------------------
; THE 'MAKE ROOM' SUBROUTINE
; --------------------------

L0F9E:  EX      DE,HL                   ; first new location to DE
        LD      HL,$0028                ; overhead 40 bytes.

L0FA2:  CALL    L0F8F                   ; check free memory.

; now increase the two data stack pointers.

        LD      HL,($3C37)              ; fetch value of STKBOT
        ADD     HL,BC                   ; add required room.
        LD      ($3C37),HL              ; update STKBOT.

        LD      HL,($3C3B)              ; fetch value of SPARE
        PUSH    HL                      ; take a copy of 'old' value
        ADD     HL,BC                   ; add required room.
        LD      ($3C3B),HL              ; update SPARE.

        EX      (SP),HL                 ; new SPARE value to stack,
                                        ; old SPARE value to HL.
        PUSH    HL                      ; push old SPARE value.
        AND     A                       ; clear carry.

        SBC     HL,DE                   ; get length of stack and 12
        LD      B,H                     ;
        LD      C,L                     ;
        POP     HL                      ; old spare
        POP     DE                      ; new spare
        RET     Z                       ; return if same.

; else new SPARE must be higher than old spare.

        DEC     HL                      ; point to end of data stack
        DEC     DE                      ; adjust destination.
        LDDR                            ; copy the Data Stack + gap upwards.

L0FC2:  INC     HL                      ; point to first new location.

        RET                             ; return.

; -------------------
; THE 'VARIABLE' WORD
; -------------------
; VARIABLE name
; (n -- )
; Sets up a variable with the given name, and initializes its value to n.

L0FC4:  DEFM    "VARIABL"               ; 'name field'
        DEFB    'E' + $80

        DEFW    L0F75                   ; 'link field'

L0FCE:  DEFB    $08                     ; 'name length field'

        DEFW    L1085                   ; 'code field' - create and enclose

; ---

L0FD1:  DEFW    L0FF0                   ; push word DE
        DEFW    L0F4E                   ; ,

        DEFW    L04B6                   ; exit

; -------------------
; THE 'CONSTANT' WORD
; -------------------
; CONSTANT name
; (n -- )
; Defines a constant with the given name and value n.

L0FD7:  DEFM    "CONSTAN"               ; 'name field'
        DEFB    'T' + $80

        DEFW    L0FCE                   ; 'link field'

L0FE1:  DEFB    $08                     ; 'name length field'

L0FE2:  DEFW    L1085                   ; 'code field' - create and enclose

; ---

L0FE4:  DEFW    L0FF5                   ; pad??
        DEFW    L0F4E                   ; ,
        DEFW    L04B6                   ; exit

; ---
; ???

x0fea   DEFB    $DC                     ;;
x0feb   DEFB    $FE                     ;;  0feb + fedc = 0Ec7 = CREATE

; ->
L0FEC:  JR      L0FF0                   ; skip forward

x0fee   DEFB    $D5                     ;;
x0fef   DEFB    $FF                     ;;  0fef + ffd5 = 0fc4 = VARIABLE

; ---

L0FF0:  RST     10H                     ; push word DE
        JP      (IY)                    ; to 'next'.

; ---

x0FF3   DEFB    $E3                     ;;
x0ff4   DEFB    $FF                     ;;  0ff4 + ffe3 = 0fd7 = CONSTANT

; --> pad

L0FF5:  EX      DE,HL
        LD      E,(HL)
        INC     HL
        LD      D,(HL)
        RST     10H                     ; push word DE
        JP      (IY)                    ; to 'next'.

; ------------------
; THE 'LITERAL' WORD
; ------------------
; (n -- )
; Compiles the top of the stack into a word definition as a literal.
; Compiles integers. decimal 4102 = $1006. c.f. $1055

L0FFC:  DEFM    "LITERA"                ; 'name field'
        DEFB    'L' + $80

        DEFW    L0FE1                   ; 'link field'

L1005:  DEFB    $47                     ; 'name length field'

L1006:  DEFW    L1108                   ; 'code field' - compile

; ---

L1008:  DEFW    L1011                   ; stack next word
        DEFW    L0F4E                   ; ,
        DEFW    L04B6                   ; exit

; ---

x100E:  DEFB    $02                     ;;
x100f   DEFB    $FF                     ;; 100f + ff02 = 0f11 nah!
x1010   DEFB    $FF                     ;;

; -----------------------------------
; The 'Stack Next Word' Internal Word
; -----------------------------------

L1011:  DEFW    L1013                   ; headerless 'code field'

; ---

L1013:  LD      B,$01                   ; counter - one word to push

L1015:  POP     HL                      ; drop the 'Next Word' pointer.
        LD      E,(HL)                  ; low byte to E.
        INC     HL                      ; increment pointer.
        LD      D,(HL)                  ; high byte to D.

; -> E B=1 (one byte op)

L1019:  INC     HL                      ; increment the 'Next Word' pointer

L101A:  PUSH    HL                      ; the 'Next Word' pointer goes to
                                        ; the Return Stack.
        RST     10H                     ; stack Data Word DE
        DJNZ    L1015                   ; loop back if more than one.

L101E:  JP      (IY)                    ; to 'next'.


; ----------------
; THE 'ASCII' WORD
; ----------------
; Takes the next word from the input buffer, and yields the ASCII code
; of its first character. If compiling, then compiles this as a literal.
;
; e.g.      :STARS 0 DO ASCII * EMIT LOOP ;
; (--ASCII code)         (if interpreting)
; (--)                   (if compiling)

L1020:  DEFM    "ASCI"                  ; 'name field'
        DEFB    'I' + $80

        DEFW    L1005                   ; 'link field'

L1027:  DEFB    $45                     ; 'name length field' (immediate mode)

L1029:  DEFW    L0EC3                   ; 'code field' - docolon

; ----------------

L102A:  DEFW    L104B                   ; stk_data
        DEFB    $20                     ; space delimiter
        DEFW    L05AB                   ; word  to pad
        DEFW    L0E09                   ; 1+
        DEFW    L0896                   ; C@
        DEFW    L1A0E                   ; end-forth.

        BIT     6,(IX+$3E)              ; FLAGS
        JR      Z,L101E                 ; back to a jp (iy)

        CALL    L04B9                   ; forth

L103E:  DEFW    L1011                   ; stack next word
        DEFW    L104B                   ; (stk_data)
        DEFW    L0F4E                   ; ,
        DEFW    L0F5F                   ; c,
        DEFW    L04B6                   ; exit

; ---

x1048   DEFB    $01                     ;; ?

x1049   DEFB    $D6                     ;; ?
x104a   DEFB    $FF                     ;; ?  104a + ffd6 = 1020 = ASCII

; ----------------------------
; The 'stk-data' Internal Word
; ----------------------------
; used succinctly to stack the following byte as a word.

L104B:  DEFW    L104D                   ; headerless 'code field'

; ---

L104D:  POP     HL                      ; retrieve the 'Next Word' pointer.

        LD      E,(HL)                  ; fetch the single byte from there.
        LD      D,$00                   ; set high order byte to zero.

        LD      B,$01                   ; set counter to 1.

        JR      L1019                   ; back to stack one word and
                                        ; put the incremented pointer back on
                                        ; the Return Stack.

; --------------------------
; The 'stk_fp' Internal Word
; --------------------------
; stack and enclose a floating point number - two words.

L1055:  DEFW    L1108                   ; headerless 'code field' - compile

; ---

        DEFW    L1064                   ; stack two words.
        DEFW    L0885                   ; swap
        DEFW    L0F4E                   ; ,
        DEFW    L0F4E                   ; ,
        DEFW    L04B6                   ; exit
; ---

x1061   DEFB    $04                     ;;
x1062   DEFB    $FF                     ;; 1062 + ff04 = 0f66 XX
x1063   DEFB    $FF                     ;;

; -----------------------------------
; The 'STACK TWO WORDS' Internal Word
; -----------------------------------

L1064:  DEFW    L1066                   ; headerless 'code field'

; ---

L1066:  LD      B,$02                   ; set counter to two

        JR      L1015                   ; back to stack 2 words


; -----------------
; THE 'DEFINER' WORD
; -----------------
; Used with 'DOES>' to define new defining words. i.e. words that themselves
; define new words.
; The format is
; DEFINER name
;       defining routine
; DOES>
;       action routine
; ;
; name is the name of the new defining word; when executed it will set up
; the header of a new word and use its defining routine to set up the
; parameter field. When this new word in its turn is executed, its parameter
; field will be put on the stack and the action routine will be executed.

L106A:  DEFM    "DEFINE"                ; 'name field'
        DEFB    'R' + $80

        DEFW    L1027                   ; 'link field'

L1073:  DEFB    $07                     ; 'name length field'

L1074:  DEFW    L1085                   ; 'code field' - create and enclose

; ---

L1076:  DEFW    L1085                   ; create and enclose
        DEFW    L0460                   ; here

        DEFW    L104B                   ; stk-data

        DEFB    $0C                     ; 12                    marker byte

        DEFW    L0F83                   ; allot2
        DEFW    L1276                   ; branch
L1081:  DEFW    $FE34                   ; back to L0EB6

; ---

x1083   DEFB    $E6                     ;;
x1084   DEFB    $FF                     ;; 1084 + ffe6 = 106a = DEFINER

; ---
;; createe and fill
; ----
; used seven times as a code word.

L1085:  CALL    L0FF0                   ; push word DE (save addr nxt wrd on DS)

        DEFW    L0ED0                   ; create
        DEFW    L086B                   ; dup
        DEFW    L08B3                   ; @
        DEFW    L0460                   ; here
        DEFW    L0E29                   ; 2-
        DEFW    L08C1                   ; !

L1094:  DEFW    L0E13                   ; 2+
        DEFW    L109A                   ; pop DE
        DEFW    L04B6                   ; exit

; -----------
; pop word DE
; -----------
; branch to addr on stack???

L109A:  DEFW    L109C                   ; headerless 'code field'

; ---

L109C:  RST     18H                     ; unstack Data Word DE

        JP      L0EC3                   ; start new thread.

; ---------------
; THE 'CALL' WORD
; ---------------
; (address -- )
; Executes Z80 machine code at address on the stack. The code is terminated
; by a jp (iy)
; e.g. in hex
; DEFINER CODE DOES> CALL ;
; CODE EI FB C, FD C, E9 C,
; The word EI will enable interrupts.

L10A0:  DEFM    "CAL"                   ; 'name field'
        DEFB    'L' + $80

        DEFW    L1073                   ; 'link field'

L10A6:  DEFB    $04                     ; 'name length field'

L10A7:  DEFW    L10A9                   ; 'code field'

; ---

L10A9:  RST     18H
        EX      DE,HL

        JP      (HL)

; ----------------
; THE 'DOES>' WORD
; ----------------
; See DEFINER.

L10AC:  DEFM    "DOES"                  ; 'name field'
        DEFB    '>' + $80

        DEFW    L10F4                   ; 'link field'

L10B3:  DEFB    $45                     ; 'name length field' (immediate mode)

L10B4:  DEFW    L1108                   ; 'code field' - compile

L10B6:  DEFW    L10E8                   ; exit

        DEFW    L12D8                   ; check??

        DEFB    $0C                     ; 12

        DEFW    L10CD                   ;
        DEFW    L104B                   ; stk_data

        DEFB    $CD                     ; data                  call ?

        DEFW    L0F5F                   ; C,
        DEFW    L1011                   ; stack next word
        DEFW    L0FF0                   ; (push word DE)
        DEFW    L0F4E                   ; ,
        DEFW    L104B                   ; stk-data

        DEFB    $0A                     ; ten                   marker byte.

        DEFW    L04B6                   ; exit

; -----------------------
; The '???' Internal Word
; -----------------------

L10CD:  DEFW    L0EC3                   ; headerless 'code field' - docolon

; ---

        DEFW    L086B                   ; dup
        DEFW    L0E29                   ; 2-
        DEFW    L15B5                   ; namefield
        DEFW    L0460                   ; here
        DEFW    L0DE1                   ; -
        DEFW    L0E1F                   ; 1-
        DEFW    L0F4E                   ; ,
        DEFW    L0460                   ; here
        DEFW    L0885                   ; swap
        DEFW    L08C1                   ; !
        DEFW    L04B6                   ; exit

; ---

x10e5   DEFB    $05                     ;;

x10e6   DEFB    $C5                     ;;
x10e7   DEFB    $FF                     ;; 10e7 + ffc5 = 10ac = DOES>

; ---

L10E8:  DEFW    L04B8                   ; exit?

; -------------------
; THE 'COMPILER' WORD
; -------------------
; Used with 'RUNS>' for defining new compiling words, i.e. words that are
; used within word definitions to give an immediate effect of compiling
; some information into the dictionary.
; (This is traditionally done with IMMEDIATE, but COMPILER...RUNS> works
; better with EDIT etc.)

L10EA:  DEFM    "COMPILE"               ; 'name field'
        DEFB    'R' + $80

        DEFW    L10A6                   ; 'link field'

L10F4:  DEFB    $08                     ; 'name length field'

L10F5:  DEFW    L1085                   ; 'code field' - create and enclose

; ---

        DEFW    L1108                   ; compile
        DEFW    L1160                   ; immediate
        DEFW    L0460                   ; here
        DEFW    L104B                   ; stk_data

L10FF:  DEFB    $0B                     ; 11                    marker byte

        DEFW    L0F83                   ; allot2
        DEFW    L1276                   ; branch
L1104:  DEFW    $FDB1                   ; back to L0EB6

; ---

x1106   DEFB    $E3                     ;;
x1107   DEFB    $FF                     ;; 1107 + ffe3 = 10ea = COMPILER

; ---------------------
; THE 'COMPILE' ROUTINE
; ---------------------
; Instead of executing code words as they are encountered, lay them down in
; the dictionary along with any parameters.

L1108:  BIT     6,(IX+$3E)              ; test FLAGS - compiler mode ?
        JR      NZ,L1110                ; skip error if so.

        RST     20H                     ; Error 4.
        DEFB    $04                     ; Compiling word used in interpret mode.

L1110:  CALL    L0FF0                   ; push word DE (then jp (iy))

        DEFW    L086B                   ; dup
        DEFW    L08B3                   ; @
        DEFW    L0F4E                   ; ,
        DEFW    L1276                   ; branch
L111B:  DEFW    $FF78                   ; to L1094 - definer code

; ----------------
; THE 'RUNS>' WORD
; ----------------
; See COMPILER

L111D:  DEFM    "RUNS"                  ; 'name field'
        DEFB    '>' + $80

        DEFW    L10B3                   ; 'link field'

L1124:  DEFB    $45                     ; 'name length field' (immediate mode)

L1125:  DEFW    L1108                   ; 'code field' - compile

; ---

L1127:  DEFW    L1140                   ; vv
        DEFW    L12D8                   ; check-for
        DEFB    $0B                     ; 11                    marker byte.
        DEFW    L0885                   ; swap
        DEFW    L0F5F                   ; c,
        DEFW    L10CD                   ; ?
        DEFW    L1011                   ; stack next word
        DEFW    L1142
        DEFW    L0F4E                   ; ,

        DEFW    L104B                   ; stk-data
        DEFB    $0A                     ; ten.                  marker byte.
        DEFW    L04B6                   ; exit

; ---

x113d   DEFB    $05                     ;;

x113e   DEFB    $DE                     ;;
x113f   DEFB    $FF                     ;; 113f + ffde = 111d = RUNS>

; ---

L1140:  DEFW    L04B8

L1142:  POP     HL
        PUSH    DE
        EX      DE,HL

        RST     10H                     ; push word DE
        LD      B,D
        LD      C,E
        POP     DE
        PUSH    DE
        DEC     DE
        DEC     DE

        CALL    L159E                   ;

        POP     DE
        PUSH    BC
        JP      L0EC3                   ;

; --------------------
; THE 'IMMEDIATE' WORD
; --------------------
; (  --  )
; The most recent word in the current vocabulary is made immediate, so that
; it will execute even in compile mode.

L1154:  DEFM    "IMMEDIAT"              ; 'name field'
        DEFB    'E' + $80

        DEFW    L1124                   ; 'link field'

L115F:  DEFB    $09                     ; 'name length field'

L1160:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L1162:  DEFW    L0480                   ; current
        DEFW    L08B3                   ; @
        DEFW    L08B3                   ; @
        DEFW    L1A0E                   ; end-forth.

L116A:  RST     18H                     ; pop word DE
        EX      DE,HL
        SET     6,(HL)
        JP      (IY)                    ; to 'next'.

; ---------------------
; THE 'VOCABULARY' WORD
; ---------------------
; (  --  )
; Defines a new vocabulary with the given name.

L1170:  DEFM    "VOCABULAR"             ; 'name field'
        DEFB    'Y' + $80

        DEFW    L115F                   ; 'link field'

L117C:  DEFB    $0A                     ; 'name length field'

L117D:  DEFW    L1085                   ; 'code field' - create and enclose

; ---

L117F:  DEFW    L11B5                   ; set context
        DEFW    L0480                   ; current
        DEFW    L08B3                   ; @
        DEFW    L0E13                   ; 2+
        DEFW    L0F4E                   ; ,
        DEFW    L0688                   ; stk-zero
        DEFW    L0F5F                   ; C,
        DEFW    L0460                   ; here
        DEFW    L1011                   ; stack next word
        DEFW    $3C35                   ; (VOCLNK)
        DEFW    L086B                   ; dup
        DEFW    L08B3                   ; @
        DEFW    L0F4E                   ; ,
        DEFW    L08C1                   ; !
        DEFW    L04B6                   ; exit

; ----------------------
; THE 'DEFINITIONS' WORD
; ----------------------
; (  --  )
; The CONTEXT vocabulary is made the CURRENT vocabulary as well.

L119D:  DEFM    "DEFINITION"            ; 'name field'
        DEFB    'S' + $80

        DEFW    L117C                   ; 'link field'

L11AA:  DEFB    $0B                     ; 'name length field'

L11AB:  DEFW    L11AD                   ; 'code field'

; ---

L11AD:  LD      HL,($3C33)              ; CONTEXT
        LD      ($3C31),HL              ; CURRENT
        JP      (IY)                    ; to 'next'.

; ---

L11B5:  LD      ($3C33),DE              ; CONTEXT
        JP      (IY)                    ; to 'next'.

; ---

; -------------
; THE 'IF' WORD
; -------------
; (n -- )
; Used in the form
; IF ... THEN
; or
; IF ... ELSE ... THEN
; In the first form, if n is non-zero then the words between IF and THEN
; are executed; otherwise they are skipped over.
; In the second form, if n is non-zero then the words between IF and ELSE
; are executed and those between ELSE and THEN are skipped over, while if
; n is zero then the words between IF and ELSE are skipped over and those
; between ELSE and THEN are executed.

L11BB:  DEFB    'I'                     ; 'name field'
        DEFB    'F' + $80

        DEFW    L13E0                   ; 'link field'

L11BF:  DEFB    $42                     ; 'name length field' (immediate word)

        DEFW    L1108                   ; 'code field' - compile

; ---

        DEFW    L1283                   ; ?branch
        DEFW    L0460                   ; here

        DEFW    L104B                   ; stk_data
        DEFB    $02                     ; 2 locations required for jump length
        DEFW    L0F83                   ; allot2
        DEFW    L04B6                   ; exit

; ----------------
; THE 'WHILE' WORD
; ----------------
; (n -- )
; Used in BEGIN ... WHILE ... REPEAT. If n = 0 then skips over to just past
; REPEAT.

L11CD:  DEFM    "WHIL"                  ; 'name field'
        DEFB    'E' + $80

        DEFW    L11BF                   ; 'link field'

L11D4:  DEFB    $45                     ; 'name length field' (immediate mode)

L11D5:  DEFW    L1108                   ; 'code field' - compile

; ---

        DEFW    L1288                   ; ?branch

        DEFW    L12D8                   ; check-for
        DEFB    $01                     ;  1
        DEFW    L0460                   ; here
        DEFW    L104B                   ; stk-data
        DEFB    $04                     ;  four
        DEFW    L0F83                   ; allot
        DEFW    L04B6                   ; exit

; ---------------
; THE 'ELSE' WORD
; ---------------
; (  --  )
; Used with IF and THEN.

L11E5:  DEFM    "ELS"                   ; 'name field'
        DEFB    'E' + $80

        DEFW    L11D4                   ; 'link field'

L11EB:  DEFB    $44                     ; 'name length field' (immediate mode)

L11EC:  DEFW    L1108                   ; 'code field' - compile

; ---

        DEFW    L1271                   ; branch

        DEFW    L12D8                   ; check-for
        DEFB    $02                     ; two
        DEFW    L0F83                   ; allot2
        DEFW    L1225                   ; ?
        DEFW    L0460                   ; here
        DEFW    L0E29                   ; 2-
        DEFW    L104B                   ; stk-data
        DEFB    $02                     ; two
        DEFW    L04B6                   ; exit

; ---------------
; THE 'THEN' WORD
; ---------------
; Used with IF.

L1200:  DEFM    "THE"                   ; 'name field'
        DEFB    'N' + $80

        DEFW    L11EB                   ; 'link field'

L1206:  DEFB    $44                     ; 'name length field' (immediate mode)

L1207:  DEFW    L1108                   ; 'code field' - compile

; ---

        DEFW    L12A4                   ; end?

        DEFW    L12D8                   ; check-for
        DEFB    $02
        DEFW    L1225                   ; ?
        DEFW    L04B6                   ; exit

; ---------------
; THE 'BEGIN' WORD
; ---------------
; (  --  )
; Used with either UNTIL or WHILE...REPEAT.

L1212:  DEFM    "BEGI"                  ; 'name field'
        DEFB    'N' + $80

        DEFW    L1206                   ; 'link field'

L1219:  DEFB    $45                     ; 'name length field' (immediate mode)

L121A:  DEFW    L1108                   ; 'code field' - compile

; ---

        DEFW    L129F
        DEFW    L0460                   ; here
        DEFW    L104B                   ; stk_data
        DEFB    $01                     ; 1
        DEFW    L04B6                   ; exit

; -----------------------
; The '???' Internal Word
; -----------------------

L1225:  DEFW    L0EC3                   ; headerless 'code field' - docolon

; ---

        DEFW    L086B                   ; dup
        DEFW    L0460                   ; here
        DEFW    L0885                   ; swap
        DEFW    L0DE1                   ; -
        DEFW    L0E1F                   ; 1-
        DEFW    L0885                   ; swap
        DEFW    L08C1                   ; !
        DEFW    L04B6                   ; exit

; -----------------------
; The '???' Internal Word
; -----------------------

L1237:  DEFW    L0EC3                   ; headerless 'code field' - docolon

; ---

        DEFW    L0460                   ; here
        DEFW    L0DE1                   ; -
        DEFW    L0E1F                   ; 1-
        DEFW    L0F4E                   ; ,
        DEFW    L04B6                   ; exit


; -----------------
; THE 'REPEAT' WORD
; -----------------
; (  --  )
; Used in construction BEGIN ... WHILE .. REPEAT.
; Causes a jump back to just after BEGIN.


L1243:  DEFM    "REPEA"                 ; 'name field'
        DEFB    'T' + $80

        DEFW    L1219                   ; 'link field'

L124B:  DEFB    $46                     ; 'name length field' (immediate mode)

L124C:  DEFW    L1108                   ; 'code field' - compile

; ---

L124E   DEFW    L1276                   ; branch
L1250:  DEFW    L12D8                   ; check_for
        DEFB    $04                     ; four
        DEFW    L0885                   ; swap
        DEFW    L1237                   ; ?
        DEFW    L1225                   ; ?
        DEFW    L04B6                   ; exit

; ----------------
; THE 'UNTIL' WORD
; ----------------
; (n -- )
; Used in BEGIN ... UNTIL.
; Loops back to BEGIN if n = 0

L125B:  DEFM    "UNTI"                  ; 'name field'
        DEFB    'L' + $80

        DEFW    L124B                   ; 'link field'

L1262:  DEFB    $45                     ; 'name length field' (immediate mode)

L1263:  DEFW    L1108                   ; 'code field' - compile

; ---

        DEFW    L128D                   ; ?branch
        DEFW    L12D8                   ; check_for
        DEFB    $01                     ;
        DEFW    L1237                   ; ?
        DEFW    L04B6                   ; exit

; ---

x126E   DEFB    $02                     ;;

x126F   DEFB    $75                     ;;
x1270   DEFB    $FF                     ;; 1270 + ff75 = 11e5 = ELSE

; ---


L1271:  DEFW    L1278                   ; ?

; ---
x1273   DEFB    $02                     ;;

x1274   DEFB    $CE                     ;;
x1275   DEFB    $FF                     ;; 1275 + ffce = 1243 = REPEAT

; --------------------------
; The 'branch' Internal Word
; --------------------------

L1276:  DEFW    L1278                   ; headerless 'code field'

; ---

L1278:  POP     HL                      ; drop next word pointer
        LD      E,(HL)                  ; read the 16-bit offset
        INC     HL                      ; that is
        LD      D,(HL)                  ; stored there.

L127C:  ADD     HL,DE                   ; add to current address.

        JP      L04BA                   ; jump back into address loop so that
                                        ; a new address gets stacked as IP.

; ---

x1280   DEFB    $02                     ;;

x1281   DEFB    $39                     ;;
x1282   DEFB    $FF                     ;; 1282 + ff39 = 11bb = IF

; ---

L1283:  DEFW    L128F                   ; from IF, convert, line, min, etc.

; ---

x1285   DEFB    $02                     ;;

x1286   DEFB    $46                     ;;
x1287   DEFB    $FF                     ;; 1287 + ff46 = 11cd = WHILE

; ---

L1288:  DEFW    L128F                   ; from WHILE

; ---

x128A   DEFB    $02                     ;;

x128B   DEFB    $CF                     ;;
x128C   DEFB    $FF                     ;; 128c + ffcf = 125b = UNTIL

; ---------------------------
; The '?branch' Internal Word
; ---------------------------

L128D:  DEFW    L128F                   ; headerless 'code field'

; ---

L128F:  CALL    L084E                   ; stk_to_bc

        LD      A,B                     ; test for
        OR      C                       ; zero

; -> from +loop
L1294:  JR      Z,L1278                 ; make the jump to "branch" if zero.

        POP     HL                      ; else drop the pointer.
        INC     HL                      ; step over.
        INC     HL                      ; the jump bytes
        JP      L04BA                   ; jump back into address loop so that
                                        ; a new address gets stacked as IP.

; ---

x129C   DEFB    $00                     ;;
x129D   DEFB    $74                     ;;
x129E   DEFB    $FF                     ;; 129e + ff74 = 1212 = BEGIN

; ---

L129F:  DEFW    L04B9                   ; forth

; ---

x12A1   DEFB    $00                     ;;
x12A2   DEFB    $5D                     ;;
x12A3   DEFB    $FF                     ;; 12a3 + ff5d = 1200 = THEN

; ---

L12A4:  DEFW    L04B9

; -------------
; THE 'DO' WORD
; -------------
; (limit, initial value -- )
; Sets up a DO loop, initializing the loop counter to the initial value.
; The limit and loop counter are stored on the return stack.
; See LOOP and +LOOP.

L12A6:  DEFB    'D'                     ; 'name field'
        DEFB    'O' + $80

        DEFW    L1262                   ; 'link field'

L12AA:  DEFB    $42                     ; 'name length field' (immediate mode)

L12AB:  DEFW    L1108                   ; 'code field' - compile

; ---

        DEFW    L1323                   ; shuffle
        DEFW    L0460                   ; here
        DEFW    L104B                   ; stk_data
        DEFB    $03                     ; 3                     marker byte.
        DEFW    L04B6                   ; exit

; ---------------
; THE 'LOOP' WORD
; ---------------
; (  --  )
; Like +LOOP (below) but the number added onto the loop counter is 1.

L12B6:  DEFM    "LOO"                   ; 'name field'
        DEFB    'P' + $80

        DEFW    L12AA                   ; 'link field'

L12BC:  DEFB    $44                     ; 'name length field' (immediate mode)

L12BD:  DEFW    L1108                   ; 'code field' - compile

; ---

        DEFW    L1332                   ; shuffle more

L12C1:  DEFW    L12D8                   ; check-for
        DEFB    $03                     ; 3                     marker byte
        DEFW    L1237                   ; ?
        DEFW    L04B6                   ; exit

; ----------------
; THE '+LOOP' WORD
; ----------------
; (n -- )
; Used with DO. Adds n to the loop counter, and loops back if the loop counter
; is now less than the limit (if n >= 0) or greater than the limit (if n < 0).

L12C8:  DEFM    "+LOO"                  ; 'name field'
        DEFB    'P' + $80

        DEFW    L12BC                   ; 'link field'

L12CF:  DEFB    $45                     ; 'name length field' (immediate mode)

L12D0:  DEFW    L1108                   ; 'code field' - compile

; ---

L12D2:  DEFW    L133C                   ; ?
        DEFW    L1276                   ; branch

L12D6:  DEFW    $FFEA                   ; back to L12C1

; -----------------------------
; The 'check-for' Internal Word
; -----------------------------
; Checks for expected marker byte which indicates stack is balanced and that
; a previous mandatory word was present.

L12D8:  DEFW    L12DA                   ; headerless 'code field'

; ---

L12DA:  RST     18H                     ; pop word DE
        POP     HL                      ;
        LD      A,(HL)                  ;
        INC     HL                      ;
        PUSH    HL                      ;
        SUB     E                       ;
        OR      D                       ;

        JR      Z,L132D                 ; to next via jp (iy).

; else...

        RST     20H                     ; Error 5
        DEFB    $05                     ; Word is not properly structured.

; ------------
; THE 'I' WORD
; ------------
; ( -- loop counter)
; Copies the top of the return stack to the data stack. This will be either
; the loop counter for the innermost DO...LOOP, or the number most recently
; transferred by >R.


L12E5:  DEFB    'I' + $80               ; 'name field'

        DEFW    L11AA                   ; 'link field'

L12E8:  DEFB    $01                     ; 'name length field'

L12E9:  DEFW    L12EB                   ; 'code field'

; ---

L12EB:  POP     BC                      ; pop return address
        POP     DE                      ; pop the loop counter to DE.
        PUSH    DE                      ; now restore the stack
        PUSH    BC                      ; exactly as it was.

        RST     10H                     ; push Data Word DE - inner loop counter

        JP      (IY)                    ; to 'next'.

; -------------
; THE 'I'' WORD
; -------------
; ( -- limit)
; Copies the second number down on the return stack to the data stack
; (so in a DO loop it copies  the limit of the loop).

L12F2:  DEFB    'I'                     ; 'name field'
        DEFB    $A7                     ; "'" + $80

        DEFW    L12E8                   ; 'link field'

L12F6:  DEFB    $02                     ; 'name length field'

L12F7:  DEFW    L12F9                   ; 'code field'

; ---

L12F9:  LD      HL,$0004                ; two bytes per entry.
        JR      L1307                   ; forward to use the 'J' indexing
                                        ; routine

; ------------
; THE 'J' WORD
; ------------
; ( -- loop counter)
; Copies the third entry on the return stack to the data stack.
; This will be either the loop counter for the second innermost DO loop
; or the number put on the return stack by the most recent >R.

L12FE:  DEFB    'J' + $80               ; 'name field'

        DEFW    L12F6                   ; 'link field'

L1301:  DEFB    $01                     ; 'name length field'

L1302:  DEFW    L1304                   ; 'code field'

; ---

L1304:  LD      HL,$0006                ; two bytes per entry

; -> I' joins here with HL=4

L1307:  ADD     HL,SP                   ; index the stack pointer.
        LD      E,(HL)                  ; low order byte to E
        INC     HL                      ; address high byte.
        LD      D,(HL)                  ; DE now holds a copy of the required
                                        ; entry from the Return Stack

        RST     10H                     ; stack Data Word DE

        JP      (IY)                    ; to 'next'.

; ----------------
; THE 'LEAVE' WORD
; ----------------
; (  --  )
; Forces termination of a DO loop at the next LOOP or +LOOP by setting the
; loop counter equal to the limit.

L130E:  DEFM    "LEAV"                  ; 'name field'
        DEFB    'E' + $80

        DEFW    L1301                   ; 'link field'

L1315:  DEFB    $05                     ; 'name length field'

L1316:  DEFW    L1318                   ; 'code field'

; ---

L1318:  POP     BC                      ; pop return address to BC.
        POP     HL                      ; pop the loop counter.
        POP     HL                      ; now the limit.
        PUSH    HL                      ; push unaltered limit.
        PUSH    HL                      ; push counter - now limit.
        PUSH    BC                      ; restore return address.

        JP      (IY)                    ; to 'next'.

; ---


x1320   DEFB    $00                     ;;
x1321   DEFB    $84                     ;;
x1322   DEFB    $FF                     ;; 1322 + ff84 = 12a6 = DO

; -----------------------
; The '???' Internal Word
; -----------------------

L1323:  DEFW    L1325                   ; headerless 'code field'

; ---

L1325:  CALL    L084E                   ; stk_to_bc
        RST     18H                     ; pop word DE
        POP     HL
        PUSH    DE

L132B:  PUSH    BC
        PUSH    HL

L132D:  JP      (IY)                    ; to 'next'.

; ---

x132F   DEFB    $02                     ;;
x1330   DEFB    $85                     ;;
x1331   DEFB    $FF                     ;; 1331 + ff85 = 12b6 = LOOP

; -----------------------
; The '???' Internal Word
; -----------------------

L1332:  DEFW    L1334                   ; headerless 'code field'

; ---

L1334:  LD      DE,$0001
        JR      L133F                   ; forward =>

; ---

x1339   DEFB    $02
x133A   DEFB    $8D
x133B   DEFB    $FF

; -----------------------
; The '???' Internal Word
; -----------------------
; loop counter + n
; Note. ADC HL,DE is used in preference to ADD HL,DE as affects P/O flag

L133C:  DEFW    L133E                   ; headerless 'code field'

; ---

L133E:  RST     18H                     ; pop word DE - number to be added (n)
; =>
L133F:  POP     BC                      ; pop return address to BC.
        POP     HL                      ; loop counter to HL.
        AND     A                       ; clear carry.
        ADC     HL,DE                   ; add the number specified.
        LD      A,D                     ; save MSB of (n) in A.
        POP     DE                      ; now pop the limit to DE.
        SCF                             ; set carry.
        JP      PE,L1358                ; jump forward with overflow.

        PUSH    DE                      ; push limit
        PUSH    HL                      ; push adjusted counter.
        RLCA                            ; now test sign of number (n)
        JR      NC,L1350                ;

        EX      DE,HL

L1350:  CALL    L0C99                   ;

        CCF

        JR      NC,L1358                ;

        POP     HL
        POP     HL

L1358:  PUSH    BC
        SBC     A,A
        JP      L1294                   ; jump to branch on zero.

; ------------
; THE '(' WORD
; ------------
; Starts a comment terminated by ')'

L135D:  DEFB    '(' + $80               ; 'name field'

        DEFW    L13D4                   ; 'link field'

L1360:  DEFB    $41                     ; 'name length field' (immediate mode)

L1361:  DEFW    L1108                   ; 'code field' - compile

; ---

L1363:  DEFW    L1379                   ;
        DEFW    L104B                   ; stk_data

        DEFB    $29                     ; character ')'         - delimiter

L1368:  DEFW    L0460                   ; here
        DEFW    L0885                   ; swap
        DEFW    L0F83                   ; allot2
        DEFW    L139F                   ; find)
        DEFW    L0885                   ; swap
        DEFW    L08C1                   ; !

        DEFW    L04B6                   ; exit

; ---

x1376   DEFB    $FF                     ;;
x1377   DEFB    $E5                     ;;
x1378   DEFB    $FF                     ;; 1378 + ffe5 = 135d = '('

; -----------------------
; The '???' Internal Word
; -----------------------

L1379:  DEFW    L137B                   ; headerless 'code field'

; ---

L137B:  POP     HL
        LD      E,(HL)
        INC     HL
        LD      D,(HL)

        INC     DE

        JP      L127C                   ;

; -------------
; THE '."' WORD
; -------------
; (  --  )
; Prints the following string terminated by ".

L1383:  DEFB    '.'                     ; 'name field'
        DEFB    '"' + $80

        DEFW    L1360                   ; 'link field'

L1387:  DEFB    $42                     ; 'name length field' (immediate mode)

L1388:  DEFW    L1108                   ; 'code field' - compile

; ---

L138A:  DEFW    L1396                   ; pr_embedded string.
        DEFW    L104B                   ; stk_data
        DEFB    $22                     ; '"'                   - delimiter

        DEFW    L1276                   ; branch
L1391:  DEFW    $FFD6                   ; back to 1368 (1392+$FFD6)
                                        ; same routine as for matching comments

; ---

x1393   DEFB    $FF                     ;;
x1394   DEFB    $EE                     ;;
x1395   DEFB    $FF                     ;; 1395 + ffee = 1383 = ."

; -----------------------
; The '???' Internal Word
; -----------------------
; print string embedded in Dictionary

L1396:  DEFW    L1398                   ; headerless 'code field'

; ---

L1398:  POP     DE
        CALL    L0979                   ; pr_string1
        PUSH    DE
        JP      (IY)                    ; to 'next'.

; -----------------------
; The '???' Internal Word
; -----------------------
; enclose comment
; comments may be multiple
; e.g. : SV ( system) ( variables) CLS BEGIN 0 0 AT 15360 80 TYPE 0 UNTIL ;


L139F:  DEFW    L13A1                   ; headerless 'code field'

; ---

L13A1:  RST     18H                     ; pop word DE
        PUSH    DE                      ; save delimiter.

        CALL    L05E1                   ; find the ')' delimiter

        LD      H,D
        LD      L,E
        ADD     HL,BC
        LD      A,(HL)
        POP     HL                      ; pop the delimiter.
        CP      L
        JR      Z,L13B8                 ; forward with a match.         =->

        EX      DE,HL                   ;
        RST     10H                     ; push word DE
        LD      DE,$0578                ; addr retype?

        CALL    L1815                   ; pr2

        JR      L13A1                   ; loop back

; ---
; =->

L13B8:  PUSH    DE
        PUSH    BC
        LD      HL,($3C37)              ; STKBOT

        CALL    L0F9E                   ; routine MAKE ROOM

        POP     BC
        POP     DE
        PUSH    DE
        PUSH    BC
        EX      DE,HL
        LDIR                            ; copy comment to dictionary.
        POP     BC
        LD      D,B
        LD      E,C
        RST     10H                     ; push word DE
        POP     DE

        CALL    L07DA                   ;

        JP      (IY)                    ; to 'next'.

; ------------
; THE '[' WORD
; ------------
; (  --  )
; Enters interpret mode.

L13D1:  DEFB    '[' + $80               ; 'name field'

        DEFW    L12CF                   ; 'link field'

L13D4:  DEFB    $41                     ; 'name length field' (immediate mode)

L13D5:  DEFW    L13D7                   ; 'code field'

; ---

L13D7:  RES     6,(IX+$3E)              ; FLAGS
        JP      (IY)                    ; to 'next'.

; ------------
; THE ']' WORD
; ------------
; (  --  )
; Enters compile mode.

L13DD:  DEFB    ']' + $80               ; 'name field'

        DEFW    L1315                   ; 'link field'

L13E0:  DEFB    $01                     ; 'name length field'

L13E1:  DEFW    L13E3                   ; 'code field'

; ---

L13E3:  SET     6,(IX+$3E)              ; FLAGS
        JP      (IY)                    ; to 'next'.


; ---------------
; THE 'EXIT' WORD
; ---------------
; (  --  )
; Exits immediately from the word in whose definition it is contained.
; Cannot be used between DO and LOOP or +LOOP, nor between >R and R>.

L13E9:  DEFM    "EXI"                   ; 'name field'
        DEFB    'T' + $80

        DEFW    L1387                   ; 'link field'

L13EF:  DEFB    $04                     ; 'name length field'

L13F0:  DEFW    L04B8                   ; 'code field'

; -------------------
; THE 'REDEFINE' WORD
; -------------------
; REDEFINE name
; (  --  )
; Takes word 'name' and replaces it with the most recent word in the
; dictionary. Updates entire dictionary to take changes into account.
; Most commonly used as
;  EDIT name
;  REDEFINE name

L13F2:  DEFM    "REDEFIN"               ; 'name field'
        DEFB    'E' + $80

        DEFW    L13EF                   ; 'link field'

L13FC:  DEFB    $08                     ; 'name length field'

L13FD:  DEFW    L13FF                   ; 'code field'

; ---

L13FF:  CALL    L0F2E                   ; blank stack

        LD      HL,($3C31)              ; CURRENT

        LD      E,(HL)
        INC     HL
        LD      D,(HL)

        EX      DE,HL                   ; transfer value to HL
        INC     HL
        LD      ($2705),HL              ; store in pad

        PUSH    HL                      ; (*)

        CALL    L15C0                   ; get 'name field' address

        LD      ($270D),HL              ; name field addr
        LD      ($2707),BC              ; parameter field addr
        LD      ($270B),DE              ; length field value

        LD      HL,($3C37)              ; STKBOT
        SBC     HL,DE
        JP      NZ,L14DA                ; forward if not matched to Error 11.

        POP     DE                      ; (*)

        RST     10H                     ; push word DE

        CALL    L04B9                   ; forth

L1429:  DEFW    L1610                   ; prvcur
        DEFW    L063D                   ; find
        DEFW    L1A0E                   ; end-forth.

; ---

L1425:  RST     18H                     ; pop word DE
        LD      HL,$C3AF
        ADD     HL,DE
        JP      NC,L14CF                ;

        EX      DE,HL
        LD      ($2703),HL

        CALL    L15C0                   ; get 'name field' address

        LD      ($2701),HL

L1441:  PUSH    HL
        LD      ($2709),DE
        LD      A,B
        OR      C
        LD      DE,($2707)
        JR      Z,L1452                 ;

        LD      A,D
        OR      E
        JR      Z,L14CF                 ;

L1452:  POP     HL
        LD      BC,($270D)
        SBC     HL,BC
        EX      DE,HL
        ADD     HL,DE
        LD      ($2707),HL
        LD      HL,($270B)
        ADD     HL,DE
        LD      BC,($2709)
        AND     A
        SBC     HL,BC
        LD      ($270B),HL
        LD      BC,$002E                ; 46d
        ADD     HL,BC
        BIT     7,H
        JR      NZ,L147F                ;

        LD      BC,($3C3B)              ; SPARE
        ADD     HL,BC
        JR      C,L14CF                 ;

        SBC     HL,SP
        JR      NC,L14CF                ;

L147F:  LD      HL,($2703)
        PUSH    HL
        DEC     HL
        DEC     HL
        LD      B,(HL)
        DEC     HL
        LD      C,(HL)
        LD      HL,($2705)
        PUSH    HL
        DEC     HL
        DEC     HL
        LD      (HL),B
        DEC     HL
        LD      (HL),C
        POP     HL
        ADD     HL,DE
        POP     BC
        AND     A
        SBC     HL,BC
        LD      ($2705),HL
        LD      DE,($2701)
        LD      HL,($2709)
        AND     A
        SBC     HL,DE
        LD      B,H
        LD      C,L
        PUSH    DE
        PUSH    BC

        CALL    L14DC                   ; RECLAIM

        LD      HL,($270B)
        POP     BC
        ADD     HL,BC
        LD      B,H
        LD      C,L
        POP     HL
        PUSH    BC

        CALL    L0F9E                   ; routine MAKE ROOM

        EX      DE,HL                   ;
        LD      HL,($270D)              ;
        LD      BC,($270B)              ;
        ADD     HL,BC                   ;
        POP     BC                      ;
        PUSH    BC                      ;
        PUSH    HL                      ;

        LDIR                            ;

        POP     DE
        POP     BC

        CALL    L14DC                   ; RECLAIM
        CALL    L14F8                   ;

        JP      (IY)                    ; to 'next'.

; ---

L14CF:  LD      HL,($3C31)              ; CURRENT
        LD      DE,($2705)
        DEC     DE
        LD      (HL),E
        INC     HL
        LD      (HL),D

L14DA:  RST     20H                     ; Error 11
        DEFB    $0B                     ; Error in REDEFINE or FORGET

; ---------------------------
; THE 'RECLAIMING' SUBROUTINE
; ---------------------------

L14DC:  LD      HL,($3C37)              ; fetch STKBOT
        AND     A                       ; clear carry flag
        SBC     HL,BC                   ; subtract number of bytes to reclaim.
        LD      ($3C37),HL              ; update STKBOT

        LD      HL,($3C3B)              ; fetch SPARE
        SBC     HL,BC                   ; subtract number of bytes to reclaim.
        LD      ($3C3B),HL              ; update SPARE

        SBC     HL,DE                   ; subtract
        RET     Z                       ; return if same address.

        PUSH    BC                      ;
        LD      B,H                     ;
        LD      C,L                     ;
        POP     HL                      ;
        ADD     HL,DE                   ;

        LDIR                            ;

        RET                             ;

; ---
;
; ---

L14F8:  LD      BC,$3C31                ; CURRENT

        CALL    L1557                   ;
        CALL    L1557                   ;

        LD      BC,$3C40                ; addr. of "FORTH" in RAM.

L1504:  LD      HL,($3C37)              ; STKBOT
        SCF                             ;
        SBC     HL,BC                   ;
        RET     C                       ;

L150B:  LD      A,(BC)                  ;
        RLA                             ;
        INC     BC                      ;
        JR      NC,L150B                ;

        INC     BC                      ;
        INC     BC                      ;
        CALL    L1557                   ;
        INC     BC                      ;
        CALL    L1557                   ;

L1519:  CALL    L15FB                   ; routine INDEXER

; -------------------------------------------------------

        DEFW    L0EC3                   ; DE value
L151E:  DEFB    $1C                     ; to L153A

        DEFW    L1085                   ; DE value
L1521:  DEFB    $16                     ; to L1537

        DEFW    L1108                   ; DE value
L1524:  DEFB    $13                     ; to L1537

        DEFW    L11B5                   ; DE value
L1527:  DEFB    $18                     ; to L153F

        DEFW    $0000                   ; zero end marker

; -------------------------------------------------------

L152A:  LD      HL,$FFF9
        ADD     HL,BC

        LD      C,(HL)
        INC     HL
        LD      B,(HL)
        DEC     HL

        ADD     HL,BC

        LD      B,H
        LD      C,L
        JR      L1504                   ;

; -------------------------------------------------------

L1537:  CALL    L1557                   ;

; ->

L153A:  CALL    L1548                   ;
        JR      L1504                   ;

; ---

L153F:  CALL    L1557                   ;
        INC     BC                      ;
        CALL    L1557                   ;
        JR      L1504                   ;

; -------------------------------------------------------

; XXX?

L1548:  CALL    L1557                   ;
        LD      HL,L04B6                ;
        AND     A                       ;
        SBC     HL,DE                   ;
        RET     Z                       ;

        CALL    L159E                   ;

        JR      L1548                   ;

; ---
; often called twice
; ---


L1557:  LD      A,(BC)                  ; lo byte
        LD      E,A                     ;
        INC     BC                      ;
        LD      A,(BC)                  ; hi byte
        LD      D,A                     ;
        DEC     BC                      ; BC now unchanged, DE contents

        CALL    L1568                   ; routine below. header?

        EX      DE,HL                   ; value to DE
        LD      A,E                     ;
        LD      (BC),A                  ; lo byte
        INC     BC                      ;
        LD      A,D                     ;
        LD      (BC),A                  ; hi byte
        INC     BC                      ;
        RET                             ; to next - BC+=2

; ---

L1568:  LD      HL,($2701)              ; first bytes of pad.
        AND     A                       ;
        SBC     HL,DE                   ; subtract the DE value read from
                                        ; memory
        LD      H,D                     ;
        LD      L,E                     ; transfer that DE to HL as well

        RET     NC                      ; return if HL was higher than DE

        LD      HL,($2709)              ; tape header
        SBC     HL,DE
        JR      NC,L1584                ; forward if higher to

        LD      HL,($270D)
        SBC     HL,DE
        JR      C,L1592                 ; forward if lower to

        LD      HL,($270B)              ;
        ADD     HL,DE
        RET                             ; return

; ---

L1584:  LD      HL,($2703)
        SBC     HL,DE
        LD      HL,($2707)
        RET     C

        LD      HL,($2705)
        ADD     HL,DE
        RET

; ---

L1592:  LD      HL,($2701)
        ADD     HL,DE
        LD      DE,($270D)
        AND     A
        SBC     HL,DE
        RET

; ---

L159E:  DEC     DE
        LD      A,(DE)
        RLA
        RET     NC

L15A2:  DEC     DE
        DEC     DE
        LD      A,(DE)
        LD      L,A                     ; low byte
        LD      H,$00                   ; make high byte zero
        INC     A                       ; test offset for $FF.
        JR      NZ,L15B1                ; forward if not.

        LD      A,(BC)
        LD      L,A
        INC     BC
        LD      A,(BC)
        LD      H,A
        INC     BC

L15B1:  ADD     HL,BC

        LD      B,H
        LD      C,L
        RET

; ---
;
; ---


L15B5:  DEFW    L15B7

; ---

L15B7:  RST     18H                     ; pop word DE

        EX      DE,HL

        CALL    L15E7                   ; WORDSTART1

        EX      DE,HL

        RST     10H                     ; push word DE
        JP      (IY)                    ; to 'next'.

; ---
;
; ---

L15C0:  PUSH    HL
        LD      E,(HL)
        INC     HL
        LD      D,(HL)

L15C4:  CALL    L15FB                   ; routine INDEXER

; -------------------------------------------------------

        DEFW    L1108
L15C9:  DEFB    $0B                     ; to L15D4 - find parameter field

        DEFW    L1085
L15CC:  DEFB    $08                     ; to L15D4 - find parameter field

        DEFW    $0000                   ; zero end_marker.

; -------------------------------------------------------

L15CF:  LD      BC,$0000                ; zero indicates no parameter field.
        JR      L15DB                   ; forward to consider total length.

; -------------------------------------------------------

L15D4:  POP     HL                      ; retrieve the code field address
        PUSH    HL                      ; save it again

        INC     HL                      ; step past the
        INC     HL                      ; address word
        LD      C,(HL)                  ; and get following address
        INC     HL                      ; which if in RAM could be the
        LD      B,(HL)                  ; parameter field to              BC.

; ->

L15DB:  POP     HL                      ; retrieve the code field address
        PUSH    HL                      ; and save it again

        DEC     HL                      ; the name length field
        DEC     HL                      ; link field high order byte
        DEC     HL                      ; link field low order byte
        DEC     HL                      ; possible length field high
        LD      D,(HL)                  ; save in D
        DEC     HL                      ; possible length field low
        LD      E,(HL)                  ; save in E
        ADD     HL,DE                   ; add this length
        EX      DE,HL                   ; and save result in              DE.

        POP     HL                      ; retrieve code field address

; ->
; indexes the header information of a FORTH word

L15E7:  DEC     HL                      ; point to name length field

; =>
L15E8:  LD      A,H                     ; fetch high order byte of the
                                        ; header address.
        CP      $3C                     ; compare to RAM location
        LD      A,(HL)                  ; fetch length byte.
        RES     6,A                     ; reset the immediate mode bit
        JR      C,L15F2                 ; forward if definition is in ROM.

        ADD     A,$02                   ; else add extra for 'length field'

L15F2:  DEC     HL                      ; step past the
        DEC     HL                      ; link to previous word.

L15F4:  DEC     HL                      ; now address last letter on name.
        DEC     A                       ; decrement the length
        JR      NZ,L15F4                ; loop back until at first letter  HL.

        RET                             ; return.

; -------
; INDEXER
; -------

; indexerloop

L15F9:  INC     HL                      ; step past the
        PUSH    HL                      ; offset byte.

; -> Call Entry point

L15FB:  POP     HL                      ; drop return address - points to byte
                                        ; after the call.
        LD      A,(HL)                  ; read low-order byte
        INC     HL                      ; increment address once
        PUSH    HL                      ; push return address

        LD      H,(HL)                  ; read high-order byte.
        LD      L,A                     ; now HL holds the read word
        OR      H                       ; test for two zeros.
        RET     Z                       ; two zeros - return
                                        ; (ret addr is second NOP)

        SBC     HL,DE                   ; compare to value passed in DE

        POP     HL                      ; now increment the
        INC     HL                      ; return address on machine stack.

        JR      NZ,L15F9                ; loop back if read word is not
                                        ; equal to DE

        PUSH    DE                      ; else preserve DE

        LD      D,$00                   ; a 1 byte relative jump.
        LD      E,(HL)                  ; read one-byte offset.
        ADD     HL,DE                   ; add to read address.

        POP     DE                      ; restore DE

        JP      (HL)                    ; >>>

; ---

L1610:  DEFW    L0EC3                   ; 'code field' - docolon

        DEFW    L0E1F                   ; 1-
        DEFW    L0E29                   ; 2-
        DEFW    L08B3                   ; @
        DEFW    L0480                   ; current
        DEFW    L08B3                   ; @
        DEFW    L08C1                   ; !
        DEFW    L04B6                   ; exit

; ---------------------------------
; THE 'FIND WORD IN RAM' SUBROUTINE
; ---------------------------------
; This subroutine is used by FORGET, EDIT and LIST.
; First use the standard FORTH word find to get address of word (in pad).
; If word does not exist then returned value will be zero.
; The lowest word in RAM is the FORTH word at L3C51 so a check is made
; against this address.

L1620:  CALL    L04B9                   ; forth
        DEFW    L063D                   ; find

L1625:  DEFW    L1A0E                   ; end-forth.

        RST     18H                     ; pop word DE

        LD      HL,$C3AF                ; i.e $0000 - $3C51

        ADD     HL,DE                   ; add to test value.
        RET     C                       ; carry signals that word exists in RAM.
                                        ; return the address in DE.

; else generate an error code.

        RST     20H                     ; Error 13
        DEFB    $0D                     ; Error word not found or is in ROM.

; -----------------
; THE 'FORGET' WORD
; -----------------
; FORGET name.
; Erases the word 'name' and all subsequently defined names from the dictionary.

L162F:  DEFM    "FORGE"                 ; 'name field'
        DEFB    'T' + $80

        DEFW    L13FC                   ; 'link field'

L1637:  DEFB    $06                     ; 'name length field'

L1638:  DEFW    L163A                   ; 'code field'

; ---

L163A:  LD      HL,($3C31)              ; CURRENT
        LD      DE,($3C33)              ; CONTEXT
        AND     A
        SBC     HL,DE

        JP      NZ,L14DA                ;

        CALL    L1620                   ; findramword

        LD      HL,$FFFB
        ADD     HL,DE
        LD      ($3C39),HL              ; SPARE
        SET     2,(IX+$3E)              ; FLAGS

        RST     20H                     ; Invoke error routine.
        DEFB    $FF                     ; No error

; ---------------
; THE 'EDIT' WORD
; ---------------
; EDIT name
; Lists word 'name' at bottom of the screen to be edited. Lists 18 lines at
; a time, then waits for editing until ENTER is pressed.
; A new version of the word is entered at the end of the dictionary.
; While editing, cursor up and cursor down are needed to move the cursor
; from one line to another. DELETE LINE deletes one line.

L1657:  DEFM    "EDI"                   ; 'name field'
        DEFB    'T' + $80

        DEFW    L1637                   ; 'link field'

L165D:  DEFB    $04                     ; 'name length field'

L165E:  DEFW    L1660                   ; 'code field'

; ---

L1660:  CALL    L1620                   ; findramword

        SET     3,(IX+$3E)              ; update FLAGS output -> input buffer
        JR      L1675                   ; forward to list routine the difference
                                        ; being that the listing will go to the
                                        ; lower screen.

; ---------------
; THE 'LIST' WORD
; ---------------
; LIST name
; (  --  )
; Lists word 'name' on the screen. It must have been defined by :, DEFINER,
; or COMPILER. Lists about 18 lines at a time and waits for key depression
; (shifted space breaks).

L1669:  DEFM    "LIS"                   ; 'name field'
        DEFB    'T' + $80

        DEFW    L165D                   ; 'link field'

L166F:  DEFB    $04                     ; 'name length field'

L1670:  DEFW    L1672                   ; 'code field'

; ---

L1672:  CALL    L1620                   ; findramword

; edit path joins here but carriage returns are printed as zeros.

L1675:  LD      A,$0D                   ; prepare a carriage return.
        RST     08H                     ; print_ch

        BIT     3,(IX+$3E)              ; test FLAGS output->input buffer?

        PUSH    DE

        CALL    NZ,L02D8                ; call if so to initialize buffer

        POP     BC                      ; LD DE,(BC)

        LD      A,(BC)
        LD      E,A
        INC     BC
        LD      A,(BC)
        LD      D,A
        DEC     BC

        CALL    L15FB                   ; routine INDEXER

; -------------------------------------------------------

L168A:  DEFW    L0EC3                   ; DE value
L168C:  DEFB    $0B                     ; offset to L1697

L168D:  DEFW    L1108                   ; DE value
L168F:  DEFB    $0D                     ; offset to L169C

L1690:  DEFW    L1085                   ; DE value
L1692:  DEFB    $1F                     ; offset to L16B1

        DEFW    $0000                   ; zero end-marker

; -------------------------------------------------------

L1695:  RST     20H                     ; Error 14
        DEFB    $0E                     ; Word unlistable.

; Only words defined by ':', 'DEFINER' or 'COMPILER' are listable.

; -------------------------------------------------------

; ':'
L1697:  LD      HL,$0002
        JR      L16B4                   ;
; ---

L169C:  PUSH    DE
        LD      HL,$0002
        ADD     HL,BC
        LD      A,(HL)
        INC     HL
        LD      H,(HL)
        LD      L,A
        DEC     HL
        DEC     HL
        DEC     HL

        LD      L,(HL)
        LD      A,L
        RLCA
        SBC     A,A
        LD      H,A

        CALL    L180E                   ; pr_int_hl?

        POP     DE

L16B1:  LD      HL,$0004


L16B4:  ADD     HL,BC
        PUSH    HL
        PUSH    BC

        CALL    L17E4                   ;

        POP     DE
        POP     BC

        CALL    L17E4                   ;

        LD      (IX+$14),$01            ; LISTWSx

L16C3:  LD      (IX+$16),$10            ; LISTWSx

L16C7:  CALL    L1708                   ; index_table

        JR      C,L16D2                 ;

        DEC     (IX+$16)                ; LISTWSx
        JP      P,L16C7                 ;

L16D2:  BIT     3,(IX+$3E)              ; FLAGS
        JR      NZ,L16E8                ; branch forward  =->

        JR      C,L1702                 ;

        LD      HL,$3C26                ; KEYCOD
        LD      (HL),$00                ;

L16DF:  LD      A,(HL)                  ;
        AND     A                       ;
        JR      Z,L16DF                 ; loop back while zero

        CALL    L04E4                   ; check break

        JR      L16C3                   ; loop back

; =->

L16E8:  PUSH    AF
        RES     3,(IX+$3E)              ; FLAGS
        PUSH    BC

        CALL    L04B9                   ; forth

        DEFW    L0578                   ; retype        - allow user to retype
        DEFW    L0506                   ; line          - interpret buffer
        DEFW    L1A0E                   ; end-forth.


        SET     3,(IX+$3E)              ; FLAGS

        CALL    L02D8                   ;

        POP     BC
        POP     AF
        JR      NC,L16C3                ;

L1702:  RES     3,(IX+$3E)              ; FLAGS
        JP      (IY)                    ; to 'next'.

; -------------------------------------------------------

; called once

L1708:  LD      A,($3C14)               ; LISTWS2
        LD      ($3C15),A               ; LISTWS3

        LD      (IX+$13),$05            ; LISTWS

L1712:  LD      A,(BC)
        LD      E,A
        INC     BC
        LD      A,(BC)
        LD      D,A
        INC     BC

L1718:  CALL    L15FB                   ; routine INDEXER

; -------------------------------------------------------

L171B:  DEFW    L1283                   ;
L171D:  DEFB    $40                     ; offset to L175D

L171E:  DEFW    L1271                   ;
L1720:  DEFB    $44                     ; offset to L1764

L1721:  DEFW    L12A4                   ;
L1723:  DEFB    $48                     ; offset to L176B

L1724:  DEFW    L129F                   ;
L1726:  DEFB    $37                     ; offset to L175D

L1727:  DEFW    L128D                   ;
L1729:  DEFB    $42                     ; offset to L176B

L172A:  DEFW    L1288                   ;
L172C:  DEFB    $38                     ; offset to L1764

L172D:  DEFW    L1276                   ;
L172F:  DEFB    $3C                     ; offset to L176B

L1730:  DEFW    L1323                   ;
L1732:  DEFB    $2B                     ; offset to L175D

L1733:  DEFW    L1332                   ;
L1735:  DEFB    $36                     ; offset to L176B

L1736:  DEFW    L133C                   ;
L1738:  DEFB    $33                     ; offset to L176B

L1739:  DEFW    L10E8                   ;
L173B:  DEFB    $29                     ; offset to L1764

L173C:  DEFW    L1140                   ;
L173E:  DEFB    $26                     ; offset to L1764

L173F:  DEFW    L1011                   ;
L1741:  DEFB    $3B                     ; offset to L177C

L1742:  DEFW    L1064                   ;
L1744:  DEFB    $47                     ; offset to L178B

L1745:  DEFW    L104B                   ;
L1747:  DEFB    $51                     ; offset to L1798

L1748:  DEFW    L1379                   ;
L174A:  DEFB    $62                     ; offset to L17AC

L174B:  DEFW    L1396                   ;
L174D:  DEFB    $63                     ; offset to L17B0

L174E:  DEFW    L04B6                   ;
L1750:  DEFB    $54                     ; offset to L17A4

L1751:  DEFW    $0000                   ; zero end-marker

; -------------------------------------------------------

; default action

L1753:  CALL    L17E1                   ;

L1756:  DEC     (IX+$13)                ; LISTWS
        JR      NZ,L1712                ;
        AND     A
        RET

; ---

L175D:  LD      HL,($3C14)              ; LISTWS2
        LD      H,L
        INC     L
        JR      L1770                   ;

; ---

L1764:  LD      HL,($3C14)              ; LISTWS2
        LD      H,L
        DEC     H
        JR      L1770                   ;

; ---

L176B:  LD      HL,($3C14)              ; LISTWS2
        DEC     L
        LD      H,L

L1770:  LD      ($3C14),HL              ; LISTWS2
        LD      (IX+$13),$01            ; LISTWS
        DEC     (IX+$16)                ; LISTWSx
        JR      L1753                   ;

; ---

L177C:  CALL    L17DA                   ;

        RST     10H                     ; push word DE
        LD      DE,$09B3                ; '.' addr

L1783:  CALL    L17C1                   ; routine INDENT
        CALL    L1815                   ; pr2

        JR      L1756                   ;

; ---

L178B:  CALL    L17DA                   ;
        RST     10H                     ; push word DE
        CALL    L17DA                   ;
        RST     10H                     ; push word DE
        LD      DE,$0AAF                ; 'F.' addr
        JR      L1783                   ;

; ---

L1798:  LD      A,(BC)
        PUSH    AF

        CALL    L17E1                   ;

        POP     AF
        RST     08H                     ; print_ch

        LD      A,$20                   ; a space character
        RST     08H                     ; print_ch

        JR      L1756                   ;

; ---

L17A4:  CALL    L1808                   ; pr_inline

        DEFB    $0D                     ; newline
        DEFB    ';'                     ; ;
        DEFB    $8D                     ; inverted newline

        SCF                             ;
        RET                             ;

; ---

L17AC:  LD      A,$29                   ; character ')' - end of comment.
        JR      L17B2                   ;

L17B0:  LD      A,$22                   ; character '"' - quote

L17B2:  PUSH    AF
        PUSH    BC
        CALL    L17E1                   ;
        POP     DE
        CALL    L0979                   ; pr_string1
        LD      B,D
        LD      C,E
        POP     AF

        RST     08H                     ; print_ch

        AND     A
        RET

; -------------------------------------------------------

L17C1:  LD      A,($3C15)               ; LISTWS3
        AND     A
        RET     M

        PUSH    BC                      ; preserve BC
        LD      B,A                     ; transfer count to B

        LD      A,$0D                   ; carriage return.
        RST     08H                     ; print_ch

        INC     B                       ; test indentation.
        DEC     B                       ;
        JR      Z,L17D4                 ;

L17CF:  LD      A,$20                   ; a space character
        RST     08H                     ; print_ch

        DJNZ    L17CF                   ;

L17D4:  LD      (IX+$15),$FF            ; LISTWS3

        POP     BC                      ; restore BC
        RET                             ; return.

; ---

L17DA:  LD      A,(BC)
        LD      E,A
        INC     BC
        LD      A,(BC)
        LD      D,A
L17DF:  INC     BC
        RET

; ---

L17E1:  CALL    L17C1                   ; routine INDENT

L17E4:  EX      DE,HL
        DEC     HL
        LD      A,(HL)
        BIT     7,A
        JR      NZ,L17F0                ;

        CALL    L15E8                   ; routine WORDSTART

        JR      L17FB                   ;

; ---

L17F0:  EX      DE,HL

        CALL    L15A2                   ;

        INC     DE
        LD      A,(DE)
        LD      L,A
        INC     DE
        LD      A,(DE)
        LD      H,A
        ADD     HL,DE

; pr_string_sp

L17FB:  LD      A,(HL)
        AND     $7F
        RST     08H                     ; print_ch
        BIT     7,(HL)
        INC     HL
        JR      Z,L17FB                 ;

        LD      A,$20
        RST     08H                     ; print_ch
        RET

; ---------------------------------------
; THE 'INLINE PRINT STRING SPACE' ROUTINE
; ---------------------------------------
;

L1808:  EX      (SP),HL
        CALL    L17FB                   ; pr_string_sp
        EX      (SP),HL
        RET

; ---------------------------
; THE 'PRINT INTEGER' ROUTINE
; ---------------------------
; in HL

; -> called twice
L180E:  LD      DE,$09B3                ; '.' addr
        PUSH    DE                      ; but save it as we need DE?

        EX      DE,HL                   ; transfer HL to DE.
        RST     10H                     ; push word DE, was HL, on Data Stack.
        POP     DE                      ; restore L09B3 again

; -> called twice.
L1815:  PUSH    BC                      ; preserve BC.

        CALL    L04BF                   ; executes '.' word

; the '.' exits so expects another word here


L1819:  DEFW    L181B

L181B:  DEFW    L181D

L181D   POP     BC                      ;
        POP     BC                      ; restore BC.

        RET                             ; return.

; ---------------------------------
; THE 'CASSETTE INTERFACE' ROUTINES
; ---------------------------------

; ---
; tape???
; ---

L1820:  PUSH    IY

        PUSH    HL
        POP     IY

        LD      HL,L1892
        PUSH    HL

        LD      HL,$E000
        BIT     7,C
        JR      Z,L1832                 ;
        LD      H,$FC
L1832:  INC     DE
        DEC     IY
        DI
        XOR     A

L1837:  LD      B,$97

L1839:  DJNZ    L1839                   ;
        OUT     ($FE),A
        XOR     $08
        INC     L
        JR      NZ,L1843                ;
        INC     H
L1843:  JR      NZ,L1837                ;
        LD      B,$2B
L1847:  DJNZ    L1847                   ;
        OUT     ($FE),A
        LD      L,C
        LD      BC,$3B08
L184F:  DJNZ    L184F                   ;
        LD      A,C
        OUT     ($FE),A
        LD      B,$38
        JP      L188A                   ;

L1859:  LD      A,C
        BIT     7,B

L185C:  DJNZ    L185C                   ;

        JR      NC,L1864                ;

        LD      B,$3D
L1862:  DJNZ    L1862                   ;

L1864:  OUT     ($FE),A
        LD      B,$3A
        JP      NZ,L1859                ;
        DEC     B
        XOR     A
L186D:  RL      L
        JP      NZ,L185C                ;
        DEC     DE
        INC     IY
        LD      B,$2E

        LD      A,$7F
        IN      A,($FE)
        RRA
        RET     NC

        LD      A,D
        CP      $FF
        RET     NC

        OR      E
        JR      Z,L188F                 ;

        LD      L,(IY+$00)
L1887:  LD      A,H
        XOR     L
        LD      H,A
L188A:  XOR     A
        SCF
        JP      L186D                   ; JUMP back

; ---

L188F:  LD      L,H
        JR      L1887                   ;

L1892:  POP     IY                      ; restore the original IY value so that
                                        ; words can be used gain.

        EX      AF,AF'                  ;;
        LD      B,$3B                   ;

L1897:  DJNZ    L1897                   ; self-loop for delay.

        XOR     A
        OUT     ($FE),A

        LD      A,$7F                   ; read the port $7FFE
        IN      A,($FE)                 ; keyrows SPACE to V.
        RRA
        EI                              ; Enable Interrupts.

        JP      NC,L04F0                ; jump if SPACE pressed to Error 3
                                        ; 'BREAK pressed'.

        EX      AF,AF'                  ;;
        RET                             ; return.

; ---
; READ BYTES FROM TAPE
; ---

L18A7:  DI
        PUSH    IY
        PUSH    HL
        POP     IY
        LD      HL,L1892
        PUSH    HL
        LD      H,C
        EX      AF,AF'                  ; save carry
        XOR     A
        LD      C,A

L18B5:  RET     NZ

L18B6:  LD      L,$00
L18B8:  LD      B,$B8

        CALL    L1911                   ;

        JR      NC,L18B5                ;

        LD      A,$DF
        CP      B
        JR      NC,L18B6                ;

        INC     L
        JR      NZ,L18B8                ;

L18C7:  LD      B,$CF

        CALL    L1915                   ;

        JR      NC,L18B5                ;

        LD      A,B
        CP      $D8
        JR      NC,L18C7                ;

        CALL    L1915                   ;
        RET     NC

        CALL    L18FC                   ;
        RET     NC

        CCF
        RET     NZ

        JR      L18F0                   ;

; ---

L18DF:  EX      AF,AF'
        JR      NC,L18E7                ;
        LD      (IY+$00),L
        JR      L18EC                   ;

; ---

L18E7:  LD      A,(IY+$00)
        XOR     L
        RET     NZ

L18EC:  INC     IY
        DEC     DE
        EX      AF,AF'

L18F0:  CALL    L18FC                   ;

        RET     NC

        LD      A,D
        OR      E
        JR      NZ,L18DF                ;

        LD      A,H
        CP      $01
L18FB:  RET

; ---

L18FC:  LD      L,$01
L18FE:  LD      B,$C7

        CALL    L1911                   ;

        RET     NC

        LD      A,$E2
        CP      B
        RL      L
        JP      NC,L18FE                ;

        LD      A,H
        XOR     L
        LD      H,A
        SCF
        RET

; ---

L1911:  CALL    L1915                   ;
        RET     NC

L1915:  LD      A,$14
L1917:  DEC     A

        JR      NZ,L1917                ;

        AND     A

L191B:  INC     B
        RET     Z

        LD      A,$7F
        IN      A,($FE)
        RRA
        RET     NC

        XOR     C
        AND     $10
        JR      Z,L191B                 ;

        LD      A,C
        CPL
        LD      C,A
        SCF
        RET

; ---------------
; THE 'SAVE' WORD
; ---------------
; SAVE name.
; Saves entire dictionary in RAM on a dictionary type cassette file with the
; given name. Makes a noise on the internal loudspeaker.

L192D:  DEFM    "SAV"                   ; 'name field'
        DEFB    'E' + $80

        DEFW    L166F                   ; 'link field'

L1933:  DEFB    $04                     ; 'name length field'

L1934:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

        DEFW    L1A10                   ; word to pad
        DEFW    L1A4F                   ;  prep some sort of header?
        DEFW    L04B6                   ; exit

; ----------------
; THE 'BSAVE' WORD
; ----------------
; BSAVE name
; (m, n -- )
; Save n bytes to bytes type cassette file 'name' starting at
; address m.
;

L193C:  DEFM    "BSAV"                  ; 'name field'
        DEFB    'E' + $80

        DEFW    L1933                   ; 'link field'

L1943:  DEFB    $05                     ; 'name length field'

L1944:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L1946:  DEFW    L1A3D                   ; prep_header
        DEFW    L1A4F                   ; prep some sort of header?
        DEFW    L04B6                   ; exit


; ----------------
; THE 'BLOAD' WORD
; ----------------
; BLOAD name
; (m, n -- )
; Load at most n bytes of bytes type cassette file 'name' starting at
; address m. ERROR 10 if the file has more than m bytes.
;
L194C:  DEFM    "BLOA"                  ; 'name field'
        DEFB    'D' + $80

        DEFW    L1943                   ; 'link field'

L1953:  DEFB    $05                     ; 'name length field'

L1954:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

        DEFW    L1A3D                   ; prep_header
        DEFW    L1A74                   ; ld-bytes??
        DEFW    L1AB8                   ; tapeFF
        DEFW    L04B6                   ; exit

; -----------------
; THE 'VERIFY' WORD
; -----------------
; VERIFY name
; (  --  )
; Verifies dictionary on tape against dictionary in RAM.

L195E:  DEFM    "VERIF"                 ; 'name field'
        DEFB    'Y' + $80

        DEFW    L1953                   ; 'link field'

L1966:  DEFB    $06                     ; 'name length field'

L1967:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L1969:  DEFW    L1A10                   ; word to pad
        DEFW    L1271                   ; branch
L196D:  DEFW    $000F                   ; 15 bytes forward to L197D


; ------------------
; THE 'BVERIFY' WORD
; ------------------
; BVERIFY name
; (m, n -- )
; Verify at most n bytes of bytes type cassette file 'name' against
; RAM starting at address m. ERROR 10 if the file has more than m bytes.
; For BLOAD and BVERIFY, if m = 0, then starts at the address the bytes
; were saved from. If n = 0, then doesn't care about the length.
;

L196F:  DEFM    "BVERIF"                ; 'name field'
        DEFB    'Y' + $80

        DEFW    L1966                   ; 'link field'

L1978:  DEFB    $07                     ; 'name length field'

L1979:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L197B:  DEFW    L1A3D                   ; prep_header

; ->

L197D:  DEFW    L1A74                   ; ld_bytes
        DEFW    L1ABE                   ; tape00
        DEFW    L04B6                   ; exit

; ---------------
; THE 'LOAD' WORD
; ---------------
; LOAD name
; (  --  )
; Searches for a dictionary cassette file 'name' and loads it in, adding it
; to end of old dictionary. Writes to the screen all files found on tape.
; For best results turn the tone control on the tape recorder right down
; (as bass as possible) and the volume control to about three-quarters
; maximum.

L1983:  DEFM    "LOA"                   ; 'name field'
        DEFB    'D' + $80

        DEFW    L1978                   ; 'link field'

L1989:  DEFB    $04                     ; 'name length field'

L198A:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L198C:  DEFW    L1A10                   ; word to pad

        DEFW    L1A0E                   ; end-forth.

        LD      HL,($3C37)              ; STKBOT
        LD      ($230E),HL
        EX      DE,HL
        LD      HL,$FFCC
        ADD     HL,SP
        AND     A
        SBC     HL,DE
        LD      ($230C),HL

        CALL    L04B9                   ; forth

L19A4:  DEFW    L1A74                   ; ld_bytes
        DEFW    L1AB8                   ; tapeFF
        DEFW    L1A0E                   ; end-forth.

        LD      BC,($3C37)              ; STKBOT
        LD      HL,$3C50
        LD      ($2701),HL
        INC     HL
        LD      ($2709),HL
        LD      HL,($2325)
        ADD     HL,BC
        LD      ($3C37),HL              ; STKBOT
        LD      HL,$C3AF
        ADD     HL,BC
        LD      ($270B),HL
        LD      DE,($2329)
        ADD     HL,DE
        LD      DE,($3C4C)
        LD      ($3C4C),HL
        PUSH    BC
        PUSH    DE


L19D4:  LD      ($270D),SP
        CALL    L1504                   ;
        POP     BC
        POP     HL
L19DD:  BIT     7,(HL)
        INC     HL
        JR      Z,L19DD                 ;
        INC     HL
        INC     HL
        LD      (HL),C
        INC     HL
        LD      (HL),B
        LD      HL,($3C37)              ; STKBOT
        LD      BC,$000C                ; allow twelve bytes for underflow.
        ADD     HL,BC
        LD      ($3C3B),HL              ; SPARE
        JP      (IY)                    ; to 'next'.

; ---

L19F3:  DEFW    L0EC3                   ; 'code field' - docolon
        DEFW    L104B                   ; stk_data
        DEFB    $20                     ; a space delimiter
        DEFW    L05AB                   ; word          (to pad)
        DEFW    L1A0E                   ; end-forth.

; ---

L19FC:  CALL    L0F2E                   ; blank stack

L19FF:  RST     18H                     ; pop word DE

        LD      A,$20                   ;
        LD      (DE),A                  ;
        LD      DE,$270C                ;
        LD      HL,$27FF                ;

        CALL    L07FA                   ; routine SPACE_FILL

        JP      (IY)                    ; to 'next'.

; ---


L1A0E:  DEFW    L18FB                   ; location of RET instruction.

; ---

L1A10:  DEFW    L0EC3                   ; 'code field' - docolon
        DEFW    L19F3                   ; word to pad
        DEFW    L1A0E                   ; end-forth.

        XOR     A                       ;
        LD      ($2301),A               ;
        LD      HL,$3C51                ;
        LD      ($230E),HL              ;
        EX      DE,HL                   ;
        LD      HL,($3C37)              ; STKBOT
        AND     A                       ;
        SBC     HL,DE
        LD      ($230C),HL
        LD      HL,($3C4C)
        LD      ($2310),HL
        LD      HL,$3C31                ; CURRENT
        LD      DE,$2312
        LD      BC,$0008                ;

        LDIR                            ;

        JP      (IY)                    ; to 'next'.

; ---


L1A3D:  DEFW    L0EC3                   ; 'code field' - docolon
        DEFW    L19F3                   ; word to pad
        DEFW    L1011                   ; stack next word
        DEFW    $230C                   ; header location
        DEFW    L08C1                   ; !     store int at address
        DEFW    L1011                   ; stack next word
        DEFW    $230E                   ; header location
        DEFW    L08C1                   ; !     store int at address
        DEFW    L04B6                   ; exit

; ---

L1A4F:  DEFW    L1A51

L1A51:  LD      A,($2302)               ; length of word in pad
        AND     A
        JR      Z,L1AB6                 ; forward if null.

        LD      HL,($230C)
        LD      A,H
        OR      L
        JR      Z,L1AB6                 ;

        PUSH    HL
        LD      DE,$0019                ;
        LD      HL,$2301                ; pad using ROM priority
        LD      C,D                     ;

        CALL    L1820                   ;

        POP     DE
        LD      HL,($230E)              ;
        LD      C,$FF

        CALL    L1820                   ;

        JP      (IY)                    ; to 'next'.

; ---
; ld_bytes
; ---

L1A74:  DEFW    L1A76

L1A76:  LD      DE,$0019
        LD      HL,$231A
        LD      C,D

        SCF

        CALL    L18A7                   ;

        JR      NC,L1A76                ; loop back until read

        LD      DE,$231A
        LD      A,(DE)
        AND     A
        JR      NZ,L1A95                ;

        CALL    L1808                   ; pr_inline

; ---

L1A8D:  DEFB    $0D                     ; newline
        DEFM    "Dict"
        DEFB    ':' + $80               ;

L1A93:  JR      L1A9F                   ;

; ---

L1A95:  CALL    L1808                   ; pr_inline

L1A98:  DEFB    $0D                     ; newline

        DEFM    "Bytes"
        DEFB    ':' + $80               ;

; ---

L1A9F:  LD      HL,$2301
        LD      BC,$0B0B
        JR      L1AA9                   ;

; ---

L1AA7:  LD      A,(DE)
        RST     08H                     ; print_ch

L1AA9:  LD      A,(DE)
        CP      (HL)
        JR      NZ,L1AAE                ;
        DEC     C

L1AAE:  INC     HL
        INC     DE
        DJNZ    L1AA7                   ;

        JR      NZ,L1A76                ;
        JP      (IY)                    ; to 'next'.

; ---

L1AB6:  RST     20H                     ; Error 10
        DEFB    $0A                     ; Tape error

; ---
;
; ---

L1AB8:  DEFW    L1ABA                   ; headerless 'code field'

L1ABA:  LD      B,$FF
        JR      L1AD0                   ; forward to +->

; ---
;
; ---

L1ABE:  DEFW    L1AC0                   ; headerless 'code field'

L1AC0:  LD      HL,$2312
        LD      DE,$232B
        LD      B,$08

L1AC8:  LD      A,(DE)
        INC     DE
        CP      (HL)
        INC     HL
        JR      NZ,L1AB6                ; back to tape error

        DJNZ    L1AC8                   ; back for all 8

; common code - B is $00 from above or $FF from previous.

L1AD0:  LD      HL,($230C)
        LD      DE,($2325)
        LD      A,H
        OR      L
        JR      Z,L1ADF                 ; skip if zero

        SBC     HL,DE
        JR      C,L1AB6                 ; back to tape error

L1ADF:  LD      HL,($230E)
        LD      A,H
        OR      L
        JR      NZ,L1AE9                ; skip if zero
        LD      HL,($2327)

L1AE9:  LD      C,$FF
        RR      B

        CALL    L18A7                   ;

        JR      NC,L1AB6                ; back to report tape error

        JP      (IY)                    ; to 'next'.

; ==========================================================
; THE 'FLOATING POINT ARITHMETIC' ROUTINES
; ==========================================================

; ---------------------
; THE 'PREP_FP' ROUTINE
; ---------------------
; ( f1, f2 -- m1, m2 )
; -> from add/mult/div
; Entered with two floating point numbers on the stack.
; The exponents are stored in the first two bytes of FP_WS and the third byte
; is loaded with the manipulated result sign.
; the two exponent locations on the Data Stack are blanked leaving just the
; binary coded mantissas.

; Begin by clearing the first part of the workspace.

L1AF4:  LD      BC,$3C0F                ; byte 15 of the 19 bytes at FP_WS

        XOR     A                       ; clear accumulator.

L1AF8:  LD      (BC),A                  ; clear the workspace.
        DEC     C                       ; decrement low byte of address.
        JR      NZ,L1AF8                ; and back until at $3C00

;

        LD      HL,($3C3B)              ; fetch end of data stack+1 from SPARE.
        LD      DE,$FFFC                ; prepare  -4

        DEC     HL                      ; point to last byte of stack.
        LD      C,(HL)                  ; sign/exponent of (f2) to C.
        LD      (HL),A                  ; replace with zero to take overflow.

        ADD     HL,DE                   ; subtract four from address

; update system variable SPARE - this could be deferred.

        INC     HL                      ; point to location after (f1).
        LD      ($3C3B),HL              ; update system variable SPARE
        DEC     HL                      ; point to exponent of (f1)

        LD      B,(HL)                  ; sign/exponent of (f1) to B.
        LD      (HL),A                  ; replace with zero.

; At this stage we have the sign/exponent of (f1) in B and the sign/exponent
; of (f2) in C. The next section places the sign bit of (f1) in but 7 of A
; and the sign bit of (f2) in bit 6 of A. The other bits are of no importance.

        LD      A,C                     ; transfer C to A.
        RRCA                            ; rotate sign bit to bit 6.
        XOR     B                       ; XOR B
        AND     $7F                     ; mask off bits to restore
        XOR     B                       ; bit 6 as it was, bit 7 of B to A.

L1B13   LD      ($3C02),A               ; FP_WS_02             see L1C2F

        RES     7,B                     ; make both numbers
        RES     7,C                     ; positive

        LD      ($3C00),BC              ; store the exponents at start of FP_WS

        INC     HL                      ; point to (f2) again.
        EX      DE,HL                   ; transfer f2 pointer to DE, HL now -4
        ADD     HL,DE                   ; subtract four to point HL at (f1)
        RET                             ; return.

; On exit, HL -> (f1), DE -> (f2), B = exponent of (f1), C = exponent of (f2).

; -----------------------------
; THE 'SHIFT_ADDEND' SUBROUTINE
; -----------------------------

L1B22:  LD      A,$09
        CP      B
        JR      NC,L1B28                ;

        LD      B,A                     ; set shift counter to nine. i.e clear.

L1B28:  LD      C,$04                   ; four bytes
        INC     HL
        INC     HL
        INC     HL                      ; point to highest byte

        XOR     A                       ; prepare to start with a blank nibble.

L1B2E:  RRD                             ; A=0000 XXXX --> 7654->3210 =(HL)
                                        ;          \_____<-______/

        DEC     HL                      ; point to next lower byte on Data Stack
        DEC     C                       ; decrement the byte counter.
        JR      NZ,L1B2E                ; loop for all 4 bytes = 1 nibble shift

        INC     HL                      ; set pointer to start of number again
        DJNZ    L1B28                   ; decrement the shift counter and loop.

        ADD     A,$FB                   ; add minus five to last nibble lost
                                        ; will set the carry flag if 5 or more.

        PUSH    HL                      ;; preserve pointer to start of addend.

L1B3A:  LD      A,(HL)                  ; fetch the pair of BCD nibbles.

        ADC     A,B                     ; increment if carry set (B = 0)
        DAA                             ; Decimal Adjust Accumulator
                                        ; ($99 becomes $00 with carry set).

        LD      (HL),A                  ; put nibbles back.
        INC     HL                      ; point to next significant pair of
                                        ; binary coded decimal digits.
        JR      C,L1B3A                 ; and ripple any rounding through.

        POP     HL                      ;; retrieve the pointer to start.
        RET                             ; return.

; ---------------------------
; THE 'BCD NEGATE' SUBROUTINE
; ---------------------------
; Negates the four byte, 8 nibble, binary coded decimal on the Data Stack.
; For example -123.456
; is prepared as $00 $12 $34 $56
; and negated as $99 $87 $65 $34

L1B43:  PUSH    BC                      ; preserve the two
        PUSH    HL                      ; main registers used.

        LD      B,$04                   ; set byte counter to four.
        AND     A                       ; clear carry.

L1B48:  LD      A,$00                   ; set to zero without disturbing carry.

        SBC     A,(HL)                  ; subtract pair of digits
        DAA                             ; Decimal Adjust Accumulator
                                        ; adjusts as if from 100 setting carry

        LD      (HL),A                  ; place adjusted decimals back.

        INC     HL                      ; next location on Data Stack.

        DJNZ    L1B48                   ; loop for all 4 bytes.

        POP     HL                      ; restore the
        POP     BC                      ; saved registers.

        RET                             ; return.

; ------------------------------
; THE 'BCD OPERATION' SUBROUTINE
; ------------------------------
; This versatile routine performs the binary coded decimal addition of
; two floating point values with C = 1.
; The second entry point is used in multiplication.

; ->
L1B53:  LD      C,$01                   ; signal the operation is addition.

; -> (with c!=0)
L1B55:  PUSH    HL                      ; preserve the
        PUSH    DE                      ; three main
        PUSH    BC                      ; registers.

        LD      A,C                     ; treat C as a binary coded decimal.
        AND     $0F                     ; isolate the right-hand nibble.
        LD      B,A                     ; transfer R.H. nibble to B

        XOR     C                       ; A now has L.H. nibble.
        LD      C,A                     ; place in C.

; this next magical routine converts the two BCD digits to binary.
; imagine we started with ninety-nine so C = 1001 0000  and B = 0000 1001

        RRCA                            ;    0100 1000
        RRCA                            ;    0010 0100
        ADD     A,C                     ;    1011 0100
        RRCA                            ;    0101 1010
        ADD     A,B                     ;    0110 0011  = 99 binary

        LD      C,A                     ;    binary multiplier in C

; note that for simple addition C is unchanged and still contains 1.

        LD      B,$04                   ; four bytes to consider
        XOR     A                       ; clear accumulator ensuring no initial
                                        ; carry is fed into the loop.

; loop

L1B67:  PUSH    BC                      ; push the counters.
        PUSH    DE                      ; push the (f2) pointer

        PUSH    HL                      ; push the (f1) pointer.

        ADD     A,(HL)                  ; add any running carry to (f1) cell.

        DAA                             ; Decimal Adjust Accumulator
                                        ; possibly setting carry.

        LD      L,A                     ; result to L
        LD      A,(DE)                  ; fetch (f2) cell value.
        LD      H,$00                   ; set high bytes H and D to
        LD      D,H                     ; zero without disturbing carry

        RL      H                       ; now pick up any carry in H.

        AND     A                       ; test (f2) cell value.
        JR      Z,L1B91                 ; skip forward to just store the carry
                                        ; result if the addend value is zero.

        LD      E,A                     ; else DE now holds cell value.

L1B77:  SRL     C                       ; shift counter C   0->76543210->C

        JR      NC,L1B83                ; skip addition if no carry.

; else perform HL=HL+DE in BCD.

        LD      A,L                     ; fetch low byte of (f1) cell.
        ADD     A,E                     ; add to low byte of (f2) cell.
        DAA                             ; DAA.
        LD      L,A                     ; result in L and carry.

        LD      A,H                     ; fetch high byte possibly 1 from carry
        ADC     A,D                     ; add in any carry from above (D=0)
        DAA                             ; comes into play with multiplication.
        LD      H,A                     ; result to H.

L1B83:  INC     C                       ; test the counter for zero.
        DEC     C                       ; (will be if addition)
        JR      Z,L1B91                 ; forward when zero ->

; else is BCD multiplication - double the DE value.

        LD      A,E                     ;
        ADD     A,A                     ;
        DAA                             ;
        LD      E,A                     ;

        LD      A,D                     ;
        ADC     A,A                     ;
        DAA                             ;
        LD      D,A                     ;

        JR      L1B77                   ; back to continue multiplying by C.

; ---

; ->
L1B91:  EX      DE,HL                   ; transfer result to DE.

        POP     HL                      ; pop (f1) cell pointer
        LD      (HL),E                  ; insert result.
        LD      A,D                     ; transfer any carry to A
        POP     DE                      ; pop the (f2) pointer
        POP     BC                      ; pop the counter, and initial C value.

        INC     DE                      ; increment (f2) cell pointer.
        INC     HL                      ; increment (f1) cell pointer.

        DJNZ    L1B67                   ; loop back for all 4 bytes.

        POP     BC                      ; restore the
        POP     DE                      ; three main
        POP     HL                      ; registers.

        RET                             ; return.

; -------------
; THE 'F-' WORD
; -------------
; ( f1, f2 -- f1-f2 )
; Subtracts top two floating point numbers.
;
; just flip the sign and then do floating point addition.

L1B9F:  DEFB    'F'                     ; 'name field'
        DEFB    '-' + $80

        DEFW    L1989                   ; 'link field'

L1BA3:  DEFB    $02                     ; 'name length field'

L1BA4:  DEFW    L0EC3                   ; 'code field' - docolon

; ---

L1BA6:  DEFW    L1D0F                   ; fnegate
        DEFW    L1A0E                   ; end-forth.

        JR      L1BB3                   ; forward to floating point addition.

; -------------
; THE 'F+' WORD
; -------------
; ( f1, f2 -- f1+f2 )
; Adds top two floating point numbers.

L1BAC:  DEFB    'F'                     ; 'name field'
        DEFB    '+' + $80

        DEFW    L1BA3                   ; 'link field'

L1BB0:  DEFB    $02                     ; 'name length field'

L1BB1:  DEFW    L1BB3                   ; 'code field'

; ---

L1BB3:  CALL    L1AF4                   ; PREP_FP

        LD      A,C                     ; take exponent of second number (f2).
        SUB     B                       ; subtract exponent of first (f1).
        PUSH    AF                      ; save result flags.

        JR      NC,L1BC1                ; forward if second number >= first.

        EX      DE,HL                   ; else swap the pointers.
        NEG                             ; negate negative result.
        LD      (IX+$00),B              ; place B in FP_WS_0  (was C).

L1BC1:  LD      B,A                     ; put positive subtraction result in B.

        CALL    NZ,L1B22                ; routine SHIFT_ADDEND aligns digits if
                                        ; exponents are not equal.

        POP     AF                      ; retrieve subtraction result flags.
        JR      NC,L1BC9                ; forward is second number was >= first.

        EX      DE,HL                   ; else switch the pointers back.

L1BC9:  LD      B,$02                   ; two floating point numbers to consider

        LD      C,(IX+$02)              ; FP_WS_02

L1BCE:  RL      C                       ; test sign bit first bit 7 then bit 6.

        CALL    C,L1B43                 ; routine BCD neg if carry

        EX      DE,HL                   ; switch number pointers.

        DJNZ    L1BCE                   ; decrement counter and loop if second
                                        ; number still to do.

        CALL    L1B53                   ; the BCD ADDITION routine.

; The routine preserves main registers so HL->(f1), DE->(f2) and B is zero.

        DEC     DE                      ; point to highest byte of result which
                                        ; could be $99 if one negative number
                                        ; involved or $98 if two negatives.

        LD      A,(DE)                  ; fetch the result sign byte.
        ADD     A,$68                   ; add $68 causing carry if negative.
        RR      B                       ; pick up carry in bit 7 of B, which
                                        ; was zero so zero flag now set if none.

        LD      (IX+$02),B              ; place result sign in  FP_WS_02

        CALL    NZ,L1B43                ; routine BCD_NEGATE if negative result.

; if the

L1BE5:  LD      A,(DE)                  ;
        AND     A                       ;

        JR      NZ,L1C02                ;

; else A is zero.

        DEC     (IX+$00)                ; decrement the result exponent FP_WS_00
        DEC     (IX+$00)                ; as two nibbles will be moved at a time

        PUSH    DE                      ; save pointer to 4th byte

        LD      H,D                     ; make HL
        LD      L,E                     ; equal to DE
        DEC     HL                      ; minus one.

        LD      BC,$03FF                ; counter for three bytes. The $FF
                                        ; value ensures B is not affected by
                                        ; the LDD instruction. Also A is 0.

L1BF6:  OR      (HL)                    ; (detects if the three bytes are zero)

        LDD                             ; copy HL contents one location higher
                                        ; to that addressed by DE. Also dec bc.

        DJNZ    L1BF6                   ; repeat for all 3 bytes

        EX      DE,HL                   ; make HL address lowest location
        LD      (HL),B                  ; and insert a zero into vacated byte.

        POP     DE                      ; restore the pointer to the 4th byte.

        JR      NZ,L1BE5                ; jump back to the end test if something
                                        ; was being shifted through.

; else all four bytes are zero - i.e. the result of the addition is zero.

        JP      (IY)                    ; to 'next'.

; ---

; The branch was to here, from the end test above, when the 4th byte had been
; filled.
; Before joining common code, ensure that the initial block move will be
; ineffective.

L1C02:  LD      D,H                     ; make DE the same as HL - the source
        LD      E,L                     ; and the destination are the same.

; -> common code from mult and above.

L1C04:  PUSH    DE                      ; save start location.

        LD      BC,$0004                ; 4 bytes to consider.
        LDIR                            ; block move sets DE to one past dest.

        POP     HL                      ; restore start of source.

        DEC     DE                      ; DE now addresses 4th byte.

L1C0C:  LD      A,(DE)                  ; load the 4th byte to accumulator.
        AND     A                       ; test for zero.

        JR      Z,L1C21                 ; skip forward if so.

        CP      $10                     ; test if one or two nibbles populated
                                        ; setting carry for a single nibble.

        SBC     A,A                     ; $00 for two nibbles, $FF for one.
        INC     A                       ; $01                  $00
        INC     A                       ; $02 for two nibbles, $01 for one :-)

        LD      B,A                     ; nibble count to B.
        ADD     A,(IX+$00)              ; add count to FP_WS_00 the result
        LD      ($3C00),A               ; exponent and place back in FP_WS_00.

        CALL    L1B22                   ; routine 'shift_addend' moves all the
                                        ; nibbles to the right.

        JR      L1C0C                   ; back to pick up byte and then to
                                        ; next routine.

; ---

; now test for a result that is too large or too small.
; Note. these results may have arisen from multiplication or addition.

L1C21:  LD      A,($3C00)               ; fetch result exponent from FP_WS_00

        DEC     A                       ; decrement?
        CP      $BF                     ; compare lower limit
        INC     A                       ; increment?

        JR      NC,L1C3D                ; forward if less to ZERO_RSLT

        CP      $80                     ; compare upper limit
        JR      NC,L1C3B                ; forward to Error 8 - Overflow

        LD      B,A                     ; save unsigned exponent in B.

; now combine result sign and the exponent.
; for addition then FP_WS_02 contains either $80 or $00 and most of what
; follows does not apply.
; for multiplication then bit 7 is sign of (f1) bit 6 is sign of (f2).

L1C2F   LD      A,($3C02)               ; FP_WS_02           see L1B13

        LD      C,A                     ; save a copy in C
        RLA                             ; rotate bit 6 to 7
        XOR     C                       ; XOR bit 7 - minus * minus = a plus.
        AND     $80                     ; only interested in bit 7.
        XOR     B                       ; combine with exponent.
        LD      (DE),A                  ; and place in sign/exp on Data Stack.

        JP      (IY)                    ; to 'next'.

; ---

L1C3B:  RST     20H                     ; Error 8.
        DEFB    $08                     ; Overflow in floating-point arithmetic.

; ------------------------------------
; THE 'ZERO RESULT' TERMINATING BRANCH
; ------------------------------------

L1C3D:  LD      BC,$0400                ; count 4 bytes, fill byte is zero.

L1C40:  LD      (HL),C                  ; insert a zero.
        INC     HL                      ; next location.
        DJNZ    L1C40                   ; repeat for all 4 bytes.

        JP      (IY)                    ; to 'next'.




; -------------
; THE 'F*' WORD
; -------------
; (f1, f2 -- f1*f2)
; Multiplies top two floating point numbers and leaves result on the stack.

L1C46:  DEFB    'F'                     ; 'name field'
        DEFB    '*' + $80

        DEFW    L1BB0                   ; 'link field'

L1C4A:  DEFB    $02                     ; 'name length field'

L1C4B:  DEFW    L1C4D                   ; 'code field'

; ---

L1C4D:  CALL    L1AF4                   ; routine PREP_FP prepares the two
                                        ; numbers on the Data Stack placing the
                                        ; exponents and signs in FP_WS.

        XOR     A                       ; set accumulator to zero.
        CP      B                       ; compare to exponent of (f1).
        SBC     A,A                     ; $00 if zero or $FF
        AND     C                       ; combine with exponent of (f2).

        JR      Z,L1C3D                 ; back if zero to exit via ZERO_RSLT.

        PUSH    HL                      ; save pointer to first number - result.

        LD      BC,$3C02                ; set BC to location before free
                                        ; workspace set to zero by PREP_FP.

        PUSH    BC                      ; push onto machine stack.

        LD      B,$03                   ; count three bytes - six nibbles.

L1C5D:  LD      C,(HL)                  ; fetch BCD pair to C
        INC     HL                      ; address more significant pair.

        EX      (SP),HL                 ; Data Stack pointer to machine stack,
                                        ; workspace pointer to HL.
        INC     HL                      ; increment workspace pointer.

        CALL    L1B55                   ; routine BCD_OP multiplies C by each
                                        ; of the 4 bytes of (f2) laying the
                                        ; result down in workspace at HL

        EX      (SP),HL                 ; swap in multiplier pointer to HL,
                                        ; workspace pointer to machine stack.

        DJNZ    L1C5D                   ; repeat for all three bytes.

        LD      BC,($3C00)              ; fetch raw exponents from FP_WS_00/01
        LD      A,B                     ; add the exponents
        ADD     A,C                     ; together.

        SUB     $42                     ; adjust for sign

        LD      ($3C00),A               ; put the result back in FP_WS_00.

        POP     HL                      ; pop workspace pointer to HL.
        POP     DE                      ; pop result pointer to DE.

        JR      L1C04                   ; back to common code to copy the 4
                                        ; bytes from the workspace to the
                                        ; Data Stack and then set exponent
                                        ; and sign.

; -------------
; THE 'F/' WORD
; -------------
; ( f1, f2 -- f1/f2 )
; Divides two floating point numbers.

L1C76:  DEFB    'F'                     ; 'name field'
        DEFB    '/' + $80

        DEFW    L1C4A                   ; 'link field'

L1C7A:  DEFB    $02                     ; 'name length field'

L1C7B:  DEFW    L1C7D                   ; 'code field'

;---

L1C7D:  CALL    L1AF4                   ; routine PREP_FP prepares the two
                                        ; numbers (f1) and (f2) placing the
                                        ; raw exponents in the first two
                                        ; locations of workspace, the signs in
                                        ; the next location and clearing the
                                        ; sixteen remaining locations.
                                        ; This must be the one that uses them
                                        ; all.

        XOR     A                       ; set accumulator to zero.
        CP      B                       ; compare to exponent of dividend (f1).
        JR      Z,L1C3D                 ; forward if zero to ZERO_RSLT.

        CP      C                       ; compare to exponent of divisor (f2).
        JR      Z,L1C3B                 ; back if zero to Error 8 - Overflow.
                                        ; division by zero.

; HL points to first number on stack, DE to second.

        INC     DE                      ;
        INC     DE                      ;
        LD      A,(DE)                  ; get first two digits to A
        DEC     DE                      ;
        DEC     DE                      ; back to first

        ADD     A,$01                   ; add one (e.g. 99 would give 9A)
        DAA                             ; adjust  (e.g. $9A would be $00 carry)
        EX      AF,AF'                  ; save the flags
        EX      DE,HL                   ; HL now points to divisor

        CALL    L1B43                   ; routine BCD negate the divisor

        EX      DE,HL                   ; point back again.
        PUSH    HL                      ; save pointer to first - the result.

        LD      DE,$3C10                ; destination FP_WS_10
        LD      BC,$0004                ; four bytes

        LDIR                            ; copy to end of FP_WS
                                        ; (+ one byte of list_ws)

        EX      DE,HL                   ; HL points to last cell plus one.
        DEC     HL                      ; Now points to last byte copied.

        LD      B,$05                   ; count 5.

; loop

L1CA2:  PUSH    DE                      ;
        LD      A,(HL)                  ;
        DEC     HL                      ;
        LD      E,(HL)                  ;

        EX      AF,AF'                  ;
        LD      C,A                     ;
        EX      AF,AF'                  ;

        INC     C                       ;
        DEC     C                       ;
        JR      NZ,L1CB0                ;

        LD      E,A                     ;
        JR      L1CCB                   ;

; ---

L1CB0:  PUSH    BC                      ;
        LD      B,$02                   ;

L1CB3:  LD      D,$10                   ;

L1CB5:  SLA     E                       ;
        RLA                             ;
        RL      D                       ;
        JR      NC,L1CB5                ;

        INC     D                       ;

L1CBD:  SUB     C                       ;
        DAA                             ;
        INC     E                       ;
        JR      NC,L1CBD                ;

        DEC     D                       ;
        JR      NZ,L1CBD                ;

        ADD     A,C                     ;
        DAA                             ;
        DEC     E                       ;
        DJNZ    L1CB3                   ;

        POP     BC                      ;

L1CCB:  LD      C,E                     ;
        POP     DE                      ;
        INC     C                       ;
        DEC     C                       ;
        JR      Z,L1CE8                 ;

        PUSH    HL                      ;
        DEC     HL                      ;
        DEC     HL                      ;

        CALL    L1B55                   ; bcd_op mult

        PUSH    DE                      ;

        LD      DE,$FFFB                ; -4
        ADD     HL,DE                   ;

        LD      DE,$3C03                ; FP_WS_03
        LD      A,C                     ;
        LD      (DE),A                  ;

        CALL    L1B53                   ; bcd_op add

        POP     DE                      ;
        POP     HL                      ;
        INC     HL                      ;
        INC     B                       ;

L1CE8:  DJNZ    L1CA2                   ;

        LD      HL,($3C00)              ; FP_WS
        LD      A,H                     ;
        SUB     L                       ;
        ADD     A,$40                   ;

        LD      HL,$3C08                ; FP_WS
        LD      B,A                     ;
        LD      A,($3C0B)               ;
        AND     A                       ;
        JR      NZ,L1CFE                ;

        DEC     B                       ;
        DEC     B                       ;
        DEC     HL                      ;

L1CFE:  LD      (IX+$00),B              ;

        POP     DE                      ;

        JP      L1C04                   ; back to common code to copy the 4
                                        ; bytes from the workspace to the
                                        ; Data Stack and then set exponent
                                        ; and sign.

; ------------------
; THE 'FNEGATE' WORD
; ------------------
; ( f -- -f )
; Floating point negation.
; Toggle the sign bit unless the number is zero (four zero bytes).

L1D05:  DEFM    "FNEGAT"                ; 'name field'
        DEFB    'E' + $80

        DEFW    L1C7A                   ; 'link field'

L1D0E:  DEFB    $07                     ; 'name length field'

L1D0F:  DEFW    L1D11                   ; 'code field'

; ---

L1D11:  RST     18H                     ; pop word from data stack to DE.

        LD      A,D                     ; exponent byte to A.
        AND     A                       ; test for zero.
        JR      Z,L1D18                 ; forward if so to leave undisturbed.

        XOR     $80                     ; else toggle the sign bit

L1D18:  LD      D,A                     ; exponent byte to D.
        RST     10H                     ; push word DE on data stack.

        JP      (IY)                    ; to 'next'.

; --------------
; THE 'INT' WORD
; --------------
; (f -- n)
; Converts signed floating point number to signed single length integer.
; Truncates towards zero.
; Result in range -32768 to 32767

L1D1C:  DEFM    "IN"                    ; 'name field'
        DEFB    'T' + $80

        DEFW    L1D0E                   ; 'link field'

L1D21:  DEFB    $03                     ; 'name length field'

L1D22:  DEFW    L1D24                   ; 'code field'

; ---

L1D24:  LD      HL,($3C3B)              ; fetch value from SPARE.
        DEC     HL                      ; now points to end of data stack.

        LD      DE,$0000                ; initialize 16-bit result.

L1D2B:  LD      A,(HL)                  ; fetch the exponent byte.

        RLCA                            ; double exponent moving sign bit to 0.

        CP      $82                     ; compare exponent to plus 1.
        JR      C,L1D45                 ; forward if number is smaller than 1
                                        ; to return the result DE.

; else the number is >= 1.0

        XOR     A                       ; clear accumulator.
        DEC     HL                      ; point to the first pair of BCD digits.

        CALL    L0732                   ; call shift_fp

        INC     HL                      ; point to exponent.

        EX      DE,HL                   ; pointer to DE, integer to HL.

; before adding in the nibble from the mantissa, multiply any previous result
; by ten.

        LD      B,H                     ; make a copy of HL in BC.
        LD      C,L                     ;

        ADD     HL,HL                   ; * 2
        ADD     HL,HL                   ; * 4
        ADD     HL,BC                   ; * 5
        ADD     HL,HL                   ; * 10

        LD      C,A                     ; leftmost nibble from mantissa to C.
        LD      B,$00                   ; prepare to add just the nibble.
        ADD     HL,BC                   ; add into the result.
        EX      DE,HL                   ; switch back to DE

        JR      L1D2B                   ; back to loop.

; ---

L1D45:  DEC     HL                      ; skip redundant components of Floating
        DEC     HL                      ; Point number addressing the
                                        ; lower two bytes on the data stack.
        LD      (HL),D                  ; insert high-order byte first.
        DEC     HL                      ; point to location beneath.
        LD      (HL),E                  ; insert low-order byte.

        LD      DE,L0D94                ; 'pos' addr.

        JP      L04BF                   ; exit via 'pos' routine.

; -----------------
; THE 'UFLOAT' WORD
; -----------------
; (un -- f)
; Converts unsigned single length integer to floating point.
; e.g. 65535 16 bit number converted to  32-bit float 8-bit sign/exponent
; 6-nibble BCD mantissa.    $45  6 5 5 3 5 0

L1D50:  DEFM    "UFLOA"                 ; 'name field'
        DEFB    'T' +$80

        DEFW    L1D21                   ; 'link field'

L1D58:  DEFB    $06                     ; 'name length field'

L1D59:  DEFW    L1D5B                   ; 'code field'

; ---

L1D5B:  RST     18H                     ; pop word off stack to DE
        EX      DE,HL                   ; now HL

        LD      BC,$1000                ; count 16 bits, set C to zero.
        LD      D,C
        LD      E,C                     ; initialize DE to zero.

L1D62:  ADD     HL,HL                   ; double

        LD      A,E                     ;
        ADC     A,A                     ; add carry to low byte
        DAA                             ; adjust
        LD      E,A                     ;

        LD      A,D                     ;
        ADC     A,A                     ; add carry to high byte
        DAA                             ; adjust
        LD      D,A                     ;

        RL      C                       ; pick up overflow
        DJNZ    L1D62                   ; loop  back for 16 bits

        RST     10H                     ; DE to Data stack.

        LD      D,$46                   ; exponent byte   +6
        LD      E,C                     ; low byte

        RST     10H                     ; higher word of float to stack.

        DEC     HL                      ; point to
        DEC     HL                      ; lower on stack

        CALL    L0740                   ; normalize routine.

        JP      (IY)                    ; to 'next'.

; -------------------
; THE 'CHARACTER SET'
; -------------------
; The 96 ASCII character bitmaps are copied to RAM during initialization and
; the 8x8 characters can afterwards be redefined by the user.
; Some ROM space is saved by supplying the blank top line of most characters
; and in case of the middle range (capitals with no descenders) the bottom
; line as well. Only the final copyright symbol is held in ROM as an 8x8
; character.


; $20 - Character: ' '          CHR$(32)

L1D7B:  DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000

; $21 - Character: '!'          CHR$(33)

        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00000000
        DEFB    %00010000
        DEFB    %00000000

; $22 - Character: '"'          CHR$(34)

        DEFB    %00100100
        DEFB    %00100100
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000

; $23 - Character: '#'          CHR$(35)

        DEFB    %00100100
        DEFB    %01111110
        DEFB    %00100100
        DEFB    %00100100
        DEFB    %01111110
        DEFB    %00100100
        DEFB    %00000000

; $24 - Character: '$'          CHR$(36)

        DEFB    %00001000
        DEFB    %00111110
        DEFB    %00101000
        DEFB    %00111110
        DEFB    %00001010
        DEFB    %00111110
        DEFB    %00001000

; $25 - Character: '%'          CHR$(37)

        DEFB    %01100010
        DEFB    %01100100
        DEFB    %00001000
        DEFB    %00010000
        DEFB    %00100110
        DEFB    %01000110
        DEFB    %00000000

; $26 - Character: '&'          CHR$(38)

        DEFB    %00010000
        DEFB    %00101000
        DEFB    %00010000
        DEFB    %00101010
        DEFB    %01000100
        DEFB    %00111010
        DEFB    %00000000

; $27 - Character: '''          CHR$(39)

        DEFB    %00001000
        DEFB    %00010000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000

; $28 - Character: '('          CHR$(40)

        DEFB    %00000100
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00000100
        DEFB    %00000000

; $29 - Character: ')'          CHR$(42)

        DEFB    %00100000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00100000
        DEFB    %00000000

; $2A - Character: '*'          CHR$(42)

        DEFB    %00000000
        DEFB    %00010100
        DEFB    %00001000
        DEFB    %00111110
        DEFB    %00001000
        DEFB    %00010100
        DEFB    %00000000

; $2B - Character: '+'          CHR$(43)

        DEFB    %00000000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00111110
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00000000

; $2C - Character: ','          CHR$(44)

        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00010000

; $2D - Character: '-'          CHR$(45)

        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00111110
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000

; $2E - Character: '.'          CHR$(46)

        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00011000
        DEFB    %00011000
        DEFB    %00000000

; $2F - Character: '/'          CHR$(47)

        DEFB    %00000000
        DEFB    %00000010
        DEFB    %00000100
        DEFB    %00001000
        DEFB    %00010000
        DEFB    %00100000
        DEFB    %00000000

; $30 - Character: '0'          CHR$(48)

        DEFB    %00111100
        DEFB    %01000110
        DEFB    %01001010
        DEFB    %01010010
        DEFB    %01100010
        DEFB    %00111100
        DEFB    %00000000

; $31 - Character: '1'          CHR$(49)

        DEFB    %00011000
        DEFB    %00101000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00111110
        DEFB    %00000000

; $32 - Character: '2'          CHR$(50)

        DEFB    %00111100
        DEFB    %01000010
        DEFB    %00000010
        DEFB    %00111100
        DEFB    %01000000
        DEFB    %01111110
        DEFB    %00000000

; $33 - Character: '3'          CHR$(51)

        DEFB    %00111100
        DEFB    %01000010
        DEFB    %00001100
        DEFB    %00000010
        DEFB    %01000010
        DEFB    %00111100
        DEFB    %00000000

; $34 - Character: '4'          CHR$(52)

        DEFB    %00001000
        DEFB    %00011000
        DEFB    %00101000
        DEFB    %01001000
        DEFB    %01111110
        DEFB    %00001000
        DEFB    %00000000

; $35 - Character: '5'          CHR$(53)

        DEFB    %01111110
        DEFB    %01000000
        DEFB    %01111100
        DEFB    %00000010
        DEFB    %01000010
        DEFB    %00111100
        DEFB    %00000000

; $36 - Character: '6'          CHR$(54)

        DEFB    %00111100
        DEFB    %01000000
        DEFB    %01111100
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %00111100
        DEFB    %00000000

; $37 - Character: '7'          CHR$(55)

        DEFB    %01111110
        DEFB    %00000010
        DEFB    %00000100
        DEFB    %00001000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00000000

; $38 - Character: '8'          CHR$(56)

        DEFB    %00111100
        DEFB    %01000010
        DEFB    %00111100
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %00111100
        DEFB    %00000000

; $39 - Character: '9'          CHR$(57)

        DEFB    %00111100
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %00111110
        DEFB    %00000010
        DEFB    %00111100
        DEFB    %00000000

; $3A - Character: ':'          CHR$(58)

        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00010000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00010000
        DEFB    %00000000

; $3B - Character: ';'          CHR$(59)

        DEFB    %00000000
        DEFB    %00010000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00100000

; $3C - Character: '<'          CHR$(60)

        DEFB    %00000000
        DEFB    %00000100
        DEFB    %00001000
        DEFB    %00010000
        DEFB    %00001000
        DEFB    %00000100
        DEFB    %00000000

; $3D - Character: '='          CHR$(61)

        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00111110
        DEFB    %00000000
        DEFB    %00111110
        DEFB    %00000000
        DEFB    %00000000

; $3E - Character: '>'          CHR$(62)

        DEFB    %00000000
        DEFB    %00010000
        DEFB    %00001000
        DEFB    %00000100
        DEFB    %00001000
        DEFB    %00010000
        DEFB    %00000000

; $3F - Character: '?'          CHR$(63)

        DEFB    %00111100
        DEFB    %01000010
        DEFB    %00000100
        DEFB    %00001000
        DEFB    %00000000
        DEFB    %00001000

; $40 - Character: '@'          CHR$(64)

        DEFB    %00111100
        DEFB    %01001010
        DEFB    %01010110
        DEFB    %01011110
        DEFB    %01000000
        DEFB    %00111100

; $41 - Character: 'A'          CHR$(65)

        DEFB    %00111100
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01111110
        DEFB    %01000010
        DEFB    %01000010

; $42 - Character: 'B'          CHR$(66)

        DEFB    %01111100
        DEFB    %01000010
        DEFB    %01111100
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01111100

; $43 - Character: 'C'          CHR$(67)

        DEFB    %00111100
        DEFB    %01000010
        DEFB    %01000000
        DEFB    %01000000
        DEFB    %01000010
        DEFB    %00111100

; $44 - Character: 'D'          CHR$(68)

        DEFB    %01111000
        DEFB    %01000100
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01000100
        DEFB    %01111000

; $45 - Character: 'E'          CHR$(69)

        DEFB    %01111110
        DEFB    %01000000
        DEFB    %01111100
        DEFB    %01000000
        DEFB    %01000000
        DEFB    %01111110

; $46 - Character: 'F'          CHR$(70)

        DEFB    %01111110
        DEFB    %01000000
        DEFB    %01111100
        DEFB    %01000000
        DEFB    %01000000
        DEFB    %01000000

; $47 - Character: 'G'          CHR$(71)

        DEFB    %00111100
        DEFB    %01000010
        DEFB    %01000000
        DEFB    %01001110
        DEFB    %01000010
        DEFB    %00111100

; $48 - Character: 'H'          CHR$(72)

        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01111110
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01000010

; $49 - Character: 'I'          CHR$(73)

        DEFB    %00111110
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00111110

; $4A - Character: 'J'          CHR$(74)

        DEFB    %00000010
        DEFB    %00000010
        DEFB    %00000010
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %00111100

; $4B - Character: 'K'          CHR$(75)

        DEFB    %01000100
        DEFB    %01001000
        DEFB    %01110000
        DEFB    %01001000
        DEFB    %01000100
        DEFB    %01000010

; $4C - Character: 'L'          CHR$(76)

        DEFB    %01000000
        DEFB    %01000000
        DEFB    %01000000
        DEFB    %01000000
        DEFB    %01000000
        DEFB    %01111110

; $4D - Character: 'M'          CHR$(77)

        DEFB    %01000010
        DEFB    %01100110
        DEFB    %01011010
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01000010

; $4E - Character: 'N'          CHR$(78)

        DEFB    %01000010
        DEFB    %01100010
        DEFB    %01010010
        DEFB    %01001010
        DEFB    %01000110
        DEFB    %01000010

; $4F - Character: 'O'          CHR$(79)

        DEFB    %00111100
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %00111100

; $50 - Character: 'P'          CHR$(80)

        DEFB    %01111100
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01111100
        DEFB    %01000000
        DEFB    %01000000

; $51 - Character: 'Q'          CHR$(81)

        DEFB    %00111100
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01010010
        DEFB    %01001010
        DEFB    %00111100

; $52 - Character: 'R'          CHR$(82)

        DEFB    %01111100
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01111100
        DEFB    %01000100
        DEFB    %01000010

; $53 - Character: 'S'          CHR$(83)

        DEFB    %00111100
        DEFB    %01000000
        DEFB    %00111100
        DEFB    %00000010
        DEFB    %01000010
        DEFB    %00111100

; $54 - Character: 'T'          CHR$(84)

        DEFB    %11111110
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000

; $55 - Character: 'U'          CHR$(85)

        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %00111110

; $56 - Character: 'V'          CHR$(86)

        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %00100100
        DEFB    %00011000

; $57 - Character: 'W'          CHR$(87)

        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01000010
        DEFB    %01011010
        DEFB    %00100100

; $58 - Character: 'X'          CHR$(88)

        DEFB    %01000010
        DEFB    %00100100
        DEFB    %00011000
        DEFB    %00011000
        DEFB    %00100100
        DEFB    %01000010

; $59 - Character: 'Y'          CHR$(89)

        DEFB    %10000010
        DEFB    %01000100
        DEFB    %00101000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000

; $5A - Character: 'Z'          CHR$(90)

        DEFB    %01111110
        DEFB    %00000100
        DEFB    %00001000
        DEFB    %00010000
        DEFB    %00100000
        DEFB    %01111110

; $5B - Character: '['          CHR$(91)

        DEFB    %00001110
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00001110

; $5C - Character: '\'          CHR$(92)

        DEFB    %00000000
        DEFB    %01000000
        DEFB    %00100000
        DEFB    %00010000
        DEFB    %00001000
        DEFB    %00000100

; $5D - Character: ']'          CHR$(93)

        DEFB    %01110000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %01110000

; $5E - Character: '^'          CHR$(94)

        DEFB    %00010000
        DEFB    %00111000
        DEFB    %01010100
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000

; $5F - Character: '_'          CHR$(95)

        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %11111111

; $60 - Character:  Â£           CHR$(96)

        DEFB    %00011100
        DEFB    %00100010
        DEFB    %01111000
        DEFB    %00100000
        DEFB    %00100000
        DEFB    %01111110
        DEFB    %00000000

; $61 - Character: 'a'          CHR$(97)

        DEFB    %00000000
        DEFB    %00111000
        DEFB    %00000100
        DEFB    %00111100
        DEFB    %01000100
        DEFB    %00111110
        DEFB    %00000000

; $62 - Character: 'b'          CHR$(98)

        DEFB    %00100000
        DEFB    %00100000
        DEFB    %00111100
        DEFB    %00100010
        DEFB    %00100010
        DEFB    %00111100
        DEFB    %00000000

; $63 - Character: 'c'          CHR$(99)

        DEFB    %00000000
        DEFB    %00011100
        DEFB    %00100000
        DEFB    %00100000
        DEFB    %00100000
        DEFB    %00011100
        DEFB    %00000000

; $64 - Character: 'd'          CHR$(100)

        DEFB    %00000100
        DEFB    %00000100
        DEFB    %00111100
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %00111110
        DEFB    %00000000

; $65 - Character: 'e'          CHR$(101)

        DEFB    %00000000
        DEFB    %00111000
        DEFB    %01000100
        DEFB    %01111000
        DEFB    %01000000
        DEFB    %00111100
        DEFB    %00000000

; $66 - Character: 'f'          CHR$(102)

        DEFB    %00001100
        DEFB    %00010000
        DEFB    %00011000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00000000

; $67 - Character: 'g'          CHR$(103)

        DEFB    %00000000
        DEFB    %00111100
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %00111100
        DEFB    %00000100
        DEFB    %00111000

; $68 - Character: 'h'          CHR$(104)

        DEFB    %01000000
        DEFB    %01000000
        DEFB    %01111000
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %00000000

; $69 - Character: 'i'          CHR$(105)

        DEFB    %00010000
        DEFB    %00000000
        DEFB    %00110000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00111000
        DEFB    %00000000

; $6A - Character: 'j'          CHR$(106)

        DEFB    %00000100
        DEFB    %00000000
        DEFB    %00000100
        DEFB    %00000100
        DEFB    %00000100
        DEFB    %00100100
        DEFB    %00011000

; $6B - Character: 'k'          CHR$(107)

        DEFB    %00100000
        DEFB    %00101000
        DEFB    %00110000
        DEFB    %00110000
        DEFB    %00101000
        DEFB    %00100100
        DEFB    %00000000

; $6C - Character: 'l'          CHR$(108)

        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00001100
        DEFB    %00000000

; $6D - Character: 'm'          CHR$(109)

        DEFB    %00000000
        DEFB    %01101000
        DEFB    %01010100
        DEFB    %01010100
        DEFB    %01010100
        DEFB    %01010100
        DEFB    %00000000

; $6E - Character: 'n'          CHR$(110)

        DEFB    %00000000
        DEFB    %01111000
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %00000000

; $6F - Character: 'o'          CHR$(111)

        DEFB    %00000000
        DEFB    %00111000
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %00111000
        DEFB    %00000000

; $70 - Character: 'p'          CHR$(112)

        DEFB    %00000000
        DEFB    %01111000
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %01111000
        DEFB    %01000000
        DEFB    %01000000

; $71 - Character: 'q'          CHR$(113)

        DEFB    %00000000
        DEFB    %00111100
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %00111100
        DEFB    %00000100
        DEFB    %00000110

; $72 - Character: 'r'          CHR$(114)

        DEFB    %00000000
        DEFB    %00011100
        DEFB    %00100000
        DEFB    %00100000
        DEFB    %00100000
        DEFB    %00100000
        DEFB    %00000000

; $73 - Character: 's'          CHR$(115)

        DEFB    %00000000
        DEFB    %00111000
        DEFB    %01000000
        DEFB    %00111000
        DEFB    %00000100
        DEFB    %01111000
        DEFB    %00000000

; $74 - Character: 't'          CHR$(116)

        DEFB    %00010000
        DEFB    %00111000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00010000
        DEFB    %00001100
        DEFB    %00000000

; $75 - Character: 'u'          CHR$(117)

        DEFB    %00000000
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %00111100
        DEFB    %00000000

; $76 - Character: 'v'          CHR$(118)

        DEFB    %00000000
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %00101000
        DEFB    %00101000
        DEFB    %00010000
        DEFB    %00000000

; $77 - Character: 'w'          CHR$(119)

        DEFB    %00000000
        DEFB    %01000100
        DEFB    %01010100
        DEFB    %01010100
        DEFB    %01010100
        DEFB    %00101000
        DEFB    %00000000

; $78 - Character: 'x'          CHR$(120)

        DEFB    %00000000
        DEFB    %01000100
        DEFB    %00101000
        DEFB    %00010000
        DEFB    %00101000
        DEFB    %01000100
        DEFB    %00000000

; $79 - Character: 'y'          CHR$(121)

        DEFB    %00000000
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %01000100
        DEFB    %00111100
        DEFB    %00000100
        DEFB    %00111000

; $7A - Character: 'z'          CHR$(122)

        DEFB    %00000000
        DEFB    %01111100
        DEFB    %00001000
        DEFB    %00010000
        DEFB    %00100000
        DEFB    %01111100
        DEFB    %00000000

; $7B - Character: '{'          CHR$(123)

        DEFB    %00001110
        DEFB    %00001000
        DEFB    %00110000
        DEFB    %00110000
        DEFB    %00001000
        DEFB    %00001110
        DEFB    %00000000

; $7C - Character: '|'          CHR$(124)

        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00001000
        DEFB    %00000000

; $7D - Character: '}'          CHR$(125)

        DEFB    %01110000
        DEFB    %00010000
        DEFB    %00001100
        DEFB    %00001100
        DEFB    %00010000
        DEFB    %01110000
        DEFB    %00000000

; $7E - Character: '~'          CHR$(126)

        DEFB    %00110010
        DEFB    %01001100
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000
        DEFB    %00000000

; $7F - Character:  Â©           CHR$(127)

        DEFB    %00111100
        DEFB    %01000010
        DEFB    %10011001
        DEFB    %10100001
        DEFB    %10100001
        DEFB    %10011001
        DEFB    %01000010
L1
         FFB:  DEFB    %00111100


; ---------------
; THE 'SPARE' ROM
; ---------------

L1FFC:  DEFB    $FF                     ; unused

; ----------
; THE 'LINK'
; ----------

; The FORTH word copied to RAM links back to L1FFF

L1FFD:  DEFW    L1D58                   ; pointer to prev - UFLOAT
L1FFF:  DEFB    $00                     ; length of dummy word zero


.END

; -----------
;
; -----------
; ----------------------
; THE 'SYSTEM VARIABLES'
; ----------------------
; "Here is a list of system variables. We have given them all names, but that
; is just for ease of reference. The Ace will not recognize these names,
; except for a few, like 'BASE', that are FORTH words. I've written these
; FORTH words in bold type in the usual way."
;
;
; FP_WS         $3C00 (15360)   19 bytes used as work space for floating point
;                               arithmetic.
;
; LISTWS        $3C13 (15379)   5 bytes used as workspace by 'LIST' and 'EDIT'.
;
; RAMTOP        $3C18 (15384)   2 bytes - the first address past the last
;                               address in RAM.
;
; HLD           $3C1A (15386)   2 bytes. The address of the latest character
;                               held in the pad by formatted output.
;                               ('#', 'HOLD' and so on).
;
; SCRPOS        $3C1C (15388)   2 bytes. The address of the place in video RAM
;                               where the next character is to be printed
;                               (i.e. the 'print position').
;
; INSCRN        $3C1E (15390)   2 bytes. The address of the start of the
;                               current 'logical line' in the input buffer.
;
; CURSOR        $3C20 (15392)   2 bytes. The address of the cursor in the
;                               input buffer.
;
; ENDBUF        $3C22 (15394)   2 bytes. The address of the end of the current
;                               logical line in the input buffer.
;
; L_HALF        $3C24 (15396)   2 bytes. The address of the start of the the
;                               input buffer. The input buffer itself is stored
;                               in the video RAM, where you see it.
;
; KEYCOD        $3C26 (15398)   1 byte. The ASCII code of the last key pressed.
;
; KEYCNT        $3C27 (15399)   1 byte. Used by the routine that reads the
;                               keyboard.
;
; STATIN        $3C28 (15400)   1 byte. Used by the routine that reads the
;                               keyboard.
;
; EXWRCH        $3C29 (15401)   2 bytes. This is normally 0 but it can be
;                               changed to allow printing to be sent to some
;                               device other than the screen.
;
; FRAMES        $3C2B (15403)   4 bytes. These four bytes form a double length
;                               integer that counts the time since the Ace was
;                               switched on in 50ths of a second.
;
; XCOORD        $3C2F (15407)   1 byte. The x-coordinate last used by 'PLOT'.
;
; YCOORD        $3C30 (15408)   1 byte. The y-coordinate last used by 'PLOT'.
;
; CURRENT       $3C31 (15409)   2 bytes. The parameter field address for the
;                               vocabulary word of the current vocabulary.
;
; CONTEXT       $3C33 (15411)   2 bytes. The parameter field address for the
;                               vocabulary word of the context vocabulary.
;
; VOCLNK        $3C35 (15413)   2 bytes. The address of the fourth byte in the
;                               parameter field - the vocabulary linkage - of
;                               the vocabulary word of the most recently
;                               defined vocabulary.
;
; STKBOT        $3C37 (15415)   2 bytes. The address of the next byte into
;                               which anything will be enclosed in the
;                               dictionary, i.e. one byte past the present end
;                               of the dictionary.
;                               'HERE' is equivalent to 15415 @.
;
; DICT          $3C39 (15417)   2 bytes. The address of the length field in the
;                               newest word in the dictionary. If that length
;                               field is correctly filled in then DICT may
;                               be 0.
;
; SPARE         $3C3B (15419)   2 bytes. The address of the first byte past the
;                               top of the stack.
;
; ERR_NO        $3C3D (15421)   1 byte. This is usually 255, meaning "no error".
;                               If 'ABORT' is used, and ERR_NO is between 0 and
;                               127, then "ERROR" will be printed out, followed
;                               by the error number ERR_NO.
;
; FLAGS         $3C3E (15422)   1 byte. Shows the state of various parts of the
;                               system, each bit showing whether something
;                               particular is happening or not. Some of these
;                               may be useful.
;
;                               Bit 2, when 1, shows that there is an incomplete
;                               definition at the end of the dictionary.
;
;                               Bit 3, when 1, shows that output is to fed into
;                               the input buffer.
;
;                               Bit 4, when 1, shows that the Ace is in
;                               invisible mode.
;
;                               Bit 6, when 1, shows that the Ace is in compile
;                               mode.
;
; BASE          $3C3F (15423)   1 byte. The system number base.
;
;
;
; -----------------------------------------------------------------------------
;                                    ---------
;                                   -------------------------------------------
; ------------                     --------------------------------------------
; ACE KEYBOARD                    ---------
; ------------                   ---------
;
;+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+
;|   ! | |   @ | |   # | |   $ | |   % | |   & | |   ' | |   ( | |   ) | |   _ |
;| 1 []| | 2 []| | 3 []| | 4 []| | 5 []| | 6 []| | 7 []| | 8   | | 9   | | 0 []|
;+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+
; DELETE   CAPS            INV    <=        ^       v        =>  GRAPHIC  DELETE
;  LINE    LOCK           VIDEO
;+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+
;|     | |     | |     | |   < | |   > | |   [ | |   ] | |   Â© | |   ; | |   " |
;| Q   | | W   | | E   | | R   | | T   | | Y   | | U   | | I   | | O   | | P   |
;+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+
;
;+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+
;|   ~ | |   | | |   \ | |   { | |   } | |   ^ | |   - | |   + | |   = | |     |
;| A   | | S   | | D   | | F   | | G   | | H   | | J   | | K   | | L   | |ENTER|
;+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+
;
;+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+
;|     | |   : | |   Â£ | |   ? | |   / | |   * | |   , | |   . | | SYM | |     |
;|SHIFT| | Z   | | X   | | C   | | V   | | B   | | N   | | M   | |SHIFT| |SPACE|
;+-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+ +-----+
;
;
;                     [] mosaic graphic          Â£  currency symbol
;
; -----------------------------------------------------------------------------
