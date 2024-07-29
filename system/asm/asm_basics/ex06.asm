; program to print the arguments 
; in an assembly program using _start rather than main
; for more information look at 
; how statically program runs on linux by eli 
; https://eli.thegreenplace.net/2012/08/13/how-statically-linked-programs-run-on-linux
.intel_syntax noprefix

.global _start

.global puts
.global exit

.text
_start:
   mov r12, [rsp]
   mov r13, rsp
loop:
   cmp r12, 0
   jz end
   dec r12
   add r13, 8

   mov rdi, [r13]
   call puts@plt

   jmp loop

end:
   xor rdi, rdi
   call exit@plt



; program that prints the arguments passed to it over
; the command line to a c program 
global main
extern puts

section .text
main:
    push rdi
    push rsi
    sub rsp, 8

    mov rdi, [rsi]
    call puts

    add rsp, 8
    pop rsi
    pop rdi
    add rsi, 8
    dec rdi
    jnz main
    ret

