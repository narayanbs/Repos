; time

%include "functions.asm"

global _start

section .data
msg:  db  'Seconds since Jan 01 1970: ', 0h

section .text
_start:
  mov rdi, msg
  call sprint

  mov rax, 201
  xor rdi, rdi
  syscall

  mov rdi, rax
  call iprintLF

  call quit
