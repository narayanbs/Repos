; socket accept

%include "functions.asm"

global _start

SECTION .data
; our response string
response db 'HTTP/1.1 200 OK', 0Dh, 0Ah, 'Content-Type: text/html', 0Dh, 0Ah, 'Content-Length: 14', 0Dh, 0Ah, 0Dh, 0Ah, 'Hello World!', 0Dh, 0Ah, 0h

SECTION .bss

buffer resb 255,                ; variable to store request headers

section .text

_start:
  xor rax, rax
  xor rdi, rdi
  xor rsi, rsi
  xor rdx, rdx

_socket:
  mov rax, 41   ; socket system call
  mov rdi, 2    ; PF_INET
  mov rsi, 1    ; SOCK_STREAM
  mov rdx, 0    ; protocol = 0
  syscall

_bind:
 mov rdi, rax   ; move return value of SYS_SOCKETCALL into rdi
 push dword 0x00000000  ; push 0 dec onto the stack IP ADDRESS (0.0.0.0)
 push word 0x2923   ; push 9001 dec onto stack PORT (reverse byte order)
 push word 2    ; push 2 dec onto stack AF_INET

 mov rax, 49
 mov r9, rdi  ; store rdi in r9 to save parent file descriptor
 mov rsi, rsp ; store rsp in rsi
 mov rdx, 16  ; arguments length 
 syscall 

_listen:
  mov rax, 50
  mov rsi, 4 
  syscall

_accept:
  mov rax, 43
  mov rdi, r9   ; r9 which has parent fd put into rdi. prevents fork loop error
  xor rsi, rsi
  mov rdx, rsi
  syscall

_fork:
 
    mov     rax, 57             ; invoke SYS_FORK (kernel opcode 57)
    mov     rdi, rax            ; move return value of SYS_SOCKET into rdi (file descriptor for accepted socket, or -1 on error)
    syscall                     ; call the kernel
 
    cmp     rax, 0              ; if return value of SYS_FORK in rax is zero we are in the child process
    jz      _read               ; jmp in child process to _read
 
    jmp     _accept          ; jmp in parent process to _accept
 
_read:
    ;mov     rdi, rax
    mov     rdx, 255            ; number of bytes to read (we will only read the first 255 bytes for simplicity)
    mov     rsi, buffer         ; move the memory address of our buffer variable into rsi
    mov     rax, 0              ; invoke SYS_READ (kernel opcode 0)
    syscall                     ; call the kernel
 
    mov     rax, buffer         ; move the memory address of our buffer variable into rax for printing
    call    sprintLF            ; call our string printing function

_write:
 
    mov     rdx, 78             ; move 78 dec into edx (length in bytes to write)
    mov     rsi, response       ; move address of our response variable into ecx
    ;mov     rdi, esi            ; move file descriptor into ebx (accepted socket id)
    mov     rax, 1              ; invoke SYS_WRITE (kernel opcode 1)
    syscall                 ; call the kernel

_close:
 
    ;mov     rdi, esi            ; move esi into ebx (accepted socket file descriptor)
    mov     rax, 3              ; invoke SYS_CLOSE (kernel opcode 3)
    syscall                 ; call the kernel


_exit:
  call quit





