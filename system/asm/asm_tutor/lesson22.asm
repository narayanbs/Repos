; file write
%include "functions.asm"

global _start

section .data
filename: db  'readme.txt', 0h
contents: db  'Hello chunai', 0h

section .text
_start:
  mov rax, 85       ; create file
  mov rdi, filename
  mov rsi, 0777o
  syscall

  mov rbx, rax    ; create file returns file descriptor in rax register

  mov rax, 1      ; syscall for write
  mov rdi, rbx
  mov rsi, contents
  mov rdx, 13
  syscall 

  call quit
