#include <sys/types.h>   
#include <unistd.h>   
#include <stdlib.h>   
#include <stdio.h>   
#include <string.h>   
  
int checkConnect(char *dst, int cnt)   
{   
    int i = 0;   
    FILE *stream;   
    char recvBuf[16] = {0};   
    char cmdBuf[256] = {0};   
  
    if (NULL == dst || cnt <= 0)   
        return -1;   
  
    sprintf(cmdBuf, "ping %s -c %d -i 0.2 | grep time= | wc -l", dst, cnt);   
    stream = popen(cmdBuf, "r");   
    fread(recvBuf, sizeof(char), sizeof(recvBuf)-1, stream);   
    pclose(stream);   
  
    if (atoi(recvBuf) > 0)   
    	return 0;   
  
    return -1;   
}   
  
int main(int argc, char *argv[])   
{   
    if (argc != 3)   
    {   
        printf("Please set the arguments as follow:\n");   
        printf("%s <destination> <count>\n", argv[0]);   
        return -1;   
    }   
  
    if (checkConnect(argv[1], atoi(argv[2])))   
        printf("Network is not up to %s\n", argv[1]);   
    else  
        printf("Network is up to %s\n", argv[1]);   
  
    return 0;   
}  
