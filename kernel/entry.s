[bits 32]
[extern kmain]

global _start

_start:
    mov esp, 0x90000
    call kmain

.hang:
    hlt
    jmp .hang