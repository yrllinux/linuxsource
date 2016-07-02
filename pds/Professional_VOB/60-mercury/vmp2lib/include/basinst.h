/*****************************************************************************
   模块名      : BAS-HD
   文件名      : basinst.h
   创建时间    : 2003年 6月 25日
   实现功能    : 适配实例头文件
   作者        : 张明义
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/03/21  4.0         张宝卿      代码优化
******************************************************************************/
#ifndef _BASINST_H_
#define _BASINST_H_

#include "osp.h"
#include "mcuconst.h"
#include "basstruct.h"
#include "mcustruct.h"

#include "eqpcfg.h"

#include "kdvmedianet.h"
#include "kdvadapter_hd.h"
#ifdef _LINUX_
#include "libsswrap.h"
#endif


#define STATUS_CIF				(u8)1
#define STATUS_CIF_ADJ2HD		(u8)2
#define STATUS_HD				(u8)3
#define STATUS_HD_ADJ2CIF		(u8)4


#define MAXNUM_RCV              (u8)4   //四路接收

//#define MAXNUM_BASHD_CHANNEL    (u8)2
#define MAXNUM_BASHD_CHANNEL    (u8)1   //一个实例

#define MAX_VIDEO_FRAME_SIZE (512 * 1024)
#define QC_MODE_QUALITY      ((u8)0x1)
#define FRAME_RATE           (25)
#define PORT_SNDBIND         20000

const u16 EnVidFmt[6][12]=
{
    //类型               压缩码率控制参数 最大关键帧 最大量化 最小量化 编码比特率K 图像制式 采集端口 帧率       图像质量         视频高度 视频宽度
    //mpeg4
    MEDIA_TYPE_MP4,           0,            75,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //261
    MEDIA_TYPE_H261,          0,            75,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //262(mpeg-2)
    MEDIA_TYPE_H262,          0,            75,        31,      2,       2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //263
    MEDIA_TYPE_H263,          1,            75,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //h263+
    MEDIA_TYPE_H263PLUS,      0,            300,       31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //h264
    MEDIA_TYPE_H264,          0,            300,       51,      10,      2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720
};

//高清适配
class CHdAdpGroup
{
private:
	BOOL32           m_bIsVga;
	BOOL32           m_bIsStart;
	u32              m_dwNetBand;
	u32              m_dwRtcpBackAddr;   //rtcp回发地址
	u16              m_wRtcpBackPort;    //rtcp回发端口
    u16              m_wLocalStartPort;  //为本地接收的rtp端口，rtcp端口为rtp端口+1
    u32              m_dwDestAddr;       //发送的目的地址
	u16              m_wDestPort;        //第一路发送目的端口，第二路发送端口为第一路发送目的端口 + 10
    
	
    TVideoEncParam   m_atVidEncParam[2];            //编码参数
	TMediaEncrypt    m_atMediaDec[MAXNUM_RCV];      //解密参数
	TMediaEncrypt    m_atMediaEnc[2];               //加密参数
	TDoublePayload   m_atDbPayload[MAXNUM_RCV];     //载荷
	BOOL32           m_bNeedRS[MAXNUM_RCV];         //是否需要重传
	TRSParam         m_tRsParam[MAXNUM_RCV];        //重传参数
	u16              m_awBufTime[2];                //发送缓存时间
    

	//FIXME：临时放开，稍后提供接口
public:
	CKdvMediaRcv    *m_pcMediaRcv[MAXNUM_RCV];                 //四路接受
	CKdvMediaSnd     m_cMediaSnd[2];                //最多两路发送
	CKdvAdapterGroup m_cAptGrp;                     //一组适配


public:
	CHdAdpGroup(BOOL32 bIsVgaGroup = FALSE):m_bIsVga(bIsVgaGroup),
                                            m_dwNetBand(0),
                                            m_bIsStart(FALSE)
	{
	}
	~CHdAdpGroup()
	{
		StopAdapter();
		m_cAptGrp.DestroyGroup();
	}

	BOOL IsVga(void)
	{
		return m_bIsVga;
	}

	BOOL IsStart(void)
	{
		return m_bIsStart;
	}

