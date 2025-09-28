[org 0x7c00]

BOOT_DRIVE db 0x0
ERROR_MSG: db 'disk error', 0
mov [BOOT_DRIVE], dl
mov bp, 0x9000
mov sp, bp
call loadk
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