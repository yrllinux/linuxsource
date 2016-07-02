// license.cpp : Defines the entry point for the console application.
//
#include "stdio.h"
#include "stdlib.h"
#include "kdvencrypt.h"


enum { ENCRYPTION, DECRYPTION };

bool Using_DES(char *Out,char *In,long textlen,int keylen,const char *Key,bool Type = ENCRYPTION);


//-----------------------------------------------------------------------------------
// 初始置换（initial permutation）IP列表
const static char IP_TABLE[64] = {
58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};

// 扩展置换（expansion operation）E列表
static const char E_TABLE[48] = {
32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1
};

//密钥置换PC1列表
const static char PC1_TABLE[56] = {
57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
};

// 密钥置换中每轮移动的位数
const static char LOOP_TABLE[16] = {
1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};

// 压缩置换PC2列表
const static char PC2_TABLE[48] = {
14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

// S-盒代替（substitution box）
const static char S_BOX[8][4][16] = {
//S盒1
14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13,
// S盒2
15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9,
// S盒3
10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12,
// S盒4
7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14,
// S盒5
2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3,
// S盒6
12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13,
// S盒7
4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12,
// S盒8
13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
};

// P-盒置换（straight permutation ）或称为直接置换
const static char P_TABLE[32] = {
16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10,
2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25
};

// 末置换（final permutation）IP^-1列表
const static char IPF_TABLE[64] = {
40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25
};

//-----------------------------------------------------------------------------------
typedef bool (*SUBKEY_P)[16][48];
//-----------------------------------------------------------------------------------
static void DES(char Out[8], char In[8], const SUBKEY_P pskey, bool Type);//标准DES加/解密
static void SETKEY(const char* Key, int len);// 设置密钥
static void Set_SubKey(SUBKEY_P pskey, const char Key[8]);// 设置子密钥
static void F_FUNCTION(bool In[32], const bool Ki[48]);// f 函数完成扩展置换、S-盒代替和P盒置换
static void S_BOXF(bool Out[32], const bool In[48]);// S-盒代替函数
static void TRANSFORM(bool *Out, bool *In, const char *Table, int len);// 变换函数
static void XOR(bool *InA, const bool *InB, int len);//异或函数
static void CYCLELEFT(bool *In, int len, int loop);// 循环左移函数
static void ByteToBit(bool *Out, const char *In, int bits);// 字节组转换成位组函数
static void BitToByte(char *Out, const bool *In, int bits);// 位组转换成字节组函数
//-----------------------------------------------------------------------------------
static bool SubKey[1][16][48];// 子密钥
static char T[256], deskey[16];
//-----------------------------------------------------------------------------------
//以下是各个函数的定义
//-----------------------------------------------------------------------------------
bool Using_DES(char *Out, char *In, long textlen, int keylen,const char *Key, bool Type)
{
	if( !( Out && In && Key && (textlen=(textlen+7)&0xfffffff8) ) )
	{
		return false;
	}

	SETKEY(Key, keylen);
	for(long i=0,j=textlen>>3; i<j; ++i,Out+=8,In+=8)
	DES(Out, In, &SubKey[0], Type); // 1次DES
	return true;
}

void ByteToBit(bool *Out, const char *In, int bits)
{
	for(int i=0; i<bits; ++i)
	Out[i] = (In[i>>3]>>(i&7)) & 1;
}

void BitToByte(char *Out, const bool *In, int bits)
{
	memset(Out, 0, bits>>3);
	for(int i=0; i<bits; ++i)
	Out[i>>3] |= In[i]<<(i&7);
}

void CYCLELEFT(bool *In, int len, int loop)
{
	memcpy(T, In, loop);
	memcpy(In, In+loop, len-loop);
	memcpy(In+len-loop, T, loop);
}

void Set_SubKey(SUBKEY_P pskey, const char Key[8])
{
	static bool K[64], *KL=&K[0], *KR=&K[28];
	ByteToBit(K, Key, 64);
	TRANSFORM(K, K, PC1_TABLE, 56);
	for(int i=0; i<16; ++i)
	{
		CYCLELEFT(KL, 28, LOOP_TABLE[i]);
		CYCLELEFT(KR, 28, LOOP_TABLE[i]);
		TRANSFORM((*pskey)[i], K, PC2_TABLE, 48);
	}
}
	
void SETKEY(const char* Key, int len)
{
	memset(deskey, 0, 24);
	memcpy(deskey, Key, len>24?24:len);
	Set_SubKey(&SubKey[0], &deskey[0]);
}

void S_BOXF(bool Out[32], const bool In[48])
{
	for(char i=0,j,k; i<8; ++i,In+=6,Out+=4)
	{
		j = (In[0]<<1) + In[5];
		k = (In[1]<<3) + (In[2]<<2) + (In[3]<<1) + In[4];
		ByteToBit(Out, &S_BOX[i][j][k], 4);
	}
}

void F_FUNCTION(bool In[32], const bool Ki[48])
{
	static bool MR[48];
	TRANSFORM(MR, In, E_TABLE, 48);
	XOR(MR, Ki, 48);
	S_BOXF(In, MR);
	TRANSFORM(In, In, P_TABLE, 32);
}

void TRANSFORM(bool *Out, bool *In, const char *Table, int len)
{
	for(int i=0; i<len; ++i)
	T[i] = In[ Table[i]-1 ];
	memcpy(Out, T, len);
}

void XOR(bool *InA, const bool *InB, int len)
{
	for(int i=0; i<len; ++i)
	InA[i] ^= InB[i];
}

void DES(char Out[8], char In[8], const SUBKEY_P pskey, bool Type)
{
	static bool M[64], tmp[32], *Li=&M[0], *Ri=&M[32];
	ByteToBit(M, In, 64);
	TRANSFORM(M, M, IP_TABLE, 64);
	if( Type == ENCRYPTION )
	{
		for(int i=0; i<16; ++i)
		{
			memcpy(tmp, Ri, 32);
			F_FUNCTION(Ri, (*pskey)[i]);
			XOR(Ri, Li, 32);
			memcpy(Li, tmp, 32);
		}
	}
	else
	{
		for(int i=15; i>=0; --i)
		{
			memcpy(tmp, Li, 32);
			F_FUNCTION(Li, (*pskey)[i]);
			XOR(Li, Ri, 32);
			memcpy(Ri, tmp, 32);
		}
	}
	TRANSFORM(M, M, IPF_TABLE, 64);
	BitToByte(Out, M, 64);
}

unsigned char text[] = " Suzhou Keda Technology Co., Ltd. Mcu and mt 中国苏州";
char key[] = "Skeudzahcooum";  // cpu id 

int main(int argc, char* argv[])
{
	
	printf("Hello World!\n");
	
	char buf[512];
	memset( buf, 0, sizeof(buf) );

	memcpy( buf, text, sizeof(text) );

	puts( buf );
	printf("\n***********************\n");

	Using_DES(buf, buf, sizeof(text), sizeof(key), key, ENCRYPTION );
	

	int i = 0;
	for( i = 0; i < sizeof(buf); i++ )
	{
		printf("%x", buf[i]);
	}

	printf("\n***********************\n");
	
	Using_DES(buf, buf, sizeof(text), sizeof(key), key, DECRYPTION );

	
	puts( buf );
	printf("\n***********************\n");
	
	memset( buf, 0, sizeof(buf) );
	
	memset( text, 0, sizeof(text) );
	
	text[0] = 17; // 00010001
	text[1] = 17; // 00010001

	memcpy(buf, text, 2 );

	puts( buf );
	printf("\n***********************\n");

	Using_DES(buf, buf, sizeof(text), sizeof(key), key, ENCRYPTION );
	

	for( i = 0; i < sizeof(buf); i++ )
	{
		printf("%x", buf[i]);
	}

	printf("\n***********************\n");


	text[0] = 273; // 10010001
	text[1] = 273; // 10010001

	memset( buf, 0, sizeof(buf) );
	memcpy(buf, text, 2 );

	puts( buf );
	printf("\n***********************\n");

	Using_DES(buf, buf, sizeof(text), sizeof(key), key, ENCRYPTION );
	

	for( i = 0; i < sizeof(buf); i++ )
	{
		printf("%x", buf[i]);
	}

	printf("\n***********************\n");

	int n = 1;
	return 0;
}
