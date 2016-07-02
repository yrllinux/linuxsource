#include "mcueqpsim.h"
#include "evmcueqp.h"
#include "eqpcfg.h"
#include "evmcu.h"
#include "codecwrapperdef_hd.h"
#include "kdvdef.h"

CMcuEqpSimApp g_cMcuEqpSimSsnApp;

u32 g_dwLocalIpAddr;

extern "C" void McuEqpSimuInit(u32 dwLocalIpAddr)
{
    g_cMcuEqpSimSsnApp.CreateApp("McuSimu", AID_MCU_PERIEQPSSN, 80);
	//::OspPost(MAKEIID(AID_MCU_PERIEQPSSN, CInstance::EACH), OSP_POWERON);

    for (u16 wInstNo=1; wInstNo<=MAXNUM_MCU_PERIEQP; wInstNo++)
    {
        ::OspPost(MAKEIID(AID_MCU_PERIEQPSSN, wInstNo),
                  OSP_POWERON, (u8*)&dwLocalIpAddr, sizeof(u32));
    }

    //g_dwLocalIpAddr = dwLocalIpAddr;

    //OspPrintf(TRUE, FALSE, "g_dwLocalIpAddr = 0x%x\n", g_dwLocalIpAddr);
}

CMcuInst::CMcuInst()
{
    //m_dwMcuIpAddr = g_dwLocalIpAddr;
    //OspPrintf(TRUE, FALSE, "77m_dwMcuIpAddr = 0x%x\n", ntohl(m_dwMcuIpAddr));
}

/*====================================================================
    函数名      ClearInst
    功能        清除实例内容
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         zhangsh         创建
====================================================================*/
void CMcuInst::ClearInst()
{
	m_dwEqpIpAddr = 0; //外设接收地址
	m_wEqpStartPort = 0; //外设起始端口
	m_byEqpId = 0;		//设备ID号
	m_byEqpType = 0;    //外设类型
	m_dwEqpNode = 0;
	m_dwEqpIId = 0;
	memset ( (u8 *)m_cServMsg, 0, sizeof(m_cServMsg) );
}

