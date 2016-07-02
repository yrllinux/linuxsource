/*****************************************************************************
   模块名      : tvwall
   文件名      : tvwall.h
   相关文件    : tvwall.cpp
   文件实现功能: 提供了tvwall(包括hdu等tvwall设备)的基本功能接口
   作者        : 刘旭
   版本        : V0.9  Copyright(C) 2001-2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2011/06/01  4.6         刘旭	       创建
******************************************************************************/

#ifndef MCU_TVWALL_H
#define MCU_TVWALL_H

#include "mcustruct.h"
#include "eqpbase.h"

#define		MAX_NUM_TVW_CHNNL				(u8)16				// Tvw设备通道数量最大16个
#define		MAX_NUM_TVW_CFG_NUM				MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL

// TV Wall设备类型定义
enum ETvwEqpType
{
	E_TVW_TYPE_TVWALL = 0,
	E_TVW_TYPE_HDU,
	E_TVW_TYPE_HDU_L,
	E_TVW_TYPE_HDU_H,
	E_TVW_TYPE_HDU_SCHEME,

	E_TVW_TYPE_SUM
};

typedef TTWMember  TTvwMember;

// 判断是否是合法的Tvw通道号
#define IsValidTvwChnnl( a ) ((a) < MAXNUM_PERIEQP_CHNNL ? TRUE : FALSE)

/** 
*  @功    能@ :  判断是否是合法的Hdu设备 
*/
extern BOOL32 IsValidHduEqp(const TEqp& tEqp);

/** 
*  @功    能@ :  根据Eqp Id和通道号判断是否是合法的Hdu
*/
extern BOOL32 IsValidHduChn(const u8 byHduId, const u8 byChnnlIdx, const u8 bySubChnId = 0);

/** 
*  @功    能@ :  判断是否是合法的Tvwall设备, 包含Hdu
*/
extern BOOL32 IsValidTvwEqp(const TEqp& tEqp);

/** 
*  @功    能@ :  根据Eqp Id和通道号判断是否是合法的Tv wall设备, 包含Hdu
*/
extern BOOL32 IsValidTvw(const u8 byTvwId, const u8 byChnnlIdx );

/** 
*  @功    能@ :  清空Hdu通道状态
*/
extern void ClearHduChnnlStatus( const u8 byEqpId, const u8 byChnnlIdx = 0xFF );

/** 
*  @功    能@ :  清空Tvw通道状态
*/
extern void ClearTvwChnnlStatus( const u8 byEqpId, const u8 byChnnlIdx = 0xFF );

/** 
*  @功    能@ :  获取电视墙描述
*/
extern const char* const GetHduTypeStr( const TEqp& tHdu );

/** 
*  @功    能@ :  判断Hdu模式是否支持
*/
extern BOOL32 IsValidHduMode(const u8 byHduChnMode);

class CTvwEqp;

// 电视墙通道定义
class CTvwChnnl /*: public CSwitchBase*/
{
public:
	CTvwChnnl() { Clear(); }

	void Clear();
	BOOL32 IsFree() { return m_byConfIdx != 0 ; }
	BOOL32 IsNull() { return m_byEqpId == 0; }

	// 设置或获取静音状态
	void SetMute( const BOOL32 bMute ) { m_byMuteFlag = bMute ? 1 : 0; }
	BOOL32 IsMute ( ) const { return m_byMuteFlag; }

	// 设置或获取通道状态
	void SetStatus( const u8 byStatus ) { m_byStatus = byStatus; }
	u8 GetStatus( ) const { return m_byStatus; }

	// 设置或获取通道音量大小
	void SetVolume( u8 byVolume ) { m_byVolume = byVolume; }
	u8 GetVolume () const { return m_byVolume; }

	// 设置或获取会议索引
	void SetConfIdx ( const u8 byConfIdx ) { m_byConfIdx = byConfIdx; }
	u8 GetConfIdx( ) const { return m_byConfIdx; }

	// 设置或获取所属外设索引
	void SetEqpId( const u8 byEqpId ) { m_byEqpId = byEqpId; }
	u8 GetEqpId () const { return m_byEqpId; }

	// 设置或获取所属外设索引
	void SetChnnlIdx( const u8 byChnnlIdx ) { m_byChnnlIdx = byChnnlIdx; }
	u8 GetChnnlIdx () const { return m_byChnnlIdx; }

	// 设置或获取通道成员
	void SetMember( const TTvwMember& tMember ) { memcpy(&m_tMember, &tMember, sizeof(m_tMember)); }
	TTvwMember GetMember() const { return m_tMember; }

