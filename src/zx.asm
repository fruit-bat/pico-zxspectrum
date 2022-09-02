;===========================================
; Z80X new instructions too
;===========================================

  org $0000
  equ bankPort, $7ffd

main:
  ld sp, stack. //"." for last before next symbol as stack grows down
  ld hl, startVec
  jp vector

startVec:
  db @start //@ for page and screen 1
  dw start
  //more vectors here

vector:// enter vector with hl set to the 3 byte vector address/bank switch
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
  jpj hl            //indirect vector to return to onReturn

.onReturn:
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

start:
  //the start up code
  ret

  bank      //skip out of ROM page into RAM initial bank
stack:
  fill $ff  //for 256 bytes

temphl:
  dw 0

lastBank:
  db 0

  end
