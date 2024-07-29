; file creation
%include "functions.asm"

global _start

section .data
filename: db  'readme.txt', 0h

section .text
_start:
  mov rax, 85     ; syscall for create
  mov rdi, filename
  mov rsi, 0777o ;  permissions
  syscall

  call quit