/*====================================================================
    函数名      InstanceEntry
    功能        消息实例
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         zhangsh         创建
====================================================================*/
void CMcuInst::InstanceEntry( CMessage* const pMsg )
{
    //OspPrintf(TRUE, TRUE, "[CMcuInst]Recv msg %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event));

	switch ( pMsg->event )
	{
	case OSP_POWERON:
        m_dwMcuIpAddr = *(u32*)pMsg->content;
        //log(LOGLVL_EXCEPTION, "1m_dwMcuIpAddr = 0x%x\n", m_dwMcuIpAddr);
        //::OspPrintf(TRUE, TRUE, "2m_dwMcuIpAddr = 0x%x\n", ntohl(m_dwMcuIpAddr));
		if(!Init())
		{
			exit(0);
		}
		NEXTSTATE(IDLE);
		break;

	case HDU_MCU_REG_REQ:               //HDU登记注册    //4.6
	case TVWALL_MCU_REG_REQ:			//电视墙登记请求
	case REC_MCU_REG_REQ:				//录像机登记请求
	case MIXER_MCU_REG_REQ:				//混音器登记请求
	case BAS_MCU_REG_REQ:				//适配器登记请求
	case VMP_MCU_REGISTER_REQ:          //画面合成器登记请求
	case PRS_MCU_REGISTER_REQ:			//PRS向MCU注册请求
		MsgRegProc(pMsg);
		break;

	case OSP_DISCONNECT:
		Disconnect();
		break;
	//MCU到外设的消息
    //4.6HDU 
	case MCU_HDU_START_PLAY_REQ:        // HDU开始播放通知
	case MCU_HDU_STOP_PLAY_REQ:         // HDU停止播放通知
	case MCU_HDU_GETSTATUS_REQ:         // 取HDU状态

	case MCU_TVWALL_START_PLAY_REQ:	    //外设开始播放通知
	case MCU_TVWALL_STOP_PLAY_REQ:		//外设通知播放通知
		
	case MCU_REC_STARTREC_REQ:			//开始录像
	case MCU_REC_PAUSEREC_REQ:			//暂停录像
	case MCU_REC_RESUMEREC_REQ:			//恢复录像
	case MCU_REC_STOPREC_REQ:			//停止录像
	case MCU_REC_STARTPLAY_REQ:			//开始放像
	case MCU_REC_PAUSEPLAY_REQ:			//暂停放像
	case MCU_REC_RESUMEPLAY_REQ:		//恢复放像
	case MCU_REC_STOPPLAY_REQ:			//停止放像
	case MCU_REC_FFPLAY_REQ:			//会议放像快进
	case MCU_REC_FBPLAY_REQ:			//会议放像快退
	case MCU_REC_SEEK_REQ:				//会议放像进度调整
	case MCU_REC_LISTALLRECORD_REQ:		//查询录像机记录
	case MCU_REC_DELETERECORD_REQ:      //删除记录
	case MCU_REC_PUBLISHREC_REQ:        //发布录像请求
	case MCU_REC_CANCELPUBLISHREC_REQ:  //取消发布录像请求
		
	case MCU_MIXER_STARTMIX_REQ:        //MCU开始混音请求
	case MCU_MIXER_STOPMIX_REQ:         //MCU停止混音请求
	case MCU_MIXER_ADDMEMBER_REQ:       //加入成员请求
	case MCU_MIXER_REMOVEMEMBER_REQ:    //删除成员请求
	case MCU_MIXER_FORCEACTIVE_REQ:     //强制成员混音
	case MCU_MIXER_CANCELFORCEACTIVE_REQ:    //取消成员强制混音
	case MCU_MIXER_SETCHNNLVOL_CMD:     //设置某通道音量消息
	case MCU_MIXER_SETMIXDEPTH_REQ:     //设置混音深度请求消息
	case MCU_MIXER_SEND_NOTIFY:         //MCU给混音器的是否要发送码流通知
	case MCU_MIXER_VACKEEPTIME_CMD:     //设置语音激励延时保护时长消息
	case MCU_MIXER_SETMEMBER_CMD:       //设置混音成员
		
	case MCU_BAS_STARTADAPT_REQ:		//启动适配请求
	case MCU_BAS_STOPADAPT_REQ:			//停止适配请求
	case MCU_BAS_SETADAPTPARAM_CMD:		//改变适配参数命令
		
	case MCU_VMP_STARTVIDMIX_REQ:       //MCU请求VMP开始工作
	case MCU_VMP_STOPVIDMIX_REQ:        //MCU请求VMP停止工作
	case MCU_VMP_CHANGEVIDMIXPARAM_CMD: //MCU请求VMP改变复合参数
	case MCU_VMP_GETVIDMIXPARAM_REQ:    //MCU请求VMP回送复合参数
	case MCU_VMP_SETCHANNELBITRATE_REQ: //改变画面合成的码率
		
	case MCU_PRS_SETSRC_REQ:			//MCU请求PRS开始保存信息源
	case MCU_PRS_ADDRESENDCH_REQ:		//MCU请求PRS添加一个重传通道
	case MCU_PRS_REMOVERESENDCH_REQ:	//MCU请求PRS移出一个重传通道
	case MCU_PRS_REMOVEALL_REQ:			//MCU请求PRS移出所有的重传通道
/*
        if (pMsg->event == MCU_PRS_SETSRC_REQ)
        {
            CServMsg cServMsg( pMsg->content, pMsg->length );
            TPrsParam tParam;
            cServMsg.GetMsgBody( (u8 *)&tParam, sizeof(tParam) );
            OspPrintf( TRUE, FALSE,"[MCU PRS]:RTP---0x%x : %x\n",
                tParam.m_tLocalAddr.GetIpAddr(),
                tParam.m_tLocalAddr.GetPort());
            OspPrintf( TRUE, FALSE,"[MCU PRS]:RTCP---0x%x : %x\n",
                tParam.m_tRemoteAddr.GetIpAddr(),
                tParam.m_tRemoteAddr.GetPort());
        }
*/
		ProcMcuToEqpMsg( pMsg );
		break;
	//外设回应消息
    //4.6 
	case HDU_MCU_START_PLAY_ACK:        // 开始播放应答
    case HDU_MCU_START_PLAY_NACK:       // 开始播放应答
	case HDU_MCU_STOP_PLAY_ACK:         // 停止播放应答
	case HDU_MCU_STOP_PLAY_NACK:        // 停止播放应答

	case HDU_MCU_GETSTATUS_ACK:         // 取状态应答
	case HDU_MCU_GETSTATUS_NACK:        // 取状态应答

	case REC_MCU_STARTREC_ACK:			//开始录像应答
	case REC_MCU_STARTREC_NACK:			//开始录像应答
	case REC_MCU_PAUSEREC_ACK:			//暂停录像应答
	case REC_MCU_PAUSEREC_NACK:			//暂停录像应答
	case REC_MCU_RESUMEREC_ACK:			//恢复录像应答
	case REC_MCU_RESUMEREC_NACK:		//恢复录像应答
	case REC_MCU_STOPREC_ACK:			//停止录像应答
	case REC_MCU_STOPREC_NACK:			//停止录像应答
	case REC_MCU_STARTPLAY_ACK:			//开始放像应答
	case REC_MCU_STARTPLAY_NACK:		//开始放像应答
	case REC_MCU_PAUSEPLAY_ACK:			//暂停放像应答
	case REC_MCU_PAUSEPLAY_NACK:		//暂停放像应答
	case REC_MCU_RESUMEPLAY_ACK:		//恢复放像应答
	case REC_MCU_RESUMEPLAY_NACK:		//恢复放像应答
	case REC_MCU_CHANGERECMODE_ACK:		//改变录像模式
	case REC_MCU_CHANGERECMODE_NACK:	//改变录像模式
	case REC_MCU_STOPPLAY_ACK:			//停止放像应答
	case REC_MCU_STOPPLAY_NACK:			//停止放像应答
	case REC_MCU_FFPLAY_ACK:			//放像快进应答
	case REC_MCU_FFPLAY_NACK:			//放像快进应答
	case REC_MCU_FBPLAY_ACK:			//放像快退应答
	case REC_MCU_FBPLAY_NACK:			//放像快退应答
	case REC_MCU_SEEK_ACK:				//放像进度调整应答
	case REC_MCU_SEEK_NACK:				//放像进度调整像应答
	case REC_MCU_RECSTATUS_NOTIF:		//录像机状态通知
	case REC_MCU_RECORDCHNSTATUS_NOTIF:	//录像机录像信道状态通知
	case REC_MCU_PLAYCHNSTATUS_NOTIF:	//录像机放像信道状态通知
	case REC_MCU_RECORDPROG_NOTIF:		//当前录像进度通知
	case REC_MCU_PLAYPROG_NOTIF:		//当前放像进度通知
	case REC_MCU_EXCPT_NOTIF:			//录像机异常通知消息
	case REC_MCU_LISTALLRECORD_ACK:     //录像机列应答(完毕)消息
	case REC_MCU_LISTALLRECORD_NACK:    //录像机拒绝录像列表请求
	case REC_MCU_LISTALLRECORD_NOTIF:	//录像机记录通知
    case REC_MCU_DELETERECORD_ACK:      //删除记录应答消息
	case REC_MCU_DELETERECORD_NACK:      //删除记录拒绝消息
	case REC_MCU_PUBLISHREC_ACK:		//发布录像应答
	case REC_MCU_PUBLISHREC_NACK:		//发布录像拒绝
	case REC_MCU_CANCELPUBLISHREC_ACK:  //取消发布录像应答
	case REC_MCU_CANCELPUBLISHREC_NACK: //取消发布录像拒绝
	case MIXER_MCU_ACTIVEMMBCHANGE_NOTIF:  //混音激励成员改变通知
	case MIXER_MCU_CHNNLVOL_NOTIF:      //某通道音量通知消息
	case MIXER_MCU_SETMIXDEPTH_ACK:     //设置混音深度应答消息
	case MIXER_MCU_SETMIXDEPTH_NACK:    //设置混音深度拒绝消息
	case MIXER_MCU_STARTMIX_ACK:        //同意开始混音应答
	case MIXER_MCU_STARTMIX_NACK:       //拒绝开始混音应答
	case MIXER_MCU_STOPMIX_ACK:         //同意停止混音应答
	case MIXER_MCU_STOPMIX_NACK:        //拒绝停止混音应答
	case MIXER_MCU_ADDMEMBER_ACK:       //加入成员应答消息
	case MIXER_MCU_ADDMEMBER_NACK:      //加入成员应答消息
	case MIXER_MCU_REMOVEMEMBER_ACK:    //删除成员应答消息
	case MIXER_MCU_REMOVEMEMBER_NACK:   //删除成员应答消息
	case MIXER_MCU_FORCEACTIVE_ACK:     //强制成员混音应答
	case MIXER_MCU_FORCEACTIVE_NACK:    //强制成员混音拒绝
	case MIXER_MCU_CANCELFORCEACTIVE_ACK:    //取消成员强制混音应答
	case MIXER_MCU_CANCELFORCEACTIVE_NACK:   //取消成员强制混音拒绝
		
	case BAS_MCU_STARTADAPT_ACK:		//启动适配应答
	case BAS_MCU_STARTADAPT_NACK:		//启动适配拒绝
	case BAS_MCU_STOPADAPT_ACK:         //停止适配应答
	case BAS_MCU_STOPADAPT_NACK:        //停止适配拒绝
		
	case VMP_MCU_STARTVIDMIX_ACK:       //VMP给MCU开始工作确认
	case VMP_MCU_STARTVIDMIX_NACK:      //VMP给MCU开始工作拒绝
	case VMP_MCU_STARTVIDMIX_NOTIF:     //VMP给MCU开始工作通知
	case VMP_MCU_STOPVIDMIX_ACK:        //VMP给MCU停止工作确认
	case VMP_MCU_STOPVIDMIX_NACK:       //VMP给MCU停止工作拒绝
	case VMP_MCU_STOPVIDMIX_NOTIF:      //VMP给MCU停止工作通知
	case VMP_MCU_CHANGEVIDMIXPARAM_ACK: //VMP给MCU改变复合参数确认
	case VMP_MCU_CHANGEVIDMIXPARAM_NACK://VMP给MCU改变复合参数拒绝
	case VMP_MCU_CHANGESTATUS_NOTIF:    //VMP给MCU改变复合参数通知
	case VMP_MCU_GETVIDMIXPARAM_ACK:    //VMP给MCU回送复合参数确认
	case VMP_MCU_GETVIDMIXPARAM_NACK:   //VMP给MCU回送复合参数拒绝
	case VMP_MCU_SETCHANNELBITRATE_ACK: //改变码率应答
	case VMP_MCU_SETCHANNELBITRATE_NACK://改变码率拒绝
	case VMP_MCU_NEEDIFRAME_CMD:        //画面合成请求I帧
		
		
	case PRS_MCU_SETSRC_ACK:			//PRS给MCU保存信息源确认
	case PRS_MCU_SETSRC_NACK:			//PRS给MCU保存信息源拒绝
	case PRS_MCU_ADDRESENDCH_ACK:		//PRS给MCU停止工作确认
	case PRS_MCU_ADDRESENDCH_NACK:		//PRS给MCU停止工作拒绝
	case PRS_MCU_REMOVERESENDCH_ACK:	//PRS给MCU改变复合参数确认
	case PRS_MCU_REMOVERESENDCH_NACK:	//PRS给MCU改变复合参数拒绝
	case PRS_MCU_REMOVEALL_ACK:			//PRS给MCU改变复合参数确认
	case PRS_MCU_REMOVEALL_NACK:		//PRS给MCU改变复合参数拒绝
	case PRS_MCU_ADDRESENDCH_NOTIF:		//PRS给MCU的停止结果
	case PRS_MCU_REMOVERESENDCH_NOTIF:	//PRS给MCU的状态改变结果
	case PRS_MCU_REMOVEALL_NOTIF:		//PRS给MCU的状态改变结果
	case TVWALL_MCU_START_PLAY_ACK:
	case TVWALL_MCU_START_PLAY_NACK:
	case TVWALL_MCU_STOP_PLAY_ACK:
	case TVWALL_MCU_STOP_PLAY_NACK:
		ProcEqpMcuNotif( pMsg );
		break;
	case MIXER_MCU_GRPSTATUS_NOTIF:     //混音组状态通知
	case MIXER_MCU_MIXERSTATUS_NOTIF:   //混音器状态通知
		ProcMixNotif( pMsg );
		break;
	case BAS_MCU_BASSTATUS_NOTIF:		//适配器状态通知
	case BAS_MCU_CHNNLSTATUS_NOTIF:     //适配通道状态通知
		ProcBasNotif( pMsg );
		break;
	case PRS_MCU_PRSSTATUS_NOTIF:		//VMP给MCU的状态上报
	case PRS_MCU_SETSRC_NOTIF:			//PRS给MCU的保存信息源结果
		ProcPrsNotif( pMsg );
		break;
	case TVWALL_MCU_STATUS_NOTIF:	    //电视墙状态通知
		ProcTvWallNotif( pMsg );
		break;
	case VMP_MCU_VMPSTATUS_NOTIF:       //画面合成器状态通知
		ProcVmpNotif( pMsg );
		break;
    case HDU_MCU_CHNNLSTATUS_NOTIF:
        ProcHduNotif( pMsg );
		break;
	
	//测试用例发送查询命令返回
	case 10:
		{
			SetSyncAck(&m_tEqpStatus[m_byEqpId] ,sizeof(TPeriEqpStatus) );
		}
		break;
	//测试用例发送设置初始的信息
	case 11:
		SetDefaultData(pMsg);
		break;
	case 12:
		ClearAckBuf();
		break;
	default:
		break;
	}
}

