[org 0x7c00]

BOOT_DRIVE db 0x0
ERROR_MSG: db 'disk error', 0
mov [BOOT_DRIVE], dl
mov bp, 0x9000
mov sp, bp
call loadk
call mmap
call gfxi
call gfxe
call pms
jmp $

printstr:
  pusha
  mov ah, 0xe
.loop:
  mov al, [bx]
  cmp al, 0
  je .done
  int 0x10
  add bx, 1
  jmp .loop
.done:
  popa
  ret

gfxi:
    pusha
    push es
    xor ax, ax
    mov es, ax
    mov di, 0x7E00
    mov ax, 0x4F01
    mov cx, 0x11B
    int 0x10
    pop es
    popa
    ret

gfxe:
    pusha
    push es
    xor ax, ax
    mov es, ax
    mov di, 0x7E00
    mov ax, 0x4F02
    mov bx, 0x11B | 0x4000
    int 0x10
    pop es
    popa
    ret

MMAPADDR equ 0x8000
MMAPCOUNT equ 0x7DFE

mmap:
    push ax
    push bx
    push cx
    push dx
    push si
    push di
    push es
    mov ax,  0x0000
    mov es, ax
    mov di, MMAPADDR
    mov word [MMAPCOUNT], 0
    db 0x66
    xor ebx, ebx
    xor si, si

.le820:
    db 0x66
    mov eax, 0xE820
    db 0x66
    mov edx, 0x534D4150
    db 0x66
    mov ecx, 24
    int 0x15
    jc .int12
    db 0x66
    cmp eax, 0x534D4150
    jne .int12
    add di, cx
    inc si
    db 0x66
    test ebx, ebx
    jnz .le820
    mov [MMAPCOUNT], si
    pop es
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret

.int12:
    pop es
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    xor ax, ax
    int 0x12
    mov bx, ax
    mov ax, bx
    mov cx, 1024
    mul cx
    mov si, MMAPADDR
    mov word [si+0], 0
    mov word [si+2], 0
    mov word [si+4], 0
    mov word [si+6], 0
    mov word [si+8], ax
    mov word [si+10], dx
    mov word [si+12], 0
    mov word [si+14], 0
    mov word [si+16], 1
    mov word [si+18], 0
    mov word [si+20], 0
    mov word [si+22], 0
    mov word [MMAPCOUNT], 1
    ret

disk_load:
  pusha
  push dx
  mov ah, 0x02
  mov al, dh
  mov ch, 0
  mov dh, 0
  mov cl, 2
  int 0x13
  pop dx
  jc disk_error
  cmp al, dh
  jne disk_error
  popa
  ret
disk_error:
  mov bx, ERROR_MSG
  call printstr
  jmp $

gdt_start:
gdt_null:
  dd 0x0
  dd 0x0
gdt_code:
  dw 0xffff
  dw 0x0
  db 0x0
  db 10011010b
  db 11001111b
  db 0x0
gdt_data:
  dw 0xffff
  dw 0x0
  db 0x0
  db 10010010b
  db 11001111b
  db 0x0
gdt_end:

gdt_descriptor:
  dw gdt_end - gdt_start - 1
  dd gdt_start

CODE_SEG EQU gdt_code - gdt_start
DATA_SEG EQU gdt_data - gdt_start




a20l:
  in al, 0x92
  or al, 00000010b
  out 0x92, al
  ret
[bits 16]
loadk:
  mov ax, 0x1000
  mov es, ax
  mov bx, 0x0000
  mov dh, 47
  mov dl, [BOOT_DRIVE]
  call disk_load
  ret

[bits 16]
pms:
  call a20l
  cli
  lgdt [gdt_descriptor]
  mov eax, cr0
  or eax, 0x1
  mov cr0, eax
  jmp CODE_SEG:init_pm

[bits 32]
init_pm:
  mov ax, DATA_SEG
  mov ds, ax
  mov ss, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ebp, 0x90000
  mov esp, ebp
  call BEGIN_PM

[bits 32]
BEGIN_PM:
  call 0x10000
  jmp $

times 510-($-$$) db 0x0
dw 0xaa55
