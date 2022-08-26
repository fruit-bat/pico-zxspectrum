;===========================================
; Macros for Z80X new instructions
;===========================================

;prefix of ED xx block extensions
macro ed
  defb edh
endm

;prefix of arch ED 46 xx extensions
macro ed46
  ed
  defb 46h
endm

; get register value based on low nibble
macro reglownib reg
if reg=="b"
  0
else
if reg=="c"
  1
else
if reg=="d"
  2
else
if reg=="e"
  3
else
if reg=="h"
  4
else
if reg=="l"
  5
else
if reg=="(hl)"
  6
else
if reg=="a"
  7
else
  ;fail
  x
endif
endif
endif
endif
endif
endif
endif
endif
endm

;zx - interrupt mode 3
macro zx
  ed
  defb 4eh
endm

;ld - extension
macro ld tar, src
if tar=="(bc)"&~((src=="a")|(src=="(hl)"))
  ed
  defb ((reglownib src)|88h)
else
if tar=="(de)"&~((src=="a")|(src=="(hl)"))
  ed
  defb ((reglownib src)|98h)
else
if src=="(bc)"&~((tar=="a")|(tar=="(hl)"))
  ed
  defb ((reglownib tar)|80h)
else
if src=="(bc)"&~((tar=="a")|(tar=="(hl)"))
  ed
  defb ((reglownib tar)|90h)
else
  ld tar, src
endif
endif
endif
endif
endm
