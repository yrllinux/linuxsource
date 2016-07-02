#include <stdio.h>
#include <string.h>
int yhsj(void)
{
	int p[20][100];
	memset(p,0,sizeof(p));
	int num=10;
	int tmp=10;
	int i=0;
	while(i++ < 10){
		p[i][num++]=1;
		p[i][tmp--]=1;				
	}
	int ii;
	int jj;
	for(ii=0;ii<10;ii++)
		for(jj=0;jj<20;jj++)
		{
			if(ii>=3 && jj>=3)
				p[ii][jj]=p[ii-1][jj-1]+p[ii-1][jj+1];
			if(jj==19)
			{
				printf("\n");
			}			
			if(p[ii][jj] == 0)
				printf("   ");
			if(p[ii][jj] != 0)
				printf("%3d",p[ii][jj]);
		}

}
int main(int argc, char *argv[])
{
	yhsj();
	return 0;
}
