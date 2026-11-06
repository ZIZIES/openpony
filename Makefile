CC     = gcc
CFLAGS = -ffreestanding -fno-stack-protector -fno-pic -fno-pie \
         -mno-red-zone -mno-sse -mno-mmx -mno-80387 \
         -fno-tree-loop-distribute-patterns \
         -O2 -Wall -Wextra -Wno-array-bounds

HDRS := $(wildcard include/*.h)

KSRC := $(wildcard kernel/*.c)
KOBJ := $(patsubst kernel/%.c,build/%.o,$(KSRC))
LSRC := $(wildcard lib/*.c)
LOBJ := $(patsubst lib/%.c,build/lib_%.o,$(LSRC))

# build/builtins.mk (made by tools/builtins.sh) sets BFILES and BDIRS.
# a bad program name or a duplicate stops the build right here.
ifneq ($(MAKECMDGOALS),clean)
include build/builtins.mk
endif
BOBJ := $(patsubst %,build/bf/%.o,$(BFILES)) $(patsubst %,build/bd/%.o,$(BDIRS))

.DELETE_ON_ERROR:

all: disk.img

build/builtins.mk: tools/builtins.sh $(wildcard builtins) $(wildcard builtins/*/)
	mkdir -p build
	tools/builtins.sh vars > $@

build/stage1.bin: canterlot/stage1.asm
	mkdir -p build
	nasm -f bin $< -o $@

build/stage2.bin: canterlot/stage2.asm
	mkdir -p build
	nasm -f bin $< -o $@

build/entry.o: kernel/entry.asm
	mkdir -p build
	nasm -f elf64 $< -o $@

# kernel/*.c
build/%.o: kernel/%.c $(HDRS)
	mkdir -p build
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

# lib/*.c : our libc
build/lib_%.o: lib/%.c $(HDRS)
	mkdir -p build
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

# the shell's command table, generated from builtins/
build/builtins.c: build/builtins.mk tools/builtins.sh $(wildcard builtins/*.c) $(wildcard builtins/*/main.c)
	tools/builtins.sh table > $@

build/builtins.o: build/builtins.c $(HDRS)
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

# builtins/NAME.c -> build/bf/NAME.o
# main() is renamed to NAME_main, and every other symbol is made local so
# two programs can both have a function called helper() without clashing
build/bf/%.o: builtins/%.c $(HDRS)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@.raw
	objcopy --redefine-sym main=$*_main $@.raw $@.ren
	objcopy -G $*_main $@.ren $@
	rm -f $@.raw $@.ren

# builtins/NAME/*.c -> build/bd/NAME.o (all files linked into one object first)
.SECONDEXPANSION:
build/bd/%.o: $$(wildcard builtins/$$*/*.c) $$(wildcard builtins/$$*/*.h) $(HDRS)
	mkdir -p build/bd/$*
	set -e; for f in builtins/$*/*.c; do \
	  $(CC) $(CFLAGS) -Iinclude -Ibuiltins/$* -c $$f -o build/bd/$*/$$(basename $$f .c).o; \
	done
	ld -r -o $@.raw build/bd/$*/*.o
	objcopy --redefine-sym main=$*_main $@.raw $@.ren
	objcopy -G $*_main $@.ren $@
	rm -f $@.raw $@.ren

build/kernel.elf: build/entry.o $(KOBJ) $(LOBJ) $(BOBJ) build/builtins.o kernel/linker.ld
	ld -T kernel/linker.ld -o $@ build/entry.o $(KOBJ) $(LOBJ) $(BOBJ) build/builtins.o

build/kernel.bin: build/kernel.elf
	objcopy -O binary $< $@
	@test $$(stat -c %s $@) -le 32768 || { echo "kernel is bigger than 32 KiB (64 sectors): raise the sector count in canterlot/stage2.asm (kdap) and here"; rm -f $@; exit 1; }
	truncate -s 32768 $@

disk.img: build/stage1.bin build/stage2.bin build/kernel.bin
	cat build/stage1.bin build/stage2.bin build/kernel.bin > $@

run: disk.img
	qemu-system-x86_64 -drive format=raw,file=disk.img -display gtk,zoom-to-fit=on -serial stdio -d cpu_reset,guest_errors -no-reboot

clean:
	rm -rf build disk.img

.PHONY: all run clean