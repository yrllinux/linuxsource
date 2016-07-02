#ifndef _WATCHDOGDEF_H_
#define _WATCHDOGDEF_H_

#include "kdvtype.h"
#include "kdvdef.h"
#include "osp.h"
#include "eventid.h"

//////////////////////////////// MDSC/HDSC APP ID ///////////////////////////////
#define  WD_SER_APP_ID             (AID_DSC_BGN+1)
#define  WD_CLT_APP_ID             (AID_DSC_BGN+2) 


//////////////////////////////// 宏定义 ///////////////////////////////
//服务器参数 
#define  WD_SER_IP                 inet_addr( "127.0.0.1" )
#define  WD_SER_TCP_PORT           (u16) 3500
#define  WD_SER_TELNET_PORT        (u16) 3501

#define  WD_MODULE_NUM             (u16) 6 

//////////////////////////////// 消息定义 ///////////////////////////////

/*EV_MODULECONCMD
  请求连接
  消息内容: (ModuleParameter ，1)
*/
OSPEVENT(EV_MODULECONCMD,            (EV_DSCCONSOLE_BGN +1) );

/*ev_WatchDogConInd
  连接成功返回
  内容：
  */
OSPEVENT(EV_MODULECONIND,            (EV_DSCCONSOLE_BGN +2) );

/*ev_ModuleSetState
  设置状态
  内容：(ModuleParameter ，1)
*/
OSPEVENT(EV_MODULESETSTATE,           (EV_DSCCONSOLE_BGN +5) );


//模块类别
enum  emModuleType 
{
    emINVALIDMODULE  = -1,          //未知模块
    emGK             = 0,           //GK
    emT120           = 1,           //数据会议
    emFIREWALL       = 2,           //防火墙
    emMP             = 3,           //MMP
    emMTADP          = 4,           //mtAdp
    emPRS            = 5            //PRS
};

//模块状态
enum emModuleState
{
    emSTOP            = 0,          //停止
    emRUNNING         = 1           //运行
};

//模块参数
typedef struct tagModuleParameter
{
    tagModuleParameter() : emType(emINVALIDMODULE), emState(emSTOP) {}

    emModuleType    emType;     //模块类型
    emModuleState   emState;    //模块状态
} IModuleParameter, *PIModuleParameter;

#endif
