  ; Program that shows the abi for vargag function with vector arguments.
  ; integers are passed in rdi, rsi, rdx ... 
  ; and floating point arguments are passed in xmm0, xmm1, ....
  ; Note rax should contain the number of vector (floating point) arguments,
  ; when we invoke printf. This is part of the ABI
global _start

  extern printf

  section .data
float_value: dq 3.14234
float_value1: dq 4.444
format: db "%f and %d and %f and %d ", 0ah

  section .text
_start:
  mov rdi, format
  mov rsi, 100
  mov rdx, 200
  movsd xmm0, [float_value] 
  movsd xmm1, [float_value1]
  mov rax, 2
  call printf

  mov rax, 60
  xor rdi, rdi
  syscall
  

