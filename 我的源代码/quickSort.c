/*
基本思想是：通过一趟排序将要排序的数据分割成独立的两部分，
其中一部分的所有数据都比另外一部分的所有数据都要小，
然后再按此方法对这两部分数据分别进行快速排序，
整个排序过程可以递归进行，以此达到整个数据变成有序序列。
*/
#include <stdio.h>
void quicksort(int array[], int left, int right) 
{
	if(left > right)
		return;
		
	int base = array[left];
	int i = left;
	int j = right;
	while(i != j)
	{
		//从最右边找到比base小的为止 
		while(array[j] >= base && i < j)
			j--;
		
		//从最左边找到比base大的为止 
		while(array[i] <= base && i < j)
			i++;
		
		//交换二者 
		if(i < j)
		{
			int tmp = array[i];
			array[i] = array[j];
			array[j] = tmp;
		}
	}
	
	//交换base跟中间值 
	array[left] = array[i];
	array[i] = base;
			
	quicksort(array, left, i-1); 
	quicksort(array, i+1, right); 
}

int main(int argc, char *argv[])
{
	int b[10] = {2,5,1,9,7,6,8,4,3,0};
	int j = 0;
	quicksort(b,0,9);
	for(j=0;j<10;j++)
		printf("%d  ",b[j]);
	printf("\n");
	return 0;
}
