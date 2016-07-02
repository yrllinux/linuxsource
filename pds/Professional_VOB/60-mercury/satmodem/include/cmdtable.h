
/*!
 *\file CmdTable.h
 *\brief 对命令表的所有操作的定义；
 */

#ifndef _CMD_TABLE_H_
#define _CMD_TABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "modeminterface.h"
#include "modemservice.h"

#ifdef _LINUX_
	#include "string.h"
#endif

void        InitCmdTable(CmdTableItem* table);

int         InsertCmdTable(CmdTableItem* table,
                           u32 ip,
                           u16 port, 
                           cmd_t cmd,
                           const InCmd* item);      /*< 初始化整个表 */

InCmd*      LookforCmdTable(CmdTableItem* table,
                            u32 ip,
                            u16 port,
                            cmd_t cmd);             /*< 查找指定命令 */

void        BuildSingleItem(cmd_t cmd,
                            u8 trynum,
                            u32 value,
                            InCmd *item);           /* 根据输入参数，生成一个命令项 */

#ifdef __cplusplus
}
#endif


#endif