/*====================================================================
    函数名      Init
    功能        初始化
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         zhangsh         创建
====================================================================*/
BOOL CMcuInst::Init()
{
	ClearInst();

	//nodeid = ::OspCreateTcpNode(INADDR_ANY, 2020);
    //::OspPrintf(TRUE, TRUE, "CMcuInst::Init() nodeid = %d\n", nodeid);
	memset( m_tEqpStatus ,0 ,sizeof(m_tEqpStatus) );

	return TRUE;
}

/*====================================================================
	函数  : MsgRegProc
	功能  : 注册消息处理函数
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CMcuInst::MsgRegProc(CMessage* const pMsg)
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
	BOOL bCheck = TRUE;

	TPeriEqpRegReq* ptReg = (TPeriEqpRegReq *)cServMsg.GetMsgBody();

	if ( m_dwEqpIpAddr != 0 && m_dwEqpIpAddr != ptReg->GetPeriEqpIpAddr() )
	{
			bCheck = FALSE;
	}
	else
		m_dwEqpIpAddr = ptReg->GetPeriEqpIpAddr();

	if (m_wEqpStartPort != 0 && m_wEqpStartPort != ptReg->GetStartPort() )
	{
			bCheck = FALSE;
	}
	else
		m_wEqpStartPort = ptReg->GetStartPort();

	if ( m_byEqpId != 0 && m_byEqpId != ptReg->GetEqpId())
	{
			bCheck = FALSE;
	}
	else
		m_byEqpId = ptReg->GetEqpId();

	
	memset( m_tEqpStatus,0,sizeof(m_tEqpStatus) );
	
	
	m_dwEqpIId       = pMsg->srcid;
	m_dwEqpNode        = pMsg->srcnode;

	::OspNodeDiscCBReg( m_dwEqpNode, GetAppID(), 1 );//注册锻炼消息

	TPeriEqpRegAck   tRegAck;
    tRegAck.SetMcuIpAddr(g_dwLocalIpAddr);
	//tRegAck.SetMcuIpAddr( inet_addr("127.0.0.1") );
	tRegAck.SetMcuStartPort( 13000 );
	
	cServMsg.SetMsgBody( (u8*)&tRegAck ,sizeof( tRegAck ));
	TPrsTimeSpan tPrs;
	tPrs.m_wFirstTimeSpan = 10;
	tPrs.m_wRejectTimeSpan = 40;
	tPrs.m_wSecondTimeSpan = 20;
	tPrs.m_wThirdTimeSpan = 30;
	cServMsg.CatMsgBody( (u8*)&tPrs, sizeof(TPrsTimeSpan) );
	if ( bCheck )
	{
		OspPrintf( TRUE, FALSE, "Eqp:%s-%u Type-%u Registered\n", ptReg->GetEqpAlias(), ptReg->GetEqpId(), ptReg->GetEqpType() );
		m_tEqpStatus[m_byEqpId].m_byOnline = TRUE;
		post( m_dwEqpIId, pMsg->event+1, cServMsg.GetServMsg(),cServMsg.GetServMsgLen(),m_dwEqpNode);
		NextState( (u32)NORMAL );
	}
	else
	{
		post( m_dwEqpIId, pMsg->event+2, cServMsg.GetServMsg(),cServMsg.GetServMsgLen(),m_dwEqpNode);
	}
}

/*====================================================================
    函数名      MsgDisconnectProc
    功能        断链消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         zhangsh         创建
====================================================================*/
void CMcuInst::MsgDisconnectProc()
{
	memset( &m_tEqpStatus[m_byEqpId] ,0 ,sizeof(TPeriEqpStatus) );
	ClearInst();
	printf("Disconnectd!\n");
	NEXTSTATE( IDLE );
}

