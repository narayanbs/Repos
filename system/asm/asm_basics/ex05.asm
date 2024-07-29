; program that contains a function 
; to return the maximum of three numbers
;
global maxofthree
section .text

maxofthree:
  mov rax, rdi
  cmp rax, rsi
  cmovl rax, rsi
  cmp rax, rdx
  cmovl rax, rdx
  ret
