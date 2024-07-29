for asm files with no c functions and with global _start
nasm -f elf64 ex06.asm && ld ex06.o -o ex06

for asm files with c functions and with global main
nasm -f elf64 ex06.asm && gcc -no-pie ex06.o -o ex06

for asm files with c functions and  _start instead of main
nasm -f elf64 ex06.asm && gcc -nostartfiles -no-pie ex06.o -o ex06


Note: The reason for -no-pie while using gcc is because, 
Debian switched to PIC/PIE binaries in 64-bits mode and GCC by default will try to link the object as PIC.
(Position independent code/ Position independent executable).
But We use absolute addresses in our examples like mov rdi, message.. So gcc will complain and raise an error.
To prevent this we switchoff PIE by -no-pie flag. 

