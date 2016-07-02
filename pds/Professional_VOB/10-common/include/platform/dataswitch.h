/*****************************************************************************
   模块名      : DataSwitch
   文件名      : dataswitch.h
   相关文件    : 
   文件实现功能: DataSwitch模块接口函数定义
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/03/18  4.0         靳志业        创建
   2005/08/10  4.0         靳志业        实现3.6中新增的功能
******************************************************************************/

#ifndef DATASWITCH_H
#define DATASWITCH_H

#define DSVERSION       "Dataswitch 40.20.01.01.061214"

#include "osp.h"

/* DataSwitch 句柄 */
#define DSID    u32

/*DataSwitch 返回值*/
#define DSOK    1
#define DSERROR 0

#define INVALID_DSID  0xffffffff   /*无效的句柄值*/

/**
 * @func FilterFunc
 * @brief 接收过滤函数
 *
 * 每个接收结点有一个过滤函数，当此结点接收到UDP数据包时执行此函数。
 * 并根据函数的返回值，来动态地决定是否对此数据包转发。
 *
 * @param dwRecvIP        - 接收IP
 * @param wRecvPort      - 接收Port
 * @param dwSrcIP         - 源IP
 * @param SrcPort       - 源Port
 * @param pData          - [in, out]数据包；可以修改；
 * @param uLen           - [in, out]数据包长度；修改后的长度不可超过最大值
 * @return 0, 该数据包有效； 其他值，该数据包无效；
 */	
typedef u32 (*FilterFunc)(u32 dwRecvIP, u16 wRecvPort, u32 dwSrcIP, u16 wSrcPort, 
  u8* pData, u32 uLen);


#define SENDMEM_MAX_MODLEN        (u8)32        // 发送时允许的最大修改长度
    
/**
 * 数据包转发目标：指明具体的IP, port对
 * DS所控制的多个IP，对于数据包的转发应该不产生影响。也就是说，如果
 * 有两个IP都可以将一个数据包正确的地转发出去，则使用哪一个IP是没有
 * 关系的。所以，转发输出接口IP是不需要的。
 */
typedef struct tagNetSndMember
{
    u32   dwIP;        /* 转发目的IP地址   */
    u16  wPort;       /* 转发目的端口号   */
    long            lIdx;        /* 接口索引 */
    u32   errNum;      /* 出错次数*/
    long            errNo;       /* 错误号 */
    void *          lpuser;      /* user info */
    /* 为实现发送时修改数据而增加的部分 */
    u16  wOffset;        /* 修改部分的起始位置; 目前未使用 */
    u16  wLen;		/* 修改部分的长度，必须为4的倍数；为零表示不修改 */
    char            pNewData[SENDMEM_MAX_MODLEN];
    /* 要修改的数据 */
    void *          pAppData;       /* 用户自定义的回调函数的参数 */

}TNetSndMember;


typedef void (*SendCallback)(u32 dwRecvIP, u16 wRecvPort,
                             u32 dwSrcIP,  u16 wSrcPort,
                             TNetSndMember *ptSends,            // 转发目标列表
                             u16* pwSendNum,                    // 转发目标个数 
                             u8* pUdpData, u32 dwUdpLen);


/************************************************************
 * 函数名：dsCreate
 *
 * 功能：  运行DataSwitch模块
 *
 * 实现说明： 
 * Dataswitch支持“多用户”；每次调用dsCreate就会返回一个
 * 用户标识。添加、删除时均要匹配用户标志，转发时则不考虑。
 *
 * 返回值： 
 * @return INVALID_DSID: 表示失败 ; 否则成功
 ************************************************************/
API DSID dsCreate( u32, u32 * );

/************************************************************
 * 函数名：dsDestroy
 *
 * 功能： 终止DataSwitch的运行
 *
 * 实现说明：
 * 一次删除一个用户，直到所有用户都删除，所有资源才全释放。
 *
 * 参数说明：
 * @param dsId  - 用户标识
 * 
 * 返回值：
 ************************************************************/
API void dsDestroy( DSID dsId );
 