	// 获取通道所在的TEqp对象
	TEqp GetTvwEqp();

	const CTvwChnnl& operator== ( const CTvwChnnl& );

private:
	u8				m_byStatus;								// 通道状态
	u8				m_byMuteFlag : 1;						// 是否被静音. 1，被静音; 0, 没有
	u8				m_byVolume;								// 音量大小
	u8				m_byConfIdx;							// 所属会议
	u8				m_byEqpId;								// 所属Eqp的Id
	u8				m_byChnnlIdx;							// 所属Eqp下的通道号
	TTvwMember		m_tMember;								// 实际通道成员
};

class CConfTvwChnnl : public CTvwChnnl
{
public:
	// 清空
	void Clear() { memset(this, 0, sizeof(CConfTvwChnnl)); CTvwChnnl::Clear(); }

	// 设置或获取通道会议配置的成员
	void SetConfMember( const u8 byMember ) { m_byConfMember = byMember; }
	u8 GetConfMember() const { return m_byConfMember; }

	// 设置或获取会议中通道成员类型
	void SetConfMemberType( const u8 byMemberType ) { m_byConfMemberType = byMemberType; }
	u8 GetConfMemberType() const { return m_byConfMemberType; }

	const CConfTvwChnnl& operator== ( const CConfTvwChnnl& );

private:
    u8    m_byConfMember;									// 该通道成员索引
    u8    m_byConfMemberType;								// 该通道成员跟随类型
};

// 电视墙设备定义
class CTvwEqp : public TEqp
{
public:											// 基本属性
	// 设置或获取通道数量
	//void SetChnnlNum ( const u8 byNum ) { m_byChnnlNum = byNum; }
	u8 GetChnnlNum() const ;/*{ return m_byChnnlNum; }*/

	// 设置或获取某个通道
	void SetChnnl( const u8 byChnnlIdx, const CTvwChnnl& cChnnl );
	CTvwChnnl* GetChnnl( const u8 byChnnlIdx );

	// 设置或获取在线状态
	void SetOnline( const BOOL32 bOnline = TRUE ) { m_byOnline = bOnline ? 1 : 0; }
	BOOL32 IsOnline( ) const { return m_byOnline != 0 ? TRUE : FALSE; }

	// 设置或获取合法状态
	void SetValid( const BOOL32 bValid = TRUE ) { m_byValid = bValid ? 1 : 0; }
	BOOL32 IsValid( ) const { return m_byValid != 0 ? TRUE : FALSE; }

	// 设置或获取电视墙设备类型
	void SetTvwType( const ETvwEqpType eType ) { m_eTvwEqpType = eType; }
	ETvwEqpType GetTvwType() const { return m_eTvwEqpType; }

	// 设置或获取媒体能力集
	void SetCapSet( const TSimCapSet& tCapSet){ memcpy(&m_tSimCapSet, &tCapSet, sizeof(TSimCapSet));}
	TSimCapSet GetCapSet() const { return m_tSimCapSet; }

public:										 // 基本功能
	// 清空某个通道
	void ClearChnnl( const u8 byChnnlIdx );
	// 清空所有通道
	void ClearAllChnnl();

	// 寻找一个空闲的通道
	CTvwChnnl* FindFreeChnnl();
	// 寻找源为tSrc的电视墙通道
	u8 FindChnnl( const TMt& tSrc, CTvwChnnl* pacChnnl, const u8 byChnnlNum );
	// 寻找为byConfIdx这个会议服务的通道
	u8 FindChnnl( const u8 byConfIdx, CTvwChnnl* pacChnnl, const u8 byChnnlNum );
	// 寻找为某个下级mcu服务的通道
	u8 FindChnnlByMcu( const TMcu& tSubMcu, CTvwChnnl* pacChnnl, const u8 byChnnlNum );
	
public:
	// 将TEqp赋值给CTvwEqp
	const CTvwEqp& operator= ( const TEqp& );

	// 判断是否与TEqp对象相等
	BOOL32 operator== ( const TEqp& );

private:
	// 构造函数, 禁止默认构造
	CTvwEqp();

private:
	/*******************************************************************************/
	/* 目前CTvwEqp只是对tvwall进行了一些封装, 数据源仍然采用mcudata和mcuvc中的数据 */
	/* 以后彻底重构时, CTvwEqp将会封装tvwall的所有数据							   */
	/*******************************************************************************/
	u8				m_byChnnlNum;							// 通道的数量
	CTvwChnnl		m_acTvwChnnl[MAX_NUM_TVW_CHNNL];		// 通道数组, 最大
	u8				m_byOnline : 1;							// 是否在线
	u8				m_byValid : 1;							// 是否合法(被配置支持)
	ETvwEqpType		m_eTvwEqpType;							// 电视墙设备类型