	BOOL Create(u32                   dwMaxRcvFrameSize, 
				u16                   wRtcpBackPort,    //rtcp回发端口
			    u16                   wLocalStartPort,  //rtp端口，rtcp端口为rtp端口+1
			    u32                   dwDestIp,         //发送目的Ip地址，
	            u16                   dwDestPort,
                u8                    byFpgaErrTimes,
				u32				      dwContext);      //第一路发送目的端口，第二路发送端口为第一路发送目的端口 + 10
 

	//退出组
	BOOL DestroyGroup(void)
	{
		m_cAptGrp.DestroyGroup();
		return TRUE;
	}

	//开始适配
	BOOL StartAdapter(BOOL32 bStartMediaRcv = FALSE);

	//停止适配
	BOOL StopAdapter(BOOL32 bStopMediaRcv = TRUE);

	//设置视频编码参数
    BOOL SetVideoEncParam(TVideoEncParam * ptVidEncParam, BOOL32 bSetNetParam = TRUE);

    //改变视频适配解码参数
    BOOL ChangeVideoDecParam(TVideoDecParam * ptVidDecParam);

	//获取视频编码参数
    BOOL GetVideoEncParam(TVideoEncParam& tVidEncParam);

	//视频适配编码参数
	BOOL GetSecVideoEncParam(TVideoEncParam& tVidEncParam);
	
	/*设置图像的网络接收重传参数*/
	BOOL  SetNetRecvFeedbackVideoParam(TRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);

	/*设置图像的网络发送重传参数*/
	BOOL  SetNetSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);

	/*设置第二路适配图像的网络发送重传参数*/
	BOOL  SetNetSecSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);

	//设置视频编码加密key字串以及 加密模式 Aes or Des
    BOOL SetVidEncryptKey(const TMediaEncrypt& tMediaEnc)
	{
		u8  aEncKey[MAXLEN_KEY];
		s32 iLen = 0;
		const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptKey(aEncKey, &iLen);
		m_cMediaSnd[0].SetEncryptKey((s8*)aEncKey, iLen, GetMediaEncMode(const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode()));
		return TRUE;
	}

    //设置第二路视频编码加密key字串以及 加密模式 Aes or DesBOOL SetSecVidEncryptKey(const TMediaEncrypt& tMediaEnc)
    BOOL SetSecVidEncryptKey(const TMediaEncrypt& tMediaEnc)    
	{
		u8  aEncKey[MAXLEN_KEY];
		s32 iLen = 0;
		const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptKey(aEncKey, &iLen);
		m_cMediaSnd[1].SetEncryptKey((s8*)aEncKey, iLen, GetMediaEncMode(const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode()));
		return TRUE;
	}
	
	//设置 H.263+/H.264 等动态视频载荷的 Playload值
    BOOL SetVideoActivePT(u8 byIdx, u8 byRmtActivePT, u8 byRealPT )
	{
        m_pcMediaRcv[byIdx]->SetActivePT(byRmtActivePT, byRealPT);
		return TRUE;
	}

	//设置 视频解码key字串 以及 解密模式 Aes or Des
    BOOL SetVidDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
        for(u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx++)
        {
		    m_atMediaDec[byIdx].SetEncryptKey((u8*)pszKeyBuf, wKeySize);
		    m_atMediaDec[byIdx].SetEncryptMode(byDecryptMode);
		    m_pcMediaRcv[byIdx]->SetDecryptKey(pszKeyBuf, wKeySize, byDecryptMode);
        }
		return TRUE;
	}

    BOOL SetVidDecryptKey(const TMediaEncrypt & tMediaEnc)
    {
        u8  aEncKey[MAXLEN_KEY];
        s32 iLen = 0;
        const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptKey(aEncKey, &iLen);

        for(u8 byIdx = 0; byIdx < MAXNUM_RCV; byIdx++)
        {
            m_atMediaDec[byIdx].SetEncryptKey((u8*)aEncKey, iLen);
            m_atMediaDec[byIdx].SetEncryptMode(GetMediaEncMode(const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode()));
            m_pcMediaRcv[byIdx]->SetDecryptKey((s8*)aEncKey, iLen, GetMediaEncMode(const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode()));
        }
		return TRUE;
    }

	//请求关键帧
	BOOL SetFastUpdata(void);

    // 平滑发送支持
	void SetSmoothSendRule(void);
	void ClearSmoothSendRule(void);
    void StopSmoothSend();	

	//得到组的状态
    BOOL GetGroupStatus(TAdapterGroupStatus &tAdapterGroupStatus);

	//得到视频通道的统计信息
	BOOL GetVideoChannelStatis(u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis);

	//显示通道信息,将两路编码都打印出来
	void ShowChnInfo(void);

    void UpdateNetParam(u16 wRtcpBackPort,    //rtcp回发端口
                        u16 wLocalStartPort,  //rtp端口，rtcp端口为rtp端口+1
                        u32 dwDestIp,         //发送目的Ip地址，
                        u16 dwDestPort);

