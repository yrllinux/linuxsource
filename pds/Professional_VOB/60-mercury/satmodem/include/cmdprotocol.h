/*!
*\brief 协议包的封装
*/

#ifndef _CMD_PROTOCOL_H_
#define _CMD_PROTOCOL_H_

//#include "osp.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    
    int AssemSendPack(cmd_t cmd, u32 val, u8 *pack);        /*< 根据命令号与值，装配一个发送包 */
    int DisassemRecvPack(const u8 *pack, int len,           \
                         u32 ip, u16 port);                 /*< 从一个接收包中分解出指定命令的值 */
    
#ifdef __cplusplus
}
#endif


#endif

