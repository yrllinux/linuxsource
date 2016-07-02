/*
二分查找可以解决（预排序数组的查找）问题：只要数组中包含T（即要查找的值），
那么通过不断缩小包含T的范围，最终就可以找到它。一开始，范围覆盖整个数组。
将数组的中间项与T进行比较，可以排除一半元素，范围缩小一半。
就这样反复比较，反复缩小范围，最终就会在数组中找到T，或者确定原以为T所在的范围实际为空。
对于包含N个元素的表，整个查找过程大约要经过log(2)N次比较
*/
#include <stdio.h>

int midFind(int array[], int len, int value)
{
	int left = 0;
	int right = len-1;
	int mid = 0;
	
	while(left <= right)
	{
		mid = left + ((right - left)>>1);
		if(value > array[mid])
		{
			left = mid+1;
		}
		else if(value < array[mid])
		{
			right = mid-1;
		}
		else
			return mid;
	}
	return -1;
}

int main(int argc, char *argv[])
{
	int array[] = {1, 9, 10, 14, 17, 22, 44, 51, 77};
	int len = sizeof(array)/sizeof(int);
	int value = 13;
	printf("%d\n", midFind(array, len, value));
	return 0;
}
