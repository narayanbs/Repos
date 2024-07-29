;  program to invoke a C function from assembly
; nasm -f elf64 ex03.asm && gcc -no-pie ex03.o -o ex03
;
global main
extern puts

section .data
message: db  "Hola, mundo",0

section .text
main:
  mov rdi, message
  ; stack should be aligned to 16-bytes. Since return address of main is already on stack
  ; we need to add another 8-bytes before making the function call to puts. 
  sub rsp, 8
  call puts
  add rsp, 8
  ret


; The same program but with _start 
; nasm -f elf64 ex03.asm && gcc -nostartfiles -no-pie ex03.o -o ex03

global _start
extern puts

section .data
message: db "Hola, mundo", 0

section .text
_start:
  mov rdi, message
  call puts

  mov rax, 60
  xor rdi, rdi
  syscall
