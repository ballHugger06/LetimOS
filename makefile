all: kernel.elf initdir/kernel/kernel.elf ledisk.img start_qemu clean

first_time:
	mkdir initdir
	mkdir initdir/kernel
	mkdir initdir/fonts
	cp font.psf initdir/fonts

kernel.elf: kernel.c
	x86_64-elf-gcc -Wall -fpic -ffreestanding -fno-stack-protector -nostdinc -nostdlib -Iinclude -mno-red-zone -c kernel.c -o kernel.o
	x86_64-elf-ld -nostdlib -n -T link.ld kernel.o -o kernel.elf
	x86_64-elf-strip -s -K mmio -K fb -K bootboot -K environment -K initstack kernel.elf

initdir/kernel/kernel.elf: kernel.elf
	cp kernel.elf initdir/kernel

ledisk.img: initdir/kernel/kernel.elf
	./mkbootimg jason.json ledisk.img

start_qemu: ledisk.img
	qemu-system-x86_64 -m 256 -cpu core2duo -bios OVMF.fd -drive file=ledisk.img,if=ide,format=raw

clean: initdir/kernel/kernel.elf kernel.elf ledisk.img
	rm ledisk.img
	rm kernel.elf
	rm kernel.o
	rm initdir/kernel/kernel.elf

start_qemu_and_clean: start_qemu clean	