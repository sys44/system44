[bits 32]
[extern kmain]

global _start

_start:
    mov esp, 0x90000
    ; abandon all hope, ye who enter here.
    call kmain

.hang:
    hlt
    jmp .hang