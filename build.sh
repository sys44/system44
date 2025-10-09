#!/bin/sh

CC=tcc

CFLAGS="-m32 -ffreestanding -nostdlib -fno-pie -fno-builtin -fno-stack-protector -O2 -Wall -Wextra -Wno-unused-parameter"
LDFLAGS="-m elf_i386 -Ttext 0x10000 -e _start"

echo "assemble: boot/boot.s"
nasm boot/boot.s -f bin -o boot.bin

echo "assemble: kernel/entry.s"
nasm kernel/entry.s -f elf -o entry.o

echo "compile: kernel/init.c"
$CC $CFLAGS -c kernel/init.c -o init.o

echo "compile: kernel/shell.c"
$CC $CFLAGS -c kernel/shell.c -o shell.o

echo "compile: drivers/vga.c"
$CC $CFLAGS -c drivers/vga.c -o vga.o

echo "compile: drivers/keyboard.c"
$CC $CFLAGS -c drivers/keyboard.c -o keyboard.o

echo "compile: drivers/tty.c"
$CC $CFLAGS -c drivers/tty.c -o tty.o

echo "compile: drivers/ata.c"
$CC $CFLAGS -c drivers/ata.c -o ata.o

echo "compile: drivers/fb.c"
$CC $CFLAGS -c drivers/fb.c -o fb.o

echo "compile: lib/memory.c"
$CC $CFLAGS -c lib/memory.c -o memory.o

echo "compile: lib/string.c"
$CC $CFLAGS -c lib/string.c -o string.o

echo "compile: mm/lmm.c"
$CC $CFLAGS -c mm/lmm.c -o lmm.o

echo "compile: mm/pmm.c"
$CC $CFLAGS -c mm/pmm.c -o pmm.o

echo "compile: mm/paging.c"
$CC $CFLAGS -c mm/paging.c -o paging.o

echo "linking files.."
ld $LDFLAGS -o kernel.elf \
    entry.o \
    init.o \
    shell.o \
    vga.o \
    tty.o \
    keyboard.o \
    fb.o \
    memory.o \
    string.o \
    lmm.o \
    pmm.o \
    paging.o

echo "objcopy: kernel.elf"
objcopy -O binary kernel.elf kernel.bin

echo "creating final image.."
cat boot.bin kernel.bin > floppy.img

echo "build complete. cleaning build files.."
rm boot.bin kernel.bin entry.o init.o shell.o vga.o keyboard.o tty.o ata.o fb.o memory.o string.o lmm.o pmm.o paging.o kernel.elf
