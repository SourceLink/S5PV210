led_on_c.bin : start.o	led_on_c.o
	arm-linux-ld -Tlink.lds -g $^ -o led_on_c.elf
	arm-linux-objcopy -O binary -S led_on_c.elf $@ 
	arm-linux-objdump -D led_on_c.elf > led_on_c.dis

start.o : start.s
	arm-linux-gcc -c $^ -o $@

led_on_c.o : led_on_c.c
	arm-linux-gcc -c $^ -o $@


clean:
	rm -f *.o *.bin *.elf *.dis
