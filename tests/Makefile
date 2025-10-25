CC := i686-elf-gcc
CFLAGS := -B/usr/lib/gcc/i686-elf/15.1.0 -m32 -nostdlib -ffreestanding
LDFLAGS := -Wl,-Ttext,0x200000,--oformat=binary
PACK := ../scripts/pack_uex.py
GENKFS := ../scripts/genkfs.py
UEX_ADDR := 00200000

SRCS := $(shell find . -type f -name '*.c')

.PHONY: all genkfs clean
all: $(SRCS)
	@set -e
	@for src in $(SRCS); do \
	  name=$$(echo $$src | sed 's@^\./@@; s@/@-@g; s/\.c$$/.uex/'); \
	  out=../rootfs/$$name; \
	  echo "Building $$src -> $$out"; \
	  mkdir -p $$(dirname $$out); \
	  $(CC) $(CFLAGS) $$src $(LDFLAGS) -o $$out.bin; \
	  $(PACK) $$out.bin $(UEX_ADDR) $$out; \
	  rm -f $$out.bin; \
	done
	@$(MAKE) genkfs

genkfs:
	$(GENKFS)
	mv kfs.img ../rootfs/kfs.img

clean:
	@echo "Cleaning ../rootfs .uex files"
	@find ../rootfs -type f -name '*.uex' -delete
	@rm -f kfs.img
