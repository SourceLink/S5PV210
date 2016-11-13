SECTIONS
{
	. = 0xD0022800;
	
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
