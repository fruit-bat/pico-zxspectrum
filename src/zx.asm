//===========================================
// Z80X new instructions too
//===========================================
// Write it how you want it
// The following code is to build a template for making all manor
// of assemblies for the Z80X processor extensions
// the syntax is decided for the Java ZXASM project and used throughout.

// Partially wrote by Simon Jackson @jackokring github.

  org $0000
bankPort:     equ $7ffd         // 128k banking port
portPlusA:    equ $1ffd         // +2A/+3A port for extra ROMs
featurePort:  equ $3ffd         // added feature port

main:
.rest00:
//===========================================
// RESET STACK POINTER
//===========================================
  // divMMC compat ok
  di
  ld sp, endDStack //stack grows down
  im 2
  jr .start
  upto $08
.rst08:
//===========================================
// esxDOS call function (NOT dot command)
//===========================================
  jp .errorDivMMC // almost useless
  // jp nn --------------- $d1e3 address for a vectored crash if no divMMC
  // if there is a divMMC connected it will switch and vector to its own
  // handler. As nn is its own handler in its own ROM.
  // This is about the neatest solution but has no presence checking.

  // The DOS ROM will likely still be paged in on the return
  // and so there needs to be an executed "jp $1FFA" to un-map esxDOS
  // so some copy and execute from memory
  // try "jp .bankoutMMC" after return from any rst $10 x calls
  // can't do it from within ROM

  // The 48k ROM is not paged in so danger on all indirect basic calls
  // RETURN ERROR NULL
.errorDivMMC:
  xor a     //EOK
  scf       // but with carry set error
  pop hl
  inc hl  // skip command
  jp hl
  upto $10
.rst10:        // warm start
//===========================================
// Warm start and load a register set
//===========================================
  jr .warm
.loadReg:
  pop hl
  pop af
  pop bc
  pop de
  ex (sp), hl
  ret
  upto $18
.rst18:
//===========================================
// Evaluation loop and save a register set
//===========================================
  jr .exe
.saveReg:
  ex (sp), hl
  push de
  push bc
  push af
  push hl
  ret
  upto $20
.rst20:
  upto $28
.rst28:
  upto $30
.rst30:
  upto $38
.rst38:          // interrupt IM 2
//===========================================
// IRQ with divMMC sophisticates
//===========================================
  // adapted to leave push af 48k BASIC ROM execution on there divMMC
  push hl                   //1
  ld hl, (userInt)          //3
  jpc hl                    //1 - New!
  //jr sysInt
.sysInt:
  pop hl
  // the system interrupt code fixed in ROM

  reti
//===========================================
// Startup initialization code
//===========================================
.start:
  //the start up code to initial once only values for cold boot

//===========================================
// The Pre-NMI space
//===========================================


  upto $66
.nmi66:
//===========================================
// The NMI
//===========================================
  // might do divMMC compat ok
  push af     // can't find respective pop af in divMMC exit on space
  // so either there's a missing retn or NMI returns via other means
  // very likely does some weird stuff
  pop af
//===========================================
// Warm and evaluation loop (NO ENTRY POINT)
//===========================================
.warm:            // THIS IS NOT THE ENTRY POINT rst 10 (does not stack)
  //ex (sp), hl     // pop down as no return
  pop hl
  // warm boot code and NMI does it too
  ld hl, .userIntDefault  // a default user interrupt
  call .setIntVec
  call .compileEndMMC // make a nice convenience RAM function at bankoutMMC
.exe:             // THIS IS NOT THE ENTRY POINT rst 18 (stacks an instance)
  //main loop as initialized now

  jr .exe               // loop back
//===========================================
// Banking vectoring
//===========================================
.vector:// enter vector with hl set to the 3 byte vector address/bank switch
  ld (temphl), hl
  ld h, (lastBank)
  push hl         //save lastbank used
  push af
  ld a, (hl)
  push bc
  load bc, bankPort
  out (c), a
  ld (lastBank), a  //put new lastBank
  pop bc
  pop af          //restored
  ld hl, .onReturn  //local sub-label address
  push hl           //return there
  ld hl, (temphl)
  inc hl
  jpj hl            //indirect vector and return to .onReturn
