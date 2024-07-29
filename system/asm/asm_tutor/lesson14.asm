; program to divide two numbers

%include "functions.asm"

global _start

section .data
msg1: db  " remainder "

section .text
_start:
  mov rax, 99
  mov rbx, 9
  div rbx
  push rdx
  mov rdi, rax
  call iprint
  mov rdi, msg1
  call sprint
  pop rdx
  mov rdi, rdx
  call iprintLF

  call quit
