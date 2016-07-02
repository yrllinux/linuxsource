
/******************************************************************************
模块名  : 
文件名  : INTERFACE.H
相关文件    ：
文件实现功能：
作者        ：
版本        ：

The least char of the version indicates the platform of this file. 0 for stardard c 1 for etiplatform

-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2004-10-20  1.00         CHUKAIYAN	create


*******************************************************************************/

#ifndef _INTERFACE_H
#define _INTERFACE_H

#include "handle_def.h"

typedef struct {
	int						iWholeSpF;
	float					fFracSpF, fSlotLag;
	TFrameParams			tFrmPara;
	VOLATILE float			fAvgSlotsPerFrm;
}TEncode;

void LoopInital(tHandleDef * gCurHandle);
int LoopEncoder(tHandleDef * rCurHandle,int iXR[2][2][576],char * pDest);

void LoopExit(tHandleDef * rCurHandle);



#endif