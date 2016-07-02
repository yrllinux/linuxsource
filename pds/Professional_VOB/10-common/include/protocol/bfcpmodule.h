#ifndef BFCPMODULE_H
#define BFCPMODULE_H

#include "bfcpdata.h"
//#include "sipcapset.h"

#ifdef WIN32
#define BFCPCALLBACK __cdecl
#else
#define BFCPCALLBACK
#endif

class CBfcpBinary;

typedef void (BFCPCALLBACK* CBOpenDualChannel)( void *ptSipCap );

typedef void (BFCPCALLBACK* CBCloseDualChannel)( void *ptSipCap );

typedef BOOL (BFCPCALLBACK* CBDualCapsetChanged)( void *ptSipCap );

typedef struct tagBfcpCBFunction
{
	CBOpenDualChannel      m_cbOpenDualChannel;
	CBCloseDualChannel     m_cbCloseDualChannel;
	CBDualCapsetChanged    m_cbDualCapsetChanged;
	
	void Clear()
	{
		m_cbOpenDualChannel = NULL;
		m_cbCloseDualChannel = NULL;
		m_cbCloseDualChannel = NULL;
	}

	tagBfcpCBFunction()
	{
		Clear();
	}
}TBfcpCBFunction;

class CBfcpModule
{
public:
	CBfcpModule( void *pcCapset, TBfcpCBFunction *ptBfcpCB, u16 wFloorId, u32 dwConferenceId, u16 wUserId, TRequestAddress& tRequestAddress );
	~CBfcpModule();

	//客户端：发送floor请求（Hello、FloorRequest、FloorRelease）
	BOOL SendFloorRequireMessage( EmBfcpPrimitive emPrimitive );
	//服务器端：发送floor状态（FloorStatus）：bFlag为TRUE，则floor为Granted状态；否则floor为Released状态
	BOOL SendFloorStatusMessage( BOOL bFlag = TRUE );
	//服务器端：发送FloorRequestStatus应答：bFlag为TRUE，则floor为Granted状态；否则floor为Denied状态
	BOOL SendFloorRequestStatusMessage( BOOL bFlag = TRUE );
	//服务器端：检测BFCP消息
	s32 HandleBfcpMessage( SOCKHANDLE tSocket );
	//对BFCP消息的回应
	BOOL BfcpResponse( CBfcpPacketData *pcPacketData );
	//更新BFCP相关参数
	void UpdateBfcpModuleInfo( u16 wFloorId, u32 dwConferenceId, u16 wUserId );
	
private:
	BOOL SendBfcpFloorRequest( TFloorRequestInfo& tFloorRequestInfo );
	BOOL SendBfcpFloorRelease();
	BOOL SendBfcpFloorRequestQuery();
	BOOL SendBfcpFloorRequestStatus( TBfcpCommonHeader& tHeader, TGroupedAttribute& tFloorRequestInfo );
	BOOL SendBfcpUserQuery();
	BOOL SendBfcpUserStatus();
	BOOL SendBfcpFloorQuery();
	BOOL SendBfcpFloorStatus( TGroupedAttribute& tFloorRequestInfo );
	BOOL SendBfcpChairAction();
	BOOL SendBfcpChairActionAck();
	BOOL SendBfcpHello();
	BOOL SendBfcpHelloAck();
	BOOL SendBfcpError( u16 wTransactionId, EmBfcpErrorCode emErrorCode, s8 *pchErrorInfo, EmBfcpPrimitive emPrimitive = emBfcpPrimitiveBegin );
	u16 GetIdleIndex( s8 *pchIdName );
	void ConstructFloorRequestInfo( EmBfcpPrimitive emPrimitive, TGroupedAttribute& tFloorRequestInfo, BOOL bFlag = TRUE );

	u16 m_wFloorId;
	u32 m_dwConferenceId;
	u16 m_wTransactionId;
	u16 m_wUserId;
	u16 m_wFloorRequestId;
	u16 m_wOngoingFloorRequestNum;
	BOOL m_bIsParticipant;
	TBfcpId m_atTransactionId[MAX_TRANSACTION_ID];
	TBfcpId m_atFloorRequestId[MAX_FLOOR_REQUEST_ID];
	EmBfcpRequestStatus m_emRequestStatus;

	void *m_pcCapset;
	TBfcpCBFunction m_tBfcpCbFun;
	CBfcpBinary *m_pcBfcpBinary;
};

void CheckBfcpMessageServer( CBfcpModule& cModuleServer );

// Log Debug Interface
API void bfcploghelp();
API void bfcplogon( s32 nLevel );
API void bfcplogoff();
API void bfcpmoduleversion();
API void RegBfcpFunLinux();
void BfcpLog( const s8 *pFormat, va_list vaList );
void BfcpPrintf( const s8 *pFormat, ... );

#endif