/************************************************************
 * 函数名： dsAdd
 *
 * 功能： 增加转发目标唯一的转发规则
 *
 * 实现说明：
 * “目前唯一”的含义是：如果已经存在的交换规则中转发目标
 * 与当前规则的相同，则先删除这些规则。
 *
 * 参数说明：
 * @param dsId          - 用户标识
 * @param dwRecvIP      - 接收IP，即接收数据包的目的IP
 * @param wRecvPort     - 接收Port，即接收数据包的目的Port
 * @param dwInLocalIP   - 接收数据包的网络接口
 * @param dwSendtoIP    - 转发目的IP
 * @param wSendtoPort   - 转发目的Port
 * @param dwOutLocalIP  - 转发数据包所用本地IP
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsAdd(DSID dsId,
              u32  dwRecvIP ,
              u16  wRecvPort,
              u32  dwInLocalIP,
              u32  dwSendtoIP,
              u16  wSendtoPort,
              u32  dwOutLocalIP = 0);

/************************************************************
 * 函数名：dsRemove
 *
 * 功能： 删除转发目标为指定地址的转发规则
 * 
 * 实现说明：
 *
 * 参数说明：
 * @param dsId          - 用户标识
 * @param dwSendtoIP    - 转发目的IP
 * @param wSendtoPort   - 转发目的Port
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsRemove(DSID dsId,
                 u32  dwSendtoIP,
                 u16  wSendtoPort);

/************************************************************
 * 函数名：dsAddDump
 *
 * 功能： 增加Dump规则
 * DUMP规则：从指定地址接收的数据包不被转发。
 * 如果一个接收地址只有DUMP规则，则接收的数据包不被转发；
 * 如果还有其他转发规则，则按照其他规则转发。
 *
 * 实现说明：
 * DUMP规则允许被创建多次，因为它与其他规则是可以并存的，
 * 包括它自身；
 *
 * 参数说明：
 * @param dsId          - 用户标识
 * @param dwRecvIP      - 接收IP，即接收数据包的目的IP
 * @param wRecvPort     - 接收Port，即接收数据包的目的Port
 * @param dwInLocalIP   - 接收数据包的网络接口
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsAddDump(DSID dsId, u32 dwRecvIP, u16 wRecvPort, u32 dwInLocalIP);

/************************************************************
 * 函数名：dsRemoveDump
 *
 * 功能： 删除Dump规则
 *
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId          - 用户标识
 * @param dwRecvIP      - 接收IP，即接收数据包的目的IP
 * @param wRecvPort     - 接收Port，即接收数据包的目的Port
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsRemoveDump(DSID dsId, u32 dwRecvIP, u16 wRecvPort);

/************************************************************
 * 函数名：dsAddManyToOne
 * 
 * 功能： 增加多对一的转发规则
 * 注意，它与dsAdd的语义正好相反，两者混合使用时要小心。
 * 
 * 实现说明：
 *
 * 参数说明：
 * @param dsId          - 用户标识
 * @param dwRecvIP      - 接收IP，即接收数据包的目的IP
 * @param wRecvPort     - 接收Port，即接收数据包的目的Port
 * @param dwInLocalIP   - 接收数据包的网络接口
 * @param dwSendtoIP    - 转发目的IP
 * @param wSendtoPort   - 转发目的Port
 * @param dwOutLocalIP  - 转发数据包所用本地IP
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsAddManyToOne(DSID dsId ,
                       u32  dwRecvIP,
                       u16  wRecvPort,
                       u32  dwInLocalIP,
                       u32  dwSendtoIP,
                       u16  wSendtoPort,
                       u32  dwOutLocalIP = 0);

/************************************************************
 * 函数名：dsRemoveAllManyToOne
 * 
 * 功能： 删除所有转发目标为指定地址的多对一规则
 * 
 * 注意：它的语义与dsRemove完全相同（可以从接口参数上证明）
 * 提供这个接口是为了不让两类不同语义的接口混合使用。一个
 * 程序中，应当始终使用某一类接口。
 *
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId          - 用户标识
 * @param dwSendtoIP    - 转发目的IP
 * @param wSendtoPort   - 转发目的Port
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsRemoveAllManyToOne(DSID dsId ,
                             u32  dwSendtoIP,
                             u16  wSendtoPort);

/************************************************************
 * 函数名：dsRemoveManyToOne
 *
 * 功能： 删除指定的多对一规则
 *
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId          - 用户标识
 * @param dwRecvIP      - 接收IP，即接收数据包的目的IP
 * @param wRecvPort     - 接收Port，即接收数据包的目的Port
 * @param dwInLocalIP   - 接收数据包的网络接口
 * @param dwSendtoIP    - 转发目的IP
 * @param wSendtoPort   - 转发目的Port
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsRemoveManyToOne(DSID dsId ,
                          u32  dwRecvIP,
                          u16  wRecvPort,
                          u32  dwSendtoIP,
                          u16  wSendtoPort );

/************************************************************
 * 函数名：dsAddSrcSwitch
 *
 * 功能： 增加按源转发的规则
 * 
 * 按源转发：使用此规则时，接收数据包的源地址必须与规则中的源
 * 地址相等。每次转发时，都要先根据数据包中的源地址查找转发规
 * 则，如果找不到，则使用默认源地址（即0@0）的转发规则。
 * 注意：该接口支持多点到一点。如果dwSrcIP和wSrcPort都为零，
 * 它就完全等同于dsAddManyToOne。
 *
 * 实现说明：
 *
 * 参数说明：
 * @param dsId          - 用户标识
 * @param dwRecvIP      - 接收IP，即接收数据包的目的IP
 * @param wRecvPort     - 接收Port，即接收数据包的目的Port
 * @param dwInLocalIP   - 接收数据包的网络接口
 * @param dwSrcIP       - 接收数据包的源IP
 * @param wSrcPort      - 接收数据包的源Port
 * @param dwSendtoIP    - 转发目的IP
 * @param wSendtoPort   - 转发目的Port
 * @param dwOutLocalIP  - 转发数据包所用本地IP
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/      
API u32 dsAddSrcSwitch(DSID dsId ,
                       u32  dwRecvIP,
                       u16  wRecvPort,
                       u32  dwInLocalIP,
                       u32  dwSrcIP ,
                       u16  wSrcPort,
                       u32  dwSendtoIP,
                       u16  wSendtoPort,
                       u32  dwOutLocalIP = 0);

/************************************************************
 * 函数名：dsRemoveAllSrcSwitch
 *
 * 功能： 删除所有指定的按源转发规则
 *
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId          - 用户标识
 * @param dwRecvIP      - 接收IP，即接收数据包的目的IP
 * @param wRecvPort     - 接收Port，即接收数据包的目的Port
 * @param dwSrcIP       - 接收数据包的源IP
 * @param wSrcPort      - 接收数据包的源Port
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/      
API u32 dsRemoveAllSrcSwitch(DSID dsId,
                             u32  dwRecvIP,
                             u16  wRecvPort,
                             u32  dwSrcIP,
                             u16  wSrcPort);

/************************************************************
 * 函数名：dsRemoveSrcSwitch
 * 
 * 功能： 删除指定的按源转发的规则
 *
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId          - 用户标识
 * @param dwRecvIP      - 接收IP，即接收数据包的目的IP
 * @param wRecvPort     - 接收Port，即接收数据包的目的Port
 * @param dwSrcIP       - 接收数据包的源IP
 * @param wSrcPort      - 接收数据包的源Port
 * @param dwSendtoIP    - 转发目的IP
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/      
API u32 dsRemoveSrcSwitch(DSID dsId,
                          u32  dwRecvIP,
                          u16  wRecvPort,
                          u32  dwSrcIP,
                          u16  wSrcPort,
                          u32  dwSendtoIP,
                          u16  wSendtoPort);

/************************************************************
 * 函数名：dsSetFilterFunc
 * 
 * 功能： 设置过滤函数
 *
 * 注意：函数指针可以为空，表示清除原有的设置；另外，由于
 * DataSwitch是在另外的线程中调用此函数的，要设置的函数必
 * 须是全局函数；设置的参数，也必须是全局有效的。
 *
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId          - 用户标识
 * @param dwRecvIP      - 接收IP，即接收数据包的目的IP
 * @param wRecvPort     - 接收Port，即接收数据包的目的Port
 * @param ptFunc        - 过滤函数指针
 * @param FuncParam     - 为过滤函数传递的参数
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsSetFilterFunc(DSID dsId,
			u32 dwRecvIP, 
                        u16 wRecvPort, 
                        FilterFunc ptFunc);
 
/************************************************************
 * 函数名：dsRemoveAll
 * 
 * 功能： 删除所有的转发规则
 * 
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId          - 用户标识
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/      
API u32 dsRemoveAll( DSID dsId );

/**********************************************************
 * 函数: dsSpecifyFwdSrc                                  *
 * 功能: 为指定接收地址设置转发数据包所填充的源地址       *
 * 输入:
 * @param dsId          - DSID
 * @param OrigIP        - 原始IP
 * @param OrigPort      - 原始Port
 * @param MappedIP      - 映射IP
 * @param MappedPort    - 映射Port
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/
API u32 dsSpecifyFwdSrc(DSID dsId, u32 OrigIP, u16 OrigPort, u32 MappedIP, u16 MappedPort);

/**********************************************************
 * 函数: dsResetFwdSrc                                    *
 * 功能: 恢复指定地址转发数据包的源地址
 * 输入:
 * @param dsId          - DSID
 * @param OrigIP        - 原始IP
 * @param OrigPort      - 原始Port
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/
API u32 dsResetFwdSrc(DSID dsId, u32 OrigIP, u16 OrigPort);

 /**********************************************************
 * 函数: dsSetSendCallback                                *
 * 功能: 设置发送回调函数                                 *
 *       取消时，设置函数指针为NULL即可
 * 输入: dsID           - 创建交换模块时的句柄            *
 *       dwLocalIp      - 本地接收IP                      *
 *       wLocalPort     - 本地接收端口号                  *
 *       dwSrcIP        - 转发目的IP地址                  *
 *       wSrcPort       - 转发目的端口号                  *
 *       pfCallback     - 回调函数
 * 输出: 无                                               *
 * 返回: 无                                               *
 **********************************************************/    
