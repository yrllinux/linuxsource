#ifndef _MTMC_RMTTESTER_H
#define _MTMC_RMTTESTER_H
class CMtMcRmtTester
{
private:
	u32 m_mcNode;				//MC节点号
public:
	//=====================================
	//初始化接口
	//=====================================
	BOOL Init(u32 mcNode = 0);
public:
	//=====================================
	//操作接口
	//=====================================
	//激活MC
	void SetMcActive();
	//去激活MC
	void SetMcInActive();
	//设置新呼叫处理模式
	void SetNewCallMode(u8 mode);
	//发起呼叫
	void MakeCall(u32 ip,u16 port = 1720,u8 callType = CALL_TYPE_CREATE);
	//注册已有会议信息
	void SetConfInfo(TMCRegConf& tRegConf);
public:
	//=====================================
	//查询接口
	//=====================================
	//获取MC当前状态
	u32 GetMcState();
	//某个终端是否在会议中
	BOOL IsInConf(u32 ip);
	//某个终端编号是否在会议中
	BOOL IsLabelInConf(TTERLABEL label);
	//查询当前发言人
	TTERLABEL GetFloorer();
	//查询当前主席
	TTERLABEL GetChair();
	//查询会议信息
	CConference GetConfInfo();
	//查询会议模式
	u8 GetConfMode();
	//查询申请发言终端
	TTERLABEL GetApplyFloorer();
	//查询申请主席终端
	TTERLABEL GetApplyChair();
	//查询将成为发言人终端
	TTERLABEL GetToBeFloorer();
	//查询当前视频源
	TTERLABEL GetSrcLabel();
protected:
	//发送消息给MC
	BOOL PostMsgToMc(u16 wEvent,
					  const void* pBody = NULL,
					  u16 wLen = 0,
					  PTHANDLE ptHandle = NULL);
	//发送集成测试消息接口
	BOOL SendIntTestMsgToMc(u32 dwEvent,
							const void* pBody = NULL,
							u16  wLen = 0,
							void* ackbuf=NULL,
							u16  ackbuflen=0,
							u16* realacklen=NULL);
};
#endif 