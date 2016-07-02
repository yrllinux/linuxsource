#include <stdio.h>
int main(int argc, char *argv[])
{
	int i = 2;
	while(i<1000)
	{
		if(i==2 || i==3 || i==5 || i==7)
		{
			printf("%d ", i);
			i++;
			continue;
		}
			
		if(i%2 == 0 || i%3 == 0 || i%5 == 0 || i%7 == 0)
		{
			i++;
			continue;
		}

		printf("%d ", i);	
		i++;		
	}
	printf("\n");
	return 0;
}