API u32 dsSetSendCallback( DSID dsId, 
                u32 dwRecvIP, 
                u16 wRecvPort,
                u32 dwSrcIP,
                u16 wSrcPort,
                SendCallback pfCallback);

 /**********************************************************
 * 函数: dsSetAppDataForSend                              *
 * 功能: 为发送目标设置一个自定义的指针                   *
 *       取消时，设置函数指针为NULL即可
 * 输入: dsID           - 创建交换模块时的句柄            *
 *       dwLocalIp      - 本地接收IP                      *
 *       wLocalPort     - 本地接收端口号                  *
 *       dwSrcIP        - 源IP地址                  *
 *       wSrcPort       - 源端口号                  *
 *       dwDstIP        - 转发目的IP地址                  *
 *       wDstPort       - 转发目的端口号                  *
 *       pAppData       - 自定义指针
 * 输出: 无                                               *
 * 返回: 
 *     DSOK:成功 DSERROR:失败                             *
 **********************************************************/    
API u32 dsSetAppDataForSend( DSID dsId, 
                u32 dwRecvIP, 
                u16 wRecvPort,
                u32 dwSrcIP,
                u16 wSrcPort,
                u32 dwDstIP,
                u16 wDstPort,
                void * pAppData);
                
/**********************************************************
 * 函数: dsGetRecvPktCount                                *
 * 功能: 查询接收总包数                                   *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwRecvIP   - 本地接收IP                         *
 *       wRecvPort  - 本地接收端口号                     *
 *       dwSrcIP     - 源IP                               *
 *       wSrcPort    - 源端口号                           *
 *       dwRecvPktCount  - 接收总包数                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/    
API u32 dsGetRecvPktCount( DSID dsId , u32 dwRecvIP , u16 wRecvPort ,
						  u32 dwSrcIP , u16 wSrcPort , u32& dwRecvPktCount );

/**********************************************************
 * 函数: dsGetSendPktCount                                *
 * 功能: 查询发送总包数                                   *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwRecvIP   - 本地接收IP                         *
 *       wRecvPort  - 本地接收端口号                     *
 *       dwSrcIP     - 源IP                               *
 *       wSrcPort    - 源端口号                           *
 *       dwSendtoIP     - 转发目的IP地址                     *
 *       wSendtoPort    - 转发目的端口号                     *
 *       dwSendPktCount  - 发送总包数                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/    
API u32 dsGetSendPktCount( DSID dsId , u32 dwRecvIP , u16 wRecvPort ,
						  u32 dwSrcIP , u16 wSrcPort ,
						  u32 dwSendtoIP, u16 wSendtoPort, u32& dwSendPktCount );
						  
/**********************************************************
 * 函数: dsGetRecvBytesCount	                          *
 * 功能: 查询接收总字节数                                 *
 * 输入:												  *
 * 输出: 无                                               *
 * 返回: 返回即时接收字节数								  *
 **********************************************************/
