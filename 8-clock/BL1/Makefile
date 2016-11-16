sdram.bin : start.o clock_init.o ddr_init.o sd_reload.o
	arm-linux-ld -Tlink.lds -g $^ -o sdram.elf
	arm-linux-objcopy -O binary -S sdram.elf $@ 
	arm-linux-objdump -D sdram.elf > sdram.dis
	gcc Source210_image.c -o Source210
	./Source210 $@ 210.bin

%.o : %.S
	arm-linux-gcc -c $^ -o $@

%.o : %.c
	arm-linux-gcc -c $^ -o $@


clean:
	rm -f *.o *.bin *.elf *.dis Source210
