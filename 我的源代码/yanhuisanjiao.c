//#define N 13 //定义三角最大行,可自行设定，但最大不要超过65536； 
#include <stdio.h> 
int main(void)
{ 
	int n,i,j; int a[100][1000]; 
	printf("How Many Rows Do You Want:\n"); 
	scanf("%d",&n); 
	
 	//初始化数组； 
	for(i=0;i<=n;i++)
		for(j=0;j<=2*n;j++)
			a[i][j]=0; 

	a[1][n]=1; 
	//设置杨辉三角的数值； 
	for(i=0;i<=n;i++)	
		for(j=0;j<2*n;j++)		
			if(a[i][j]!=0)
			{ 
				a[i+1][j-1]=a[i][j-2]+a[i][j]; 
				a[i+1][j+1]=a[i][j+2]+a[i][j]; 
			}
			  
	//打印杨辉三角； 
	for(i=0;i<=n;i++)
	{ 
		for(j=0;j<2*n;j++)
		{ 
			if(a[i][j]!=0) 
				printf("%4d",a[i][j]); //打印格式自设，如加“\t”或空格等； 
			 //主要是为了打印的图像美观； 
			else
				printf("   ");  
			if(j==(2*n-1))
				printf("\n");
		}
	}
	getchar();
	getchar();
	return 0;
}
