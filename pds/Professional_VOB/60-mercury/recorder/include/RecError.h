/*****************************************************************************
   模块名      : 录像机
   文件名      : RecError.h
   相关文件    : 
   文件功能    : 录像机错误码定义
   作者        : 张明义
   版本        : V0.9  Copyright(C) 2001-2003 KDC, All rights reserved.

-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/

#ifndef _KDV_RECORDER_ERROR_CODE_H_
#define _KDV_RECORDER_ERROR_CODE_H_
#include "ErrorId.h"

#define ERR_REC_NOERROR         ERR_REC_BGN + 0        //没有错误
#define ERR_REC_NORECORD        ERR_REC_BGN + 1        //没有记录
#define ERR_REC_OPENRECORD      ERR_REC_BGN + 2        //打开记录失败
#define ERR_REC_UMMATCHCMD      ERR_REC_BGN + 3        //不匹配的命令！例如让一个播放设备开始录像
#define ERR_REC_VALIDCHN        ERR_REC_BGN + 4        //无效的通道
#define ERR_REC_RECORDING       ERR_REC_BGN + 5        //正在录像
#define ERR_REC_PLAYING         ERR_REC_BGN + 6        //正在播放
#define ERR_REC_NOTPLAY         ERR_REC_BGN + 7        //播放通道没在播放 
#define ERR_REC_NOTRECORD       ERR_REC_BGN + 8        //录像通道没在录像
#define ERR_REC_ACTFAIL         ERR_REC_BGN + 9        //执行指定的请求动作失败
#define ERR_REC_CURDOING        ERR_REC_BGN + 10       //请求的动作当前正在执行
#define ERR_REC_RCMD_TO_PCHN    ERR_REC_BGN + 11       //一条播放通道命令发到录像通道
#define ERR_REC_PCMD_TO_RCHN    ERR_REC_BGN + 12       //一条录像通道命令发到播放通道
#define ERR_REC_DISKFULL        ERR_REC_BGN + 13       //磁盘满
#define ERR_REC_FILEDAMAGE      ERR_REC_BGN + 14       //文件损坏
#define ERR_REC_FILEEMPTY       ERR_REC_BGN + 15       //空文件
#define ERR_REC_FILEUSING       ERR_REC_BGN + 16       //指定文件正在被操作
#define ERR_REC_FILENOTEXIST    ERR_REC_BGN + 17       //指定文件不存在
#define ERR_REC_PUBLICFAILED    ERR_REC_BGN + 18       //发布失败
#define ERR_REC_RENAMEFILE      ERR_REC_BGN + 19       //文件名冲突,更改文件名失败
#define ERR_REC_FILEEXISTED     ERR_REC_BGN + 20       //文件名冲突,无法录像
#define ERR_REC_PLAYERRPAYLOAD  ERR_REC_BGN + 21       //放像文件媒体载荷类型与会议不匹配
#define ERR_REC_CALLLIBFAILED   ERR_REC_BGN + 22       //调用录像库失败,建议重新安装
#define ERR_REC_SETMOTHODFAILED ERR_REC_BGN + 23       //录像方式设置失败
#define ERR_REC_PALYFILERES		ERR_REC_BGN + 24	   //录像文件分辨率偏大
#define ERR_REC_FILENAMETOOLONG ERR_REC_BGN + 25       //输入文件名过长    // add by jlb  081026
#define ERR_REC_HAS_UNDERLINE	ERR_REC_BGN + 26       //用户输入录像文件文件名中包含下划线字符 // [8/1/2011 liuxu]
#define ERR_REC_WRITE_FILE_ERR	ERR_REC_BGN + 27       //写文件出错, 录像终止 // [8/1/2011 liuxu]
#define ERR_REC_CREATE_SNDSOCK  ERR_REC_BGN + 28    //发送模块创建socket失败 // [pengjie 2011/8/25]
#define ERR_REC_PLAYFILE_HPDISMATCH  ERR_REC_BGN + 29    //会议不支持放像文件的high profile属性[zhouyiliang 20120309]

#ifndef RPC_S_SERVER_UNAVAILABLE
#define RPC_S_SERVER_UNAVAILABLE 0x800706ba		//RPC_S_SERVER_UNAVAILABLE = 0x800706ba 与服务器断开连接
#endif


#endif /*!_KDV_RECORDER_ERROR_CODE_H_*/
