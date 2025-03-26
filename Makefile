all:
	nasm -f bin ./boot.asm -o ./boot.bin
	dd if=./message.txt bs=512 count=1 conv=sync >> ./boot.bin
