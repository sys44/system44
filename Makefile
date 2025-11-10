CC       = i686-elf-gcc
AS       = nasm
LD       = ld
OBJCOPY  = objcopy

GCCVER   = 15.1.0
CFLAGS   = -B/usr/lib/gcc/i686-elf/$(GCCVER) -m32 -s -ffreestanding -nostdlib \
           -fno-pie -fno-builtin -fno-stack-protector -O3 -Os -fomit-frame-pointer -fno-asynchronous-unwind-tables -fno-unwind-tables -fmerge-all-constants
ASFLAGS  = -f elf
LDFLAGS  = -m elf_i386 -Ttext 0x10000 -e _start --strip-all

SRC_DIRS = kernel/core kernel/drivers kernel/lib kernel/mm kernel/fs kernel/exe kernel/int kernel/sched
BOOT_DIR = boot
IMAGE    = floppy.img

EXCLUDE_SRCS ?= kernel/drivers/tty_vtm.c kernel/drivers/vga.c kernel/mm/paging.c

C_SOURCES := $(filter-out $(EXCLUDE_SRCS),$(wildcard $(addsuffix /*.c,$(SRC_DIRS))))
S_SOURCES := $(wildcard $(addsuffix /*.s,$(SRC_DIRS)))
OBJS := $(patsubst %.c,%.o,$(C_SOURCES)) $(patsubst %.s,%.o,$(S_SOURCES))
OBJS := $(sort $(OBJS))

KERNEL_OBJS := $(filter kernel/%.o,$(OBJS))
OTHER_OBJS := $(filter-out $(KERNEL_OBJS),$(OBJS))
LINK_OBJS := $(KERNEL_OBJS) $(OTHER_OBJS)

all: $(IMAGE)
	@echo "build complete"

%.o: %.c
	@echo "CC: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	@echo "CC: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	@echo "CC: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@echo "AS: $<"
	@nasm -f elf32 $< -o $@

boot.bin: $(BOOT_DIR)/boot.s
	@echo "AS: $<"
	@$(AS) -f bin $< -o $@

kernel.elf: $(OBJS)
	@echo "LD: kernel.elf"
	@$(LD) $(LDFLAGS) -o $@ $(LINK_OBJS)

kernel.bin: kernel.elf
	@echo "STRIP: kernel.elf"
	@strip kernel.elf
	@echo "OBJCOPY: kernel.bin"
	@$(OBJCOPY) -O binary $< $@

$(IMAGE): boot.bin kernel.bin
	@echo "FINISH: floppy.img"
	@cat $^ > $@
	@truncate -s 1440k $@

clean:
	@echo "cleaning.."
	@rm -f $(OBJS) boot.bin kernel.bin kernel.elf $(IMAGE)
	@$(MAKE) --no-print-directory -C tests clean

# If the shutdown command doesn't work, change this to "qemu-system-i386 -fda $< -hda rootfs/kfs.img -device isa-debug-exit,iobase=0x501,iosize=0x1"
# This same message is also in core/shell.c
run: $(IMAGE)
	qemu-system-i386 -fda $< -hda rootfs/kfs.img -device es1370 > /dev/null 2>&1 

tests:
	$(MAKE) --no-print-directory -C tests

.PHONY: all clean run tests
