sd_reload.bin : start.o	sd_reload.o
	arm-linux-ld -Tlink.lds -g $^ -o sd_reload.elf
	arm-linux-objcopy -O binary -S sd_reload.elf $@ 
	arm-linux-objdump -D sd_reload.elf > sd_reload.dis
	gcc Source210_image.c -o Source210
	./Source210 $@ 210.bin

%.o : %.s
	arm-linux-gcc -c $^ -o $@

%.o : %.c
	arm-linux-gcc -c $^ -o $@


clean:
	rm -f *.o *.bin *.elf *.dis Source210
