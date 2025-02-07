#!/bin/bash
set -e

# Assemble the 64-bit bootloader
nasm -f bin boot.asm -o boot.bin

# Compile the 64-bit kernel
x86_64-elf-gcc -ffreestanding -m64 -c kernel.c -o kernel.o
x86_64-elf-ld -Ttext 0x100000 --oformat binary kernel.o -o kernel.bin

# Create a bootable disk image
qemu-img create -f raw myos.img 10M
dd if=boot.bin of=myos.img bs=512 count=1 conv=notrunc
dd if=kernel.bin of=myos.img bs=512 seek=1 conv=notrunc

echo "Build complete. Run with: qemu-system-x86_64 -drive file=myos.img,format=raw -m 512M -nographic"