private:

	BOOL SetVideoEncSend(u32 dwNetBand,
		                 u16 wLoacalStartPort, //为本地接收的rtp端口，rtcp端口为rtp端口+1
			             u32 dwDestIp, u16 wDestPort);      //第一路发送目的端口，第二路发送端口为第一路发送目的端口 + 10)


	BOOL SetSecVideoEncSend(u32 dwNetBand,
		                    u16 wLoacalStartPort, //为本地接收的rtp端口，rtcp端口为rtp端口+1
			                u32 dwDestIp,         //发送目的Ip地址，
	                        u16 dwDestPort);      //第一路发送目的端口，第二路发送端口为第一路发送目的端口 + 10)

    u8   GetMediaEncMode(u8 byConfEncMode)
    {
        u8 byMediaEncMode = DES_ENCRYPT_MODE;

        switch (byConfEncMode)
        {
        case CONF_ENCRYPTMODE_AES:
            byMediaEncMode = AES_ENCRYPT_MODE;
            break;
        case CONF_ENCRYPTMODE_DES:
        case CONF_ENCRYPTMODE_NONE:
            break;
        default:
            break;
        }
        return byMediaEncMode;
    }
};

class CBasInst : public CInstance
{
protected:
    enum ESTATE
    {
        IDLE,
        READY,
        NORMAL,
    };

//临时释放出来，稍后整理
//private:
public:
	
	TChannelInfo    m_tChnInfo;
	CHdAdpGroup     *m_pAdpCroup;
public:
    CBasInst(  );
    ~CBasInst(  );

    // ----------- 消息入口 ------------
    void InstanceEntry( CMessage* const pMsg );
    void DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp );
    
    // --------- 消息处理函数 ----------
	void  DaemonProcPowerOn( CMessage* const pMsg, CApp* pcApp );    //初始化
    void  DaemonProcOspDisconnect( CMessage* const pMsg, CApp* pcApp ); //断链处理
    void  DaemonProcConnectTimeOut( BOOL32 bIsConnectA );               //连接总入口
    void  DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA );              //注册总入口    
    void  DaemonProcMcuRegAck(CMessage* const pMsg, CApp* pcApp );      //处理注册
    void  DaemonProcMcuRegNack(CMessage* const pMsg);
	void  DeamonProcGetMsStatusRsp( CMessage* const pMsg ); 
    void  DaemonProcSetQosCmd( CMessage* const pMsg );                  //设置Qos值

    void  ProcInitBas();
    void  ProcStopBas();
    void  ProcStartAdptReq( CServMsg& );  //开始适配
    void  ProcChangeAdptCmd( CServMsg& ); //调整适配参数
    void  ProcStopAdptReq( CServMsg& );   //停止适配    
	void  ProcFastUpdatePicCmd( CServMsg& );                    //Mcu请求Bas编关键帧
    void  ProcTimerNeedIFrame();  //Bas请求Mcu发关键帧

	// ----------- 功能函数 ------------
    BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );      //具体连接函数
	void Register( BOOL32 bIsRegiterA, u32 dwMcuNode );           //向MCU注册
    s32  SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );
    void  SendChnNotif( BOOL32 bUsed);                            //发送通道状态给Mcu

    void ComplexQos( u8& byValue, u8 byPrior );                       //组合Qos值
	void ClearInstStatus( void );                                     //清空本实例保存的状态信息
	   


    BOOL32 GetMediaType( u8 byType, s8* aszTypeStr ); //取媒体类型
	
	//显示信息
	void StatusShow(void);
    // 发送通道状态
    void SendChnStatus(void);

	void AdpStatusAdjust(void);


    /************************************************************************/
    /*                          VMPHD                                       */
    /************************************************************************/
    
    void MsgStartVidMixProc( CMessage * const pcMsg );            //开始复合
    void MsgStopVidMixProc( CMessage * const pcMsg );             //停止复合
    void MsgChangeVidMixParamProc( CMessage * const pcMsg );      //改变复合参数
    void MsgGetVidMixParamProc( CMessage * const pcMsg );         //查询复合参数
    void MsgSetBitRate( CMessage * const pcMsg );
    void ProcSetQosInfo( CMessage * const pcMsg );
    void MsgFastUpdatePicProc(void);

};



