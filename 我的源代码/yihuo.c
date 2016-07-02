#include <stdio.h>
int main(int argc, char *argv[])
{
	int num=0;
	int arrary[5]={2,3,4,2,4};
	for(int i=0;i<5;i++)
		num ^=arrary[i];
	printf("singal num:%d\n",num);
	return 0;
}
