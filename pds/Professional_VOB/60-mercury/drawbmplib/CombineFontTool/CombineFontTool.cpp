// CombineFontTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CombineFontTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		CString strHello;
		strHello.LoadString(IDS_HELLO);
		cout << (LPCTSTR)strHello << endl;
	}

	// 
	const char achDefaultEnglish[] = "..\\res\\font\\En24";
	const char achDefaultChinese[] = "..\\res\\font\\Cn24";
	const char achEn50[] = "..\\res\\font\\En3232";
	const char achCn50[] = "..\\res\\font\\Cn3232";
	const char achDefaultFont[] = "..\\res\\font\\fontlib";

	// 字库类型
	// 定义为字符型，目地是只占一个字节
	char FT_HZK16 = 0x01;   // 16*16点阵 
	char FT_HZK24 = 0x02;	// 24*24点阵
	char FT_HZK50 = 0x03;	// 50*50点阵

	// 字体大小
	char FONT_SIZE_24 = 24;	// 
	char FONT_SIZE_32 = 32;	// 

	// 字体类型
	char FONT_HEI  = 0x01; // 黑体
	char FONT_SONG = 0x02; // 宋体
	char FONT_KAI  = 0x03; // 楷体

	// 汉字库大小
	
	// 英文字库大小

	// 结束符
	char FONT_HEAD_END = 0xff;  // 

	FILE *rpEn = fopen(achDefaultEnglish, "rb");

	fseek(rpEn, 0, SEEK_END);
	int nEnFileSize = ftell(rpEn);
	
	char *pbyEnBuff = NULL;
	pbyEnBuff = new char[nEnFileSize];
	memset(pbyEnBuff, 0, nEnFileSize);

	fseek(rpEn, 0, SEEK_SET);
	fread(pbyEnBuff, nEnFileSize, 1, rpEn);// 缓存数据
	fclose(rpEn);
	rpEn = NULL;

	FILE *rpCN = fopen(achDefaultChinese, "rb");
	fseek(rpCN, 0, SEEK_END);
	int nCNFileSize = ftell(rpCN);
	
	char *pbyCNBuf = NULL;
	pbyCNBuf = new char[nCNFileSize];
	fseek(rpCN, 0, SEEK_SET);
	fread(pbyCNBuf, nCNFileSize, 1, rpCN);  // 缓存数据
	fclose(rpCN);
	rpCN = NULL;


	rpEn = fopen(achEn50, "rb");

	fseek(rpEn, 0, SEEK_END);
	int nEn32Size = ftell(rpEn);
	
	char *pbyEn50Buff = NULL;
	pbyEn50Buff = new char[nEn32Size];
	memset(pbyEn50Buff, 0, nEn32Size);

	fseek(rpEn, 0, SEEK_SET);
	fread(pbyEn50Buff, nEn32Size, 1, rpEn);// 缓存数据
	fclose(rpEn);
	rpEn = NULL;

	rpCN = fopen(achCn50, "rb");
	fseek(rpCN, 0, SEEK_END);
	int nCN32Size = ftell(rpCN);
	
	char *pbyCN32Buf = NULL;
	pbyCN32Buf = new char[nCN32Size];
	fseek(rpCN, 0, SEEK_SET);
	fread(pbyCN32Buf, nCN32Size, 1, rpCN);  // 缓存数据
	fclose(rpCN);
	rpCN = NULL;
	
	FILE *wp = fopen(achDefaultFont, "wb");
	// 字库类型
	fwrite(&FT_HZK24, sizeof(char), 1, wp);
	// 字号
	fwrite(&FONT_SIZE_24, sizeof(char), 1, wp);
	// 字体
	fwrite(&FONT_SONG, sizeof(char), 1, wp);
	// 中文字库大小
	fwrite(&nCNFileSize, sizeof(int), 1, wp);
	// 西文字库大小
 	fwrite(&nEnFileSize, sizeof(int), 1, wp);
	// 中文字库数据
	fwrite(pbyCNBuf, nCNFileSize, 1, wp);
    // 西文字库数据
 	fwrite(pbyEnBuff, nEnFileSize, 1, wp);

	// 字库类型
	fwrite(&FT_HZK50, sizeof(char), 1, wp);
	// 字号
	fwrite(&FONT_SIZE_32, sizeof(char), 1, wp);
	// 字体
	fwrite(&FONT_KAI, sizeof(char), 1, wp);
	// 中文字库大小
	fwrite(&nCN32Size, sizeof(int), 1, wp);
	// 西文字库大小
 	fwrite(&nEn32Size, sizeof(int), 1, wp);
	// 中文字库数据
	fwrite(pbyCN32Buf, nCN32Size, 1, wp);
    // 西文字库数据
 	fwrite(pbyEn50Buff, nEn32Size, 1, wp);

	// 整个字库结束符
	fwrite(&FONT_HEAD_END, sizeof(char), 1, wp); 
	
	fclose(wp);
	wp = NULL;
	
	// 释放资源
	if( NULL != pbyCNBuf)
	{
		delete []pbyCNBuf;
		pbyCNBuf = NULL;
	}

	if( NULL != pbyEnBuff )
	{
		delete []pbyEnBuff;
		pbyEnBuff = NULL;
	}
	
	return nRetCode;
}


