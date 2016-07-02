#include <stdio.h>
// #define _LARGEFILE_SOURCE
// #define _LARGEFILE64_SOURCE
// #define _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	long lp = 0xffffffff;
 	printf("Size1: %08x \n",lp );
 	lp += 1;
 	printf("Size1: %08x \n",lp );
 	//return 0;
 	char filebuf[10] = {
	 	0
	 };
	 int i = 0;
	 for(i;i<10;i++)
	 {
 		printf("%08x\n",filebuf[i]);
 	}
 	memset(filebuf, 0, sizeof(filebuf));
 	
	off_t  file_last_pos;
	off_t end = 0;
	//  FILE           *fp;
	int fp = open64(argv[1], O_RDONLY);
	if (fp < 0 ) 
	{
		printf("can't open file [%s]\n", strerror(errno));
		return 1;
	} 
	else 
	{
		printf("file open success\n");
	}
	file_last_pos = lseek(fp, 0, SEEK_SET);
	int nRet = read(fp, filebuf, 10);
	printf("nRet %1d, file_last_pos %ld\n",nRet, file_last_pos);
//	i = 0;
 	for(i = 0;i<10;i++)
	 {
 		printf("%08x\n",filebuf[i]);
 	}
 	
 	
	file_last_pos = lseek(fp, 0xffffffff + 1, SEEK_SET);
	nRet = read(fp, filebuf, 10);
	printf("nRet %1d, file_last_pos %ld\n",nRet, file_last_pos);
//	i = 0;
 	for(i = 0;i<10;i++)
	 {
 		printf("%08x\n",filebuf[i]);
 	}
	printf("Size: %1d \n",file_last_pos);
	close(fp);
	return 0;
}

//这行GCC参数很重要，原来是希望通过define的方式来解决的，但是最后还是只能通过这种方式
//gcc -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64    test.c   -o test