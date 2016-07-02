/*******************************************************
 * utils_vxworks_net.h
 * 
 * vxworks net utils
 *
 * @ 使用此文件时应该定义_VXWORKS_宏
 *
 *******************************************************/

 #ifndef __utils_vxworks_net_h
 #define __utils_vxworks_net_h

#if defined(__cplusplus) || defined(c_plusplus)
#	define BEGIN_DECL	extern "C" {
#	define END_DECL	}
#else
#	define BEGIN_DECL
#	define END_DECL
#endif

#if defined (_VXWORKS_)

#include <hostLib.h>

BEGIN_DECL

/*
struct hostent {
	char * 	h_name;
	char ** 	h_aliases;
	short 	h_addrtype;
	short 	h_length;
	char ** 	h_addr_list;
};
*/

/****************************************
 * 实现WIN32 / linux的相同函数 
 *
 *	非线程安全
 *
 ****************************************/
struct hostent * gethostbyname (const char *name)
{
	static struct hostent ret;
	static int first_addr;
	static char * hostaddr[2]; /* for luck */
	static char * aliases[1]; /* for luck */

	if ( first_addr = hostGetByName((char *) name) == ERROR ) return 0;

	hostaddr[0] = (char *) &first_addr;
	hostaddr[1] = 0;
	aliases[0] = 0;

	ret.h_name = (char *) name;
	ret.h_addrtype = AF_INET;
	ret.h_length = 4;
	ret.h_addr_list = hostaddr;
	ret.h_aliases = aliases;

	return &ret;
}


/* max length of host name, for luck */
#define MAXNAMELEN 255

/****************************************
 * 实现WIN32 / linux的相同函数 
 *
 *	非线程安全
 *
 ****************************************/
struct hostent * gethostbyaddr(const char * addr, int len, int type)
{
	static struct hostent ret;
	static char name [MAXNAMELEN+1];
	static char * hostaddr[2];
	static char * aliases[1];

	if ( len != 4 || type != AF_INET ) return 0;
	if ( hostGetByAddr (*(int *) addr, name) == ERROR ) return 0;

	hostaddr[0] = (char *) addr;
	hostaddr[1] = 0;
	aliases[0] = 0;

	ret.h_name = name;
	ret.h_addrtype = AF_INET;
	ret.h_length = 4;
	ret.h_addr_list = hostaddr;
	ret.h_aliases = aliases;

	return &ret;
}

END_DECL
	
#endif /* _VXWORKS_ */

 #endif /* __utils_vxworks_net_h */