/*====================================================================
    函数名      Disconnect
    功能        端开外设
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         zhangsh         创建
====================================================================*/
void CMcuInst::Disconnect()
{
	ClearInst();
	::OspDisconnectTcpNode( m_dwEqpNode);
	memset( &m_tEqpStatus[m_byEqpId] ,0,sizeof(TPeriEqpStatus) );
	NEXTSTATE( IDLE );
}

/*====================================================================
    函数名      SetDefaultData
    功能        设置外设的基本数据，用以验证注册
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         zhangsh         创建
====================================================================*/
void CMcuInst::SetDefaultData(CMessage* const pMsg)
{
	TEqpCfg tEqpCfg = *(TEqpCfg*)pMsg->content;
	m_dwEqpIpAddr = tEqpCfg.dwLocalIP; //外设接收地址
	m_wEqpStartPort = tEqpCfg.wRcvStartPort; //外设起始端口
	m_byEqpId = tEqpCfg.byEqpId;		//设备ID号
	m_byEqpType = tEqpCfg.byEqpType;    //外设类型
}

/*====================================================================
    函数名      ProcMcuToEqpMsg
    功能        发消息给外设
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         zhangsh         创建
====================================================================*/
BOOL CMcuInst::ProcMcuToEqpMsg( CMessage * const pMsg )
{
	if( OspIsValidTcpNode( m_dwEqpNode ) )
	{
        CServMsg cServMsg( pMsg->content, pMsg->length );                
        post( m_dwEqpIId, pMsg->event, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwEqpNode );
		//post( m_dwEqpIId, pMsg->event, pMsg, pMsg->length, m_dwEqpNode );
		return( TRUE );
	}
	else
	{
		log( LOGLVL_IMPORTANT, "CMcuInst: PeriEqp%u is offline now: Node=%u, CurState=%u, InstId=%u\n", 
			m_byEqpId, m_dwEqpNode, CurState(), GetInsID() );
		return( FALSE );
	}
}