.onReturn:          // for .vextor
  ld (temphl), hl
  pop hl
  push af
  ld a, h
  bush bc
  ld bc, bankPort
  out (c), a
  ld (lastBank), a    //restored lastBank
  pop bc
  pop af
  ld hl, (temphl)     //restore hl
  ret
//===========================================
// Copy out of ROM and execute
//===========================================
.spectrum:
  ld bc, portPlusA           //3
  ld a, 4                    //2
  out (c), a                 //2
  ld bc, bankPort            //3
  ld a, 16                   //2
  out (c), a                 //2
  rst 0                      //1
.doSpectrum
  ld bc, 15                 //3
  ld hl, .spectrum          //3
.doHLLenBC:
  ld de, stack              //3
  push de                   //1
  ldir                      //2
  ret                       //1
.compileEndMMC:
  push hl
  push af
  ld hl, bankoutMMC
  ld a, $df             // rst 08 - esxDOS command entry
  ld (hl), a
  inc hl
  // blank for command code
  inc hl
  ld a, $c3
  ld (hl), a
  inc hl
  ld a, $fa
  ld (hl), a
  inc hl
  ld a, $1f
  ld (hl), a
  pop af
  pop hl
  ret
.commandCodeDOS:
  ex (sp), hl
  push af
  ld a, (hl)
  inc hl
  push hl
  ld hl, bankoutMMC
  inc hl
  ld (hl), a
  pop hl
  pop af
  ex (sp), hl
  jp bankoutMMC   // do jump and return management

//===========================================
// Interrupt IRQ vectoring
//===========================================
.setIntVec
  // set up hl with vector to routine ending ret
  // or jp .userIntNoSys to disable system ROM routine post user
  di                        //1
  ld (userInt), hl          //3
  ei                        //1
  ret                       //1
.userIntNoSys:
  pop hl
  pop hl                    // a botch to pop stack using no registers
  reti                      // exit interrupts without doing system things
.userIntDefault:
  // a default null routine
  // always use ret except for when escaping the system
  // see .userIntNoSys for an example
  ret

//===========================================
// Echo to screen
//===========================================
charPrint:      // A is char DE is byte pointer in Y pix|X byte format
  ld b, 7
  ld h, 0
  ccf
  ld l, a
  adc hl, hl
  adc hl, hl
  adc hl, hl
  add h, $3c
  cp h, $40
  jp n, .skipUDG
  add h, $1c       // $5c - $40 UDG gap
.skipUDG:
  ldcr
  ret
attributeGetPrint:
  ld b, (attribute)
attribPrint:    // B is attribute, DE is byte pointer to normalize
  ld l, d
  ld h, 0
  ccf
  adc hl, hl
  adc hl, hl
  and l, $e0
  add h, $58       // base offset to ^attributes
  ld a, l
  add e
  ld l, a
  jr nc, .ok
  inc h
.ok:
  ld (hl), b
  ret
stringPrint:    // C is count, DE is byte pointer, HL is string pointer
  ld a, c
  ld c, 0
  cp c
  ret z
  dec a
  ld c, a
  ld a, (hl)
  inc hl
  push hl
  call charPrint
  dec d
  call attributeGetPrint
  ld a, e
  ld l, 1
  add l
  ld e, a
  jr nc, .ok
  ld a, d
  add 32
.ok:
  ld d, a
  pop hl
  jr stringPrint

notice:
  ds "ZX FORTH ROM"


//===========================================
// Save tape skip (DO NOT CHANGE)
//===========================================
  upto $04c0      // save tap divMMC esxDOS
  jr .fixSaveTap
  nop
.fixSaveTap:




//===========================================
// Load tape skip (DO NOT CHANGE)
//===========================================
  upto $0560      // load tap divMMC esxDOS
  jr .fixLoadTap
  nop
.fixLoadTap:



//===========================================
// bankoutMMC (DO NOT CHANGE)
//===========================================
  // allows banking back in and invariance when no divMMC connected
//  upto $1ff8      // bankoutMMC divMMC esxDOS
//  jr .fixBankMMC
//  ret
//.fixBankMMC:



//===========================================
// Font part in ROM
//===========================================
  upto $3c00          // $4000 - ($80 - $20) * 8
