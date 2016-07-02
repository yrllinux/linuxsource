/*****************************************************************************
模块名      : DcMtLib
文件名      : DcMtLibMacro.h
相关文件    : 
文件实现功能: DcMtLib宏定义
作者		: chenhongbin
版本        : V1.0
-----------------------------------------------------------------------------
修改记录:
日  期      版本     修改人             修改内容
2005/05/26  1.0      chenhongbin         创建
******************************************************************************/
#ifndef _DCMTLIBMACRO_H
#define _DCMTLIBMACRO_H




#define ERR_DCMT_BGN    0
//操作成功
#define DCMTLIB_OP_SUCCESS                     ERR_DCMT_BGN//26501
//初始化失败
#define ERR_DCMTLIB_INITIAL_FAIL               ERR_DCMT_BGN + 1
//功能库已经初始化
#define ERR_DCMTLIB_ALWAYS_INIT                ERR_DCMT_BGN + 2
//功能库必须初始化
#define ERR_DCMTLIB_MUST_INIT                  ERR_DCMT_BGN + 3
//终端已经在会议中
#define ERR_DCMTLIB_ALWAYS_INCONF              ERR_DCMT_BGN + 4
//终端必须在会议中
#define ERR_DCMTLIB_MUST_INCONF                ERR_DCMT_BGN + 5
//参数错误
#define ERR_INVALID_PARAMETER                  ERR_DCMT_BGN + 6

//聊天功能已经开启
#define ERR_DCMTLIB_CHAT_ALWAYS_START          ERR_DCMT_BGN + 10
//聊天功能必须开启
#define ERR_DCMTLIB_CHAT_MUST_START            ERR_DCMT_BGN + 11
//无聊天对象
#define ERR_DCMTLIB_CHAT_NO_PARTICIPANT        ERR_DCMT_BGN + 12

//文件传输功能已经开启
#define ERR_DCMTLIB_MBFT_ALWAYS_START          ERR_DCMT_BGN + 20
//文件传输功能必须开启
#define ERR_DCMTLIB_MBFT_MUST_START            ERR_DCMT_BGN + 21
//无文件传输对象
#define ERR_DCMTLIB_MBFT_NO_PARTICIPANT        ERR_DCMT_BGN + 22
//正在发送文件
#define ERR_DCMTLIB_MBFT_ALWAYS_SENDINGFILE    ERR_DCMT_BGN + 23
//当前并未发送文件
#define ERR_DCMTLIB_MBFT_NOT_SENDINGFILE       ERR_DCMT_BGN + 24
//发送文件失败
#define ERR_DCMTLIB_MBFT_SENDFILE_FAIL         ERR_DCMT_BGN + 25

//白板功能已经开启
#define ERR_DCMTLIB_SI_ALWAYS_START            ERR_DCMT_BGN + 30
//白板功能必须开启
#define ERR_DCMTLIB_SI_MUST_START              ERR_DCMT_BGN + 31

//应用共享能力级交换未成功
#define ERR_DCMTLIB_APPSHARE_CAPNEGOTIA        ERR_DCMT_BGN + 35


#endif//_DCMTLIBMACRO_H
