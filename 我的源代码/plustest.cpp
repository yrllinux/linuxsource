#include <iostream>
using namespace std;

struct A
{
	int x;
	int y;
};
 
 void tst(A *p, int num)
 {
 
	//printf("2%x\n", p);
	//p += sizeof(A);
	//printf("3%x\n", p);
	unsigned char *i = (unsigned char *)p;
	
	//printf("3%x\n", (i + sizeof(A)));
		
	A *tmp = (A*)(i + sizeof(A));
	printf("%d %d\n", tmp->x, tmp->y);
	//p++;
	//printf("1%x\n", p);
 }
 
int main(int argc, char *argv[])
{
	A a[2];
	a[0].x= 1;
	a[0].y= 2;
	
	a[1].x= 3;
	a[1].y= 4;
		
	tst(a, 2);

	return 0;
}
