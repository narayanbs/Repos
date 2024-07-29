; print a message without NUll terminate byte
; and by including an external file with functions
%include "functions.asm"

global _start

section .data
msg1: db "Hello, brave new world !", 0Ah
msg2: db "This is how we recycle in nasm", 0Ah

section .text
_start:
  mov rdi, msg1
  call sprint

  mov rdi, msg2
  call sprint

  call quit

