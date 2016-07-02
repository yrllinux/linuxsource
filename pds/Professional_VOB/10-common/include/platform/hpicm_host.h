/* creator: liujie data:30Oct2007
   this head file define the routine, struct need by outsize user mode application
 */
/******************************************************************************
模块名  ： hpi
文件名  ： hpicm_host.h (PPC侧)
相关文件： hpicm.h
文件实现功能：HPI驱动实现PPC与DM6437的通讯
作者    ：刘捷
版本    ：1.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
1/1/2008  1.0      刘捷      创建
5/21/2008  1.1    郭凯杰 修改
6/3/2008 1.2       郭凯杰修改
******************************************************************************/

/****************************** 模块的版本号命名规定 *************************
总的结构：mn.mm.ii.cc.tttt
     如  Osp 1.1.7.20040318 表示
模块名称Osp
模块1版本
接口1版本
实现7版本
04年3月18号提交

版本修改记录：
----------------------------------------------------------------------------
模块版本：hpicm 1.2.1.20080521
增加功能：增加PPC端调试接口信息
			
修改缺陷：6437侧内存泄露
                           6437侧CACHE缺陷
提交人：郭凯杰
----------------------------------------------------------------------------
模块版本：hpicm 1.3.1.20080603
增加功能：读取PPC HPI LIB版本接口API, 增加RAW READ读写方式以供调试
			      增加HPI WAIT功能
修改缺陷：关闭通道时候会内存泄露，没有对DEBUG通道释放
提交人：郭凯杰
----------------------------------------------------------------------------
模块版本：hpicm 1.4.1.20080714
增加功能：增加6437侧获取通道缓存包数接口
修改缺陷：
提交人：张方明
----------------------------------------------------------------------------
模块版本：hpicm 1.4.2.20080808
增加功能：增加Host侧支持设置dsp主频和DDR主频的Open接口；
          修改默认DDR频率由135改为162MHz，可以解决编码引起视频采集播放DMA挂死问题。
修改缺陷：
提交人：张方明
----------------------------------------------------------------------------
模块版本：hpicm 1.4.3.20080926
增加功能：为dsp侧管道req/fin加入校验，信号量保护
修改缺陷：
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：hpicm 1.4.4.20081117
增加功能：无
修改缺陷：修正了由于接口重复调用出错没有把读请求长度清0的问题
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：hpicm 1.4.5.20081211
增加功能：无
修改缺陷：对ppc侧的写请求加入了长度校验
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：hpicm 1.4.6.20081218
增加功能：无
修改缺陷：162MHz内存的相关寄存器配置修改
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：hpicm 1.4.7.20090110
增加功能：无
修改缺陷：使用No cache的内存来分配dsp侧hpi驱动所有变量
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：hpicm 1.4.8.20090603
增加功能：无
修改缺陷：发包后交验长度，若长度不对则丢包重传
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：hpicm 1.4.9.20091222
增加功能：无
修改缺陷：dm6437 DDR162参数配置
提交人：赫诚杰

****************************************************************************/

#ifndef __INC_KDC_HPICOMMUNICATION_MODULE_H__
#define __INC_KDC_HPICOMMUNICATION_MODULE_H__

#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */


#define HPI_LIB_VERSION "hpicm 1.4.9.20091222"

/* 
   the user mode call-back routine example
   int cb_nexchan_example(int chanid, u32 cap, u32 type, void * data)
   {
   .....
   }
   chan id:the new channle's id
   u32 cap:channel capability
   u32 type:the type of data transfered in this channel
   void * data: the pointer(void * cbdata) set in "HPICM_Handle->set_newchan_cb"
 */

/*stats structures*/

typedef enum
{
	STAT_READ = 0,
	STAT_WRITE 
}TRANS_MODE_T;


/**********************************************************************
 *               HPI communication modules routine                    *
 *********************************************************************/
s32 hpicm_open(s32 devid, s8 * imagename, void ** ppHPIHandle);

/* dwDspFreq = 513/594(def)/702; dwDDRFreq = 189/135/162(def) */
s32 hpicm_open_ext(s32 devid, u32 dwDspFreq, u32 dwDDRFreq, s8 * imagename, void ** ppHPIHandle);

s32 hpicm_close(void * pHPIHandle);
s32 hpicm_read_log(void * pHPIHandle, s8 * buf, ssize_t bufsz);

s32 hpicm_read_klog(void * pHPIHandle, s8 * buf, ssize_t bufsz);

s32 hpicm_blockwrite(void * pHPIHandle, s8 * buf, ssize_t bufsz);

/**********************************************************************
 *                     HPI channel open/close routine                 *
 *********************************************************************/
/* open a channel, whose attr is set in hpi_attr attr */
s32 hpicm_create_chan(void * pHPIHandle, u8 chanid, hpi_attr attr, void ** ppChanHandle);

s32 hpicm_close_chan(void * pChanHandle);

/**********************************************************************
 *                     HPI channel routine                            *
 *********************************************************************/
s32 hpicm_chan_write(void * pChanHandle, s8 * buf, s32 len);
s32 hpicm_chan_write_dual(void * pChanHandle, s8 * pHeadBuf, s32 nHeadLen, s8 * pBuf, s32 nLen);

s32 hpicm_chan_read(void * pChanHandle, s8 * buf, s32 bufsz);

s32 hpicm_chan_clear(void * pChanHandle);


s32 hpicm_call_dsp_func(void * pHPIHandle, u8 id, u32 arg);

s32 hpcim_temp_debug(void * pHPIHandle, u32 idx, u32 *val);

s32 hpcim_temp_debug_write(void * pHPIHandle, u32 idx, u32 val);

s32 hpi_raw_write(void * pHPIHandle, u32 addr, u8 *pBuff, u32 size);

s32 hpi_raw_read(void *pHPIHandle, s32 addr, u8 *pBuff, u32 size);

s32 hpicm_get_version(s8 *buff, s32 len);

s32 hpicm_get_stats(void * pChanHandle, s32 mode, u32 *byte_count, u32 *success_times, u32 *error_times);


/**********************************************************************
 *                     HPI return info routine                        *
 *********************************************************************/
s8 * hpicm_errinfo(s32 ret);
void hpicm_perror(s32 ret);


#ifdef __cplusplus
}
#endif   /* __cplusplus */

#endif