chars:
; $00 - Character: Box         CHR$(0)

  db    %00000000
  db    %00000000
  db    %00111100
  db    %00100100
  db    %00100100
  db    %00111100
  db    %00000000
  db    %00000000

; $01 - Character: Box         CHR$(1)

  db    %00000000
  db    %00000000
  db    %00111111
  db    %00100000
  db    %00100000
  db    %00111111
  db    %00000000
  db    %00000000

; $02 - Character: Box         CHR$(2)

  db    %00000000
  db    %00000000
  db    %11111100
  db    %00000100
  db    %00000100
  db    %11111100
  db    %00000000
  db    %00000000

; $03 - Character: Box         CHR$(3)

  db    %00000000
  db    %00000000
  db    %11111111
  db    %00000000
  db    %00000000
  db    %11111111
  db    %00000000
  db    %00000000

; $04 - Character: Box         CHR$(4)

  db    %00000000
  db    %00000000
  db    %00111100
  db    %00100100
  db    %00100100
  db    %00100100
  db    %00100100
  db    %00100100

; $05 - Character: Box         CHR$(5)

  db    %00000000
  db    %00000000
  db    %00111111
  db    %00100000
  db    %00100000
  db    %00100111
  db    %00100100
  db    %00100100

; $06 - Character: Box         CHR$(6)

  db    %00000000
  db    %00000000
  db    %11111100
  db    %00000100
  db    %00000100
  db    %11100100
  db    %00100100
  db    %00100100

; $07 - Character: Box          CHR$(7)

  db    %00000000
  db    %00000000
  db    %11111111
  db    %00000000
  db    %00000000
  db    %11100111
  db    %00100100
  db    %00100100

; $08 - Character: Box         CHR$(8)

  db    %00100100
  db    %00100100
  db    %00100100
  db    %00100100
  db    %00100100
  db    %00111100
  db    %00000000
  db    %00000000

; $09 - Character: Box         CHR$(9)

  db    %00100100
  db    %00100100
  db    %00100111
  db    %00100000
  db    %00100000
  db    %00111111
  db    %00000000
  db    %00000000

; $0A - Character: Box         CHR$(10)

  db    %00100100
  db    %00100100
  db    %11100100
  db    %00000100
  db    %00000100
  db    %11111100
  db    %00000000
  db    %00000000

; $0B - Character: Box         CHR$(11)

  db    %00100100
  db    %00100100
  db    %11100111
  db    %00000000
  db    %00000000
  db    %11111111
  db    %00000000
  db    %00000000

; $0C - Character: Box         CHR$(12)

  db    %00100100
  db    %00100100
  db    %00100100
  db    %00100100
  db    %00100100
  db    %00100100
  db    %00100100
  db    %00100100

; $0D - Character: Box         CHR$(13)

  db    %00100100
  db    %00100100
  db    %00100111
  db    %00100000
  db    %00100000
  db    %00100111
  db    %00100100
  db    %00100100

; $0E - Character: Box         CHR$(14)

  db    %00100100
  db    %00100100
  db    %11100100
  db    %00000100
  db    %00000100
  db    %11100100
  db    %00100100
  db    %00100100

; $0F - Character: Box         CHR$(15)

  db    %00100100
  db    %00100100
  db    %11100111
  db    %00000000
  db    %00000000
  db    %11100111
  db    %00100100
  db    %00100100

; $30 - Character: '0'          CHR$(48)

  db    %00000000
  db    %00111100
  db    %01000110
  db    %01001010
  db    %01010010
  db    %01100010
  db    %00111100
  db    %00000000

; $31 - Character: '1'          CHR$(49)

  db    %00000000
  db    %00011000
  db    %00101000
  db    %00001000
  db    %00001000
  db    %00001000
  db    %00111110
  db    %00000000

; $32 - Character: '2'          CHR$(50)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %00000010
  db    %00111100
  db    %01000000
  db    %01111110
  db    %00000000

; $33 - Character: '3'          CHR$(51)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %00001100
  db    %00000010
  db    %01000010
  db    %00111100
  db    %00000000

; $34 - Character: '4'          CHR$(52)

  db    %00000000
  db    %00001000
  db    %00011000
  db    %00101000
  db    %01001000
  db    %01111110
  db    %00001000
  db    %00000000