/*====================================================================
    函数名      ProcEqpMcuNotif
    功能        外设返回的消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         zhangsh         创建
====================================================================*/
void CMcuInst::ProcEqpMcuNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	cServMsg.SetEventId( pcMsg->event );
	switch( CurState() )
	{
	case NORMAL:
		AddToAckBuf( cServMsg );
 		break;

	default:
		break;
	}
}

/*====================================================================
    函数名      AddToAckBuf
    功能        业务消息返回
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         zhangsh         创建
====================================================================*/
void CMcuInst::AddToAckBuf( CServMsg cServMsg )
{
	for ( u8 byLoop = 0; byLoop < 10 ; byLoop ++ )
	{
		if ( cServMsg.GetEventId() == 0 )
		{
			memcpy( &m_cServMsg[byLoop], &cServMsg, sizeof(CServMsg) );
			break;
		}
	}
}

/*====================================================================
    函数名      ClearAckBuf
    功能        清除业务消息返回
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         zhangsh         创建
====================================================================*/
void CMcuInst::ClearAckBuf( )
{
	memset( (u8*)m_cServMsg, 0 , sizeof(m_cServMsg) );
}

/*====================================================================
    函数名      ProcMixNotif
    功能        混音器状态改变
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         zhangsh         创建
====================================================================*/
void CMcuInst::ProcMixNotif( CMessage *const pMsg )
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	TMixerGrpStatus tMixerGrpStatus;
	TPeriEqpStatus tMixerStatus;
	switch( pMsg->event) {
	case MIXER_MCU_GRPSTATUS_NOTIF:
		tMixerGrpStatus = *(TMixerGrpStatus*)cServMsg.GetMsgBody();
		m_tEqpStatus[m_byEqpId].m_tStatus.tMixer.m_atGrpStatus[tMixerGrpStatus.m_byGrpId] = tMixerGrpStatus;
		break;
	case MIXER_MCU_MIXERSTATUS_NOTIF:
		tMixerStatus = *(TPeriEqpStatus*)cServMsg.GetMsgBody();
		m_tEqpStatus[m_byEqpId] = tMixerStatus;
		break;
	default:
		break;
	}
}

