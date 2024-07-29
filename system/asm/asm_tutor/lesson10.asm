; program to count from 1 to 10 
%include "functions.asm"

global _start

section .text
_start:
  mov rcx, 0

nextNumber:
  inc rcx
  push rcx
  mov rdi, rcx
  call iprintLF
  pop rcx
  cmp rcx, 10
  jne nextNumber

  call quit
