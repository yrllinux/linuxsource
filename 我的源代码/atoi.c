#include <stdio.h>

int myatoi(const char *caracter)
{
	int tmp = 0;
	int flag = 0;
	const char *p = caracter;
	if(caracter == NULL)
		return -1;
	if(p[0] == '-' || p[0] == '+')
		p++;
	while(*p >= '0' && *p <= '9')
		p++;
	if((*p < '0' || *p > '9') && *p != '\0')
		return -2;	
		
	if(caracter[0] == '-')
		flag = -1;
	
	if(caracter[0] == '-' || caracter[0] == '+')
		caracter++;

	while(*caracter != '\0' )
	{
		tmp = tmp*10+ *caracter -'0';
		caracter++;	
	}
	if(flag==-1)
		return -tmp;
	return tmp;
}

int main(int argc, char *argv[])
{
	char *a="100";
	int i=0;
	i=myatoi(a);
	printf("%d \n\n",i-2);
	return 0;
}
