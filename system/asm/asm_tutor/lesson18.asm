; Execute

%include "functions.asm"

global _start

section .data
command:  db  '/usr/bin/echo', 0h
arg1: db  'Hello world asdfgf', 0h
arguments:  dq  command
            dq  arg1
            dq  0h
environment:  dq  0h

section .text
_start:
  mov rdx, environment
  mov rsi, arguments
  mov rdi, command
  mov rax, 59   ; execve system call 
  syscall

  call quit
  

