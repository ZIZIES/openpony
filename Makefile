CC     = gcc
CFLAGS = -ffreestanding -fno-stack-protector -fno-pic -fno-pie \
         -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -Wall -Wextra

KSRC := $(wildcard kernel/*.c)
KOBJ := $(patsubst kernel/%.c,build/%.o,$(KSRC))

all: disk.img

build/stage1.bin: canterlot/stage1.asm
	mkdir -p build
	nasm -f bin $< -o $@

build/stage2.bin: canterlot/stage2.asm
	mkdir -p build
	nasm -f bin $< -o $@

build/entry.o: kernel/entry.asm
	mkdir -p build
	nasm -f elf64 $< -o $@

build/%.o: kernel/%.c $(wildcard include/*.h)
	mkdir -p build
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

build/kernel.elf: build/entry.o $(KOBJ) kernel/linker.ld
	ld -T kernel/linker.ld -o $@ build/entry.o $(KOBJ)

build/kernel.bin: build/kernel.elf
	objcopy -O binary $< $@
	truncate -s 32768 $@

disk.img: build/stage1.bin build/stage2.bin build/kernel.bin
	cat build/stage1.bin build/stage2.bin build/kernel.bin > $@

run-nographic: disk.img
	qemu-system-x86_64 -drive format=raw,file=disk.img -d cpu_reset,guest_errors -no-reboot -nographic
run-graphic: disk.img
	qemu-system-x86_64 -drive format=raw,file=disk.img -d cpu_reset,guest_errors -no-reboot
run:
	@echo 'run-nographic or run-graphic'
clean:
	rm -rf build disk.img

.PHONY: all run-graphic run-nographic clean