API s64 dsGetRecvBytesCount( );

/**********************************************************
 * 函数: dsGetSendBytesCount	                          *
 * 功能: 查询发送总字节数                                 *
 * 输入:												  *
 * 输出: 无                                               *
 * 返回: 返回即时字节数									  *
 **********************************************************/
API s64 dsGetSendBytesCount( );

/************************************************************
 * 函数： dsinfo
 * 功能： 显示所有的转发规则，及所有正在监听的端口 
 * 输入：
 * 输出：
 * 返回：
 ************************************************************/      
API void dsinfo();

/************************************************************
 * 函数： dsver
 * 功能： 显示Dataswitch的版本信息
 * 输入：
 * 输出：
 * 返回：
 ************************************************************/      
API void dsver();

/************************************************************
 * 函数： dshelp
 * 功能： 显示Dataswitch所提供的命令的帮助信息
 * 输入：
 * 输出：
 * 返回：
 ************************************************************/      
API void dshelp();

/*显示定时器*/
API void dsshowrtcptimer();


/************************************************************
 * 函数： dedebug
 * 功能： 打开/关闭调试信息
 * 
 * 输入：
 * @param op            - 指明是设置还是清除：set, clear
 *                        (仅在WIN32有效)
 * 输出：
 * 返回：
 ************************************************************/      
