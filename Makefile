CC       = i686-elf-gcc
AS       = nasm
LD       = ld
OBJCOPY  = objcopy

GCCVER   = 15.1.0
CFLAGS   = -B/usr/lib/gcc/i686-elf/$(GCCVER) -m32 -ffreestanding -nostdlib \
           -fno-pie -fno-builtin -fno-stack-protector -O2 -Wall -Wextra -Wno-unused-parameter
ASFLAGS  = -f elf
LDFLAGS  = -m elf_i386 -Ttext 0x10000 -e _start

SRC_DIRS = kernel drivers lib mm fs uex int
BOOT_DIR = boot
IMAGE    = floppy.img

EXCLUDE_SRCS ?= drivers/tty_vtm.c

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
	@echo "compile: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@echo "assemble: $<"
	@$(AS) $(ASFLAGS) $< -o $@

boot.bin: $(BOOT_DIR)/boot.s
	@echo "assemble boot: $<"
	@$(AS) -f bin $< -o $@

kernel.elf: $(OBJS)
	@echo "linking kernel.."
	@$(LD) $(LDFLAGS) -o $@ $(LINK_OBJS)

kernel.bin: kernel.elf
	@echo "objcopy kernel.."
	@$(OBJCOPY) -O binary $< $@

$(IMAGE): boot.bin kernel.bin
	@echo "creating padded floppy image.."
	@cat $^ > $@
	@truncate -s 1440k $@

clean:
	@echo "cleaning.."
	@rm -f $(OBJS) boot.bin kernel.bin kernel.elf $(IMAGE)
	@$(MAKE) --no-print-directory -C tests clean

run: $(IMAGE)
	qemu-system-i386 -fda $< -hda kfs.img > /dev/null 2>&1

tests:
	$(MAKE) --no-print-directory -C tests

.PHONY: all clean run tests
