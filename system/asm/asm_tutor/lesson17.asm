; fizzbuzz 
; FizzBuzz is group word game played in schools to teach children division. 
; Players take turns to count aloud integers from 1 to 100 replacing any number divisible by 3 with the word "fizz" 
; and any number divisible by 5 with the word "buzz". Numbers that are both divisible by 3 and 5 are replaced by the word "fizzbuzz".
%include "functions.asm"

global _start

section .data
fizz: db  'Fizz', 0h
buzz: db  'Buzz', 0h

section .text
_start:
  mov r8, 0
  mov r9, 0
  mov rcx, 0

nextNumber:
  inc rcx

.checkFizz:
  mov rdx, 0
  mov rax, rcx
  mov rbx, 3
  div rbx
  mov r9, rdx
  cmp r9, 0
  jne .checkBuzz
  push r9
  push rcx
  mov rdi, fizz
  call sprint
  pop rcx
  pop r9

.checkBuzz:
  mov rdx, 0
  mov rax, rcx
  mov rbx, 5
  div rbx
  mov r8, rdx
  cmp r8, 0
  jne .checkInt
  push r8
  push rcx
  mov rdi, buzz
  call sprint
  pop rcx
  pop r8

.checkInt:
  cmp r9, 0
  je .continue
  cmp r8, 0
  je .continue
  push rcx
  mov rdi, rcx
  call iprint
  pop rcx

.continue:
  mov rax, 0Ah
  push rax
  mov rdi, rsp
  push rcx
  call sprint
  pop rcx

  pop rax
  cmp rcx, 100
  jne nextNumber

  call quit
