; print a message with NUll terminate byte
; and by including an external file with functions
%include "functions.asm"

global _start

section .data
msg1: db "Hello, brave new world !", 0Ah, 0h
msg2: db "This is how we recycle in nasm", 0Ah, 0h

section .text
_start:
  mov rdi, msg1
  call sprint

  mov rdi, msg2
  call sprint

  call quit

