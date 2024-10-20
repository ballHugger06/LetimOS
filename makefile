all: Kernel.elf ledisk.img start_qemu

kernel.elf: kernel.c
	/home/eren/opt/cross/bin/x86_64-elf-gcc -Wall -fpic -ffreestanding -fno-stack-protector -nostdinc -nostdlib -Iinclude -mno-red-zone -c kernel.c -o kernel.o
	/home/eren/opt/cross/bin/x86_64-elf-ld -nostdlib -n -T link.ld kernel.o -o kernel.elf
	/home/eren/opt/cross/bin/x86_64-elf-strip -s -K mmio -K fb -K bootboot -K environment -K initstack kernel.elf

ledisk.img: kernel.elf
	cp kernel.elf initdir/kernel
	./mkbootimg jason.json ledisk.img

start_qemu: ledisk.img
	qemu-system-x86_64 -cpu core2duo -bios OVMF.fd -drive file=ledisk.img,if=ide,format=raw
	