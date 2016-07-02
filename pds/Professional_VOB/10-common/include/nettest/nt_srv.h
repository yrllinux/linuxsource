#ifndef _NTSRVAPI_H_
#define  _NTSRVAPI_H_

extern char * NT_GetServerVer();
extern int NT_InitServer();
extern int NT_StartServer();
extern void NT_StopServer();

#endif
