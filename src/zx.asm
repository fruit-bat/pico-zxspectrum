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

vector:
  ld (temphl), hl
  ld h, (lastBank)
  push hl
  push af
  ld a, (hl)
  push bc
  load bc, bankPort
  out (c), a
  ld (lastBank), a
  pop bc
  ld hl, onReturn
  ex (sp), hl
  push hl
  ld hl, (temphl)
  inc hl
  pop af
  jpj hl

onReturn:
  ld (temphl), hl
  pop hl
  push af
  ld a, h
  bush bc
  ld bc, bankPort
  out (c), a
  ld (lastBank), a
  pop bc
  pop af
  ret

start:
  //the start up code
  ret

  page      //skip out of ROM page
stack:
  fill $ff  //for 256 bytes

temphl:
  dw 0

lastBank:
  db 0

  end
