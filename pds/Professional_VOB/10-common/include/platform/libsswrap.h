/*! \file libsswrap.h
    \brief 提供报文平滑发送支持.

    平滑发送模块允许上层应用通过API设置发往指定目地的报文的平均发送速率,
    并允许一定范围内的突发流量.

    版权所有 2007 kedacom \n
    版本: 1.0 \n
    历史:  \n
      - 初始版本.     张元一(070131)

    \author 张元一
    \date 2007-1-30
	
    \2013/3/8添加批量规则设置，以减少设置的时间
    \author 张专
*/

/** @defgroup ss 平滑发送
 *  提供报文平滑发送支持.
 *  @{
 */


#ifndef SSWRAP_H
#define SSWRAP_H

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * 错误码定义.
 */
enum ErrCode
{
    ENOSUPP,  ///< 内核不支持
    ENOROUTE, ///< 目地地址不可达
    ENORULE,  ///< 规则不存在
    EINTERNAL,///< 内部错误
    ETOOMANY, ///< 太多的规则
};

extern int SSErrno; ///< 平滑发送模块错误码,可通过此变量获取错误细节

/**
 * 添加/修改一条平滑发送规则,如果端口号为0,则不匹配端口号.\n
 * <b>注意</b>: 峰值速率必须大于或者等于平均速率,并且这个函数目前不是线程安全的
 *
 * \param dwDestIp     要进行限速的报文的目地IP,必须为主机字节序.
 * \param wDestPort    要进行限速的报文的目地端口,必须为主机字节序.
 * \param dwRate       报文的平均发送速率,单位为KBps
 * \param dwPeak       报文的峰值速率,单位为KBps.
 * \param dwSec        峰值速率持续时间
 * \return 如果成功返回1,失败则返回0 <br>
 *         可以通过SSErrno变量获取错误细节,可能的错误包括: <br>
 *           - <b>EINVAL</b>    无效的参数 <br>
 *           - <b>ENOROUTE</b>  目地地址不可达. <br>
 *           - <b>ENOSUPP</b>   内核不支持. <br>
 *           - <b>ETOOMANY</b>  太多的规则.<br>
 *           - <b>EINTERNAL</b> 内部错误.<br>
 */
s32 SetSSRule(u32 dwDestIp, u16 wDestPort, u32 dwRate, u32 dwPeak, u32 dwSec);

/**
 * 删除一条平滑发送规则,如果端口号为0,则不匹配端口号.<br>
 * <b>注意</b>: 这个函数目前不是线程安全的.
 *
 * \param dwDestIp   要删除的规则的目地IP.
 * \param wDestPort  要删除的规则的目地端口.
 * \return 如果成功返回1,失败则返回0 <br>
 *         可以通过SSErrno变量获取错误细节,可能的错误包括: <br>
 *           - <b>EINVAL</b>    无效的参数 <br>
 *           - <b>ENOROUTE</b>  目地地址不可达. <br>
 *           - <b>ENOSUPP</b>   内核不支持. <br>
 *           - <b>EINTERNAL</b> 内部错误.<br>
 */
s32 UnsetSSRule(u32 dwDestIp, u16 wDestPort);

/**
 * 添加/修改一条组播平滑发送规则,如果端口号为0,则不匹配端口号.\n
 * <b>注意</b>: 峰值速率必须大于或者等于平均速率,并且这个函数目前不是线程安全的
 *
 * \param dwSrcIp     发送组播报文的本地网口ip地址,必须为主机字节序.
 * \param wDestPort    要进行限速的报文的目地端口,必须为主机字节序.
 * \param dwRate       报文的平均发送速率,单位为KBps
 * \param dwPeak       报文的峰值速率,单位为KBps.
 * \param dwSec        峰值速率持续时间
 * \return 如果成功返回1,失败则返回0 <br>
 *         可以通过SSErrno变量获取错误细节,可能的错误包括: <br>
 *           - <b>EINVAL</b>    无效的参数 <br>
 *           - <b>ENOROUTE</b>  目地地址不可达. <br>
 *           - <b>ENOSUPP</b>   内核不支持. <br>
 *           - <b>ETOOMANY</b>  太多的规则.<br>
 *           - <b>EINTERNAL</b> 内部错误.<br>
 */
s32 SetMcSSRule(u32 dwSrcIp, u16 wDestPort, u32 dwRate, u32 dwPeak, u32 dwSec);

