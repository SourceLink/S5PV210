led_on_c.bin : led_on_c.o start.o
	arm-linux-ld -Ttext 0 -g $^ -o led_on_c.elf
	arm-linux-objcopy -O binary -S led_on_c.elf $@ 
	gcc Source210_image.c -o Source210
	./Source210 $@ 210.bin

start.o : start.s
	arm-linux-gcc -c $^ -o $@

led_on_c.o : led_on_c.c
	arm-linux-gcc -c $^ -o $@


clean:
	rm -f *.o *.bin *.elf Source210