void CMcuInst::ProcBasNotif(CMessage* const pMsg )
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	char byChnIdx;
	switch(pMsg->event) {
	case BAS_MCU_BASSTATUS_NOTIF:
		memcpy( &m_tEqpStatus[m_byEqpId] ,cServMsg.GetMsgBody(),sizeof(TPeriEqpStatus) );
		break;
	case BAS_MCU_CHNNLSTATUS_NOTIF:
		byChnIdx = cServMsg.GetChnIndex();
		memcpy( &m_tEqpStatus[m_byEqpId].m_tStatus.tBas.tChnnl[byChnIdx],
			(char*)cServMsg.GetMsgBody() +1,
			sizeof(TBasChnStatus) );
		break;
	default:
		break;
	}
}

void CMcuInst::ProcPrsNotif(CMessage* const pMsg )
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	switch(pMsg->event) {
	case PRS_MCU_PRSSTATUS_NOTIF:
	case PRS_MCU_SETSRC_NOTIF:
		memcpy( &m_tEqpStatus[m_byEqpId] ,cServMsg.GetMsgBody(),sizeof(TPeriEqpStatus) );
		break;
	default:
		break;
	}
}

void CMcuInst::ProcTvWallNotif(CMessage* const pMsg )
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	m_tEqpStatus[m_byEqpId] = *(TPeriEqpStatus*)cServMsg.GetMsgBody();
}

void CMcuInst::ProcVmpNotif(CMessage* const pMsg )
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	m_tEqpStatus[m_byEqpId] = *(TPeriEqpStatus*)cServMsg.GetMsgBody();
}

void CMcuInst::ProcHduNotif(CMessage* const pMsg )
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	m_tEqpStatus[m_byEqpId] = *(TPeriEqpStatus*)cServMsg.GetMsgBody();
}
