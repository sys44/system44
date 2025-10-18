# makefile
CC = i686-elf-gcc
GCCVER=15.1.0
AS = nasm
LD = ld
OBJCOPY = objcopy
CFLAGS = -B/usr/lib/gcc/i686-elf/15.1.0 -m32 -ffreestanding -nostdlib -fno-pie -fno-builtin -fno-stack-protector -O2 -Wall -Wextra -Wno-unused-parameter
ASFLAGS = -f elf
LDFLAGS = -m elf_i386 -Ttext 0x10000 -e _start
KERNEL_DIR = kernel
DRIVERS_DIR = drivers
LIB_DIR = lib
MM_DIR = mm
FS_DIR = fs
UEX_DIR = uex
INT_DIR = int
BOOT_DIR = boot

OBJS = entry.o \
       init.o \
       shell.o \
       panic.o \
       vga.o \
       tty.o \
       keyboard.o \
       ata.o \
       fb.o \
       fbcon.o \
       memory.o \
       string.o \
       lmm.o \
       pmm.o \
       paging.o \
       kfs.o \
       uex.o \
       int.o \
       log.o \

IMAGE = floppy.img

all: $(IMAGE)
	@echo "build complete"

boot.bin: $(BOOT_DIR)/boot.s
	@echo "assemble: $<"
	@$(AS) -f bin $< -o $@

entry.o: $(KERNEL_DIR)/entry.s
	@echo "assemble: $<"
	@$(AS) $(ASFLAGS) $< -o $@

init.o: $(KERNEL_DIR)/init.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

shell.o: $(KERNEL_DIR)/shell.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

panic.o: $(KERNEL_DIR)/panic.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

log.o: $(KERNEL_DIR)/log.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

vga.o: $(DRIVERS_DIR)/vga.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

keyboard.o: $(DRIVERS_DIR)/keyboard.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

tty.o: $(DRIVERS_DIR)/tty.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

ata.o: $(DRIVERS_DIR)/ata.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

fb.o: $(DRIVERS_DIR)/fb.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

fbcon.o: $(DRIVERS_DIR)/fbcon.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

memory.o: $(LIB_DIR)/memory.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

string.o: $(LIB_DIR)/string.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

lmm.o: $(MM_DIR)/lmm.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

pmm.o: $(MM_DIR)/pmm.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

paging.o: $(MM_DIR)/paging.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

kfs.o: $(FS_DIR)/kfs.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

uex.o: $(UEX_DIR)/uex.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

int.o: $(INT_DIR)/interrupts.c
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@


kernel.elf: $(OBJS)
	@echo "linking files.."
	@$(LD) $(LDFLAGS) -o $@ $(OBJS)

kernel.bin: kernel.elf
	@echo "objcopy: $<"
	@$(OBJCOPY) -O binary $< $@

$(IMAGE): boot.bin kernel.bin
	@echo "creating final image.."
	@cat boot.bin kernel.bin > $@

clean:
	@echo "cleaning build files.."
	@rm -f $(OBJS) boot.bin kernel.bin kernel.elf $(IMAGE)

run: $(IMAGE)
	qemu-system-i386 -fda $(IMAGE)
    
.PHONY: all clean run
