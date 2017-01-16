#include <stdio.h>  
#include <stdlib.h>  
#include <time.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <unistd.h>   
#include <sys/mman.h>  
#define AUDIO_REG_BASE   (0xF8000000)  
#define MAP_SIZE        0xFF  
  
static int dev_fd;  
int main(int argc, char **argv)  
{  

	int i;
  
    dev_fd = open("/dev/mem", O_RDWR | O_NDELAY);        
  
    if (dev_fd < 0)    
    {  
        printf("open(/dev/mem) failed.");      
        return 0;  
    }    
  
    unsigned int *map_base=(unsigned int * )mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, AUDIO_REG_BASE );  

  	for (i = 0; i < 42; i++)
   		 printf("%x \n", *(volatile unsigned int *)(map_base + i)); //打印该寄存器地址的value  
  
#if 0 // LINE IN  
    printf("%x \n", *(volatile unsigned int *)(map_base+0x30));  
  
    *(volatile unsigned int *)(map_base + 0x30) = 0x208121bc; //修改该寄存器地址的value  
    usleep(1000000);  
    *(volatile unsigned int *)(map_base + 0x30) &= ~(0x1<<16); //修改该寄存器地址的value  
    usleep(1000000);  
  
    printf("%x \n", *(volatile unsigned int *)(map_base+0x30));  
#endif  
  
    if(dev_fd)  
        close(dev_fd);  
  
    munmap(map_base,MAP_SIZE);//解除映射关系  
  
    return 0;  
}  