; $35 - Character: '5'          CHR$(53)

  db    %00000000
  db    %01111110
  db    %01000000
  db    %01111100
  db    %00000010
  db    %01000010
  db    %00111100
  db    %00000000

; $36 - Character: '6'          CHR$(54)

  db    %00000000
  db    %00111100
  db    %01000000
  db    %01111100
  db    %01000010
  db    %01000010
  db    %00111100
  db    %00000000

; $37 - Character: '7'          CHR$(55)

  db    %00000000
  db    %01111110
  db    %00000010
  db    %00000100
  db    %00001000
  db    %00010000
  db    %00010000
  db    %00000000

; $38 - Character: '8'          CHR$(56)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %00111100
  db    %01000010
  db    %01000010
  db    %00111100
  db    %00000000

; $39 - Character: '9'          CHR$(57)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %01000010
  db    %00111110
  db    %00000010
  db    %00111100
  db    %00000000

; $3A - Character: ':'          CHR$(58)

  db    %00000000
  db    %00000000
  db    %00000000
  db    %00010000
  db    %00000000
  db    %00000000
  db    %00010000
  db    %00000000

; $3B - Character: ';'          CHR$(59)

  db    %00000000
  db    %00000000
  db    %00010000
  db    %00000000
  db    %00000000
  db    %00010000
  db    %00010000
  db    %00100000

; $3C - Character: '<'          CHR$(60)

  db    %00000000
  db    %00000000
  db    %00000100
  db    %00001000
  db    %00010000
  db    %00001000
  db    %00000100
  db    %00000000

; $3D - Character: '='          CHR$(61)

  db    %00000000
  db    %00000000
  db    %00000000
  db    %00111110
  db    %00000000
  db    %00111110
  db    %00000000
  db    %00000000

; $1E - Character: SI micro     CHR$(30)

  db    %00000000
  db    %00000000
  db    %00000000
  db    %01000100
  db    %01000100
  db    %01000100
  db    %01111010
  db    %01000000

; $1F - Character: SI Omega     CHR$(31)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %01000010
  db    %00100100
  db    %00100100
  db    %01100110
  db    %00000000

; $20 - Character: ' '          CHR$(32)

  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000

; $21 - Character: '!'          CHR$(33)

  db    %00000000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00000000
  db    %00010000
  db    %00000000

; $22 - Character: '"'          CHR$(34)

  db    %00000000
  db    %00100100
  db    %00100100
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000

; $23 - Character: '#'          CHR$(35)

  db    %00000000
  db    %00100100
  db    %01111110
  db    %00100100
  db    %00100100
  db    %01111110
  db    %00100100
  db    %00000000

; $24 - Character: '$'          CHR$(36)

  db    %00000000
  db    %00001000
  db    %00111110
  db    %00101000
  db    %00111110
  db    %00001010
  db    %00111110
  db    %00001000

; $25 - Character: '%'          CHR$(37)

  db    %00000000
  db    %01100010
  db    %01100100
  db    %00001000
  db    %00010000
  db    %00100110
  db    %01000110
  db    %00000000

; $26 - Character: '&'          CHR$(38)

  db    %00000000
  db    %00010000
  db    %00101000
  db    %00010000
  db    %00101010
  db    %01000100
  db    %00111010
  db    %00000000

