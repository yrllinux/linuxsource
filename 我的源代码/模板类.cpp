#include <iostream>

template <class T>
class compare
{
public:
	T max(T x, T y)
	{
		return x>y?x:y;
	}
	T min(T x, T y)
	{
		return x<y?x:y;
	}
};

int main(int argc, char* argv[])
{
	int a = 10;
	int b = 11;
	compare<int> cmp;
	printf("The max is %d\n", cmp.max(a, b));
	printf("The min is %d\n", cmp.min(a, b));  
	
	float a1 = 10.11;
	float b1 = 9.85;
	compare<float> cmp1;
	printf("The max is %f\n", cmp1.max(a1, b1));
	printf("The min is %f\n", cmp1.min(a1, b1)); 
	return 0;
}
