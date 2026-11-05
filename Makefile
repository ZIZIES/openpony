all: disk.img

build/stage1.bin: canterlot/stage1.asm
	mkdir -p build
	nasm -f bin $< -o $@

build/stage2.bin: canterlot/stage2.asm
	mkdir -p build
	nasm -f bin $< -o $@

disk.img: build/stage1.bin build/stage2.bin
	cat build/stage1.bin build/stage2.bin > $@

run: disk.img
	qemu-system-x86_64 -drive format=raw,file=disk.img -d int,cpu_reset -no-reboot

clean:
	rm -rf build disk.img

.PHONY: all run clean