; $27 - Character: '''          CHR$(39)

  db    %00000000
  db    %00001000
  db    %00010000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000

; $28 - Character: '('          CHR$(40)

  db    %00000000
  db    %00000100
  db    %00001000
  db    %00001000
  db    %00001000
  db    %00001000
  db    %00000100
  db    %00000000

; $29 - Character: ')'          CHR$(41)

  db    %00000000
  db    %00100000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00100000
  db    %00000000

; $2A - Character: '*'          CHR$(42)

  db    %00000000
  db    %00000000
  db    %00010100
  db    %00001000
  db    %00111110
  db    %00001000
  db    %00010100
  db    %00000000

; $2B - Character: '+'          CHR$(43)

  db    %00000000
  db    %00000000
  db    %00001000
  db    %00001000
  db    %00111110
  db    %00001000
  db    %00001000
  db    %00000000

; $2C - Character: ','          CHR$(44)

  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00001000
  db    %00001000
  db    %00010000

; $2D - Character: '-'          CHR$(45)

  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00111110
  db    %00000000
  db    %00000000
  db    %00000000

; $2E - Character: '.'          CHR$(46)

  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00011000
  db    %00011000
  db    %00000000

; $2F - Character: '/'          CHR$(47)

  db    %00000000
  db    %00000000
  db    %00000010
  db    %00000100
  db    %00001000
  db    %00010000
  db    %00100000
  db    %00000000

; $30 - Character: '0'          CHR$(48)

  db    %00000000
  db    %00111100
  db    %01000110
  db    %01001010
  db    %01010010
  db    %01100010
  db    %00111100
  db    %00000000

; $31 - Character: '1'          CHR$(49)

  db    %00000000
  db    %00011000
  db    %00101000
  db    %00001000
  db    %00001000
  db    %00001000
  db    %00111110
  db    %00000000

; $32 - Character: '2'          CHR$(50)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %00000010
  db    %00111100
  db    %01000000
  db    %01111110
  db    %00000000

; $33 - Character: '3'          CHR$(51)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %00001100
  db    %00000010
  db    %01000010
  db    %00111100
  db    %00000000

; $34 - Character: '4'          CHR$(52)

  db    %00000000
  db    %00001000
  db    %00011000
  db    %00101000
  db    %01001000
  db    %01111110
  db    %00001000
  db    %00000000

; $35 - Character: '5'          CHR$(53)

  db    %00000000
  db    %01111110
  db    %01000000
  db    %01111100
  db    %00000010
  db    %01000010
  db    %00111100
  db    %00000000

; $36 - Character: '6'          CHR$(54)

  db    %00000000
  db    %00111100
  db    %01000000
  db    %01111100
  db    %01000010
  db    %01000010
  db    %00111100
  db    %00000000

; $37 - Character: '7'          CHR$(55)

  db    %00000000
  db    %01111110
  db    %00000010
  db    %00000100
  db    %00001000
  db    %00010000
  db    %00010000
  db    %00000000

; $38 - Character: '8'          CHR$(56)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %00111100
  db    %01000010
  db    %01000010
  db    %00111100
  db    %00000000

; $39 - Character: '9'          CHR$(57)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %01000010
  db    %00111110
  db    %00000010
  db    %00111100
  db    %00000000

; $3A - Character: ':'          CHR$(58)

  db    %00000000
  db    %00000000
  db    %00000000
  db    %00010000
  db    %00000000
  db    %00000000
  db    %00010000
  db    %00000000

; $3B - Character: ';'          CHR$(59)

  db    %00000000
  db    %00000000
  db    %00010000
  db    %00000000
  db    %00000000
  db    %00010000
  db    %00010000
  db    %00100000

; $3C - Character: '<'          CHR$(60)

  db    %00000000
  db    %00000000
  db    %00000100
  db    %00001000
  db    %00010000
  db    %00001000
  db    %00000100
  db    %00000000

; $3D - Character: '='          CHR$(61)

  db    %00000000
  db    %00000000
  db    %00000000
  db    %00111110
  db    %00000000
  db    %00111110
  db    %00000000
  db    %00000000

; $3E - Character: '>'          CHR$(62)

  db    %00000000
  db    %00000000
  db    %00010000
  db    %00001000
  db    %00000100
  db    %00001000
  db    %00010000
  db    %00000000

; $3F - Character: '?'          CHR$(63)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %00000100
  db    %00001000
  db    %00000000
  db    %00001000
  db    %00000000

; $40 - Character: '@'          CHR$(64)

  db    %00000000
  db    %00111100
  db    %01001010
  db    %01010110
  db    %01011110
  db    %01000000
  db    %00111100
  db    %00000000

; $41 - Character: 'A'          CHR$(65)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %01000010
  db    %01111110
  db    %01000010
  db    %01000010
  db    %00000000

; $42 - Character: 'B'          CHR$(66)

  db    %00000000
  db    %01111100
  db    %01000010
  db    %01111100
  db    %01000010
  db    %01000010
  db    %01111100
  db    %00000000

; $43 - Character: 'C'          CHR$(67)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %01000000
  db    %01000000
  db    %01000010
  db    %00111100
  db    %00000000

; $44 - Character: 'D'          CHR$(68)

  db    %00000000
  db    %01111000
  db    %01000100
  db    %01000010
  db    %01000010
  db    %01000100
  db    %01111000
  db    %00000000

; $45 - Character: 'E'          CHR$(69)

  db    %00000000
  db    %01111110
  db    %01000000
  db    %01111100
  db    %01000000
  db    %01000000
  db    %01111110
  db    %00000000

; $46 - Character: 'F'          CHR$(70)

  db    %00000000
  db    %01111110
  db    %01000000
  db    %01111100
  db    %01000000
  db    %01000000
  db    %01000000
  db    %00000000

; $47 - Character: 'G'          CHR$(71)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %01000000
  db    %01001110
  db    %01000010
  db    %00111100
  db    %00000000

; $48 - Character: 'H'          CHR$(72)

  db    %00000000
  db    %01000010
  db    %01000010
  db    %01111110
  db    %01000010
  db    %01000010
  db    %01000010
  db    %00000000

; $49 - Character: 'I'          CHR$(73)

  db    %00000000
  db    %00111110
  db    %00001000
  db    %00001000
  db    %00001000
  db    %00001000
  db    %00111110
  db    %00000000

; $4A - Character: 'J'          CHR$(74)

  db    %00000000
  db    %00000010
  db    %00000010
  db    %00000010
  db    %01000010
  db    %01000010
  db    %00111100
  db    %00000000

; $4B - Character: 'K'          CHR$(75)

  db    %00000000
  db    %01000100
  db    %01001000
  db    %01110000
  db    %01001000
  db    %01000100
  db    %01000010
  db    %00000000

; $4C - Character: 'L'          CHR$(76)

  db    %00000000
  db    %01000000
  db    %01000000
  db    %01000000
  db    %01000000
  db    %01000000
  db    %01111110
  db    %00000000

; $4D - Character: 'M'          CHR$(77)

  db    %00000000
  db    %01000010
  db    %01100110
  db    %01011010
  db    %01000010
  db    %01000010
  db    %01000010
  db    %00000000

; $4E - Character: 'N'          CHR$(78)

  db    %00000000
  db    %01000010
  db    %01100010
  db    %01010010
  db    %01001010
  db    %01000110
  db    %01000010
  db    %00000000

; $4F - Character: 'O'          CHR$(79)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %01000010
  db    %01000010
  db    %01000010
  db    %00111100
  db    %00000000

; $50 - Character: 'P'          CHR$(80)

  db    %00000000
  db    %01111100
  db    %01000010
  db    %01000010
  db    %01111100
  db    %01000000
  db    %01000000
  db    %00000000

; $51 - Character: 'Q'          CHR$(81)

  db    %00000000
  db    %00111100
  db    %01000010
  db    %01000010
  db    %01010010
  db    %01001010
  db    %00111100
  db    %00000000

; $52 - Character: 'R'          CHR$(82)

  db    %00000000
  db    %01111100
  db    %01000010
  db    %01000010
  db    %01111100
  db    %01000100
  db    %01000010
  db    %00000000

; $53 - Character: 'S'          CHR$(83)

  db    %00000000
  db    %00111100
  db    %01000000
  db    %00111100
  db    %00000010
  db    %01000010
  db    %00111100
  db    %00000000

; $54 - Character: 'T'          CHR$(84)

  db    %00000000
  db    %11111110
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00000000

; $55 - Character: 'U'          CHR$(85)

  db    %00000000
  db    %01000010
  db    %01000010
  db    %01000010
  db    %01000010
  db    %01000010
  db    %00111100
  db    %00000000

; $56 - Character: 'V'          CHR$(86)

  db    %00000000
  db    %01000010
  db    %01000010
  db    %01000010
  db    %01000010
  db    %00100100
  db    %00011000
  db    %00000000

; $57 - Character: 'W'          CHR$(87)

  db    %00000000
  db    %01000010
  db    %01000010
  db    %01000010
  db    %01000010
  db    %01011010
  db    %00100100
  db    %00000000

; $58 - Character: 'X'          CHR$(88)

  db    %00000000
  db    %01000010
  db    %00100100
  db    %00011000
  db    %00011000
  db    %00100100
  db    %01000010
  db    %00000000

; $59 - Character: 'Y'          CHR$(89)

  db    %00000000
  db    %10000010
  db    %01000100
  db    %00101000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00000000

; $5A - Character: 'Z'          CHR$(90)

  db    %00000000
  db    %01111110
  db    %00000100
  db    %00001000
  db    %00010000
  db    %00100000
  db    %01111110
  db    %00000000

; $5B - Character: '['          CHR$(91)

  db    %00000000
  db    %00001110
  db    %00001000
  db    %00001000
  db    %00001000
  db    %00001000
  db    %00001110
  db    %00000000

; $5C - Character: '\'          CHR$(92)

  db    %00000000
  db    %00000000
  db    %01000000
  db    %00100000
  db    %00010000
  db    %00001000
  db    %00000100
  db    %00000000

; $5D - Character: ']'          CHR$(93)

  db    %00000000
  db    %01110000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %01110000
  db    %00000000

; $5E - Character: '^'          CHR$(94)

  db    %00000000
  db    %00010000
  db    %00111000
  db    %01010100
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00000000

; $5F - Character: '_'          CHR$(95)

  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %11111111

; $60 - Character: ' £ '        CHR$(96)

  db    %00000000
  db    %00011100
  db    %00100010
  db    %01111000
  db    %00100000
  db    %00100000
  db    %01111110
  db    %00000000

; $61 - Character: 'a'          CHR$(97)

  db    %00000000
  db    %00000000
  db    %00111000
  db    %00000100
  db    %00111100
  db    %01000100
  db    %00111100
  db    %00000000

; $62 - Character: 'b'          CHR$(98)

  db    %00000000
  db    %00100000
  db    %00100000
  db    %00111100
  db    %00100010
  db    %00100010
  db    %00111100
  db    %00000000

; $63 - Character: 'c'          CHR$(99)

  db    %00000000
  db    %00000000
  db    %00011100
  db    %00100000
  db    %00100000
  db    %00100000
  db    %00011100
  db    %00000000

; $64 - Character: 'd'          CHR$(100)

  db    %00000000
  db    %00000100
  db    %00000100
  db    %00111100
  db    %01000100
  db    %01000100
  db    %00111100
  db    %00000000

; $65 - Character: 'e'          CHR$(101)

  db    %00000000
  db    %00000000
  db    %00111000
  db    %01000100
  db    %01111000
  db    %01000000
  db    %00111100
  db    %00000000

; $66 - Character: 'f'          CHR$(102)

  db    %00000000
  db    %00001100
  db    %00010000
  db    %00011000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00000000

; $67 - Character: 'g'          CHR$(103)

  db    %00000000
  db    %00000000
  db    %00111100
  db    %01000100
  db    %01000100
  db    %00111100
  db    %00000100
  db    %00111000

; $68 - Character: 'h'          CHR$(104)

  db    %00000000
  db    %01000000
  db    %01000000
  db    %01111000
  db    %01000100
  db    %01000100
  db    %01000100
  db    %00000000

; $69 - Character: 'i'          CHR$(105)

  db    %00000000
  db    %00010000
  db    %00000000
  db    %00110000
  db    %00010000
  db    %00010000
  db    %00111000
  db    %00000000

; $6A - Character: 'j'          CHR$(106)

  db    %00000000
  db    %00000100
  db    %00000000
  db    %00000100
  db    %00000100
  db    %00000100
  db    %00100100
  db    %00011000

; $6B - Character: 'k'          CHR$(107)

  db    %00000000
  db    %00100000
  db    %00101000
  db    %00110000
  db    %00110000
  db    %00101000
  db    %00100100
  db    %00000000

; $6C - Character: 'l'          CHR$(108)

  db    %00000000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00001100
  db    %00000000

; $6D - Character: 'm'          CHR$(109)

  db    %00000000
  db    %00000000
  db    %01101000
  db    %01010100
  db    %01010100
  db    %01010100
  db    %01010100
  db    %00000000

; $6E - Character: 'n'          CHR$(110)

  db    %00000000
  db    %00000000
  db    %01111000
  db    %01000100
  db    %01000100
  db    %01000100
  db    %01000100
  db    %00000000

; $6F - Character: 'o'          CHR$(111)

  db    %00000000
  db    %00000000
  db    %00111000
  db    %01000100
  db    %01000100
  db    %01000100
  db    %00111000
  db    %00000000

; $70 - Character: 'p'          CHR$(112)

  db    %00000000
  db    %00000000
  db    %01111000
  db    %01000100
  db    %01000100
  db    %01111000
  db    %01000000
  db    %01000000

; $71 - Character: 'q'          CHR$(113)

  db    %00000000
  db    %00000000
  db    %00111100
  db    %01000100
  db    %01000100
  db    %00111100
  db    %00000100
  db    %00000110

; $72 - Character: 'r'          CHR$(114)

  db    %00000000
  db    %00000000
  db    %00011100
  db    %00100000
  db    %00100000
  db    %00100000
  db    %00100000
  db    %00000000

; $73 - Character: 's'          CHR$(115)

  db    %00000000
  db    %00000000
  db    %00111000
  db    %01000000
  db    %00111000
  db    %00000100
  db    %01111000
  db    %00000000

; $74 - Character: 't'          CHR$(116)

  db    %00000000
  db    %00010000
  db    %00111000
  db    %00010000
  db    %00010000
  db    %00010000
  db    %00001100
  db    %00000000

; $75 - Character: 'u'          CHR$(117)

  db    %00000000
  db    %00000000
  db    %01000100
  db    %01000100
  db    %01000100
  db    %01000100
  db    %00111000
  db    %00000000

; $76 - Character: 'v'          CHR$(118)

  db    %00000000
  db    %00000000
  db    %01000100
  db    %01000100
  db    %00101000
  db    %00101000
  db    %00010000
  db    %00000000

; $77 - Character: 'w'          CHR$(119)

  db    %00000000
  db    %00000000
  db    %01000100
  db    %01010100
  db    %01010100
  db    %01010100
  db    %00101000
  db    %00000000

; $78 - Character: 'x'          CHR$(120)

  db    %00000000
  db    %00000000
  db    %01000100
  db    %00101000
  db    %00010000
  db    %00101000
  db    %01000100
  db    %00000000

; $79 - Character: 'y'          CHR$(121)

  db    %00000000
  db    %00000000
  db    %01000100
  db    %01000100
  db    %01000100
  db    %00111100
  db    %00000100
  db    %00111000

; $7A - Character: 'z'          CHR$(122)

  db    %00000000
  db    %00000000
  db    %01111100
  db    %00001000
  db    %00010000
  db    %00100000
  db    %01111100
  db    %00000000

; $7B - Character: '{'          CHR$(123)

  db    %00000000
  db    %00001110
  db    %00001000
  db    %00110000
  db    %00001000
  db    %00001000
  db    %00001110
  db    %00000000

; $7C - Character: '|'          CHR$(124)

  db    %00000000
  db    %00001000
  db    %00001000
  db    %00001000
  db    %00001000
  db    %00001000
  db    %00001000
  db    %00000000

; $7D - Character: '}'          CHR$(125)

  db    %00000000
  db    %01110000
  db    %00010000
  db    %00001100
  db    %00010000
  db    %00010000
  db    %01110000
  db    %00000000

; $7E - Character: '~'          CHR$(126)

  db    %00000000
  db    %00010100
  db    %00101000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000
  db    %00000000

; $7F - Character: ' © '        CHR$(127)

  db    %00111100
  db    %01000010
  db    %10011001
  db    %10100001
  db    %10100001
  db    %10011001
  db    %01000010
  db    %00111100
  upto $4000    // end ROM

//===========================================
// RAM Start (Bank 5)
//===========================================
screen:
  upto $5800
attributes:
  upto $5b00
padBuffer:
  upto $5c00
charUDG:
  upto $6000
  // stacks for forth
stack:
  fill $ff  //for 256 bytes
endStack:
dStack:
  fill $ff  //for 256 bytes
  // system variables
endDStack:
temphl:
  dw 0
userInt:
  dw 0
lastBank:
  dw 0
bankoutMMC:
  db 0, 0, 0, 0, 0
attribute:
  db 0


  // vectors if paging used
vectors:
  // paged vectored jumps
  db @start //@ for page and screen 1
  dw start
  //more vectors here

codeEnd:
  end
