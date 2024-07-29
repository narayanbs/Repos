; program to print the power of a number
; x ^ y 
global main
extern printf
extern puts
extern atoi

section .data

answer: db  "%d",0A,0
badArgumentCount: db  "Requires exactly two arguments", 0A, 0
negativeExponent: db "The exponent may not be negative", 0A, 0

section .text
main:
  push r12
  push r13
  push r14

  cmp rdi, 3
  jne error1

  mov r12, rsi

  mov rdi, [r12 + 16]
  call atoi
  cmp rax, 0
  jl error2
  mov r13, rax

  mov rdi, [r12 + 8]
  call atoi
  mov r14, rax

  mov rax, 1

check:
  test r13d, r13d   ; perform an and operation and see if both are zero. 
  jz gotit
  imul eax, r14d
  dec r13d
  jmp check

gotit:
  mov rdi, answer
  movsxd rsi, eax  ; sign extended move, since one operand os 32-bit and destination is a 64-bit register.
  xor rax, rax
  call printf
  jmp done

error1:
  mov rdi, badArgumentCount
  call puts
  jmp done

error2:
  mov rdi, negativeExponent
  call puts

done:
  pop r14
  pop r13
  pop r12
  ret

