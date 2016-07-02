#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	FILE *stream;
    char recvBuf[256] = {0};
    char cmdBuf[256] = {0};
    char ipBuf[256] = {0};
    int i = 100;
    char *ip = "192.168.0.105";
    char *mac = "54:AE:27:E2:E1:02";
    for(;i<110;i++)
    {
		memset(recvBuf, 0, sizeof(recvBuf));
		memset(cmdBuf, 0, sizeof(cmdBuf));
		memset(ipBuf, 0, sizeof(ipBuf));
    	
    	//查找的ip 
    	sprintf(ipBuf, "192.168.0.%d", i);
 		//要匹配的ip
	    sprintf(cmdBuf, "arping -I eth0 -c 1 %s | grep Unicast | grep -c -e %s -e %s", ipBuf, ip, mac);
  		//执行并获取结果 
     	stream = popen(cmdBuf, "r");
     	//将结果放到buf中 
    	fread(recvBuf, sizeof(char), sizeof(recvBuf)-1, stream);
    	pclose(stream);

		if(atoi(recvBuf) > 0)	
		{
  			printf("Conflict(%d)ip:%s mac:%s\n", atoi(recvBuf), ip, mac);	
		}        
	}
	return 0;
}
