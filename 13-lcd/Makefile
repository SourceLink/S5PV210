lcd.bin : start.o ddr_init.o clock_init.o uart.o irq.o lcd.o main.o
	arm-linux-ld -Tlink.lds -g $^ -o lcd.elf -lgcc -L/opt/FriendlyARM/toolschain/4.4.3/lib/gcc/arm-none-linux-gnueabi/4.4.3
	arm-linux-objcopy -O binary -S lcd.elf $@ 
	arm-linux-objdump -D lcd.elf > lcd.dis
	gcc Source210_image.c -o Source210
	./Source210 $@ 210.bin

%.o : %.S
	arm-linux-gcc -c $^ -o $@

%.o : %.c
	arm-linux-gcc -c $^ -o $@


clean:
	rm -f *.o *.bin *.elf *.dis Source210
