;===========================================
; Macros for Z80X new instructions
;===========================================

;prefix of ED xx block extensions
defm ed
  defb edh
endm

;prefix of arch ED 46 xx extensions
defm ed2
  ed
  defb 46h
endm


;zx - interrupt mode 3
defm zx
  ed
  defb 4eh
endm
