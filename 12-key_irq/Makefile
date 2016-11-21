irq.bin : start.o ddr_init.o clock_init.o uart.o irq.o main.o
	arm-linux-ld -Tlink.lds -g $^ -o irq.elf
	arm-linux-objcopy -O binary -S irq.elf $@ 
	arm-linux-objdump -D irq.elf > irq.dis
	gcc Source210_image.c -o Source210
	./Source210 $@ 210.bin

%.o : %.S
	arm-linux-gcc -c $^ -o $@

%.o : %.c
	arm-linux-gcc -c $^ -o $@


clean:
	rm -f *.o *.bin *.elf *.dis Source210
