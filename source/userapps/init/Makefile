all:
	gcc -c init.c -fno-builtin
	nasm -f elf -o crt0.o crt0.S
	gcc -T cybos.ld -o init.bin crt0.o init.o -nostdlib -nostartfiles
	cp init.bin ../../../binary/apps/init.bin
