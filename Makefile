CC     = gcc
CFLAGS = -ffreestanding -fno-stack-protector -fno-pic -fno-pie \
         -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -Wall -Wextra

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

build/kernel.o: kernel/kernel.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/kernel.elf: build/entry.o build/kernel.o kernel/linker.ld
	ld -T kernel/linker.ld -o $@ build/entry.o build/kernel.o

build/kernel.bin: build/kernel.elf
	objcopy -O binary $< $@
	truncate -s 32768 $@

disk.img: build/stage1.bin build/stage2.bin build/kernel.bin
	cat build/stage1.bin build/stage2.bin build/kernel.bin > $@

run: disk.img
	qemu-system-x86_64 -drive format=raw,file=disk.img -d int,cpu_reset -no-reboot

clean:
	rm -rf build disk.img

.PHONY: all run clean