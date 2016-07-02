/*!
 *\file MsgPipe.h
 *\brief 管道实现部分的定义
 *
 *本模块使用了一个管道，用于串行化要发送的命令。
 */

#ifndef _MSG_PIPE_H_
#define _MSG_PIPE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "kdvtype.h"
#include "modeminterface.h"
#include "modemservice.h"

#ifdef _LINUX_
#include "unistd.h"
#endif




int  CreateMsgPipe(MsgPipe *pipe);                          /*< 创建管道及相关变量 */
void CloseMsgPipe(MsgPipe *pipe);                           /*< 关闭管道 */
int  WriteMsgPipe(MsgPipe *pipe, const MdcomCmd *msg);      /*< 往管道中写入一个消息 */
int  ReadMsgPipe(MsgPipe *pipe, MdcomCmd *msg);             /*< 从管道中读出一个消息 */


int  InitSocket(SOCKET *sock);
void CloseSocket(SOCKET *sock);

u32  GetTick(void);
void Waitms(u32 millisecs);


#ifdef __cplusplus
}
#endif

#endif

