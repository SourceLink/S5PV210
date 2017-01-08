#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>



int main(int argc, char **argv)
{
	int fd;
	unsigned char key_val;
	unsigned int  ret = 0 ;
	struct pollfd fds[1];
	
	fd = open("/dev/buttons", O_RDWR);

	if (fd < 0)
		printf("can't open!\n");

	fds[0].fd 	  = fd;
	fds[0].events = POLLIN;

	while (1) {
		ret = poll(&fds[0], 1 ,5000);
		
		if (!ret) {
			printf("time out\n");
		} else {		
			read(fd, &key_val, 1);	
			printf("key_val =  0x%x \n", key_val);
		}
		
	}
			

	return 0;
}


