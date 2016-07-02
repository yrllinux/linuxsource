#include <stdio.h>
#define N 10

int fbnq(int num)
{
	if(num==1 || num==2)
		return 1;
	return fbnq(num-2)+fbnq(num-1);
}
int main(int argc, char *argv[])
{
	int i=1;
	while(i<=N)
		printf("%d ",fbnq(i++));
	return 0;
}
