#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[])
{
	char *cara = "1s3999999998abc12edfs333i78v4443277xx653uu";
	char *p = cara; 
	int count = 0;
	int len = 0;
	int tmp = -1;
	int start = 0;
	
	while(*cara != '\0')
	{
		tmp++;
		if(*cara >= '0' && *cara <= '9')
		{
			count++;
		}
		else
		{
			if(count > len)
			{
				len=count;
				start = tmp;
			}
			//tmp++;
			count =0;
		} 
		cara++;
	}
	
	start -=len;
	printf("len of char is %d\n\n",len);
	
	printf("start of char is %d\n\n",start);
	
	for(count=0;count<len;count++)
		printf("%c",p[start+count]);

	printf("\n\n");
	
	return 0;
}
