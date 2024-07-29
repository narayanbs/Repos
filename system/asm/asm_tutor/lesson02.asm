; print a message by calculating the length of the string

global _start

section .data
msg:  db "Hello, brave new world!", 0Ah

section .text

_start:
  mov rbx, msg
  mov rax, rbx

nextchar:
  cmp byte[rax], 0
  jz finished
  inc rax
  jmp nextchar

finished:
  sub rax, rbx
  mov rbx, rax

  mov rax, 1
  mov rdi, 1
  mov rsi, msg
  mov rdx, rbx
  syscall

  mov rax, 60
  mov rdi, 0
  syscall



