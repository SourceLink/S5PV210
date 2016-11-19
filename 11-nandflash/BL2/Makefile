main.bin : start.o uart.o nand.o main.o
	arm-linux-ld -Tlink.lds -g $^ -o main.elf
	arm-linux-objcopy -O binary -S main.elf $@ 
	arm-linux-objdump -D main.elf > main.dis

%.o : %.s
	arm-linux-gcc -c $^ -o $@

%.o : %.c
	arm-linux-gcc -c $^ -o $@


clean:
	rm -f *.o *.bin *.elf *.dis
