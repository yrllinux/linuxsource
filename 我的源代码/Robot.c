/*
C语言聊天机器人
*/ 
# include <stdio.h>
# include <windows.h>
# include <stdlib.h> 
# define MAXSTR 200
# define REBOT "小C说: "
# define YOUR "您 说: "
# define EXIT "-e\n"
# define NOREPLY "我不知道你说什么呢!\n" 

char * GetRebot(char * str); //处理接收的对话内容，返回机器人回复内容
void DelHr(char * str); //删除获取到的字符串中的换行
void RobotSay(char * str); //机器人回复

int main(void)
{
	char str[1024]; 
	printf("**************************聊天机器人****************************\n");
	printf("\n%sHI,我是聊天机器人小C,很高心和您认识^ ^ 退出聊天请输入-e\n",REBOT); 

	do
	{
		printf("%s",YOUR);
		scanf("%s",str);
		printf("%s",REBOT);
		
		if (str[0] != '-' && str[1] != 'e' )
		{
			RobotSay(GetRebot(str));
			printf("\n");
		}else{
			printf("和您聊天真实愉快,欢迎下次再来和我聊天~\n");
		} 
	}while(str[0] != '-' && str[1] != 'e' ); 

	return 0;
} 

char * GetRebot(char * str)
{
	static char keywords[500];
	char reply[500];
	int i = 0;
	FILE * fp; 
	if( (fp = fopen("reply","r")) == NULL)
	{
		printf("缺少核心文件!!\n");
		exit(-1);
	} 
	while ( !feof(fp) ) //获取关键字
	{
		i++;
		fgets(keywords,500,fp);
		DelHr(keywords); 
		
		if( i % 2 != 0)
		{
			if( strstr(str,keywords) != 0 )
			{
				fgets(reply,500,fp);
				return reply;
			}
		} 
	}

	fclose(fp);
	return NOREPLY; 
} 

void DelHr(char * str)
{
	int i,j;
	
	for(i=0; str[i] != '\0'; i++)
	{
		if(str[i] == '\n')
		{
			for(j=i; str[j] != '\0'; j++)
			{
				str[j] = str[j+1];
			}
		}
	}
} 

void RobotSay(char * str)
{
	int i; 
	for(i=0; str[i] != '\0'; i++)
	{
		Sleep(80);
		
		printf("%c",str[i]); 
	}
}
