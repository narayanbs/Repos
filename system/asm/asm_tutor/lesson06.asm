; print a message with line feed 

%include "functions.asm"
global _start

section .data
msg1: db "hello brave new world!!", 0h
msg2: db "This is how we recycle in nasm", 0h

section .text
_start:
  mov rdi, msg1
  call sprintLF

  mov rdi, msg2
  call sprintLF

  call quit
