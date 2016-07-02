#include <stdio.h>
void* myMemcpy(void *dest , const void* src , int count)
{
	if(dest == NULL || src == NULL)
		return NULL;
	if(count == 0)
		return dest;
		
	//·ÀÖ¹ÄÚ´æÖØ¸´  
    //assert(!(psrc<=pdst && pdst<psrc+count));      
    //assert(!(pdst<=psrc && psrc<pdst+count)); 
			
	void *p = dest;
	while(count--)
		*(char*)p++ = *(char*)src++;
		
	return dest;
}

int main(int argc, char *argv[])
{
	char *src = "abcdefg";
	char dst[10] = {0};
	myMemcpy(dst, src, sizeof("abcdefg"));
	
	printf("%s\n", dst);
	return 0;
}
