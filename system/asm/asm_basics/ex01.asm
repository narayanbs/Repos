; Program to print a message 

global _start

section .data
message: db "Hello chunai",0Ah

section .text
_start:
  ; write(sysout, message, messagelen)
  mov rax, 1     
  mov rdi, 1
  mov rsi, message
  mov rdx, 13
  syscall

  ; exit(0)
  mov rax, 60
  xor rdi, rdi
  syscall
