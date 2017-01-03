#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>



int main(int argc, char **argv)
{
	int fd;
	int val = 1;

	if (argc != 3) {
		printf("Usage :\n");
		printf("%s /dev/led? <on | off>\n",argv[0]);
		return 0;
	}

	fd = open(argv[1], O_RDWR);
		
	if (fd < 0)
		printf("can't open!\n");


	if(!strcmp(argv[2], "on"))
		val = 1;
	else if (!strcmp(argv[2], "off"))
		val = 0;
	
	write(fd, &val, 1);

	return 0;
}






