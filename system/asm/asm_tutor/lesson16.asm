; Namespace (adding .label )
;Local labels are prepended with a "." at the beginning of their name for example ".finished"
; A local label is given the namespace of the first global label above it. 
; You can jump to a local label by using the JMP instruction and the compiler will calculate which local label
; you are referencing by determining in what scope (based on the above global labels) the instruction was called.

%include "functions.asm"

global _start

section .data
msg1: db  "Jumping to finished label.", 0h
msg2: db  "Inside subroutine number: ", 0h
msg3: db  "Inside subroutine 'finished'.", 0h

section .text
_start:
  
subroutineOne:
  mov rdi, msg1
  call sprintLF
  jmp .finished

.finished:
  mov rdi, msg2
  call sprint
  mov rdi, 1
  call iprintLF

subroutineTwo:
  mov rdi, msg1
  call sprintLF
  jmp .finished

.finished:
  mov rdi, msg2
  call sprint
  mov rdi, 2
  call iprintLF

  mov rdi, msg1
  call sprintLF
  jmp finished

finished:
  mov rdi, msg3
  call sprintLF
  call quit
