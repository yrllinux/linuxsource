#include <stdio.h>

int fbnq(int n)//µİ¹é ì³²¨ÄÇÆõĞòÁĞ 
{
	if(n==1||n==2)
		return 1;
 	else
		return fbnq(n-1)+fbnq(n-2); 
} 
int main(int argc, char *argv[])
{
	int n=10,i;
	for(i=1;i<n;i++)
		printf("%d ",fbnq(i)); 
	printf("\n\n"); 	
	return 0;
}
