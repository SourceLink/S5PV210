SECTIONS
{
	. = 0xD0020010;
	
	.text : {
		start.o
		* (.text)
	}

	.data : {
		* (.data)
	}

	bss_start = .;
	.bss : {
		* (.bss)
	}
	bss_end = .;
}
