#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

int fd;

void my_signal_fun(int signum)
{
	unsigned char key_val;

	read(fd , &key_val, 1);
	printf("key_val = 0x%x\n",key_val);
}


int main(int argc, char **argv)
{

	int  Oflags;
	unsigned int  ret = 0 ;
	unsigned char key_val;
	
//	signal(SIGIO, my_signal_fun);	/* 收到io信号则执行回调函数 */
	
	fd = open("/dev/buttons", O_RDWR | O_NONBLOCK);

	if (fd < 0) {
		printf("can't open!\n");
		return -1;
	} else {
		printf("open successful\n");
	}
	
#if 0	
	fcntl(fd, F_SETOWN, getpid());			/* 将该进程指定为设备文件的所有者，目的:让内核知道信号到达时该通知哪个信号  */
	Oflags = fcntl(fd, F_SETFL);			/* 读取当前文件标志 */
	fcntl(fd, F_SETFL, Oflags | FASYNC);	/* 设置设备文件为异步通知，驱动的fasync函数被调用 */
#endif

	/*
	 * 当数据到达时，所有的注册异步通知的进程都会收到一个SIGIO信号
	 */

	while (1) {
			ret = read(fd , &key_val, 1);
			printf("key_val = 0x%x, ret = %d\n",key_val,ret);
			sleep(5);
	}
			

	return 0;
}