API void dsdebug(const char* op = NULL);

/************************************************************
 * 函数： dedebug
 * 功能： 打开/关闭更高一级调试信息
 * 单独设置此函数是因为调试信息会影响整体性能 
 * 
 * 输入：
 * @param op            - 指明是设置还是清除：set, clear
 *                        (仅在WIN32有效)
 * 输出：
 * 返回：
 ************************************************************/      
API void dsdebug2();

 /**********************************************************
 * 函数: dsSpecifyDstFwdSrc                               *
 * 功能: 为指定目的地址设置转发数据包所填充的源地址       *
 * 输入:
 * @param dsId          - DSID
 * @param dwLocalIP     - 本地IP
 * @param wLocalPort    - 本地Port
 * @param dwDstIP		- 目的IP
 * @param wDstPort		- 目的端口
 * @param dwMappedIP    - 映射IP
 * @param wMappedPort   - 映射Port
 * @param dwSrcIP		- 源IP
 * @param wSrcPort		- 源端口
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/
API u32	dsSpecifyDstFwdSrc(DSID dsId, u32 dwLocalIP, u16 wLocalPort, 
						   u32 dwDstIP , u16 wDstPort ,
						   u32 dwMappedIP, u16 wMappedPort,
						   u32 dwSrcIP = 0 , u16 wSrcPort = 0 );

 /**********************************************************
 * 函数: dsStartRtcpRR                               *
 * 功能: 设置发送RR报告包,必须存在本地节点,且未设置过发送RR包  *
 * 输入:
 * @param dsId          - DSID
 * @param dwLocalIP     - 本地IP
 * @param wLocalPort    - 本地Port
 * @param dwDstIP		- 目的IP
 * @param wDstPort		- 目的端口
 * @param dwInterval	- 间隔时间(单位:秒)
 * @param dwRtcpIP		- 接受rtcp的IP(默认则RR包信息将没有LSR DLSR信息)
 * @param wRtcpPort		- 接受rtcp的Port
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/
API u32 dsStartRtcpRR(DSID dsId ,u32 dwLocalIP , u16 wLocalPort , u32 dwDstIP , u16 wDstPort , u32 dwInterval , u32 dwRtcpIP = 0, u16 wRtcpPort = 0);


 /**********************************************************
 * 函数: dsStartRtcpRR                               *
 * 功能: 取消设置发送RR报告包，必须存在本地节点,且设置过发送RR包  *
 * 输入:
 * @param dsId          - DSID
 * @param dwLocalIP     - 本地IP
 * @param wLocalPort    - 本地Port
 * @param dwRtcpIP		- 接受rtcp的IP(默认则RR包信息将没有LSR DLSR信息)
 * @param wRtcpPort		- 接受rtcp的Port
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/
API u32 dsStopRtcpRR(DSID dsId , u32 dwLocalIP , u16 wLocalPort , u32 dwRtcpIP = 0, u16 wRtcpPort = 0);

#endif
