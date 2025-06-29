global _start

section .data
  byte_data: db 0x12
  word_data: dw 0x1234
  dword_data: dd 0x12345678
  qword_data: dq 0x123456789ABCDEF0

section .bss
  result_byte: resb 1
  result_word: resw 1
  result_dword: resd 1
  result_qword: resq 1

section .text
_start:
  ;mov al,byte[byte_data]
  ;mov byte[result_byte], al
  ; or 
  mov al, [byte_data]
  mov [result_byte], al

  ;mov ax, word[word_data]
  ;mov word[result_word], ax
  ;  or 
  mov ax, [word_data]
  mov [result_word], ax

  mov eax,dword[dword_data]
  mov dword[result_dword], eax

  mov rax,qword[qword_data]
  mov qword[result_qword], rax

  mov rax, 60
  xor rdi, rdi
  syscall
