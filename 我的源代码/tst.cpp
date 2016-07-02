#include <iostream>
int main(int argc, char *argv[])
{
	char *p = "/usr/NM-SMU.bin";
	char path[256] = {0};
	strcpy(path, p);
	 
 	char i = strlen(path);
 	while(path[i-1] != '/' && i >= 0)
 	{
	 	i--;
 	}
 	printf("i = %d\n", i);
 	
 	char FilePath[256] = {0};
 	char FileName[256] = {0};
	strncpy(FilePath, path, i);
	strncpy(FileName, &path[i], strlen(path) - i);
	
	printf("FilePath = %s\n", FilePath);
 	printf("FileName = %s\n", FileName);
 	
 	char Tmppath[256] = {0};
 	sprintf(Tmppath, "cd %s && chmod 777 %s && ./%s", FilePath, FileName, FileName);
 	printf("Tmppath = %s\n", Tmppath);
	return 0;
}
