; nasm -f elf -g -F stabs helloasm.asm 
; ld -o helloasm helloasm.o

SECTION .data
MyMsg: db "hello asm in Linux", 10
MyMsgLen: equ $-MyMsg

SECTION .bss
SECTION .text
global _start	; entry point for linker

_start:
  nop
  mov eax, 4
  mov ebx, 1
  mov ecx, MyMsg
  mov edx, MyMsgLen
  int 80H
  mov eax, 1
  xor ebx, ebx
  int 80H
  
;  .end: