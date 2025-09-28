#!/bin/bash

CFLAGS="-B/usr/lib/gcc/i686-elf/15.1.0 -m32 -ffreestanding -nostdlib -fno-pie -fno-stack-protector -O2 -Wall -Wextra -Wno-unused-parameter"
LDFLAGS="-m elf_i386 -Ttext 0x10000 -e _start"
nasm boot/boot.s -f bin -o boot.bin
nasm kernel/entry.s -f elf -o entry.o
i686-elf-gcc $CFLAGS -c kernel/init.c -o kernel.o

i686-elf-ld $LDFLAGS -o kernel.elf \
    entry.o \
    kernel.o \

i686-elf-objcopy -O binary kernel.elf kernel.bin
cat boot.bin kernel.bin > floppy.img