	TSimCapSet		m_tSimCapSet;							// 媒体能力集		
};



// VCS会议电视墙配置信息定义
class CVcsTvWallCfg
{
public:
	// 清空某个通道
	void ClearChnnl( const u8 byChnnlIdx );
	// 清空所有通道
	void ClearAllChnnl();
	
	// 寻找一个空闲的通道
	CTvwChnnl* FindFreeChnnl();
	// 寻找源为tSrc的电视墙通道
	u8 FindChnnl( const TMt& tSrc, CTvwChnnl* pacChnnl, const u8 byChnnlNum = 1 );
	// 寻找为byConfIdx这个会议服务的通道
	u8 FindChnnl( const u8 byConfIdx, CTvwChnnl* pacChnnl, const u8 byChnnlNum = 1);
	// 寻找为某个下级mcu服务的通道
	u8 FindChnnlByMcu( const TMcu& tSubMcu, CTvwChnnl* pacChnnl, const u8 byChnnlNum = 1);
	
private:
	u16				m_byChnnlNum;							// 配置的电视墙数目
	CTvwChnnl*		m_apcTvwChnnl[MAX_NUM_TVW_CFG_NUM];		// 电视墙通道信息
	TMt				m_atOrigMemb[MAX_NUM_TVW_CFG_NUM];		// 电视墙原始通道成员
};

// 类前置说明
class CVcsConfTvwMgr;

class CTvwMgrModeBase
{
public:
	// 构造函数
	CTvwMgrModeBase(  );

	// 初始化
	BOOL32 Init( const u8 byMode, CVcsTvWallCfg* pcTvwInfo );
	// 退出时的销毁函数
	void Destroy();

	u8 GetMode() const { return m_byTvwMgrMode; }

	// 进入
	virtual BOOL32 Enter( CVcsConfTvwMgr *pcMgr );
	// 退出
	virtual BOOL32 Exit();

	virtual s16 OnMessage( const CServMsg * pcMsg );

protected:
	virtual s16 OnSetMemberMsg( const CServMsg * pcMsg );
	virtual s16 OnClrMemberMsg( const CServMsg * pcMsg );
	virtual s16 OnStartMixMsg( const CServMsg * pcMsg );
	virtual s16 OnStopMixMsg( const CServMsg * pcMsg );

private:
	u8				m_byTvwMgrMode;							// 当前电视墙管理模式
	CVcsConfTvwMgr*	m_pcConfTvwMgr;							// 电视墙管理者入口
	CVcsTvWallCfg*	m_pcTvwGroup;							// 电视墙信息
};

class CTvwReviewMode : public CTvwMgrModeBase
{

};

class CTvwManuMode : public CTvwMgrModeBase
{
	
};

class CTvwAutoMode : public CTvwMgrModeBase
{
	
};

class CMcuVcInst;

class CVcsConfTvwMgr : public CVcsTvWallCfg
{
public:										 // 基本功能
	BOOL32 Init();
	void Destroy();

	// 全部参与混音
	BOOL32 StartMix();
	// 全部禁止混音
	BOOL32 StopMix();

	BOOL32 ChangeTvwMgrModel( const u8 byNewMode );

private:
	CMcuVcInst*				m_pcVc;								// 会控对象
	CTvwMgrModeBase*		m_pcVcsTvwModeMgr;					// 电视墙管理模式			
};


/************************************************************************/
/* 全局函数                                                             */
/************************************************************************/
// CTvwEqp转TEqp
TEqp	GetTEqpFrmTvw( const CTvwEqp& cTvw );

// TEqp转CTvwEqp
CTvwEqp GetTvwFromTEqp( const TEqp& tTvw );

// 从TEqp和通道索引获取对应的电视墙通道
CTvwChnnl GetTvwChnnl( const TEqp& tTvw, const u8 byChnnlIdx );

// 根据Eqp Id判读该设备是否是TvWall
BOOL32 IsTvWall( u8 byEqpId );

// 根据Eqp ID判断该设备是否是HDU设备
BOOL32 IsHdu( u8 byEqpId);

#endif