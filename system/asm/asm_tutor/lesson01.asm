; hello world asm

global _start

section .data
msg:  db 'Hello World!', 0Ah

section .text
_start:
  mov rax, 1
  mov rdi, 1
  mov rsi, msg
  mov rdx, 13
  syscall

  mov rax, 60
  mov rdi, 0
  syscall
