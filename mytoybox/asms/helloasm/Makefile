
all: helloasm

helloasm: helloasm.o
	ld -o helloasm helloasm.o
	
helloasm.o: helloasm.asm
	nasm -f elf -g -F stabs helloasm.asm
	
clean:
	rm -rf helloasm helloasm.o
