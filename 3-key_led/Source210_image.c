#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define IMG_SIZE        16 * 1024
#define HEAD_SIZE       16

int
main(int argc,char *argv[])
{
        unsigned char *ucBuffer;
        FILE          *pfile;
        unsigned int   binlen = 0;
        unsigned int   i;
	unsigned int   checksum = 0;  /* 校检合 */	
	unsigned short buffer = 0;    /* reading 1 byte from bl1 */
        /* 判断输入是否合法 */
        if(argc != 3){
                printf("usage:%s <source file> <destination file>\n",argv[1]);
                return -1;
        }

        /* 分配内存 */
        ucBuffer = (unsigned char*)malloc(IMG_SIZE);

        if(ucBuffer == NULL){
                printf("malloc buffer error\n");

                return -1;
        }
        /* 初始化内存 */
        memset(ucBuffer,0,IMG_SIZE);


	/* 打开bin文件 */
        pfile = fopen(argv[1],"rb");

        if(pfile == NULL){
                printf("source file fopen error\n");
                return -1;
        }

        /* 偏移指针移到文件尾并得到整个文件的长度 */
        fseek(pfile,0,SEEK_END);
        binlen = ftell(pfile);

        /* 将偏移指设置到文件开头 */
        fseek(pfile,0,SEEK_SET);

        /* 文件长度不得超过(16kb-16)个字节 */
        if(binlen > (IMG_SIZE - HEAD_SIZE)){
                printf("source bin is  > 16kByte\n");
                fclose(pfile);
                free(ucBuffer);
                return -1;
        }

        /* read bin to memory */
        if(fread(ucBuffer + HEAD_SIZE,1,binlen,pfile) != binlen){
                printf("fread source bin error\n");
                free(ucBuffer);
                fclose(pfile);
                return -1;
        }
	
	/* 关闭文件，不需要就关闭 */
        fclose(pfile);
	
	memcpy(ucBuffer,"SourcelinkSPV210",HEAD_SIZE);	

	/* 求出校检合 */
        for(i = 0;i <IMG_SIZE - HEAD_SIZE;i++){
		buffer = (*(volatile unsigned char*)(ucBuffer + HEAD_SIZE + i));
		checksum += buffer;
	}
	
	/* 写进缓冲区 */
	*(volatile unsigned char*)(ucBuffer + 8) = checksum;
	
	if((pfile = fopen(argv[2],"wb")) == NULL){
		printf("fopen file error");
		free(ucBuffer);
		return -1;
	}
	/* 写入文件中 */
	if(fwrite(ucBuffer,1,IMG_SIZE,pfile) != IMG_SIZE){
		printf("file fwrite error\n");
		free(ucBuffer);
		fclose(pfile);
		return -1;
	}
	
	free(ucBuffer);
	fclose(pfile);
	
	return 0;
}

