key_led.bin : start.o key_led.o
	arm-linux-ld -Ttext 0x0 $^ -o key_led.elf
	arm-linux-objcopy -O binary key_led.elf key_led.bin
	arm-linux-objdump -D key_led.elf > key_led.dis
	gcc Source210_image.c -o Source210
	./Source210 key_led.bin 210.bin


start.o : start.s
	arm-linux-gcc -c $< -o $@

key_led.o : key_led.c
	arm-linux-gcc -c $< -o $@



clean:
	rm *.o *.dis *.bin *.elf Source210 -f