/**
 * 删除一条平滑发送规则,如果端口号为0,则不匹配端口号.<br>
 * <b>注意</b>: 这个函数目前不是线程安全的.
 *
 * \param dwDestIp   要删除的规则的源ip
 * \param wDestPort  要删除的规则的目地端口.
 * \return 如果成功返回1,失败则返回0 <br>
 *         可以通过SSErrno变量获取错误细节,可能的错误包括: <br>
 *           - <b>EINVAL</b>    无效的参数 <br>
 *           - <b>ENOROUTE</b>  目地地址不可达. <br>
 *           - <b>ENOSUPP</b>   内核不支持. <br>
 *           - <b>EINTERNAL</b> 内部错误.<br>
 */
s32 UnsetMcSSRule(u32 dwSrcIp, u16 wDestPort);


/**
 * 在所有接口上关闭平滑发送功能.<br>
 * <b>注意</b>: 这个函数目前不是线程安全的.
 *
 * \return 如果成功返回1,失败则返回0 <br>
 *         可以通过SSErrno变量获取错误细节,可能的错误包括: <br>
 *           - <b>ENOSUPP</b>   内核不支持. <br>
 *           - <b>EINTERNAL</b> 内部错误.<br> 
 */
s32 CloseSmoothSending();

/**
 * 显示当前所有平滑规则.<br>
 * <b>注意</b>: 这个函数目前不是线程安全的.
 *
 * \return 如果成功返回1,失败则返回0 <br>
 *         可以通过SSErrno变量获取错误细节,可能的错误包括: <br>
 *           - <b>ENOSUPP</b>   内核不支持. <br>
 *           - <b>EINTERNAL</b> 内部错误.<br> 
 */
s32 ShowSSRule();


//2013/3/08新增加的规则批量添加接口
/************************************************/
/*
**接口：s32 SsInt(void);
**返回值：函数执行成功返回1，失败返回0。
**执行条件：在设置平滑发送规则最开始调用。
**作用：使得调用该函数的进程及其子进程的pio值-20（pio的值越小，优先级别越高），缓存文件初始化。
**Add by：张专 ---------2013/3/8------------
*/
s32 SsInit(void);

/*
**接口：s32 BatchAddSSRule(u32 dwDestIp, u16 wDestPort, u32 dwRate, u32 dwPeak, u32 dwSec);
**返回值：函数执行成功返回1，失败返回0。
**作用：向缓存中添加一条规则，规则不生效。
**Add by：张专 ---------2013/3/8------------
*/
s32 BatchAddSSRule(u32 dwDestIp, u16 wDestPort, u32 dwRate, u32 dwPeak, u32 dwSec);

/*
**接口：s32 BatchExecSSRule(void);
**返回值：函数执行成功返回1，失败返回0。
**作用：生效缓存中批量添加的规则。
**Add by：张专 ---------2013/3/8------------
*/
s32 BatchExecSSRule(void);

/*
**接口：s32 BatchAddUnsetSSRule(u32 dwDestIp, u16 wDestPort);
**返回值：函数执行成功返回1，失败返回0。
**作用：向缓存中添加一条unset规则，规则不生效。
**Add by：张专 ---------2013/3/8------------
*/
s32 BatchAddUnsetSSRule(u32 dwDestIp, u16 wDestPort);

/*
**接口：s32 BatchExecUnsetSSRule(void);
**返回值：函数执行成功返回1，失败返回0。
**作用：生效缓存中批量添加的unset规则。
**Add by：张专 ---------2013/3/8------------
*/
s32 BatchExecUnsetSSRule(void);

/*
**接口：s32 BatchAddMcRule(u32 dwDestIp, u16 wDestPort, u32 dwRate, u32 dwPeak, u32 dwSec);
**返回值：函数执行成功返回1，失败返回0。
**作用：向缓存中添加一条组播平滑发送规则，规则不生效。
**Add by：张专 ---------2013/3/8------------
*/
s32 BatchAddMcRule(u32 dwDestIp, u16 wDestPort, u32 dwRate, u32 dwPeak, u32 dwSec);

/*
**接口：s32 BatchExecMcRule(void);
**返回值：函数执行成功返回1，失败返回0。
**作用:生效缓存中批量添加的组播平滑发送规则。
**Add by：张专 ---------2013/3/8------------
*/
s32 BatchExecMcRule(void);



#ifdef __cplusplus
}
#endif

/* @} */

#endif /* SS_H */

