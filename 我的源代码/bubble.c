#include <stdio.h>
void bubble(int *arrary,int n)
{
	int i,flag,tmp;
	int count=0;
	do
	{
		count++;
		flag=0;
		for(i=0;i<n-1;i++)
		{
			count++;
			if(arrary[i]>arrary[i+1])
			{
				tmp=arrary[i];
				arrary[i]=arrary[i+1];
				arrary[i+1]=tmp;
				flag=1;
			} 
		} 
	}while(flag);
	printf("%d\n",count);
} 
int main(int argc, char *argv[])
{
	int arrary[10]={1,3,2,5,4,33,2,44,32,7};
	bubble(arrary,10);
	for(int i=0;i<10;i++)
		printf("%d ",arrary[i]);
	return 0;
}
