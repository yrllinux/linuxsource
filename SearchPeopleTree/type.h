
#ifndef _TYPE_H_
#define _TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	  /* Type definition */
/*-----------------------------------------------------------------------------
系统公用文件，开发人员严禁修改
------------------------------------------------------------------------------*/

typedef int		s32,BOOL32;
typedef unsigned long   u32;
typedef unsigned char	u8;
typedef unsigned short  u16;
typedef short           s16;
typedef char            s8;

#ifdef _MSC_VER
typedef __int64			s64;
#else 
typedef long long		s64;
#endif 

#ifdef _MSC_VER
typedef unsigned __int64 u64;
#else 
typedef unsigned long long u64;
#endif

#ifndef _MSC_VER
#ifndef LPSTR
#define LPSTR   char *
#endif
#ifndef LPCSTR
#define LPCSTR  const char *
#endif
#endif

const u16 MAX_NAME_LEN = 260;

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef NAME_LEN
#define NAME_LEN	40
#endif

#ifndef PLACE_LEN
#define PLACE_LEN	400
#endif

#ifndef NUM_LEN
#define NUM_LEN		15
#endif

#ifndef NUM_KEY
#define NUM_KEY		15
#endif

enum Do
{
	NONE,
	READ,
	WRITE,
	DELET
};

//记录文件路径名
extern s8	m_achPathFileName[MAX_NAME_LEN];																				

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _KDV_def_H_ */

/* end of file kdvdef.h */

