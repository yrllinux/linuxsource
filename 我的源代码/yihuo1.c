#include <stdio.h>
int main(int argc, char *argv[])
{
	int arrary[5]={1,1,3,4,3};
	int tmp = 0;
	int n=5;
	while(n--)
		tmp ^= arrary[n];
	printf("%d \n",tmp); 
	return 0;
}
