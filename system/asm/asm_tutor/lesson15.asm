; atoi function checker

%include "functions.asm"

global _start

section .text
_start:
  pop rcx
  pop rdx
  sub rcx, 1
  mov rdx, 0

nextArg:
  cmp rcx, 0h
  jz noMoreArgs
  pop rdi
  call atoi
  add rdx, rax
  dec rcx
  jmp nextArg

noMoreArgs:
  mov rdi, rdx
  call iprintLF
  call quit
