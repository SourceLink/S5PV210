led_on.bin:led_on.o
	arm-linux-ld -Ttext 0x0 $^ -o led_on.elf 
	arm-linux-objcopy -O binary led_on.elf led_on.bin
	arm-linux-objdump -D led_on.elf > led_on.dis
	gcc Source210_image.c -o Source210
	./Source210 $@ 210.bin

led_on.o : led_on.s
	 arm-linux-gcc -c $< -o $@


clean:
	rm -f *.o *.elf *.bin *.dis Source210
