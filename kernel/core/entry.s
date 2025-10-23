[bits 32]
[extern kmain]
global _start

_start:
    mov esp, 0x90000
    push 0x7E00
    ; abandon all hope, ye who enter here.
    call kmain
    add esp, 4
.hang:
    hlt
    jmp .hang
