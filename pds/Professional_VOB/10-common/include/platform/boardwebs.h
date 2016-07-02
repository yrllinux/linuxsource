/******************************************************************************
模块名  ： WEB SERVER
文件名  ： boardwebs.h
相关文件：
文件实现功能：WEB-SERVER模块对外提供的宏定义头文件
作者    ：zhanka
版本    ：1.0.0.0.0
---------------------------------------------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
08/17/2007  1.0         zhanka      创建
******************************************************************************/
#ifndef _BOARDWEBS_H_
#define _BOARDWEBS_H_

#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */

#define BRD_WEBS_ENABLE 1
#define BRD_WEBS_DISABLE 2

#define BRD_WEBS_INSPECT_ENABLE 3
#define BRD_WEBS_INSPECT_DISABLE 4

#define BRD_WEBS_ERR_BASE 100
//返回值类型
#define BRD_WEBS_PORT_ERR   BRD_WEBS_ERR_BASE+1	//端口绑定出错
/*================================
函数名：BrdWebsEnable
功能：启动或停止web server
算法实现：（可选项）
引用全局变量：
输入参数说明：dwEnable :BRD_WEBS_ENABLE 启动
                       :BRD_WEBS_DISABLE 停止
返回值说明： OK/ERROR/BRD_WEBS_PORT_ERR(BRD_WEBS_PORT_ERR:该错误只针对在port
                绑定出错情况下的返回,上层需要重新设置合法的port)
==================================*/
s32 BrdWebsEnable(u32 dwEnable);

/*================================
函数名：BrdWebsInspectEnable
功能：启动或停止web server 监控程序
算法实现：（可选项）
引用全局变量：
输入参数说明：dwEnable :BRD_WEBS_INSPECT_ENABLE 启动
                       :BRD_WEBS_INSPECT_DISABLE 停止
返回值说明： OK/ERROR
==================================*/
s32 BrdWebsInspectEnable(u32 dwEnable);

/*================================
函数名：BrdWebsSetPath
功能：设置web server  webroot
算法实现：（可选项）
引用全局变量：
输入参数说明：pchPath webroot 默认为/usr/webroot
返回值说明： OK/ERROR
==================================*/
s32 BrdWebsSetPath(char * pchPath);

/*================================
函数名：BrdWebsSetPort
功能：设置web server  port
算法实现：（可选项）
引用全局变量：
输入参数说明：dwPort == 0 : web监听port恢复成默认端口80 
              端口值不能小于0且不能大于65535
返回值说明： OK/ERROR/BRD_WEBS_PORT_ERR(BRD_WEBS_PORT_ERR:该错误只针对在port
                绑定出错情况下的返回,上层需要重新设置合法的port)
==================================*/
s32 BrdWebsSetPort(int dwPort);
#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /* _BOARDWEBS_H_ */

