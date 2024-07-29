; program to print fibonacci series 
; 
global main
extern printf

section .data
format: db "%20ld", 0A, 0

section .text
main:
  push rbx

  mov rcx, 90
  xor rax, rax
  xor rbx, rbx
  inc rbx

print:
  push rax
  push rcx

  mov rdi, format
  mov rsi, rax
  xor rax, rax

  call printf

  pop rcx
  pop rax

  mov rdx, rax
  mov rax, rbx
  add rbx, rdx
  dec rcx
  jnz print

  pop rbx
  ret