#define MIN_BITRATE_OUTPUT      (u16)128

struct TBasDebugVal
{
protected:
    BOOL32          m_bEnableBitrateCheat;   // 是否启用码率输出作弊
    u16             m_wDefault;              // 缺省下调的百分比
public:
    void   SetEnableBitrateCheat(BOOL32 bEnable)
    {
        m_bEnableBitrateCheat = bEnable;
    }
    BOOL32 IsEnableBitrateCheat() const
    {
        return m_bEnableBitrateCheat;
    }

    u16    GetDecRateDebug(u16 wOriginRate) const
    {
        // guzh [10/19/2007] 没有则返回缺省值
        wOriginRate = wOriginRate * m_wDefault / 100;

        if (wOriginRate > MIN_BITRATE_OUTPUT)
            return wOriginRate;
        else
            return MIN_BITRATE_OUTPUT;
    }
    void   SetDefaultRate(u16 wPctRate)
    {
        m_wDefault = wPctRate;
    }

};
class CBasCfg
{
public:
    CBasCfg(  );
    ~CBasCfg(  );
public:
    u32              m_dwMcuIId;                 //通信MCU.A表示
    u32              m_dwMcuIIdB;                //通信MCU.B表示
    u32              m_dwMcuNode;                //与MCU.A通信节点号
    u32              m_dwMcuNodeB;               //与MCU.B通信节点号

    BOOL32           m_bEmbedMcu;                //是否内嵌在MCU.A中
    BOOL32           m_bEmbedMcuB;               //是否内嵌在MCU.B中

    u32              m_dwMcuRcvIp;               //MCU.接收地址
    u16              m_wMcuRcvStartPort;         //MCU.接收起始端口号

    u8               m_byRegAckNum;              //第一次注册成功
	u32              m_dwMpcSSrc;                // guzh [6/12/2007] 业务侧会话校验值 
	u16				 m_wMTUSize;				 //MTU大小, zgc, 2007-04-02
    TEqpCfg          m_tCfg;                     //适配器配置
    TEqp             m_tEqp;                     //本设备
    TPrsTimeSpan     m_tPrsTimeSpan;             //重传时间跨度

    TBasDebugVal     m_tDebugVal;                //作弊值

	// 是否打开平滑发送的开关, zgc, 2008-03-03
	BOOL32 m_bIsUseSmoothSend;

public:
    void FreeStatusDataA( void );
    void FreeStatusDataB( void );

    void ReadDebugValues();
    BOOL32 IsEnableCheat() const
    {
        return m_tDebugVal.IsEnableBitrateCheat();
    }
    u16 GetDecRateDebug(u16 wOriginRate) const
    {
        return m_tDebugVal.GetDecRateDebug(wOriginRate);
    }

    void GetDefaultParam(u8 byEnctype,TVideoEncParam& TEncparam);
    void GetDefaultDecParam( u8 mediatype, TVideoDecParam& tParam );  //取默认解码参数
};

void baslog( s8* pszFmt, ...);
void baslogall( s8* pszFmt, ...);
s8*  ipStr( u32 dwIP );	

typedef zTemplate< CBasInst , MAXNUM_BASHD_CHANNEL, CBasCfg > CBasApp;
extern CBasApp g_cBasApp;

#endif //!_BASINST_H_
