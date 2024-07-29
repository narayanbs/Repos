; fork 

%include "functions.asm"

global _start

section .data
childMsg: db  'This is the child process', 0h
parentMsg: db   'This is the parent process', 0h

section .text
_start:
  mov rax, 57
  syscall

  cmp rax, 0
  jz child

parent:
  mov rdi, parentMsg
  call sprintLF

  call quit

child:
  mov rdi, childMsg
  call sprintLF

  call quit
