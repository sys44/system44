#!/bin/sh

CFLAGS="-B/usr/lib/gcc/i686-elf/15.1.0 -m32 -ffreestanding -nostdlib -fno-pie -fno-stack-protector -O2 -Wall -Wextra -Wno-unused-parameter"
LDFLAGS="-m elf_i386 -Ttext 0x10000 -e _start"

echo "assemble: boot/boot.s"
nasm boot/boot.s -f bin -o boot.bin

echo "assemble: kernel/entry.s"
nasm kernel/entry.s -f elf -o entry.o

echo "compile: kernel/init.c"
i686-elf-gcc $CFLAGS -c kernel/init.c -o init.o

echo "compile: kernel/shell.c"
i686-elf-gcc $CFLAGS -c kernel/shell.c -o shell.o

echo "compile: drivers/vga.c"
i686-elf-gcc $CFLAGS -c drivers/vga.c -o vga.o

echo "compile: drivers/keyboard.c"
i686-elf-gcc $CFLAGS -c drivers/keyboard.c -o keyboard.o

echo "compile: drivers/tty.c"
i686-elf-gcc $CFLAGS -c drivers/tty.c -o tty.o

echo "compile: drivers/ata.c"
i686-elf-gcc $CFLAGS -c drivers/ata.c -o ata.o

echo "compile: lib/memory.c"
i686-elf-gcc $CFLAGS -c lib/memory.c -o memory.o

echo "compile: lib/string.c"
i686-elf-gcc $CFLAGS -c lib/string.c -o string.o

echo "linking files.."
i686-elf-ld $LDFLAGS -o kernel.elf \
    entry.o \
    init.o \
    shell.o \
    vga.o \
    tty.o \
    keyboard.o \
    memory.o \
    string.o

echo "objcopy: kernel.elf"
i686-elf-objcopy -O binary kernel.elf kernel.bin

echo "creating final image.."
cat boot.bin kernel.bin > floppy.img

echo "build complete. cleaning build files.."
rm boot.bin kernel.bin entry.o init.o shell.o vga.o keyboard.o tty.o memory.o string.o kernel.elf
