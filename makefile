all: kernel.o asm.o kernel.elf initdir/kernel/kernel.elf ledisk.img start_vbox clean

target:
	x86_64-elf-gcc -Wall -fpic -ffreestanding -fno-stack-protector -nostdinc -nostdlib -Iinclude -mno-red-zone -g -c kernel.c -o kernel.o
	nasm -felf64 -g -oasm.o asm/asmprocedures.asm
	x86_64-elf-ld -nostdlib -g -n -T link.ld asm.o kernel.o -o kernel.elf
	cp kernel.elf initdir/kernel
	./mkbootimg jason.json ledisk.img
	qemu-img convert -f raw -O vdi ledisk.img ledisk.vdi
	VBoxManage modifyvm letimos --guest-debug-provider=gdb
	VBoxSDL --hda ledisk.vdi --boot c --noresize --startvm letimos
	rm ledisk.vdi
	rm ledisk.img
	rm kernel.elf
	rm kernel.o
	rm asm.o
	rm initdir/kernel/kernel.elf

debug:
	gdb --command=gdbcommands

first_time:
	VBoxManage import letimos.ovf
	mkdir initdir
	mkdir initdir/kernel
	mkdir initdir/fonts
	cp font.psf initdir/fonts

updatevm:
	rm letimos.ovf
	VBoxManage export letimos --output=letimos.ovf --ovf20

kernel.o: kernel.c
	x86_64-elf-gcc -Wall -fpic -ffreestanding -fno-stack-protector -nostdinc -nostdlib -Iinclude -mno-red-zone -c kernel.c -o kernel.o

asm.o: asm/asmprocedures.asm
	nasm -felf64 -oasm.o asm/asmprocedures.asm

kernel.elf: kernel.o asm.o 
	x86_64-elf-ld -nostdlib -n -T link.ld asm.o kernel.o -o kernel.elf
	x86_64-elf-strip -s -K mmio -K fb -K bootboot -K environment -K initstack kernel.elf

initdir/kernel/kernel.elf: kernel.elf
	cp kernel.elf initdir/kernel

ledisk.img: initdir/kernel/kernel.elf
	./mkbootimg jason.json ledisk.img

#gave up qemu, was a pain in the ass to debug. Using VirtualBox now.
start_qemu: ledisk.img
	qemu-system-x86_64 -m 1024 -cpu core2duo -bios OVMF.fd -drive file=ledisk.img,if=ide,format=raw

start_vbox: ledisk.img
	qemu-img convert -f raw -O vdi ledisk.img ledisk.vdi
	VBoxSDL --hda ledisk.vdi --boot c --noresize --startvm letimos
	rm ledisk.vdi

clean: initdir/kernel/kernel.elf kernel.elf kernel.o asm.o ledisk.img
	rm ledisk.img
	rm kernel.elf
	rm kernel.o
	rm asm.o
	rm initdir/kernel/kernel.elf
