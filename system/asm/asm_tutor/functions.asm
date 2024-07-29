; int atoi(integer)
; ascii to integer function
atoi:
  push rbx
  push rcx
  push rdx
  push rsi

  mov rsi, rdi
  mov rax, 0
  mov rcx, 0

.multiplyLoop:
  xor rbx, rbx
  mov bl, [rsi+rcx]
  cmp bl, 48
  jl .finished
  cmp bl, 57
  jg .finished

  sub bl, 48
  add rax, rbx
  mov rbx, 10
  mul ebx
  inc rcx
  jmp .multiplyLoop

.finished:
  cmp rcx, 0
  je .restore
  mov rbx, 10
  div rbx

.restore:
  pop rsi
  pop rdx
  pop rcx
  pop rbx
  ret


;-----------------------------------------------------
; void iprint(integer)
; integer printing function
iprint:
  mov rax, rdi
  mov rcx, 0

.divideLoop:
  inc rcx
  mov rdx, 0
  mov r8, 10
  idiv r8
  add rdx, 48
  push rdx
  cmp rax, 0
  jnz .divideLoop

.printLoop:
  dec rcx
  mov rdi, rsp
  push rcx
  call sprint
  pop rcx
  pop rdi
  cmp rcx, 0
  jnz .printLoop

   ret

;-------------------------------------------
; void iprintLF(integer)
; integer printing function with line feed. 
iprintLF:
  call iprint

  mov rax, 0Ah
  push rax
  mov rdi, rsp
  call sprint
  pop rax
  ret
  

;---------------------------------------------
; int slen(string message) 
; string length calculation function
slen:
  push rbx
  mov rbx, rdi
  mov rax, rbx

.nextchar:
  cmp byte[rax], 0
  jz .finished
  inc rax
  jmp .nextchar

.finished:
  sub rax, rbx
  pop rbx
  ret

;-------------------------------------------------
; sprint function 
; string printing function
sprint:
  push rbx
  call slen

  mov rbx, rax
  mov r8, rdi

  mov rax, 1
  mov rdi, 1
  mov rsi, r8
  mov rdx, rbx
  syscall

  pop rbx
  ret

;-------------------------------------------------------
; void sprintLF(string message)
; string printing with line feed
sprintLF:
  call sprint

  ; we cannot pass 0Ah directly as the argument , we need to pass the address
  ; so we push it onto the stack and take its address 
  mov rax, 0Ah
  push rax
  mov rdi, rsp
  call sprint
  pop rax
  ret

;------------------------------------------------------
; void exit()
quit:
  mov rax, 60
  mov rdi, 0
  syscall
  ret

