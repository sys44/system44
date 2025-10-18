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

# Auto-discover sources
C_SOURCES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
S_SOURCES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.s))
OBJS      = $(C_SOURCES:.c=.o) $(S_SOURCES:.s=.o)

all: $(IMAGE)
    @echo "build complete"

# Compile rules
%.o: %.c
    @echo "compile: $<"
    @$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
    @echo "assemble: $<"
    @$(AS) $(ASFLAGS) $< -o $@

# Boot sector
boot.bin: $(BOOT_DIR)/boot.s
    @echo "assemble boot: $<"
    @$(AS) -f bin $< -o $@

# Kernel
kernel.elf: $(OBJS)
    @echo "linking kernel.."
    @$(LD) $(LDFLAGS) -o $@ $^

kernel.bin: kernel.elf
    @echo "objcopy kernel.."
    @$(OBJCOPY) -O binary $< $@

# Final floppy image (1.44MB padded)
$(IMAGE): boot.bin kernel.bin
    @echo "creating padded floppy image.."
    @cat $^ > $@
    @truncate -s 1440k $@

clean:
    @echo "cleaning.."
    @rm -f $(OBJS) boot.bin kernel.bin kernel.elf $(IMAGE)

run: $(IMAGE)
    qemu-system-i386 -fda $<

.PHONY: all clean run

