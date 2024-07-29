; passing arguments

%include "functions.asm"

global _start

section .text

_start:
  pop rcx

nextArg:
  cmp rcx, 0h
  jz noMoreArgs
  pop rdi
  push rcx
  call sprintLF
  pop rcx
  dec rcx
  jmp nextArg

noMoreArgs:
  call quit
