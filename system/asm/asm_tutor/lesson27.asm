; delete a file (unlink)
%include "functions.asm"

global _start

section .data
filename: db  "readme.txt", 0h

section .text
_start:
  
  mov rax, 87
  mov rdi, filename
  syscall 

  call quit
