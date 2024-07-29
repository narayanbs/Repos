; file close

%include "functions.asm"

global _start

section .data
filename: db  "readme.txt", 0h
contents: db  "Hello chunai", 0h

section .bss
filecontents: resb  255

section .text
_start:

  mov rax, 85
  mov rdi, filename
  mov rsi, 0777o
  syscall

  mov rdi, rax

  mov rax, 1
  mov rsi, contents
  mov rdx, 13
  syscall

  mov rax, 2
  mov rdi, filename
  mov rsi, 0
  xor rdx, rdx
  syscall 

  mov rbx, rax

  mov rax, 0
  mov rdi, rbx
  mov rsi, filecontents
  mov rdx, 20
  syscall

  mov rdi, filecontents
  call sprintLF

  mov rax, 3
  mov rdi, rbx
  syscall

  call quit
   

