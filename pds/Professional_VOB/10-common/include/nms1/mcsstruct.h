/*=============================================================================
ģ   ��   ��: �������̨ҵ���ܿ�
��   ��   ��: mcsstruct.h
�� ��  �� ��: mcsstruct.cpp
�ļ�ʵ�ֹ���: mcslib�л������ݽṹ
��        ��: ���
��        ��: V4.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���      �޸�����
2005/04/27  4.0     ���        ����
=============================================================================*/


#ifndef _MCSSTRUCT_20050422_H_
#define _MCSSTRUCT_20050422_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4786)

#include "mcustruct.h"
#include "vcsstruct.h"
#include "mcsconst.h"
#include "nmscommon.h"
#include <afxmt.h>
#include "Item.h"
#include "ummessagestruct.h"
#include <map>
#include <set>
#include <vector>
using namespace std;

// �ض��������ƣ�����Ժ��������汾���������̫����롣
typedef TMcuAddrEntryV1 TMcuAddrEntryCur;
typedef TMcuAddrGroupV1 TMcuAddrGroupCur;
typedef TMtExtU			TMtExtCur;

/*************************************
************************************/
#define MAX_HDU_VMP_NUM  56
//struct TVmpParamInfo;
class CConf;

#define MCS_MAXLEN_VIDEOSOURCE_ALIAS   64
#define MCU_RE_GK_PASSWORD_LEN		   64 //MCUע��GK����

#define EQP_TYPE_HDU2_S			       100

//����ϳɲ�����Ϣ //added by twb.
typedef struct tagVmpParamInfo
{
private:
	typedef set<u32>	MtSet;
	TVMPParam_25Mem		tVmpParam;		
	MtSet			    setVmpMember;	//����ϳ����г�Ա���������
	BOOL32				bOldMcuFlag;		//�Ƿ����ϵ�MCU
	s8					achAlias[MAXLEN_EQP_ALIAS];
	
public:
	tagVmpParamInfo(){ 
		ZeroMemory((void*)&tVmpParam, sizeof(TVMPParam_25Mem) );
		ZeroMemory((void*)achAlias, sizeof(achAlias) );
		bOldMcuFlag = FALSE;
	}
	~tagVmpParamInfo(){ }
	void SetVmpParam(CConf *pConf, TVMPParam_25Mem &tParam)
	{	
		memcpy(&tVmpParam, &tParam, sizeof(TVMPParam_25Mem));
		
		u8 byVmpMember = tParam.GetMaxMemberNum();
		
		for ( u8 byIndex = 0; byIndex < byVmpMember; byIndex++ )
		{
			TVMPMember* ptVmpMember = tParam.GetVmpMember( byIndex );
			if ( ptVmpMember && ptVmpMember->IsNull() == FALSE )
			{
				u32 dwMtIdx = MAKEDWORD( ptVmpMember->GetMcuIdx(), ptVmpMember->GetMtId() );
				setVmpMember.insert( dwMtIdx );
			}
		}
	}
	void SetOldMcuFlag(BOOL32 bOldFlag)
	{
		bOldMcuFlag = bOldFlag;
	}
	BOOL32 IsOldMcu()
	{
		return bOldMcuFlag;
	}
	BOOL32 IsVmpMember(TMt &tMt)
	{
		u32 dwMtIdx = MAKEDWORD( tMt.GetMcuIdx(), tMt.GetMtId() );
		
		if ( setVmpMember.find( dwMtIdx ) == setVmpMember.end() )
		{
			return FALSE;
		}
		
		return TRUE;
	}
	
	TVMPParam_25Mem& GetVmpParam(){ return tVmpParam; }
	MtSet&  GetVmpMemberSet(){ return setVmpMember; }
	
	s8* GetVmpAlias(){ return achAlias; }
	void SetVmpAlias(const s8* pchAlias)
	{
		ZeroMemory((void*)achAlias, sizeof(achAlias));
		
		strncpy((s8*)achAlias, pchAlias, strlen(pchAlias));
	}
}TVmpParamInfo;

typedef map<u8, TVmpParamInfo> MAPVMPPARAMINFO;

//��ƵԴ�ṹ��
typedef struct tagMcsMtVideoSrcAlias    //added by twb 2011.4.14
{
    tagMcsMtVideoSrcAlias() { SetNull(); }
    
    inline void SetNull(void) { memset( this, 0, sizeof (tagMcsMtVideoSrcAlias) ); }
    
    s8          m_szMtVideoAlias[EX_VIDEOSOURCE_BASE][MCS_MAXLEN_VIDEOSOURCE_ALIAS];   //��ƵԴ����
    
    u8             m_byVidIdx[EX_VIDEOSOURCE_BASE];                                 //��ƵԴ�˿�����
	
    u8             m_byVideoSrcNum;                                //��ƵԴ����
    
}TMcsMtVideoSrcAlias;

typedef struct tagMcsMtVideoSrcAliasParam
{
    TMt                     m_tCurMt;                                       //��ǰ�ն�
	
    TMcsMtVideoSrcAlias     m_tMcsMtVideoSrcAlias;  
    
}TMcsMtVideoSrcAliasParam;


//¼�����������  ����MCU�����޸�TRecStartPara�������Ӵ˽ṹ�巽�㴫��
typedef struct tagRecParam
{
    TRecStartPara   m_tRecStartPara;
    TSimCapSet      m_tSimCapSet;
}TRecParam;

// ��ť������
typedef struct tagVcsBtnItemData
{
    tagVcsBtnItemData()
    {
		m_dwDataType = 0;
    }
	
    TVCSEntryInfo   m_tEntryInfo;
    TMt             m_tMt;
	
    u32 m_dwDataType;
	
}TItemData, *PTItemData;

//vcs Ԥ��
typedef struct tagVcsSchema
{
	tagVcsSchema()
	{
		SetNull();
	}	
	s8 m_pbySchemaName[ MAX_VCSGROUPNAMELEN ];
	vector <TMtVCSPlanAlias> m_vctTMtVCSPlanAlias;
	vector <TItemData> m_vctTItemData;

	void SetNull( )
	{
		ZeroMemory( m_pbySchemaName, sizeof( m_pbySchemaName ) );
		m_vctTMtVCSPlanAlias.clear();
		m_vctTItemData.clear();
	}

	void PopUpItem( TItemData& tItemData )
	{
		for ( u32 nIndex = 0; nIndex < m_vctTItemData.size(); nIndex ++ )
		{
			// 			if ( tItemData.m_tMt == m_vctTItemData[ nIndex ].m_tMt)
			// 			{
			// 				m_vctTItemData.erase( m_vctTItemData.begin( ) + nIndex );
			// 				break;
			// 			}
			TItemData &tItem = m_vctTItemData[nIndex];
			if ( !tItemData.m_tMt.IsNull() && !tItem.m_tMt.IsNull() )
			{
				if ( tItemData.m_tMt == tItem.m_tMt )
				{
					m_vctTItemData.erase( m_vctTItemData.begin( ) + nIndex );
					break;
				}
			}
			else
			{
				if ( memcmp( tItemData.m_tEntryInfo.achEntryName, tItem.m_tEntryInfo.achEntryName, sizeof(tItem.m_tEntryInfo.achEntryName)) == 0 )
				{
					m_vctTItemData.erase( m_vctTItemData.begin( ) + nIndex );
					break;
				}
			}
		}
	}
	
	void PopupItemAndAlias( TItemData& tItemData )
	{
		for ( u32 nIndex = 0; nIndex < m_vctTItemData.size(); ++nIndex )
		{
			TItemData &tItem = m_vctTItemData[nIndex];
			if ( !tItemData.m_tMt.IsNull() && !tItem.m_tMt.IsNull() )
			{
				if ( tItemData.m_tMt == tItem.m_tMt )
				{
					m_vctTItemData.erase( m_vctTItemData.begin( ) + nIndex );
					m_vctTMtVCSPlanAlias.erase( m_vctTMtVCSPlanAlias.begin() + nIndex );
					break;
				}
			}
			else
			{
				if ( memcmp( tItemData.m_tEntryInfo.achEntryName, tItem.m_tEntryInfo.achEntryName, sizeof(tItem.m_tEntryInfo.achEntryName)) == 0 )
				{
					m_vctTItemData.erase( m_vctTItemData.begin( ) + nIndex );
					m_vctTMtVCSPlanAlias.erase( m_vctTMtVCSPlanAlias.begin() + nIndex );
					break;
				}
			}
		}
	}
	
    //modified by twb 2010.8.31  ���Ӷ�PU�ն˵�֧��
	s32 Forward( TItemData & tItemData )
	{
		for ( u32 nIndex = 0; nIndex < m_vctTItemData.size(); nIndex ++ )
		{
			TItemData &tItem = m_vctTItemData[nIndex];
			
            BOOL32  bSameMt = FALSE;
			
			if ( !tItemData.m_tMt.IsNull() && !tItem.m_tMt.IsNull() )
			{
				if ( tItemData.m_tMt == tItem.m_tMt )
				{
					bSameMt = TRUE;
				}
			}
			else
			{
				if (tItemData.m_tEntryInfo.byAddrType == puAliasTypeIPPlusAlias)
				{
					if ( strcmp( tItemData.m_tEntryInfo.achUIName, tItem.m_tEntryInfo.achUIName) == 0 
						&& strcmp( tItemData.m_tEntryInfo.achUIName, "") != 0)
					{
						bSameMt = TRUE;
					}
				}
				else
				{
					if ( strcmp( tItemData.m_tEntryInfo.achEntryName, tItem.m_tEntryInfo.achEntryName) == 0 )
					{
						bSameMt = TRUE;
					}
				}
			}
			
            if (bSameMt)
			{
				if ( nIndex == 0)
				{
					return nIndex;
				}
				TItemData tTemData = m_vctTItemData[ nIndex - 1 ];
				m_vctTItemData[ nIndex - 1 ] = tItemData;
				m_vctTItemData[ nIndex ] = tTemData;
				
				TMtVCSPlanAlias tAlias = m_vctTMtVCSPlanAlias[ nIndex - 1 ];
				m_vctTMtVCSPlanAlias[ nIndex - 1 ] = m_vctTMtVCSPlanAlias[ nIndex ];
				m_vctTMtVCSPlanAlias[ nIndex ] = tAlias;
				return nIndex - 1;
				
			}
		}
		return -1;
	}
	
    //modified by twb 2010.8.31  ���Ӷ�PU�ն˵�֧��
	s32 Backward( TItemData & tItemData )
	{
		for ( u32 nIndex = 0; nIndex< m_vctTItemData.size(); nIndex ++ )
		{
			TItemData &tItem = m_vctTItemData[nIndex];
            BOOL32  bSameMt = FALSE;
			
			if ( !tItemData.m_tMt.IsNull() && !tItem.m_tMt.IsNull() )
			{
				if ( tItemData.m_tMt == tItem.m_tMt )
				{
					bSameMt = TRUE;
				}
			}
			else
			{
				if (tItemData.m_tEntryInfo.byAddrType == puAliasTypeIPPlusAlias)
				{
					if ( strcmp( tItemData.m_tEntryInfo.achUIName, tItem.m_tEntryInfo.achUIName) == 0 
						&& strcmp( tItemData.m_tEntryInfo.achUIName, "" ) != 0)
					{
						bSameMt = TRUE;
					}
				}
				else
				{
					if ( strcmp( tItemData.m_tEntryInfo.achEntryName, tItem.m_tEntryInfo.achEntryName) == 0 )
					{
						bSameMt = TRUE;
					}
				}
			}
            
            if (bSameMt)
			{
				if ( nIndex == m_vctTItemData.size() - 1 )
				{
					return nIndex;
				}
				TItemData tTemData = m_vctTItemData[ nIndex + 1 ];
				m_vctTItemData[ nIndex + 1 ] = tItemData;
				m_vctTItemData[ nIndex ] = tTemData;
				
				TMtVCSPlanAlias tAlias = m_vctTMtVCSPlanAlias[ nIndex + 1 ];
				m_vctTMtVCSPlanAlias[ nIndex + 1 ] = m_vctTMtVCSPlanAlias[ nIndex ];
				m_vctTMtVCSPlanAlias[ nIndex ] = tAlias;
				return nIndex + 1;
			}
		}
		return -1;
	}
	
	BOOL32 IsItemExist( const TItemData & tItemData )
	{
		for ( u32 nIndex = 0; nIndex < m_vctTItemData.size(); nIndex ++ )
		{
			TItemData &tItem = m_vctTItemData[nIndex];
			//���ж�TMt -2011.3.15 by xcr
			if ( !tItemData.m_tMt.IsNull() && !tItem.m_tMt.IsNull() )
			{
				if ( tItemData.m_tMt == tItem.m_tMt )
				{
					return TRUE;
				}
			}
			else
			{
				if ( memcmp( tItemData.m_tEntryInfo.achEntryName, tItem.m_tEntryInfo.achEntryName, sizeof(tItem.m_tEntryInfo.achEntryName)) == 0 )
				{
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	
}TVCSSchema, *PTVCSSchema;

// ����ϳ������ӵ�ͨ����
#define VMP_MEMBER_EX_NUM ( MAXNUM_VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER )

struct TVmpModuleEx : public TVmpModule
{
public:
	TVmpChnnlMember m_atVmpMemberEx[MAXNUM_MPU2VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER];
public:
	void EmptyMember(u8 byMemberId)
	{
		if (byMemberId >= MAXNUM_MPU2VMP_MEMBER)
			return;
		if (byMemberId < MAXNUM_MPUSVMP_MEMBER)
		{
			TVmpModule::EmptyMember();
		}
		else
		{
			memset(this, 0, (MAXNUM_MPU2VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER)*sizeof(TVmpChnnlMember));
		}
	}
	// ����ĳ��ͨ��
    void SetVmpMember(u8 byChnlIdx, u8 byMtIdx, u8 byMemberType)
    {
		if (byChnlIdx >= MAXNUM_MPU2VMP_MEMBER)
			return;
        if (byChnlIdx < MAXNUM_VMP_MEMBER)
        {
            m_abyVmpMember[byChnlIdx]  = byMtIdx;
            m_abyMemberType[byChnlIdx] = byMemberType;
        }
		else
		{
			for(u8 byIdx = 0; byIdx < 5; byIdx++)
			{
				if (m_atVmpMemberEx[byIdx].m_byVmpMember == 0)
				{
					m_atVmpMemberEx[byIdx].m_byChnIdx = byChnlIdx;
					m_atVmpMemberEx[byIdx].m_byVmpMember = byMtIdx;
					m_atVmpMemberEx[byIdx].m_byMemberType = byMemberType;
					break;
				}
			}		
		}
    }
	void RemoveVmpMember(u8 byChnlIdx) 
    {
		if (byChnlIdx >= MAXNUM_MPU2VMP_MEMBER)
			return;
        if(byChnlIdx < MAXNUM_VMP_MEMBER)
        {
            m_abyVmpMember[byChnlIdx]  = 0;
            m_abyMemberType[byChnlIdx] = 0;
        }
		else
		{
			for(u8 byIdx = 0; byIdx < 5; byIdx++)
			{
				if (m_atVmpMemberEx[byIdx].m_byChnIdx == byChnlIdx)
				{
					m_atVmpMemberEx[byIdx].m_byChnIdx = 0;
					m_atVmpMemberEx[byIdx].m_byVmpMember = 0;
					m_atVmpMemberEx[byIdx].m_byMemberType = 0;
				}
			}		
		}
    }
	BOOL32 SetVmpMemberEx(TVmpChnnlMember* tVmpMemberEx, u8 byCapNum )
	{
		if ( tVmpMemberEx == NULL || byCapNum == 0 || byCapNum > 5 )
		{ 
			StaticLog("[SetVmpMemberEx] invalid param!\n");
			return FALSE;
		}
		else
		{
			for(u8 byIdx = 0; byIdx < 5; byIdx++)
			{
				m_atVmpMemberEx[byIdx].clear();
			}
			memcpy((u8*)&m_atVmpMemberEx[0],tVmpMemberEx,byCapNum*sizeof(TVmpChnnlMember));
			return TRUE;
		}
	}
	u8 GetVmpMemberEx(TVmpChnnlMember* tVmpMemberEx)const
	{
		u8 byNum = 0;
		if (tVmpMemberEx == NULL)
		{
			StaticLog("[GetVmpMemberEx] invalid param!\n");
			return 0;
		}
		for(u8 byIdx = 0; byIdx < 5; byIdx++)
		{
			if(m_atVmpMemberEx[byIdx].m_byVmpMember != 0)
			{
				tVmpMemberEx[byIdx] = m_atVmpMemberEx[byIdx];
				byNum++;
			}	
		}
		return byNum;
	}
}
PACKED
;


// �����õ�VmpModule���ݽṹ��ȡ��ԭ��MCU��TVmpModule
typedef struct tagMcsVmpModule
{
	TVMPParam_25Mem m_tVMPParam;                             //���ָ���Ļ���ϳɲ���
	u8				m_abyVmpMember[MAXNUM_VMP_MEMBER];   //���ָ���Ļ���ϳɳ�Ա������������ʱTMtAlias��������+1��
	u8				m_abyMemberType[MAXNUM_VMP_MEMBER];  //����ϳɳ�Ա�ĸ��淽ʽ, VMP_MEMBERTYPE_MCSSPEC, VMP_MEMBERTYPE_SPEAKER...

	tagMcsVmpModule()
	{
		EmptyMember();
	}

	void EmptyMember()
	{
		memset( this, 0, sizeof(tagMcsVmpModule) );    
	}

	// ����ĳ��ͨ��
	void SetVmpMember(u8 byChnlIdx, u8 byMtIdx, u8 byMemberType)
	{
		if ( byChnlIdx < MAXNUM_VMP_MEMBER )
		{
			m_abyVmpMember[byChnlIdx]  = byMtIdx;
			m_abyMemberType[byChnlIdx] = byMemberType;
		}
	}

	// �Ƴ�ĳ��ͨ��
	void RemoveVmpMember(u8 byChnlIdx) 
	{
		if( byChnlIdx < MAXNUM_VMP_MEMBER )
		{
			m_abyVmpMember[byChnlIdx]  = 0;
			m_abyMemberType[byChnlIdx] = 0;
		}
	}

	// ���úϳɲ���
	void SetVmpParam(const TVMPParam &tParam)
	{
		*static_cast<TVMPParam*>(&m_tVMPParam) = tParam;
	}

	// ȡ�úϳɲ���
	TVMPParam GetVmpParam()
	{
		return m_tVMPParam;
	}

	void SetVmpModule( TVmpModule& vmpModule )
	{
		SetVmpParam( vmpModule.m_tVMPParam );
		memcpy( &m_abyMemberType, &vmpModule.m_abyMemberType, MAXNUM_MPUSVMP_MEMBER );
		memcpy( &m_abyVmpMember, &vmpModule.m_abyVmpMember, MAXNUM_MPUSVMP_MEMBER );
	}

	void GetVmpModule( TVmpModule& vmpModule )
	{
		vmpModule.m_tVMPParam = *static_cast<TVMPParam*>( &m_tVMPParam );
		memcpy( &vmpModule.m_abyMemberType, &m_abyMemberType, MAXNUM_MPUSVMP_MEMBER );
		memcpy( &vmpModule.m_abyVmpMember, &m_abyVmpMember, MAXNUM_MPUSVMP_MEMBER );
	}
}TMcsVmpModule;

struct THduChnnl
{
public:
	u8 m_byChnnlIdx;    //ͨ����
	u8 m_byEqpId; 
	u8 m_byMemberIdx; //���ָ���ĵ���ǽ��Ա������������ʱTMtAlias��������+1)���m_abyTvWallMember[x]��ֵ��193�� ��ô����ʾ��ͨ����һ����������ͨ��
	u8 m_byMemberType;  //����ǽ��Ա��������,TW_MEMBERTYPE_MCSSPEC,TW_MEMBERTYPE_SPEAKER...
	
public:
	THduChnnl( void )
	{
		Clear();
	}
	
	~THduChnnl()
	{
		Clear();
	}
	void SetHduChnnl(u8 byEqpId, u8 byChnnlIdx, u8 byMemberIdx, u8 byMemberType)
	{
		m_byEqpId = byEqpId;
		m_byChnnlIdx = byChnnlIdx;
		m_byMemberIdx = byMemberIdx;
		m_byMemberType = byMemberType;
	}
	void Clear( void ){memset(this,0,sizeof(THduChnnl));}
}
PACKED
;

struct THduVmpOne
{
public:
	THduVmpChnnl m_tHduVmpChnnl;
	THduVmpSubChnnl m_atHduVmpSubChnnl[4];
public:
	THduVmpOne()
	{
		Empty();
	}
	~THduVmpOne()
	{
		Empty();
	}
	void Empty()
	{
		m_tHduVmpChnnl.Clear();
		for(u8 byIndex = 0; byIndex < 4; byIndex++)
		{
			m_atHduVmpSubChnnl[byIndex].Clear();
		}
	}
	void SetHduVmpMember(u8 byHduEqpId, u8 byChnnlIdx, u8 byVmpStyle)
    {	
		m_tHduVmpChnnl.m_byHduEqpId = byHduEqpId;
		m_tHduVmpChnnl.m_byChnnlIdx = byChnnlIdx;
		m_tHduVmpChnnl.m_byVmpStyle = byVmpStyle;		
    }
	void SetHduVmpSubMember(u8 byHduSubEqpId, u8 byChnnlMember, u8 bySubVmpStyle)
    {
		for(u8 byIdx = 0; byIdx < 4; byIdx++)
		{
			if (m_atHduVmpSubChnnl[byIdx].m_byMemberType == 0)
			{
				m_atHduVmpSubChnnl[byIdx].m_bySubChnnlIdx = byHduSubEqpId;
				m_atHduVmpSubChnnl[byIdx].m_byMember = byChnnlMember;
				m_atHduVmpSubChnnl[byIdx].m_byMemberType = bySubVmpStyle;
				break;
			}
		}		
    }
	
	void GetHduVmpSubMember(u8 &byIndex , u8 &byChnnlMember, u8 &byMemberType)
    {	
		for(u8 byIdx = 0; byIdx < 4; byIdx++)
		{
			if (m_atHduVmpSubChnnl[byIdx].m_byMemberType != 0)
			{ 
				if (m_atHduVmpSubChnnl[byIdx].m_bySubChnnlIdx == byIndex)
				{
					byChnnlMember = m_atHduVmpSubChnnl[byIdx].m_byMember;
					byMemberType = m_atHduVmpSubChnnl[byIdx].m_byMemberType;
					break;
				}
			}	
		}
    }

	BOOL32 SetHduSubVmpMemberEx(THduVmpSubChnnl* tHduVmpSubChnnl, u8 byCapNum )
	{
		for(u8 byIdx = 0; byIdx < 4; byIdx++)
		{
			m_atHduVmpSubChnnl[byIdx].Clear();
		}
		memcpy((u8*)&m_atHduVmpSubChnnl[0],tHduVmpSubChnnl,byCapNum*sizeof(THduVmpSubChnnl));
		return TRUE;
	}
	u8 GetHduVmpMemberEx(THduVmpSubChnnl* tHduVmpSubChnnl)const
	{
		u8 byNum = 0;
		if (tHduVmpSubChnnl == NULL)
		{
			StaticLog("[GetVmpMemberEx] invalid param!\n");
			return 0;
		}
		for(u8 byIdx = 0; byIdx < 4; byIdx++)
		{
			if(m_atHduVmpSubChnnl[byIdx].m_byMemberType != 0)
			{
				tHduVmpSubChnnl[byIdx] = m_atHduVmpSubChnnl[byIdx];
				byNum++;
			}	
		}
		return byNum;
	}
	
}
PACKED
;

struct THduVmpTotalEx
{
public:
	THduVmpOne m_atHduVmpOne[MAX_HDU_VMP_NUM];
public:
	THduVmpTotalEx()
	{
		Empty();
	}
	~THduVmpTotalEx()
	{
		Empty();
	}
	void Empty()
	{
		for(u8 byIndex = 0; byIndex < MAX_HDU_VMP_NUM; byIndex++)
		{
			m_atHduVmpOne[byIndex].Empty();
		}
	}
	void SetHduVmp(THduVmpOne& tHduVmpOne)
    {
		for(u8 byIdx = 0; byIdx < MAX_HDU_VMP_NUM; byIdx++)
		{
			if (m_atHduVmpOne[byIdx].m_tHduVmpChnnl.m_byHduEqpId == 0)
			{
				m_atHduVmpOne[byIdx] = tHduVmpOne;
				break;
			}
		}		
    } 
	void SetHduVmpChnnl(THduVmpChnnl* tHduVmpChnnl)
	{
		for(u8 byNum = 0; byNum < MAX_HDU_VMP_NUM; byNum++)
		{
			if (m_atHduVmpOne[byNum].m_tHduVmpChnnl.m_byHduEqpId == 0)
			{
				m_atHduVmpOne[byNum].m_tHduVmpChnnl = *tHduVmpChnnl;
				break;
			}
		}
	}

	void SetHduVmpSubChnnl(THduVmpSubChnnl* tHduVmpSubChnnl, u8 byCapNum )
	{
		for(s32 nNum = MAX_HDU_VMP_NUM - 1; nNum >= 0; nNum--)
		{
			if (m_atHduVmpOne[nNum].m_tHduVmpChnnl.m_byHduEqpId != 0)
			{
				m_atHduVmpOne[nNum].SetHduSubVmpMemberEx(tHduVmpSubChnnl, byCapNum);
				break;
			}
		}
	}

	u8 GetHduVmp(THduVmpOne* tHduVmpOne)const
	{
		u8 byNum = 0;
		if (tHduVmpOne == NULL)
		{
			StaticLog("[GetVmpMemberEx] invalid param!\n");
			return 0;
		}
		for(u8 byIdx = 0; byIdx < MAX_HDU_VMP_NUM; byIdx++)
		{
			if(m_atHduVmpOne[byIdx].m_tHduVmpChnnl.m_byHduEqpId != 0)
			{
				tHduVmpOne[byIdx] = m_atHduVmpOne[byIdx];
				byNum++;
			}	
		}
		return byNum;
	}
}
PACKED
;

//added by spriner 20100518 �¼�mcu�Ƿ�֧�ֶ�ش�
//���͸��ϲ����Ϣ�ṹ������Ϣ��WPARAM��ָ��˽ṹ��ָ��
typedef struct tagMcsParam
{
    tagMcsParam() : m_pbyMsgBody(NULL), m_nMsgBodySize(0), m_dwErrorCode(0),
		m_bSelfProduce(FALSE) {}
    u8      *m_pbyMsgBody;          //��Ϣ��
    s32     m_nMsgBodySize;         //��Ϣ�峤��
    u32     m_dwErrorCode;          //������
    BOOL32  m_bSelfProduce;         //�Ƿ��ɴ˻���������Ϣ(Ӧ��ûʲô��)
} TMcsParam;

//��ز����ṹ
typedef struct tagMonitorParam : public TMt
{
    tagMonitorParam()
    {
        ZeroMemory( this, sizeof (tagMonitorParam) );
    }
    u32             m_dwLocalIp;        //  ����IP
    TMt             m_tVideoMt;         //  �����ƵԴ
    TMt             m_tAudioMt;         //  �����ƵԴ
    u16             m_wLocalPort;       //  ���������˿�
    u8              m_byReal;           //  0-��֡���, 1-ʵʱ���
    u8              m_byQuiet;          //  0-������, 1-����
    u8              m_byMode;           //  MODE_BOTH/MODE_VIDEO/MODE_AUDIO
    u8              m_byReserved;       //  �����ֶ�
    CConfId         m_cConfId;          //  �����
    TMediaEncrypt   m_tVideoMedia;      //  ��Ƶ����
    TDoublePayload  m_tVideoPayload;    //  ��Ƶ�غ�
    TMediaEncrypt   m_tAudioMedia;      //  ��Ƶ����
    TDoublePayload  m_tAudioPayload;    //  ��Ƶ�غ�
    TAudAACCap      m_tAudAACCap;       //  AAC�Ĳ���
	TTransportAddr  m_tVideoTransportAddr;   //  ����RCTP�˿ں�IP
	TTransportAddr  m_tAudioTransportAddr;   //  ��ƵRCTP�˿ں�IP
} TMonitorParam;

//�л���ش�����Ϣ
typedef struct tagSwitchMonitor : public TMt
{
    tagSwitchMonitor()
    {
        ZeroMemory( this, sizeof (tagSwitchMonitor) );
    }
	
    u8      m_byIndex;
    u8      m_byReal;
    u8      m_byQuiet;
    u8      m_byMode;
    CConfId m_cConfId;          //  �����
} TSwitchMonitor;

//¼���ļ��б��ṹ
typedef struct tagRecFileInfo
{
public:
/*=============================================================================
�� �� ��:tagFileInfo
��    ��:���캯��
��    ��:��
ע    ��:��
�� �� ֵ:��
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/30  4.0     ���    ����
    =============================================================================*/
    tagRecFileInfo() { Reset(); }
	
    /*=============================================================================
    �� �� ��:GetFileNum
    ��    ��:��ȡ�ļ�����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:�ļ���
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/30  4.0     ���    ����
    =============================================================================*/
    s32 inline GetFileNum(void) const { return m_nFileNum; }
	
    /*=============================================================================
    �� �� ��:IsPublic
    ��    ��:�ж�ָ����ŵ��ļ��Ƿ񷢲�
    ��    ��:s32 nIndex                     [in]    �ļ����
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/30  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsPublic(s32 nIndex) const;
	
    /*=============================================================================
    �� �� ��:Reset
    ��    ��:����¼���
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/30  4.0     ���    ����
    =============================================================================*/
    void Reset(void);
	
    /*=============================================================================
    �� �� ��:AddFileName
    ��    ��:�����ļ�
    ��    ��:LPCTSTR pszFileName            [in]    �ļ���
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/30  4.0     ���    ����
    =============================================================================*/
    BOOL32 AddFileName(const s8* pszFileName);
	
    /*=============================================================================
    �� �� ��:DelFileName
    ��    ��:ɾ��¼���ļ�ʱ�����ļ��б�
    ��    ��:LPCTSTR pszFileName            [in]    �ļ���
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
    BOOL32 DelFileName(const s8* pszFileName);
	
    /*=============================================================================
    �� �� ��:Public
    ��    ��:����ָ����ŵ��ķ���
    ��    ��:s32 nIndex                     [in]    �ļ����
	BOOL32 bPublic                 [in]    �Ƿ񷢲�
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
    BOOL32 Public(s32 nIndex, BOOL32 bPublic);
	
public:
    TEqp    m_tRecEqp;                                              //  ¼���
    s8   m_aaszFileArray[MAX_FILE_NUM][MAXLEN_REC_FILE_NAME];    //  �ļ�������
	
private:
    s32     m_nFileNum;                                             //  �ļ���
    u8      m_abyStatus[MAX_FILE_NUM];                              //  �ļ�������ʽ����
} TRecFileInfo;

//mcustatus   mcs�ౣ�������
#ifndef SETBITSTATUS
#define SETBITSTATUS(StatusValue, StatusMask, bStatus)  \
    if (bStatus)    StatusValue |= StatusMask;          \
else            StatusValue &= ~StatusMask;

#define GETBITSTATUS(StatusValue, StatusMask)  (0 != (StatusValue&StatusMask))   
#endif
class CMcsMcuStatus:public TMcu
{
public:
	enum eMcuRunningStatusMask
    {
        Mask_Mp				=  0x00000004,
        Mask_Mtadp			=  0x00000008,
		Mask_HDSC			=  0x00000020,
		Mask_MDSC			=  0x00000040,
		Mask_DSC			=  0x00000080,
		Mask_McuRunOk		=  0x80000000
    };
	
	u8		m_byPeriEqpNum;	                        //������Ŀ
	TEqp	m_atPeriEqp[MAXNUM_MCU_PERIEQP];	    //��������
	u8      m_byEqpOnline[MAXNUM_MCU_PERIEQP];      //�����Ƿ�����
	u32     m_dwPeriEqpIpAddr[MAXNUM_MCU_PERIEQP];  //����Ip��ַ
    
    u8      m_byPeriDcsNum;                         //DCS��Ŀ
    TEqp    m_atPeriDcs[MAXNUM_MCU_DCS];            //DCS����
    u8      m_byDcsOnline[MAXNUM_MCU_DCS];          //DCS�Ƿ�����
    u32     m_dwPeriDcsIpAddr[MAXNUM_MCU_DCS];      //DCS ip��ַ

    // xsl [8/26/2005] 
    u8      m_byOngoingConfNum;                     //mcu�ϼ�ʱ�������
    u8      m_byScheduleConfNum;                    //mcu��ԤԼ�������
    u16     m_wAllJoinedMtNum;                      //mcu����������ն˸���
    
    // ���� [5/29/2006]
    u8      m_byRegedGk;                            //�Ƿ�ɹ�ע��GK�����δ���û���ע��ʧ�ܣ���Ϊ0
	u16     m_wLicenseNum;							//��ǰLicense��
    TMSSynState m_tMSSynState;                      //��ǰ�������(������������Ч)
    
    // guzh [9/4/2006] 
    u8      m_byRegedMpNum;                         //��ǰ��ע���Mp����
    u8      m_byRegedMtAdpNum;                      //��ǰ��ע���H323 MtAdp����

    // guzh  [12/15/2006]
    u8      m_byNPlusState;                         //���ݷ���������״̬(mcuconst.h NPLUS_BAKSERVER_IDLE ��)
    u32     m_dwNPlusReplacedMcuIp;                 //������ݷ��������ڽ��湤���������ķ�����IP

	//zgc [12/21/2006]
	u8      m_byMcuIsConfiged;						//MCU�Ƿ����ù��ı�ʶ
	//zgc [07/26/2007]
	u8		m_byMcuCfgLevel;						// MCU������Ϣ�Ŀɿ�����
	
	//zgc [07/25/2007]
	//[31��24 23��16 15��8 7��0]
	//bit 2: no mp? ([0]no; [1]yes)
	//bit 3: no mtadp? ([0]no; [1]yes)
	//bit 4: no HDCS module?  ([0]no; [1]yes)
	//bit 5: no MDCS module?  ([0]no; [1]yes)
	//bit 7: no DSC module? ([0]no; [1]yes)
	//bit 31: MCU��ǰ�Ƿ���������? ([0]no; [1]yes)
	u32		m_dwMcuRunningState;					// MCU����״̬
	

protected:
    u32     m_dwPersistantRunningTime;              // MCU����ʱ�䣨��λ:s, linux����:497day��vx����:828day��
	u16     m_wVcsAccessNum;  //VCS��Ȩ��
public:
	// xliang [11/20/2008] 
	u16		m_wAllHdiAccessMtNum;					// HDI��Ȩ����MT��������,����8000E��˵�����ֶα��������������
//	u16		m_wStdCriAccessMtNum;					// ��������������ݲ�֧�֣�Ԥ����
	u16		m_wAccessPCMtNum;						// ����Ԥ��תΪPCMT�������� // [3/10/2010 xliang] 	
	
public:
	void SetVcsAccessNum(u16 wVcsAccessNum)
	{
		m_wVcsAccessNum = wVcsAccessNum;
	}
	u16 GetVcsAccessNum()
	{
		return m_wVcsAccessNum;
	}
	u8 GetPeriEqpNum()
	{
		return m_byPeriEqpNum;
	}

	void SetIsExistMp( BOOL32 IsExistMp ) 
	{ 
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_Mp, IsExistMp) 
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMp(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_Mp);
	}
	void SetIsExistMtadp( BOOL32 IsExistMtadp ) 
	{ 
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_Mtadp, IsExistMtadp)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMtadp(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_Mtadp);
	}
	void SetIsExistDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_DSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_DSC); 
	}
	void SetIsExistMDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_MDSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_MDSC); 
	}
	void SetIsExistHDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_HDSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistHDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_HDSC); 
	}
	void SetIsMcuRunOk( BOOL32 IsMcuRunOk ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_McuRunOk, IsMcuRunOk)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsMcuRunOk(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_McuRunOk);
	}
    void SetPersistantRunningTime(u32 dwTime) { m_dwPersistantRunningTime = htonl(dwTime);    }
    u32  GetPersistantRunningTime(void) const { return ntohl(m_dwPersistantRunningTime);    }

    void Print(void) const
    {
        OspPrintf(TRUE, FALSE, "MCU Current Status: \n" );
        OspPrintf(TRUE, FALSE, "\tIs Run OK:%d\n", IsMcuRunOk() );
        OspPrintf(TRUE, FALSE, "\tExist mp: %d, Exist mtadp: %d, Exist dsc module<DSC.%d, MDSC.%d, HDSC.%d>\n",
				IsExistMp(), IsExistMtadp(), IsExistDSC(), IsExistMDSC(), IsExistHDSC());

        OspPrintf(TRUE, FALSE, "\tConfig file state: ");
        switch(m_byMcuCfgLevel) 
        {
        case MCUCFGINFO_LEVEL_NEWEST:
            OspPrintf(TRUE, FALSE, "Success\n");
            break;
        case MCUCFGINFO_LEVEL_PARTNEWEST:
            OspPrintf(TRUE, FALSE, "Partly success\n");
            break;
        case MCUCFGINFO_LEVEL_LAST:
            OspPrintf(TRUE, FALSE, "Read fail\n");
            break;
        case MCUCFGINFO_LEVEL_DEFAULT:
            OspPrintf(TRUE, FALSE, "No cfg file\n");
            break;
        default:
            OspPrintf(TRUE, FALSE, "level error!\n");
            break;
		}
        OspPrintf(TRUE, FALSE, "\tIs Mcu Configed: %d\n", m_byMcuIsConfiged);

        OspPrintf(TRUE, FALSE, "\tIsReggedGk:%d, Mp Num:%d, H323MtAdp Num:%d, PeirEqpNum:%d, DcsNum:%d\n", 
                  m_byRegedGk, m_byRegedMpNum, m_byRegedMtAdpNum, m_byPeriEqpNum, m_byPeriDcsNum);
        
        OspPrintf(TRUE, FALSE, "\tOngoingConf:%d, ScheduleConf:%d, AllJoinedMt:%d\n", 
                  m_byOngoingConfNum, m_byScheduleConfNum, ntohs(m_wAllJoinedMtNum));

        if ( m_byNPlusState != NPLUS_NONE )
        {
            OspPrintf(TRUE, FALSE, "\tNPlusState:%d, NPlus Replaced Mcu:0x%x\n", 
                m_byNPlusState, ntohl(m_dwNPlusReplacedMcuIp));
        }
        
        if (!m_tMSSynState.IsSynSucceed() )
        {
            OspPrintf(TRUE, FALSE, "\tMS conflict entity ID:%d, Type:%d\n", 
                      m_tMSSynState.GetEntityId(), m_tMSSynState.GetEntityType() );
        }
        {
            u32 dwPersistantTime = GetPersistantRunningTime();

            u32 dwCutOffTime = 0;
            u32 dwDay = dwPersistantTime/(3600*24);
            dwCutOffTime += (3600*24) * dwDay;
            u32 dwHour = (dwPersistantTime - dwCutOffTime)/3600;
            dwCutOffTime += 3600 * dwHour;
            u32 dwMinute = (dwPersistantTime - dwCutOffTime)/60;
            dwCutOffTime += 60 * dwMinute;
            u32 dwSecond = dwPersistantTime - dwCutOffTime;
            if ( 0 == dwDay )
            {
                OspPrintf(TRUE, FALSE, "\tPersistant running time: %d.h %d.m %d.s\n",
                                         dwHour, dwMinute, dwSecond );                
            }
            else
            {
                OspPrintf(TRUE, FALSE, "\tPersistant running time: %d.day %d.h %d.m %d.s\n",
                                         dwDay, dwHour, dwMinute, dwSecond );                
            }
        }
		// xliang [11/20/2008] HDI �����ն�����
		OspPrintf(TRUE, FALSE, "\tHDI access Mt Num: %d\n", ntohs(m_wAllHdiAccessMtNum));
		OspPrintf(TRUE, FALSE, "\taccess PCMt Num: %d\n", ntohs(m_wAccessPCMtNum));
    }
};

//TMcuExt�̳���TMcu, ��¼MCU��Ϣ
typedef struct tagMcuExt : public TMcu
{
    tagMcuExt() : m_byCpuUsage(0) {}
	
    /*=============================================================================
    �� �� ��:Print
    ��    ��:��telnet���ӡ������Ϣ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/09/01  4.0     ���    ����
    =============================================================================*/
    void Print(void);
	
    TMtAlias					   m_tMcuAlias;        //  MCU����
//    TMcuStatusAfterV4R6B2          m_tMcuStatus;       //  MCU״̬
	CMcsMcuStatus                  m_tMcuStatus;       //MCU ״̬
    u8							   m_byCpuUsage;       //  CPUʹ����(0-100)
} TMcuExt;

typedef map<u16, TTvWallPollParam >	mapTwPollParam;

//CGetName ��ȡ�ն˳�����
class CGetName
{
public:
	
    enum
    {
        TYPE_NULL = 0,
			MT_MANUFACTURE,             //  �ն˵���������
			BAS_CHANNEL_STATUS,         //  ������ͨ��״̬
			BAS_VIDEO_TYPE,             //  ������ͨ����Ƶ����
			BAS_AUDIO_TYPE,             //  ������ͨ����Ƶ����
    };
	
    /*=============================================================================
    �� �� ��:GetName
    ��    ��:�õ�ĳһ��������Ӧ������
    ��    ��:u8 byValue                         [in]    Ҫȡ�Ĳ���ֵ
	u8 byType                          [in]    ����������
    ע    ��:��
    �� �� ֵ:�����ַ���
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
    static const s8* GetName(u8 byValue, u8 byType);
	
private:
/*=============================================================================
�� �� ��:GetMtManu
��    ��:�õ��ն˵�������������
��    ��:��
ע    ��:��
�� �� ֵ:��
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/26  4.0     ���    ����
    =============================================================================*/
	static const s8* GetMtManu( u8 byValue );
	
    /*=============================================================================
    �� �� ��:GetBasChannelStatus
    ��    ��:�õ�������ͨ��״̬
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	static const s8* GetBasChannelStatus( u8 byValue );
	
    /*=============================================================================
    �� �� ��:GetBasVideoType
    ��    ��:�õ�������ͨ����Ƶ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	static const s8* GetBasVideoType( u8 byValue );
	
    /*=============================================================================
    �� �� ��:GetBasAudioType
    ��    ��:�õ�������ͨ����Ƶ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	static const s8* GetBasAudioType( u8 byValue );
};

//CMtUtility���ն˻���������
class CMtUtility
{
public:
    // Mt��ʶ�ֽ�����Ϊ
    // High |   BYTE   |   BYTE  | Low
    //      |   McuID  |   MtID  |
	
    /*=============================================================================
    �� �� ��:GetMcuId
    ��    ��:����u16�õ�McuID
    ��    ��:u16 wID                            [in]    u16
    ע    ��:��
    �� �� ֵ:McuID
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    static inline u16 GetMcuId(u32 wID);
	
    /*=============================================================================
    �� �� ��:GetMtId
    ��    ��:����u16�õ�MtID
    ��    ��:u16 wID                            [in]    u16
    ע    ��:��
    �� �� ֵ:MtID
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    static inline u8 GetMtId(u32 wID);
	
    /*=============================================================================
    �� �� ��:GetwID
    ��    ��:��TMtת��Ϊu16
    ��    ��:const TMt &tMt                     [in]    TMt
    ע    ��:��
    �� �� ֵ:�ն˱�ʶ��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    static inline u32 GetwID(const TMt &tMt);
	
    /*=============================================================================
    �� �� ��:GetwID
    ��    ��:��McuID��MtID���ת��Ϊu16
    ��    ��:u8 byMcuId                         [in]    McuID
	u8 byMtId                          [in]    MtID
    ע    ��:��
    �� �� ֵ:�ն˱�ʶ��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
	static inline u32 GetwID(u16 byMcuId, u8 byMtId);
	
    /*=============================================================================
    �� �� ��:IsMcu
    ��    ��:�ж��ն��Ƿ���MCU
    ��    ��:const TMt &tMt                     [in]    �ն�
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    static inline BOOL32 IsMcu(const TMt &tMt);
	
    /*=============================================================================
    �� �� ��:IsSMcu
    ��    ��:�ж��ն��Ƿ����¼�MCU
    ��    ��:const TMt &tMt                     [in]    �ն�
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    static inline BOOL32 IsSMcu(const TMt &tMt);
	
    /*=============================================================================
    �� �� ��:IsMMcu
    ��    ��:�ж��ն��Ƿ����ϼ�MCU
    ��    ��:const TMt &tMt                     [in]    �ն�
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    static inline BOOL32 IsMMcu(const TMt &tMt);
	
    /*=============================================================================
    �� �� ��:IsLocalMcu
    ��    ��:�ж��ն��Ƿ��Ǳ���MCU
    ��    ��:const TMt &tMt                     [in]    �ն�
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/13  4.0     ���    ����
    =============================================================================*/
    static inline BOOL32 IsLocalMcu(const TMt &tMt);
	/*=============================================================================
    �� �� ��:IsSMt
    ��    ��:�ж��ն��Ƿ����¼�MCU�ϵ��ն�
    ��    ��:const TMt &tMt                     [in]    �ն�
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/05/29  4.0     �����   ����
    =============================================================================*/
	//	static inline BOOL32 IsSMt( const TMt &tMt, const TMt &tMMcu );
    /*=============================================================================
    �� �� ��:IsMMt
    ��    ��:�ж��ն��Ƿ����ϼ�MCU�ϵ��ն�
    ��    ��:const TMt &tMt                     [in]    �ն�
			 const TMt &tMMcu					[in]	�ϼ�Mcu
			 ע    ��:��
			 �� �� ֵ:BOOL
			 -------------------------------------------------------------------------------
			 �޸ļ�¼:
			 ��      ��  �汾    �޸���  �޸�����
			 2006/05/29  4.0     �����   ����
    =============================================================================*/
	//	static inline BOOL32 IsMMt( const TMt &tMt, const TMt &tMMcu );
	
	/*---------------------------------------------------------------------
	* �� �� ����
	* ��    �ܣ�
	* ����˵����
	* �� �� ֵ��
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/07/16	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
};

#pragma pack(push, 1)

#define     MAC_ADDRlENTH   6  //��¼MAC��ַ��6��U8���
struct TCRIBrdExInfo
{
protected:
	u8				m_byIndex;					    // ������
	u16             m_wTotalAuthMtNum;				// ��������Ȩ��
    u16				m_wAuthHDMtNum;					// ���������Ȩ��
    u8              m_byMAC[MAC_ADDRlENTH];						// mac ��ַ
	u16				m_wAudioMtAccessNum;			// �����������
public:
	u8 GetIndex(void) { return m_byIndex; }

	u16 GetTotalAuthMtNum(void) { return ntohs(m_wTotalAuthMtNum); }

	u16 GetAuthHDMtNum(void) { return ntohs(m_wAuthHDMtNum); }

	void GetMac(u8 byMac[MAC_ADDRlENTH]){ memcpy(byMac, m_byMAC, sizeof(m_byMAC)); }

	u16 GetAudioMtAccessNum(void) { return ntohs(m_wAudioMtAccessNum); }

	void Clear(void) { ZeroMemory(this, sizeof(TCRIBrdExInfo)); }

    TCRIBrdExInfo(void) 
	{
		memset(this, 0, sizeof(TCRIBrdExInfo));
	}
    BOOL32  IsNull(void)
    {
        TCRIBrdExInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TCRIBrdExInfo)));
    }
};

#pragma pack(pop)

// ����������Ϊ��̬���������������Ϳ���ȡ����ַ�� by ZJL
static const u64 MIXER_MASK				= 0x01;
static const u64 TVWALL_MASK			= 0x02;
static const u64 ADAPTER_MASK			= 0x04;
static const u64 VMP_MASK				= 0x08;
static const u64 PRS_MASK				= 0x10;
static const u64 MULTITVWALL_MASK		= 0x20;
static const u64 REC_MASK				= 0x40;        //  ����ר��
static const u64 MDSC_MP_MASK			= 0x80;        //  ����Ϊ8000B DSCģ��
static const u64 MDSC_MTADP_MASK		= 0x100;
static const u64 MDSC_GK_MASK			= 0x200;
static const u64 MDSC_PROXY_MASK		= 0x400;
static const u64 MDSC_DCS_MASK			= 0x800;
static const u64 HD_BAS_MASK			= 0x1000;
static const u64 MPUSVMP_MASK			= 0x2000;       //  ����ΪMPU����ģ�� xts20081215
static const u64 MPUDVMP_MASK			= 0x4000;
static const u64 MPUBAS_MASK			= 0x8000;
static const u64 MPUEVPU_MASK			= 0x10000;
static const u64 MPUEBAP_MASK			= 0x20000;
static const u64 TVSHDU_MASK			= 0x40000;
static const u64 TVSHDU2_MASK			= 0x80000;
static const u64 TVSHDU2_L_MASK			= 0x100000;
static const u64 TVSHDU2_S_MASK			= 0x200000;
static const u64 MIXER_MASK1			= 0x400000;
static const u64 MIXER_MASK2			= 0x800000;
static const u64 MIXER_MASK3			= 0x1000000;
static const u64 TVSHDU1_MASK			= 0x2000000;		//HDU-1
static const u64 TVSHDUL_MASK			= 0x4000000;		//HUD-L
static const u64 MPUBAS2_MASK			= 0x8000000;		//MPU��BAP-2��ģʽ
	//����ΪMPU2����ģʽ wyh20111219
static const u64 MPU2BASICVMP_MASK		= 0x10000000;		//MPU2-VMP
static const u64 MPU2BASICBAP_MASK		= 0x20000000;		//MPU2-BAP
static const u64 MPU2ECARDVMP_MASK		= 0x40000000;		//MPU2(Ecard)-VMP(Enhanced)
static const u64 MPU2ECARDVMP2_MASK		= 0x80000000;		//MPU2(Ecard)-VMP(Basic)*2
static const u64 MPU2ECARDBAP_MASK		= 0x100000000;		//MPU2(Ecard)-BAP(Enhanced)
static const u64 MPU2ECARDBAP2_MASK		= 0x200000000;		//MPU2(Ecard)-BAP(Basic)*2
static const u64 MPU2ECARDDVMP1_MASK	= 0x400000000;
static const u64 MPU2ECARDDBAP1_MASK	= 0x800000000;
static const u64 APU2BAS_MASK			= 0x1000000000;		//APU2(bas)

//CBrdCfg�̳���TBrdCfgInfo, ��¼���弰����������
class CBrdCfg : public TBrdCfgInfo
{
public:
    CBrdCfg() : m_byEqpExist(0) { ZeroMemory( this, sizeof(CBrdCfg) ); }
    ~CBrdCfg() {}
	
public:
/*=============================================================================
�� �� ��:operator TBrdCfgInfo
��    ��:����ǿת
��    ��:��
ע    ��:��
�� �� ֵ:TBrdCfgInfo
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/09/01  4.0     ���    ����
    =============================================================================*/
    inline operator TBrdCfgInfo(void) { return dynamic_cast<TBrdCfgInfo&>(*this); }
	
    /*=============================================================================
    �� �� ��:operator=
    ��    ��:��ֵ������
    ��    ��:��
    ע    ��:��
    �� �� ֵ:this
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/09/01  4.0     ���    ����
    =============================================================================*/
    inline CBrdCfg& operator=(const TBrdCfgInfo &tBrdCfg);
	
    /*=============================================================================
    �� �� ��:HasMixer
    ��    ��:�õ����Ƿ��л�����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    BOOL32 HasMixer(void) const { return ( ( m_byEqpExist & MIXER_MASK ) > 0); }
	
	BOOL32 HasMixer1(void) const { return ( ( m_byEqpExist & MIXER_MASK1 ) > 0); }
	
	BOOL32 HasMixer2(void) const { return ( ( m_byEqpExist & MIXER_MASK2 ) > 0); }
	
	BOOL32 HasMixer3(void) const { return ( ( m_byEqpExist & MIXER_MASK3 ) > 0); }

	// �õ����Ƿ�����Ƶ������
	BOOL32 HasMixBas(void) const { return ( ( m_byEqpExist & APU2BAS_MASK ) > 0); }
    /*=============================================================================
    �� �� ��:HasTvWall
    ��    ��:�õ����Ƿ��е���ǽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    BOOL32 HasTvWall(void) const { return ( ( m_byEqpExist & TVWALL_MASK ) > 0); }
	
    /*=============================================================================
    �� �� ��:HasAdapter
    ��    ��:�õ����Ƿ�������������
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    BOOL32 HasAdapter(void) const { return ( ( m_byEqpExist & ADAPTER_MASK ) > 0); }
	
    /*=============================================================================
    �� �� ��:HasVmp
    ��    ��:�õ����Ƿ��л���ϳ���
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    BOOL32 HasVmp(void) const { return ( ( m_byEqpExist & VMP_MASK ) > 0); }
	//xts20081216
    /*=============================================================================
    �� �� ��:HasMpuSvmp
    ��    ��:�õ����Ƿ��е�vmpģʽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    BOOL32 HasMpuSvmp(void) const { return ( ( m_byEqpExist & MPUSVMP_MASK ) > 0); }
	
    /*=============================================================================
    �� �� ��:HasMpuDvmp
    ��    ��:�õ����Ƿ���˫vmpģʽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    BOOL32 HasMpuDvmp(void) const { return ( ( m_byEqpExist & MPUDVMP_MASK ) > 0); }
	
    /*=============================================================================
    �� �� ��:HasMpuBas
    ��    ��:�õ����Ƿ���BASģʽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    BOOL32 HasMpuBas(void) const { return ( ( m_byEqpExist & MPUBAS_MASK ) > 0); }
	
    BOOL32 HasMpuBas2(void) const { return ( ( m_byEqpExist & MPUBAS2_MASK ) > 0); }
    /*=============================================================================
    �� �� ��:HasMpuEvpu
    ��    ��:�õ����Ƿ���EVPUģʽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    BOOL32 HasMpuEvpu(void) const { return ( ( m_byEqpExist & MPUEVPU_MASK ) > 0); }
	
    /*=============================================================================
    �� �� ��:HasMpuEbap
    ��    ��:�õ����Ƿ���EBAPģʽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    BOOL32 HasMpuEbap(void) const { return ( ( m_byEqpExist & MPUEBAP_MASK ) > 0); }
	
    /*=============================================================================
    �� �� ��:HasMpuHdu
    ��    ��:�õ����Ƿ���HDUģʽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    BOOL32 HasMpuHdu(void) const { return ( ( m_byEqpExist & TVSHDU_MASK ) > 0); }
	
	BOOL32 HasMpuHdu1(void) const { return ( ( m_byEqpExist & TVSHDU1_MASK ) > 0); }
	
	BOOL32 HasMpuHduL(void) const { return ( ( m_byEqpExist & TVSHDUL_MASK ) > 0); }

	BOOL32 HasMpuHdu2(void) const { return ( ( m_byEqpExist & TVSHDU2_MASK ) > 0); }
	
	BOOL32 HasMpuHdu2L(void) const { return ( ( m_byEqpExist & TVSHDU2_L_MASK ) > 0); }

	BOOL32 HasMpuHdu2S(void) const { return ( ( m_byEqpExist & TVSHDU2_S_MASK ) > 0); }
	
	//wyh20111219
    /*=============================================================================
    �� �� ��:HasMpu2Vmp
    ��    ��:�õ����Ƿ���VMPģʽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	BOOL32 HasMpu2Vmp(void) const { return ( ( m_byEqpExist & MPU2BASICVMP_MASK ) > 0); }
	
	/*=============================================================================
    �� �� ��:HasMpu2Bap
    ��    ��:�õ����Ƿ���BAPģʽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	BOOL32 HasMpu2Bap(void) const { return ( ( m_byEqpExist & MPU2BASICBAP_MASK ) > 0); }
	
	/*=============================================================================
    �� �� ��:HasMpu2EcardVmp
    ��    ��:�õ����Ƿ���VMPģʽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	BOOL32 HasMpu2EcardVmp(void) const { return ( ( m_byEqpExist & MPU2ECARDVMP_MASK ) > 0); }
	
	/*=============================================================================
    �� �� ��:HasMpu2EcardDVmp
    ��    ��:�õ����Ƿ���˫VMPģʽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	BOOL32 HasMpu2EcardDVmp(void) const { return ( ( m_byEqpExist & MPU2ECARDVMP2_MASK ) > 0); }
	BOOL32 HasMpu2EcardDVmp1(void) const { return ( ( m_byEqpExist & MPU2ECARDDVMP1_MASK ) > 0); }
	
	/*=============================================================================
    �� �� ��:HasMpu2EcardBap
    ��    ��:�õ����Ƿ���BAPģʽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	BOOL32 HasMpu2EcardBap(void) const { return ( ( m_byEqpExist & MPU2ECARDBAP_MASK ) > 0); }
	
	/*=============================================================================
    �� �� ��:HasMpu2EcardDVmp
    ��    ��:�õ����Ƿ���˫BAPģʽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	BOOL32 HasMpu2EcardDBap(void) const { return ( ( m_byEqpExist & MPU2ECARDBAP2_MASK ) > 0); }
	BOOL32 HasMpu2EcardDBap1(void) const { return ( ( m_byEqpExist & MPU2ECARDDBAP1_MASK ) > 0); }
	
    /*=============================================================================
    �� �� ��:HasPrs
    ��    ��:�õ����Ƿ��ж����ش���
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    BOOL32 HasPrs(void) const { return ( ( m_byEqpExist & PRS_MASK ) > 0); }
	
    /*=============================================================================
    �� �� ��:HasMultiTvWall
    ��    ��:�õ����Ƿ��ж໭�����ǽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/10/19  4.0     ���    ����
    =============================================================================*/
    BOOL32 HasMultiTvWall(void) const
    { return ( ( m_byEqpExist & MULTITVWALL_MASK ) > 0); }
	
    /*=============================================================================
    �� �� ��:SetMixer
    ��    ��:���û���������
    ��    ��:TEqpMixerCfgInfo &tMixerCfg        [in]    ����������
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    void SetMixer( TEqpMixerCfgInfo &tMixerCfg )
    { m_byEqpExist |= MIXER_MASK; m_tMixerCfg = tMixerCfg; }
	
    void SetMixer1( TEqpMixerCfgInfo &tMixerCfg )
    { m_byEqpExist |= MIXER_MASK1; m_tMixerCfg1 = tMixerCfg; }
	void SetMixer2( TEqpMixerCfgInfo &tMixerCfg )
    { m_byEqpExist |= MIXER_MASK2; m_tMixerCfg2 = tMixerCfg; }
	void SetMixer3( TEqpMixerCfgInfo &tMixerCfg )
    { m_byEqpExist |= MIXER_MASK3; m_tMixerCfg3 = tMixerCfg; }
	// ������Ƶ������
	void SetMixBas( TEqpMpuBasCfgInfo &tMixerBasCfg )
	{ m_byEqpExist |= APU2BAS_MASK; m_tMixerBasCfg = tMixerBasCfg; }
	
    /*=============================================================================
    �� �� ��:SetTvWall
    ��    ��:���õ���ǽ����
    ��    ��:TEqpTvWallCfgInfo &tTvWallCfg      [in]    ����ǽ����
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    void SetTvWall( TEqpTvWallCfgInfo &tTvWallCfg )
    { m_byEqpExist |= TVWALL_MASK; m_tTvWallCfg = tTvWallCfg; }
	
    /*=============================================================================
    �� �� ��:SetAdapter
    ��    ��:������������������
    ��    ��:TEqpBasCfgInfo &tAdapterCfg        [in]    ��������������
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    void SetAdapter( TEqpBasCfgInfo &tAdapterCfg )
    { m_byEqpExist |= ADAPTER_MASK; m_tAdapterCfg = tAdapterCfg; }
	
    /*=============================================================================
    �� �� ��:SetVmp
    ��    ��:���û���ϳ�������
    ��    ��:TEqpVmpCfgInfo &tVmpCfg            [in]    ����ϳ�������
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    void SetVmp( TEqpVmpCfgInfo &tVmpCfg )
    { m_byEqpExist |= VMP_MASK; m_tVmpCfg = tVmpCfg; }
	
	//xts20081216
    /*=============================================================================
    �� �� ��:SetSvmp
    ��    ��:���õ�vmpģʽ
    ��    ��:TEqpSvmpCfgInfo &tVmpCfg            [in]    ��vmpģʽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    void SetSvmp( TEqpSvmpCfgInfo &tVmpCfg )
    { m_byEqpExist |= MPUSVMP_MASK; m_tSvmpCfg = tVmpCfg; }
    /*=============================================================================
    �� �� ��:SetDvmp
    ��    ��:����˫vmpģʽ
    ��    ��:TEqpDvmpCfgInfo &tVmpCfg            [in]    ˫vmpģʽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    void SetDvmp( TEqpDvmpCfgInfo &tVmpCfg )
    { m_byEqpExist |= MPUDVMP_MASK; m_tDvmpCfg = tVmpCfg; }
    /*=============================================================================
    �� �� ��:SetMpuBas
    ��    ��:����MPUBASģʽ
    ��    ��:TEqpMpuBasCfgInfo &tVmpCfg            [in]    MPUBASģʽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    void SetMpuBas( TEqpMpuBasCfgInfo &tVmpCfg )
    { m_byEqpExist |= MPUBAS_MASK; m_tMpuBasCfg = tVmpCfg; }
    void SetMpuBas2( TEqpMpuBasCfgInfo &tVmpCfg )
    { m_byEqpExist |= MPUBAS2_MASK; m_tMpuBasCfg = tVmpCfg; }
	
	
    /*=============================================================================
    �� �� ��:SetEvpu
    ��    ��:����EVPUģʽ
    ��    ��:TEqpEvpuCfgInfo &tVmpCfg            [in]    EVPUģʽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    void SetEvpu( TEqpEvpuCfgInfo &tVmpCfg )
    { m_byEqpExist |= MPUEVPU_MASK; m_tEvpuCfg = tVmpCfg; }
    /*=============================================================================
    �� �� ��:SetEbap
    ��    ��:����EBAPģʽ
    ��    ��:TEqpEbapCfgInfo &tVmpCfg            [in]    EBAPģʽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    void SetEbap( TEqpEbapCfgInfo &tVmpCfg )
    { m_byEqpExist |= MPUEBAP_MASK; m_tEbapCfg = tVmpCfg; }
	
    /*=============================================================================
    �� �� ��:SetEbap
    ��    ��:����EBAPģʽ
    ��    ��:TEqpEbapCfgInfo &tVmpCfg            [in]    EBAPģʽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    void SetHdu( TEqpHduCfgInfo &tVmpCfg )
    { m_byEqpExist |= TVSHDU_MASK; m_tHduCfg = tVmpCfg; }
	
    void SetHdu1( TEqpHduCfgInfo &tVmpCfg )
    { m_byEqpExist |= TVSHDU1_MASK; m_tHduCfg = tVmpCfg; }
	
    void SetHduL( TEqpHduCfgInfo &tVmpCfg )
    { m_byEqpExist |= TVSHDUL_MASK; m_tHduCfg = tVmpCfg; }
	
	void SetHdu2( TEqpHduCfgInfo &tVmpCfg )
	{ m_byEqpExist |= TVSHDU2_MASK; m_tHduCfg = tVmpCfg; }
	
	void SetHdu2L( TEqpHduCfgInfo &tVmpCfg )
	{ m_byEqpExist |= TVSHDU2_L_MASK; m_tHduCfg = tVmpCfg; }

	void SetHdu2S( TEqpHduCfgInfo &tVmpCfg )
	{ m_byEqpExist |= TVSHDU2_S_MASK; m_tHduCfg = tVmpCfg; }

	//wyh20111219
	/*=============================================================================
    �� �� ��:SetMpu2Vmp
    ��    ��:����VMP-BASICģʽ
    ��    ��:TEqpSvmpCfgInfo &tVmpCfg            [in]    VMP-BASICģʽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	void SetMpu2Vmp( TEqpSvmpCfgInfo &tVmpCfg )
	{ 
		m_byEqpExist |= MPU2BASICVMP_MASK; 
		m_tSvmpCfg = tVmpCfg; 
	}
	
	/*=============================================================================
    �� �� ��:SetMpu2Bap
    ��    ��:����BAP-BASICģʽ
    ��    ��:TEqpMpuBasCfgInfo &tVmpCfg            [in]    BAP-BASICģʽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	void SetMpu2Bap( TEqpMpuBasCfgInfo &tVmpCfg )
	{
		m_byEqpExist |= MPU2BASICBAP_MASK;
		m_tMpuBasCfg = tVmpCfg;
	}
	
	/*=============================================================================
    �� �� ��:SetMpu2EcardVmp
    ��    ��:����VMP-ENHANCEDģʽ
    ��    ��:TEqpSvmpCfgInfo &tVmpCfg            [in]    VMP-ENHANCEDģʽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	void SetMpu2EcardVmp( TEqpSvmpCfgInfo &tVmpCfg)
	{
		m_byEqpExist |= MPU2ECARDVMP_MASK;
		m_tSvmpCfg3 = tVmpCfg;
	}
	
	/*=============================================================================
    �� �� ��:SetMpu2EcardDVmp
    ��    ��:����VMP-BASIC*2ģʽ
    ��    ��:TEqpSvmpCfgInfo &tVmpCfg            [in]    VMP-BASIC*2ģʽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	void SetMpu2EcardDVmp( TEqpSvmpCfgInfo &tVmpCfg)
	{
		m_byEqpExist |= MPU2ECARDVMP2_MASK;
		m_tSvmpCfg1 = tVmpCfg;
	}
	void SetMpu2EcardDVmp1( TEqpSvmpCfgInfo &tVmpCfg )
	{
		m_byEqpExist |= MPU2ECARDDVMP1_MASK;
		m_tSvmpCfg2 = tVmpCfg;
	}
	
	/*=============================================================================
    �� �� ��:SetMpu2EcardBap
    ��    ��:����BAP-ENHANCEDģʽ
    ��    ��:TEqpMpuBasCfgInfo &tVmpCfg            [in]    BAP-ENHANCEDģʽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	void SetMpu2EcardBap( TEqpMpuBasCfgInfo &tVmpCfg)
	{
		m_byEqpExist |= MPU2ECARDBAP_MASK;
		m_tMpuBasCfg3 = tVmpCfg;
	}
	
	/*=============================================================================
    �� �� ��:SetMpu2EcardDBap
    ��    ��:����BAP-BASIC*2ģʽ
    ��    ��:TEqpMpuBasCfgInfo &tVmpCfg            [in]    BAP-BASIC*2ģʽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	void SetMpu2EcardDBap( TEqpMpuBasCfgInfo &tVmpCfg)
	{
		m_byEqpExist |= MPU2ECARDBAP2_MASK;
		m_tMpuBasCfg1 = tVmpCfg;
	}
	void SetMpu2EcardDBap1( TEqpMpuBasCfgInfo &tVmpCfg )
	{
		m_byEqpExist |= MPU2ECARDDBAP1_MASK;
		m_tMpuBasCfg2 = tVmpCfg;
	}
	
    /*=============================================================================
    �� �� ��:SetPrs
    ��    ��:���ö����ش�������
    ��    ��:TPrsCfgInfo &tPrsCfg               [in]    �����ش�������
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    void SetPrs( TPrsCfgInfo &tPrsCfg )
    { m_byEqpExist |= PRS_MASK; m_tPrsCfg = tPrsCfg; }
	
    /*=============================================================================
    �� �� ��:SetMultiTvWall
    ��    ��:���ö໭�����ǽ����
    ��    ��:TEqpMTvwallCfgInfo &tMultiTvWallCfg    [in]    �����ش�������
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/10/19  4.0     ���    ����
    =============================================================================*/
    void SetMultiTvWall(TEqpMTvwallCfgInfo &tMultiTvWallCfg)
    { m_byEqpExist |= MULTITVWALL_MASK; m_tMultiTvWallCfg = tMultiTvWallCfg; }
	
    /*=============================================================================
    �� �� ��:GetMixer
    ��    ��:��ȡ����������
    ��    ��:��
    ע    ��:������ڻ�����
    �� �� ֵ:����������
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    TEqpMixerCfgInfo& GetMixer(void) { return m_tMixerCfg; }
	TEqpMixerCfgInfo& GetMixer1(void) { return m_tMixerCfg1; }
	TEqpMixerCfgInfo& GetMixer2(void) { return m_tMixerCfg2; }
	TEqpMixerCfgInfo& GetMixer3(void) { return m_tMixerCfg3; }

	// ��ȡ��Ƶ����������
	TEqpMpuBasCfgInfo& GetMixerBas(void) { return m_tMixerBasCfg; }
	
    /*=============================================================================
    �� �� ��:GetTvWall
    ��    ��:��ȡ����ǽ����
    ��    ��:��
    ע    ��:������ڵ���ǽ
    �� �� ֵ:����ǽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    TEqpTvWallCfgInfo& GetTvWall(void) { return m_tTvWallCfg; }
	
    /*=============================================================================
    �� �� ��:GetAdapter
    ��    ��:��ȡ��������������
    ��    ��:��
    ע    ��:�����������������
    �� �� ֵ:��������������
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    TEqpBasCfgInfo& GetAdapter(void) { return m_tAdapterCfg; }
	
    /*=============================================================================
    �� �� ��:GetVmp
    ��    ��:��ȡ����ϳ�������
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:����ϳ�������
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    TEqpVmpCfgInfo& GetVmp(void) { return m_tVmpCfg; }
	
	//xts20081216
    /*=============================================================================
    �� �� ��:GetSvmp
    ��    ��:��ȡ��vmpģʽ
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:��vmpģʽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    TEqpSvmpCfgInfo& GetSvmp(void) { return m_tSvmpCfg; }
	
    /*=============================================================================
    �� �� ��:GetDvmp
    ��    ��:��ȡ˫vmpģʽ
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:˫vmpģʽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    TEqpDvmpCfgInfo& GetDvmp(void) { return m_tDvmpCfg; }
	
    /*=============================================================================
    �� �� ��:GetMpuBas
    ��    ��:��ȡMPUBASģʽ
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:MPUBASģʽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    TEqpMpuBasCfgInfo& GetMpuBas(void) { return m_tMpuBasCfg; }
	
    TEqpMpuBasCfgInfo& GetMpuBas2(void) { return m_tMpuBasCfg; }
    /*=============================================================================
    �� �� ��:GetEvpu
    ��    ��:��ȡEVPUģʽ
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:EVPUģʽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    TEqpEvpuCfgInfo& GetEvpu(void) { return m_tEvpuCfg; }
	
    /*=============================================================================
    �� �� ��:GetEbap
    ��    ��:��ȡEBAPģʽ
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:EBAPģʽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    TEqpEbapCfgInfo& GetEbap(void) { return m_tEbapCfg; }
	
    /*=============================================================================
    �� �� ��:GetHdu
    ��    ��:��ȡHDUģʽ
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:HDUģʽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2008/12/16  4.0     ��̫��    ����
    =============================================================================*/
    TEqpHduCfgInfo& GetHdu(void) { return m_tHduCfg; }
    TEqpHduCfgInfo& GetHdu1(void) { return m_tHduCfg; }
	TEqpHduCfgInfo& GetHduL(void) { return m_tHduCfg; }
	//wyh20111219
	/*=============================================================================
    �� �� ��:GetMpu2Vmp
    ��    ��:��ȡVMP-BASICģʽ
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:VMP-BASICģʽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	TEqpSvmpCfgInfo& GetMpu2Vmp(void) { return m_tSvmpCfg; }
	
	/*=============================================================================
    �� �� ��:GetMpu2EcardVmp
    ��    ��:��ȡVMP-ENHANCEDģʽ
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:VMP-ENHANCEDģʽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	TEqpSvmpCfgInfo& GetMpu2EcardVmp(void) { return m_tSvmpCfg3; }
	
	/*=============================================================================
    �� �� ��:GetMpu2EcardVmp
    ��    ��:��ȡVMP-BASIC*2ģʽ
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:VMP-BASIC*2ģʽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	TEqpSvmpCfgInfo& GetMpu2EcardDVmp(void) { return m_tSvmpCfg1; }
	TEqpSvmpCfgInfo& GetMpu2EcardDVmp1(void) { return m_tSvmpCfg2; }
	
	/*=============================================================================
    �� �� ��:GetMpu2Bap
    ��    ��:��ȡBAP-BASICģʽ
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:BAP-BASICģʽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	TEqpMpuBasCfgInfo& GetMpu2Bap(void) { return m_tMpuBasCfg; }
	
	/*=============================================================================
    �� �� ��:GetMpu2EcardBap
    ��    ��:��ȡBAP-ENHANCEDģʽ
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:BAP-ENHANCEDģʽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	TEqpMpuBasCfgInfo& GetMpu2EcardBap(void) { return m_tMpuBasCfg3; }
	
	/*=============================================================================
    �� �� ��:GetMpu2EcardDBap
    ��    ��:��ȡBAP-BASIC*2ģʽ
    ��    ��:��
    ע    ��:������ڻ���ϳ���
    �� �� ֵ:BAP-BASIC*2ģʽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/12/19  4.7     ���ƻ�    ����
    =============================================================================*/
	TEqpMpuBasCfgInfo& GetMpu2EcardDBap(void) { return m_tMpuBasCfg1; }
	TEqpMpuBasCfgInfo& GetMpu2EcardDBap1(void) { return m_tMpuBasCfg2; }
	
    /*=============================================================================
    �� �� ��:GetPrs
    ��    ��:��ȡ�����ش�������
    ��    ��:��
    ע    ��:������ڶ����ش���
    �� �� ֵ:�����ش�������
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/31  4.0     ���    ����
    =============================================================================*/
    TPrsCfgInfo& GetPrs(void) { return m_tPrsCfg; }
	
    /*=============================================================================
    �� �� ��:GetMultiTvWall
    ��    ��:��ȡ�໭�����ǽ����
    ��    ��:��
    ע    ��:������ڶ໭�����ǽ
    �� �� ֵ:�໭�����ǽ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/10/19  4.0     ���    ����
    =============================================================================*/
    TEqpMTvwallCfgInfo& GetMultiTvWall(void) { return m_tMultiTvWallCfg; }
	
    /*=============================================================================
    �� �� ��:ClearPeriCfg
    ��    ��:���һ�ֻ������������
    ��    ��:u8 byPeriUnion                     [in]    MIXER_MASK | TVWALL_MASK
	| ADAPTER_MASK
	| VMP_MASK | PRS_MASK
	| MULTITVWALL_MASK
    ע    ��:�����ɸ���maskƴ����
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/09/20  4.0     ���    ����
    =============================================================================*/
    void ClearPeriCfg( u64 byPeriUnion , u8 byBrdType = 0);
	
    /*=============================================================================
    �� �� ��:Print
    ��    ��:��telnet���ӡ������Ϣ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/09/01  4.0     ���    ����
    =============================================================================*/
    void Print(void) const;
	
private:
    TEqpMixerCfgInfo    m_tMixerCfg;        //  ����������
	TEqpMixerCfgInfo    m_tMixerCfg1;        //  ����������
	TEqpMixerCfgInfo    m_tMixerCfg2;        //  ����������
	TEqpMixerCfgInfo    m_tMixerCfg3;        //  ����������
	TEqpMpuBasCfgInfo	m_tMixerBasCfg;		//	��Ƶ����������
    TEqpTvWallCfgInfo   m_tTvWallCfg;       //  ����ǽ����
    TEqpBasCfgInfo      m_tAdapterCfg;      //  ��������������
    TEqpVmpCfgInfo      m_tVmpCfg;          //  ����ϳ�������
    TPrsCfgInfo         m_tPrsCfg;          //  �����ش�������
    TEqpMTvwallCfgInfo  m_tMultiTvWallCfg;  //  �໭�����ǽ
    //xts
    TEqpSvmpCfgInfo     m_tSvmpCfg;         //��vmpģʽ
    TEqpDvmpCfgInfo     m_tDvmpCfg;         //˫vmpģʽ
    TEqpMpuBasCfgInfo   m_tMpuBasCfg;       //BASģʽ
    TEqpEbapCfgInfo     m_tEbapCfg;         //EBAPģʽ
    TEqpEvpuCfgInfo     m_tEvpuCfg;         //EVPUģʽ
    TEqpHduCfgInfo      m_tHduCfg;         //HDUģʽ	
	//wyh20111226
	TEqpSvmpCfgInfo		m_tSvmpCfg1;		//VMP-BASIC*2ģʽ
	TEqpSvmpCfgInfo		m_tSvmpCfg2;
	TEqpSvmpCfgInfo		m_tSvmpCfg3;		//VMP-ENHANCED
	TEqpMpuBasCfgInfo	m_tMpuBasCfg1;		//BAP-BASIC*2ģʽ
	TEqpMpuBasCfgInfo   m_tMpuBasCfg2;
	TEqpMpuBasCfgInfo	m_tMpuBasCfg3;		//BAP-ENHANCED
	
    u64                  m_byEqpExist;       //  �õ���������Щ����
};

//CMt�̳���TMt���洢����Mt��������Ϣ
class CMt : public TMt
{
public:
    CMt() { 
		SetNull();
	}
    CMt(const CMt &cMt);
    CMt(const TMtExtCur &tMtExt);
    CMt(const TMtStatus &tMtStatus);
	CMt(const TMt &tMt);
    ~CMt() { }
	
    /*=============================================================================
    �� �� ��:SetNull
    ��    ��:���ն��ÿ�
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    inline void SetNull(void)
	{
		TMt::SetNull();
		m_tMtExt.SetNull();
		m_tMtStatus.SetNull();
		m_tMtAlias.SetNull();
		m_tMtVersion.SetNull();
		m_dwUseStatus = 0;
		m_dwOnlineStatus = 0;
		m_tRecEqp.SetNull();
		ZeroMemory( &m_tRecProg, sizeof(m_tRecProg) );
		m_dwLastRateReqTime = 0;
		m_byUsed = 0;
		m_tMediaSrcMt.SetNull();
		m_byLockStatus = 0;
		m_tMixParam.Clear();
		m_dwEqpStatus = 0;
		m_tMcsMtVideoSrcAlias.SetNull();
	}
	
public:
/*=============================================================================
�� �� ��:operator=
��    ��:ȫ����ֵ
��    ��:const CMt& cMt                     [in]    ȫ���ն���Ϣ
ע    ��:��
�� �� ֵ:this
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
    =============================================================================*/
    inline CMt& operator=(const CMt &cMt);
	
    /*=============================================================================
    �� �� ��:operator=
    ��    ��:�����ṹ��ֵ
    ��    ��:const TMt& tMt                     [in]    �����ն���Ϣ
    ע    ��:��
    �� �� ֵ:this
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    inline CMt& operator=(const TMt &tMt);
	
    /*=============================================================================
    �� �� ��:operator=
    ��    ��:���ֽṹ��ֵ
    ��    ��:const TMtExt &tMtExt               [in]    �����ն���Ϣ
    ע    ��:��
    �� �� ֵ:this
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    inline CMt& operator=(const TMtExtCur &tMtExt);
	
    /*=============================================================================
    �� �� ��:operator=
    ��    ��:���ֽṹ��ֵ
    ��    ��:const TMtStatus &tMtStatus         [in]    �����ն���Ϣ
    ע    ��:��
    �� �� ֵ:this
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/14  4.0     ���    ����
    =============================================================================*/
    inline CMt& operator=(const TMtStatus &tMtStatus);
	
    /*=============================================================================
    �� �� ��:operator TMt
    ��    ��:ת��ΪTMt
    ��    ��:��
    ע    ��:��
    �� �� ֵ:const TMt
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    inline operator const TMt(void) const { return (*this); }
	
    /*=============================================================================
    �� �� ��:operator TMt
    ��    ��:ת��ΪTMt
    ��    ��:��
    ע    ��:��
    �� �� ֵ:TMt
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    inline operator TMt(void) { return (dynamic_cast<TMt&>(*this)); }
	
    /*=============================================================================
    �� �� ��:operator TMtExt
    ��    ��:ת��ΪTMtExt
    ��    ��:��
    ע    ��:��
    �� �� ֵ:const TMtExt
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    inline operator const TMtExtCur(void) const { return m_tMtExt; }
	
    /*=============================================================================
    �� �� ��:operator TMtExt
    ��    ��:ת��ΪTMtExt
    ��    ��:��
    ע    ��:��
    �� �� ֵ:TMtExt
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    inline operator TMtExtCur(void) { return m_tMtExt; }
	
    /*=============================================================================
    �� �� ��:operator TMtStatus
    ��    ��:ת��ΪTMtStatus
    ��    ��:��
    ע    ��:��
    �� �� ֵ:const TMtStatus
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    inline operator const TMtStatus(void) const { return m_tMtStatus; }
	
    /*=============================================================================
    �� �� ��:operator TMtStatus
    ��    ��:ת��ΪTMtStatus
    ��    ��:��
    ע    ��:��
    �� �� ֵ:TMtStatus
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    inline operator TMtStatus(void) { return m_tMtStatus; }
	
    /*=============================================================================
    �� �� ��:operator==
    ��    ��:�Ƚϲ�����
    ��    ��:const TMtStatus &tMtStatus         [in]    TMtStatus
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/27  4.0     ���    ����
    =============================================================================*/
    inline BOOL32 operator==(const TMtStatus &tMtStatus) const;
	
    /*=============================================================================
    �� �� ��:operator==
    ��    ��:�Ƚϲ�����
    ��    ��:const TMtExt &tMtExt               [in]    TMtExt
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/08  4.0     ���    ����
    =============================================================================*/
    inline BOOL32 operator==(const TMtExtCur &tMtExt) const;
	
    /*=============================================================================
    �� �� ��:operator==
    ��    ��:�Ƚϲ�����
    ��    ��:const TMt &tMt                     [in]    TMt
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/09  4.0     ���    ����
    =============================================================================*/
    inline BOOL32 operator==(const TMt &tMt) const;
	
public:
/*=============================================================================
�� �� ��:IsNull
��    ��:�ж��ն��Ƿ�Ϊ��
��    ��:��
ע    ��:��
�� �� ֵ:BOOL
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/05/09  4.0     ���    ����
    =============================================================================*/
    inline BOOL32 IsNull(void) const;
	
    /*=============================================================================
    �� �� ��:GetwID
    ��    ��:��ȡ16λ�ն˱�ʶ��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:�ն˱�ʶ��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    inline u32 GetwID(void) const;
	
    /*=============================================================================
    �� �� ��:SetMt
    ��    ��:����TMt
    ��    ��:const TMt &tMt                     [in]    TMt
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
    inline void SetMt(const TMt &tMt);
	
	/*=============================================================================
    �� �� ��:SetMt
    ��    ��:����TMt
    ��    ��:                    [in]    tMtExt
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	
    /*=============================================================================
    �� �� ��:SetMtStatus
    ��    ��:�����ն�״̬
    ��    ��:const TMtStatus &tMtStatus         [in]    �ն�״̬
    ע    ��:��
    �� �� ֵ:����m_tMtStatus��tMtStatus��һ��:TRUE; ����:FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    inline BOOL32 SetMtStatus(const TMtStatus &tMtStatus);
	
    /*=============================================================================
    �� �� ��:SyncAliasFromExt
    ��    ��:����չ�ṹͬ���ն˱���
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/12/14  4.0     ���    ����
    =============================================================================*/
    void SyncAliasFromExt(void);
	
    /*=============================================================================
    �� �� ��:ClearMtAlias
    ��    ��:����ն˱���
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/12/14  4.0     ���    ����
    =============================================================================*/
    inline void ClearMtAlias(void) { m_tMtAlias.SetNull(); }
	
    /*=============================================================================
    �� �� ��:SetMtAlias
    ��    ��:�����ն˱���
    ��    ��:const TMtAlias &tMtAlias           [in]    �ն˱���
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    2006/01/09  4.0     ����  �޸�
    =============================================================================*/
	void SetMtAlias(const TMtAlias &tMtAlias);
	
    /*=============================================================================
    �� �� ��:SetRecEqp
    ��    ��:����¼���豸
    ��    ��:const TEqp &tRecEqp                [in]    ¼���豸
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 SetRecEqp(const TEqp &tRecEqp);
	
    /*=============================================================================
    �� �� ��:SetRecProg
    ��    ��:����¼�����
    ��    ��:const TRecProg &tRecProg           [in]    ¼�����
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 SetRecProg(const TRecProg &tRecProg);
	
    /*=============================================================================
    �� �� ��:GetMtAlias
    ��    ��:��ȡ�ն˱���
    ��    ��:const mtAliasType &emType          [in]    ��������
    ע    ��:û�и������򷵻ؿ�ֵ
    �� �� ֵ:TMtAlias
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    2006/01/09  4.0     ����  �޸�
    =============================================================================*/
    TMtAlias GetMtAlias(const mtAliasType &emType) const;
	
    /*=============================================================================
    �� �� ��:GetMtAlias
    ��    ��:��ȡ�ն˱���
    ��    ��:��
    ע    ��:H.323 ID / H.320 ID > E.164 ID > IP
    �� �� ֵ:TMtAlias
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    2006/01/09  4.0     ����  �޸�
    =============================================================================*/
    TMtAlias GetMtAlias(void) const;
	
    /*=============================================================================
    �� �� ��:GetMtAliasFromExt
    ��    ��:����չ�ṹ��û�ȡ�ն˱���
    ��    ��:TMtAlias &tMtAlias                 [in]    Ҫ�õ��ն˱���
    ע    ��:
    �� �� ֵ:�õ��ִ�ֵ����TRUE���ִ�Ϊ�շ���FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/12  4.0     ���    ����
    =============================================================================*/
    BOOL32 GetMtAliasFromExt(TMtAlias &tMtAlias) const;
	
    /*=============================================================================
    �� �� ��:GetProtocol
    ��    ��:��ȡ�ն�Э������
    ��    ��:��
    ע    ��:
    �� �� ֵ:u8 Э������ PROTOCOL_TYPE_H323 PROTOCOL_TYPE_H320 PROTOCOL_TYPE_SIP
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/01/20  4.0     ����  �޸�
    =============================================================================*/
    inline u8 GetProtocol(void) const
    {
        return m_tMtExt.GetProtocolType();
    }
	
    /*=============================================================================
    �� �� ��:GetProtocol
    ��    ��:�ն��Ƿ���H320�ն�
    ��    ��:��
    ע    ��:
    �� �� ֵ:TRUE �� H320
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/01/20  4.0     ����  �޸�
    =============================================================================*/
    inline BOOL32 IsProtocolH320() const
    {
        return (GetProtocol() == PROTOCOL_TYPE_H320);
    }
	
	
    /*=============================================================================
    �� �� ��:IsOnline
    ��    ��:�ն��Ƿ�����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsOnline(void) const;
	
    /*=============================================================================
    �� �� ��:SetOnline
    ��    ��:�����Ƿ�����
    ��    ��:BOOL32 bOnLine                     [in]    �Ƿ�����
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
	void SetOnline(BOOL32 bOnLine);
	
    /*========================================================================
	�� �� ��:IsShown
	��    ��:�жϺ��д�����Ϣ�Ƿ���ʾ��
	��    ��:��
	ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
	========================================================================*/
	BOOL32 IsShown(void) const;
	
    /*=============================================================================
    �� �� ��:SetShown
    ��    ��:���øú��д�����Ϣ�Ƿ���ʾ��
    ��    ��:BOOL32 bShown                      [in]	�Ƿ���ʾ��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    void SetShown(BOOL32 bShown);
	
    /*=============================================================================
    �� �� ��:GetErrorCode
    ��    ��:��ȡ������Ϣ��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:������Ϣ��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    u16 GetErrorCode(void) const { return HIWORD(m_dwOnlineStatus); }
	
    /*=============================================================================
    �� �� ��:SetErrorCode
    ��    ��:���ô�����Ϣ��
    ��    ��:u16 wErrorCode                     [in]    ������Ϣ��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    void SetErrorCode(u16 wErrorCode);
	
    /*=============================================================================
    �� �� ��:GetRecEqp
    ��    ��:��ȡ¼���豸
    ��    ��:��
    ע    ��:��
    �� �� ֵ:TEqp
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    TEqp GetRecEqp(void) const { return m_tRecEqp; }
	
    /*=============================================================================
    �� �� ��:GetRecProg
    ��    ��:��ȡ¼�����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:TRecProg
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    TRecProg GetRecProg(void) const { return m_tRecProg; }
	
    /*=============================================================================
    �� �� ��:GetMtStatus
    ��    ��:��ȡ�ն�״̬
    ��    ��:��
    ע    ��:��
    �� �� ֵ:TMtStatus
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    TMtStatus GetMtStatus(void) const { return m_tMtStatus; }
	
    /*=============================================================================
    �� �� ��:GetMtExt
    ��    ��:��ȡ�ն���չ�ṹ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:TMtExt
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/30  4.0     ���    ����
    =============================================================================*/
    TMtExtCur GetMtExt(void) const { return m_tMtExt; }
	
    /*=============================================================================
    �� �� ��:IsStatusDumb
    ��    ��:�ж��ն��Ƿ�����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsStatusDumb(void) const;
	
    /*=============================================================================
    �� �� ��:SetStatusDumb
    ��    ��:�����ն��Ƿ�����
    ��    ��:BOOL32 bDumb                       [in]    �Ƿ�����
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
	//    void SetStatusMute(BOOL32 bDumb);
	
    /*=============================================================================
    �� �� ��:IsStatusQuiet
    ��    ��:�ж��ն��Ƿ���
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsStatusQuiet(void) const;
	
    /*=============================================================================
    �� �� ��:SetStatusQuiet
    ��    ��:�����ն��Ƿ���
    ��    ��:BOOL32 bQuiet                      [in]    �Ƿ���
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
	//    void SetStatusQuiet(BOOL32 bQuiet);
	
    /*=============================================================================
    �� �� ��:IsStatusInspect
    ��    ��:�ж��ն��Ƿ�ѡ��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsStatusInspect(void) const;
	
    /*=============================================================================
    �� �� ��:SetStatusInspect
    ��    ��:�����ն��Ƿ�ѡ��
    ��    ��:BOOL32 bInspect                    [in]    �Ƿ�ѡ��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
	//    void SetStatusInspect(BOOL32 bInspect);
	
    /*=============================================================================
    �� �� ��:IsStatusVmp
    ��    ��:�ж��ն��Ƿ���ϳ�
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsStatusVmp(void) const;
	
	
    /*=============================================================================
    �� �� ��:IsStatusRec
    ��    ��:�ж��ն��Ƿ�¼��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsStatusRec(void) const;
	
    /*=============================================================================
    �� �� ��:SetStatusRec
    ��    ��:�����ն��Ƿ�¼��
    ��    ��:BOOL32 bRec                        [in]    �Ƿ�¼��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
	//    void SetStatusRec(BOOL32 bRec);
	
    /*=============================================================================
    �� �� ��:IsStatusTvWall
    ��    ��:�ж��ն��Ƿ�������ǽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsStatusTvWall(void) const;
	
    /*=============================================================================
    �� �� ��:SetStatusTvWall
    ��    ��:�����ն��Ƿ�������ǽ
    ��    ��:BOOL32 bTvWall                     [in]    �Ƿ�������ǽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    void SetStatusTvWall(BOOL32 bTvWall);
	
	/*=============================================================================
    �� �� ��:IsStatusHdu
    ��    ��:�ж��ն��Ƿ�������ǽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/09/30  1.0     �޿���    ����
    =============================================================================*/
    BOOL32 IsStatusHdu(void) const;
	
    /*=============================================================================
    �� �� ��:SetStatusHdu
    ��    ��:�����ն��Ƿ����������ǽ
    ��    ��:BOOL32 bHdu                     [in]    �Ƿ����������ǽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/09/30  1.0     �޿���    ����
    =============================================================================*/
    void SetStatusHdu(BOOL32 bHdu);
	
    /*=============================================================================
    �� �� ��:IsStatusMix
    ��    ��:�ж��ն��Ƿ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsStatusMix(void) const;
	
    /*=============================================================================
    �� �� ��:SetStatusMix
    ��    ��:�����ն��Ƿ����
    ��    ��:BOOL32 bMix                        [in]    �Ƿ����
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    void SetStatusMix(BOOL32 bMix);
	
    /*=============================================================================
    �� �� ��:IsStatusMultiTvWall
    ��    ��:�ж��ն��Ƿ����໭�����ǽ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/10/14  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsStatusMultiTvWall(void) const;
	
    /*=============================================================================
    �� �� ��:SetStatusMultiTvWall
    ��    ��:�����ն��Ƿ����໭�����ǽ
    ��    ��:BOOL32 bMultiTvWall                [in]    �Ƿ����໭�����ǽ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/10/14  4.0     ���    ����
    =============================================================================*/
    void SetStatusMultiTvWall(BOOL32 bMultiTvWall);
	
    /*=============================================================================
    �� �� ��:IsStatusDual
    ��    ��:�ж��ն��Ƿ��ͷ���˫��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/12/20  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsStatusDual(void) const;
	
    /*=============================================================================
    �� �� ��:IsMcu
    ��    ��:�ն��Ƿ�MCU
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsMcu(void) const;
	
    /*=============================================================================
    �� �� ��:IsMMcu
    ��    ��:�ն��Ƿ��ϼ�MCU
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsMMcu(void) const;
	
    /*=============================================================================
    �� �� ��:IsSMcu
    ��    ��:�ն��Ƿ��¼�MCU
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsSMcu(void) const;
	
    /*=============================================================================
    �� �� ��:IsLocalMcu
    ��    ��:�ж��ն��Ƿ��Ǳ���MCU
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/13  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsLocalMcu(void) const;
	
    /*=============================================================================
    �� �� ��:SetLastRateReqTime
    ��    ��:������һ�η������������ն����ʵ�ʱ��
    ��    ��:u32 dwTime         [in] ʱ��
    ע    ��:
    �� �� ֵ:
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/01/20  4.0     ����  ����
    =============================================================================*/
    void SetLastRateReqTime(u32 dwTime) { m_dwLastRateReqTime = dwTime; }
	
    /*=============================================================================
    �� �� ��:GetLastRateReqTime
    ��    ��:��ȡ��һ�η������������ն����ʵ�ʱ��
    ��    ��:
    ע    ��:
    �� �� ֵ:
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/01/20  4.0     ����  ����
    =============================================================================*/
    inline u32 GetLastRateReqTime() const { return m_dwLastRateReqTime; }
	
    /*=============================================================================
    �� �� ��:GetMtVersion
    ��    ��:��ȡ�ն˰汾��Ϣ
    ��    ��:
    ע    ��:
    �� �� ֵ:
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2007/12/27  4.0     ������  ����
    =============================================================================*/
    inline TMtExt2 GetMtVersion() const { return m_tMtVersion; }
	
    /*=============================================================================
    �� �� ��:SetMtVersion
    ��    ��:�����ն˰汾��Ϣ
    ��    ��:
    ע    ��:
    �� �� ֵ:
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2007/12/27  4.0     ������  ����
    =============================================================================*/
    inline void SetMtVersion( const TMtExt2 &tMtExt2 ) { m_tMtVersion = tMtExt2; }
	
    /*=============================================================================
    �� �� ��:PrintInfo
    ��    ��:��telnet���ӡ������Ϣ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    void PrintInfo(void) const;
	
private:
#define MTSTATUS_QUIET_MASK         (u32)0x00000001
#define MTSTATUS_DUMB_MASK          (u32)0x00000002
#define MTSTATUS_INSPECT_MASK       (u32)0x00000004
#define MTSTATUS_VMP_MASK           (u32)0x00000008
#define MTSTATUS_REC_MASK           (u32)0x00000010
#define MTSTATUS_TVWALL_MASK        (u32)0x00000020
#define MTSTATUS_MIX_MASK           (u32)0x00000040
#define MTSTATUS_MULTITVWALL_MASK   (u32)0x00000080
#define MTSTATUS_DUAL_MASK          (u32)0x00000100
#define MTSTATUS_HDU_MASK			(u32)0x00000200
	
    //���λ���Ƿ����ߣ��ε�λ���Ƿ���ʾ������16λ��������ʱ�Ĵ�����
#define MT_ONLINE_MASK              (u32)0x00000001
#define MT_PROMPT_MASK              (u32)0x00000002
#define MT_ERROR_CODE_MASK          (u32)0xFFFF0000
	
    //��������
#define MT_ALIAS_IP                 (u8)0x01
#define MT_ALIAS_E164               (u8)0x02
#define MT_ALIAS_H323ID             (u8)0x04
#define MT_ALIAS_H320ID             (u8)0x08
#define MT_ALIAS_H320ALIAS          (u8)0x10
	
	
private:
    typedef struct tagMcsMtAlias
    {
        tagMcsMtAlias() { SetNull(); }
		
        inline void SetNull(void) { memset( this, 0, sizeof (tagMcsMtAlias) ); }
		
        u8           m_byAliasType;              //��������(�ϲ��ֶ�)
        s8           m_szAlias[MCS_MAXLEN_ALIAS + 128];    //H.323 ID/H.320 Alias 2010
        s8           m_szE164[MAXLEN_E164];      //E.164
        TTransportAddr  m_tIP;                      //IP
        u8              m_byLayer;                  //���(H320�ն�ר��)
        u8              m_bySlot;                   //�ۺ�(H320�ն�ר��)
        u8              m_byChnnl;                  //ͨ����(H320�ն�ר��)
    } TMcsMtAlias;
	
	
    TMcsMtAlias m_tMtAlias;             //�ն˱���
	//    TMtAlias    m_tMtAlias[3];          //  �ն˱���
    u32         m_dwUseStatus;          //  ʹ��״̬
    u32         m_dwOnlineStatus;       //  ����״̬
    TMtExtCur   m_tMtExt;               //  �ն���չ�ṹ
    TMtExt2     m_tMtVersion;           //  �ն˰汾��Ϣ��
    TMtStatus   m_tMtStatus;            //  �ն�״̬
    TEqp        m_tRecEqp;              //  ¼���豸
    TRecProg    m_tRecProg;             //  ¼�����
	
    u32         m_dwLastRateReqTime;    //  ��¼��һ�η������������ն����ʵ�ʱ��
    u8          m_byUsed;               //  �Ƿ�ռ�ã�����ģ��pcmt��VCS����Ϊ�ն��ϱ�IP����һ���޷����֣��Ӵ��ֶ��������� by xts ���������Ӫ�̣�
	//u8			m_byInVmpMeber;			// ��ʾ��cmt�ǻ���ϳ����ĳ�Ա ��UpdateMtVmpStatus ��UpdateMcuMediaSrc �õ�

	//	����ϳ�״̬
	u32			m_dwEqpStatus;

    //���浱ǰ�ն���ƵԴ����
    TMcsMtVideoSrcAlias     m_tMcsMtVideoSrcAlias;
public:
	TMt&		GetMediaSrc(){ return m_tMediaSrcMt; }
	void		SetMediaSrc( const TMt& tMediaSrc ){ m_tMediaSrcMt = tMediaSrc; }
	u8			GetLockStatus(){ return m_byLockStatus; }
	void		SetLockStatus( u8 byLockStatus ){ m_byLockStatus = byLockStatus; }
	TMixParam&	GetMixParam(){ return m_tMixParam; }
	void		SetMixParam( const TMixParam& tMixParam ){ m_tMixParam = tMixParam; }
    void        SetVideoSrcAlias( const TMcsMtVideoSrcAlias& tMcsMtVideoSrcAlias ) { m_tMcsMtVideoSrcAlias = tMcsMtVideoSrcAlias; }
    const TMcsMtVideoSrcAlias& GetVideoSrcAlias() { return m_tMcsMtVideoSrcAlias; }
	BOOL32		GetVideoSrcAlias( u8 byPort, s8* byAlias, size_t dwMaxLen );
    BOOL32      IsHaveSrcVideoAlias(){ return m_tMcsMtVideoSrcAlias.m_byVideoSrcNum ? TRUE: FALSE; }
    void        SetMtUsed(u8 byUsed){ m_byUsed = byUsed; }
    u8          GetMtUsed() { return m_byUsed; }
	//void        SetMtInVmpMeber(u8 byInVmpMeber){ m_byInVmpMeber = byInVmpMeber; }
    //u8          GetMtInVmpMeber() { return m_byInVmpMeber; }

	void		AddRelatedVmpEqp(u8 byEqpId);
	void		SubRelatedVmpEqp(u8 byEqpId);

private:
	/*=============================================================================
    �� �� ��:SetStatusVmp
    ��    ��:�����ն��Ƿ���ϳ�
    ��    ��:BOOL32 bVmp                        [in]    �Ƿ���ϳ�
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
    void SetStatusVmp(BOOL32 bVmp);
protected:
	// mcu�����������ݣ�mt������
    TMt             m_tMediaSrcMt;      //  ��MCUý��Դ(��������)
    u8              m_byLockStatus;     //  ��MCU�������(��������)
    TMixParam       m_tMixParam;        //  ��MCU��������(��������)	

	//friend void showmtvmpstatus();
	//#ifdef _DEBUG
    /*=============================================================================
    �� �� ��:Dump
    ��    ��:��ӡ����ĳ�Ա������Ϣ
    ��    ��:CDumpContext &dc                   [in]    ��ӡͷ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/25  4.0     ���    ����
    =============================================================================*/
	//    virtual void Dump(CDumpContext &dc) const;
	//#endif  //  _DEBUG
};

void ReplaceOrEraseMtAllVmpStatus(CMt& cOldMt, CMt& cNewMt);

// CConf���浥��������Ϣ
// ---------------------------------------------------------------------------------------------
// �༶������������ by Ĳ��ï 2010-7-10

/*---------------------------------------------------------------------
* ��	����CDataProxy
* ��    �ܣ����ݴ����࣬ת����mcu���͹������������ͣ���ʼ��IMcu�� VecMcuList
* ����˵��������Mcu��CConf�����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/07/16	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
class CConf;
Interface CDataProxy
{
public:
	virtual ~CDataProxy(){}
	static BOOL32 OnDataComing( void* pClass, void* pData );
	static BOOL32 TransData( CConf* pConf, TConfAllMcuInfo& tConfAllMcuInfo );
};

/*-----------------------------------------------------------
mcu _ mt
| _ mcu _ mt
|  _ mcu  
|  _ mt
IMcu ��ʾmt��Ҳ��ʾmcu��ͨ���˴˵�Ƕ�ף��γɼ���
-------------------------------------------------------------*/
typedef IItem< CMt, u32 > IMcu;

class CConf : public TConfInfo
{
	friend class CDataProxy;
protected:
	IMcu* m_piMcu; // �����ն���ɵ���
	mutable TConfAllMtInfo m_tConfAllMtInfo;
	mutable TConfAllMcuInfo m_tConfAllMcuInfo; // mcu������
	u32 m_dwMcuNbr;
public:
	TConfInfoEx		m_tConfInfoEx;						//v4r7����
	TMcsVmpModule	m_tVmpModuleEx;
public:

	//----------------------------------------------------------------------------
	// Ϊ��֤�����ܹ�ͨ�������õĺ���д������
	TplArray<CMt> tplArray;
	const TplArray<CMt>&	GetMtList( u16 dwMcuInd ) const
	{
		return tplArray;
	}
	const TplArray<CMt>&	GetMtList( TMcu& tMcu ) const
	{
		return tplArray;
	}
	
	BOOL32 GetMcuList(u8 *pbyMcuId, s32 &nNum) const { return FALSE; }
	
	BOOL32 IsSMcu(const TMt& tMt) const { return CMtUtility::IsSMcu( tMt ); }
	BOOL32 IsMMcu(const TMt& tMt) const { return CMtUtility::IsMMcu( tMt ); }
	BOOL32 IsMcu(const TMt& tMt) const { return CMtUtility::IsMcu( tMt ); }
	BOOL32 IsLocalMcu( const TMt& tMt) const { return CMtUtility::IsLocalMcu( tMt ); }

	void SetConfInfoEx(const TConfInfoEx &tConfInfoEx) { m_tConfInfoEx = tConfInfoEx; }
	TConfInfoEx& GetConfInfoEx( void ){ return m_tConfInfoEx; }
	
public:
    CConf();
	CConf(const CConf& cConf);
    ~CConf();
public:
/*---------------------------------------------------------------------
* �� �� ����GetMMcu
* ��    �ܣ�����ϼ�mcu���
* ����˵����
* �� �� ֵ���ϼ�mcu���ָ�룬���Ϊ�գ��ϼ�mcu������
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/07/31	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	IMcu* GetMMcu() const;
	/*---------------------------------------------------------------------
	* �� �� ����GetMcuIdx
	* ��    �ܣ����һ��Mcu�Լ����б��е�����
	* ����˵����[in]tMcu ����Ҫ���ҵ�mcu
	* �� �� ֵ��mcu������
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/07/29	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	u16 GetMcuIdx( const TMcu& tMcu ) const;
	
	/*---------------------------------------------------------------------
	* �� �� ����ResetOnlineStatus
	* ��    �ܣ���������mcu�µ��ն�״̬
	* ����˵����[in] wMcuIdx ����Ҫ���µ�mcu
	* �� �� ֵ���Ƿ�����
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/07/27	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	BOOL32 ResetOnlineStatus( u16 wMcuIdx );
	/*---------------------------------------------------------------------
	* �� �� ����SetAllMcuInfo
	* ��    �ܣ���������mcu�б���Ϣ
	* ����˵����[in] tConfAllMcuInfo mcu������
	* �� �� ֵ��
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/07/23	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	BOOL32 SetAllMcuInfo( TConfAllMcuInfo& tConfAllMcuInfo )
	{
		CDataProxy::TransData( this, tConfAllMcuInfo );
		return TRUE;
	}
	
	/*---------------------------------------------------------------------
	* �� �� ����SetAllMtInfoData
	* ��    �ܣ����������ն���Ϣ������mcu��Ҫ�а����ͣ�������Ҫһ��һ�ݵĿ���
	* ����˵����
	* �� �� ֵ��
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/07/23	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	BOOL32 SetAllMtInfoData( u32 dwIndex, void* pData, u32 dwBuffLen );
	
	/*---------------------------------------------------------------------
	* �� �� ����ResetAllMtInConf
	* ��    �ܣ����������նˡ�û���ڻ����о�ɾ�����ڻ����о���������״̬
	* ����˵����
	* �� �� ֵ��
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/07/28	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	void ResetAllMtInConf( IMcu* pMcu );
	
	/*---------------------------------------------------------------------
	* �� �� ����GetBaseMcu
	* ��    �ܣ���������
	* ����˵����
	* �� �� ֵ��
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/07/22	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	IMcu* GetBaseMcu() const
	{
		return m_piMcu;
	}
	
	
	/*---------------------------------------------------------------------
	* �� �� ����HasParentMcu
	* ��    �ܣ��Ƿ��и��ڵ�
	* ����˵����
	* �� �� ֵ��
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/07/22	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	BOOL32 HasParentMcu() const
	{
		return !m_tConfAllMtInfo.m_tMMCU.IsNull();
	}
	
	void PrintAllMtInfo();
	/*---------------------------------------------------------------------
	* �� �� ����GetIMcu
	* ��    �ܣ���Mcu�б�������ָ�������Ĵ���mcu��ָ��IMcu
	* ����˵����[in]wMcuIdx mcu���б��е�����
	* �� �� ֵ��IMcu
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/07/14	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	IMcu* GetIMcu( u16 wMcuIdx ) const;
	
    /*=============================================================================
    �� �� ��:operator=
    ��    ��:��ֵ�����
    ��    ��:const CConf &cConf                 [in]    Դ����
    ע    ��:��
    �� �� ֵ:this
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/07  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    const CConf& operator=(const CConf &cConf);
	
    /*=============================================================================
    �� �� ��:operator+=
    ��    ��:���������, ���µĻ�����Ϣ��������
    ��    ��:const TConfInfo &tConfInfo         [in]    ������Ϣ
    ע    ��:��
    �� �� ֵ:this
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/07  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    const CConf& operator+=(const TConfInfo &tConfInfo);
	
    /*=============================================================================
    �� �� ��:UpdateMtList
    ��    ��:�����ն��б�
    ��    ��:const TMcu &tMcu                   [in]    �б�����MCU
	const TMtExt *ptMtExt              [in]    �ն��б�����
	s32 nNum                           [in]    �ն˸���
    ע    ��:ptMtExt��ĳЩ�ն˿���Ϊ��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
	void UpdateMtList( const TMcu &tMcu, TMtExtCur *ptMtExt, s32 nNum );

    
    /*=============================================================================
    �� �� ��:UpdateMtList
    ��    ��:�����ն��б�
    ��    ��:TMcsMtVideoSrcAliasParam *ptParam           [in]    �ն���ƵԴ�б�
    s32 nNum                                    [in]    �ն˸���
    ע    ��:ptMtExt��ĳЩ�ն˿���Ϊ��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/04/14  4.0     Ϳ�ı�    ����
    =============================================================================*/
    void CConf::UpdateMtList(TMcsMtVideoSrcAliasParam *ptParam, s32 nNum = 1);
   	
	void UpdateMtList( IMcu* pMcu, TMtExtCur *ptMtExt, s32 nNum );

    /*=============================================================================
    �� �� ��:UpdateMtList
    ��    ��:�����ն�״̬�б�
    ��    ��:const TMcu &tMcu                   [in]    �б�����MCU
	const TMtStatus *ptMtStatus        [in]    �ն�״̬�б�����
	s32 nNum                           [in]    �ն˸���
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void UpdateMtList(const TMcu &tMcu, const TMtStatus *ptMtStatus, s32 nNum);
	
	void UpdateMtList(IMcu* pMcu, const TMtStatus *ptMtStatus, s32 nNum );
	
    /*=============================================================================
    �� �� ��:UpdateMtList
    ��    ��:�����ն��б�
    ��    ��:u8 byMcuId                         [in]    �ն�����MCU Id
	TplArray<CMt> &tplMt               [in]    �ն��б�
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/05  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void UpdateMtList( u16 byMcuId, TplArray<CMt> &tplMt );
	
	
    /*=============================================================================
    �� �� ��:GetMcuInfoList
    ��    ��:��ȡMCU��Ϣ�б�
    ��    ��:const TMcu &tMcu                   [in]    �ն�����MCU
    ע    ��:��
    �� �� ֵ:�ն��б�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
	2010/11/17	v4.1	Ĳ��ï	�ڵ��ն˰�����mcuindex��ʵ�������ڵ��
    =============================================================================*/
   	const IMcu*	GetMcuInfoList(const TMcu &tMcu) const
	{
		TMcu *ptMcu = const_cast<TMcu*>(&tMcu);
		u16 wMcuIdx = GetMcuIdx( *ptMcu );
		return GetIMcu( wMcuIdx );
	}
	
    /*=============================================================================
    �� �� ��:GetMcuInfoList
    ��    ��:��ȡMCU��Ϣ�б�
    ��    ��:u8 byMcuId                         [in]    �ն�����MCU��Mcu Id
    ע    ��:��
    �� �� ֵ:�ն��б�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
	const IMcu*	GetMcuInfoList(u16 byMcuId) const
	{
		return GetIMcu( byMcuId );
	}
	
	
    /*=============================================================================
    �� �� ��:GetMtList
    ��    ��:��ȡָ��MCU�ն��б�
    ��    ��:const TMcu &tMcu                   [in]    MCU    [out] tplMt
    ע    ��:��
    �� �� ֵ:�ն��б�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/13  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
	2010/12/15  v4.1    ����    ����GetAllMtList   GetMtList��vector�汾
    =============================================================================*/
	BOOL32	GetMtList( const TMcu& tMcu, TplArray<CMt>& tplMt ) const;
	BOOL32  GetMtListIncludeMcu( IMcu* pMcu, TplArray<CMt>& tplMt ) const;
	BOOL32  GetMtListIncludeMcu( IMcu* pMcu, vector<CMt>& vctMt ) const;
	BOOL32	GetAllMtList( TplArray<CMt>& tplMt ) const;
	BOOL32	GetAllMtList( vector<CMt>& vctMt ) const;
	BOOL32	GetMtList( u16 dwMcuInd, TplArray<CMt>& tplMt ) const;
	BOOL32	GetMtList( u16 dwMcuInd, vector<CMt>& vctMt ) const;
	/*---------------------------------------------------------------------
	* �� �� ����GetMtCount
	* ��    �ܣ�ͨ�������mcu����������ն˵�����������������
	* ����˵����[in] dwMcuId mcu��������
	* �� �� ֵ���ն�����
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/07/16	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	u32 GetMtCount( u16 dwMcuId ) const 
	{
		IMcu* pMcu = GetIMcu( dwMcuId );
		if ( pMcu == NULL )
		{
			return 0;
		}
		return pMcu->GetItemCount();
	}
	
	/*---------------------------------------------------------------------
	* �� �� ����GetMtCount
	* ��    �ܣ�ͨ�������mcu+mtid�Ļ�ϲ���������ն˵�����������������
	* ����˵����[in]dwMtId mcuidx+mcuid�Ļ����
	* �� �� ֵ���ն�����
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/07/16	v1.0		Ĳ��ï		����
	���ô˽ӿڵ�ʱ���мǣ���������mcu�Լ�tmtת�������mtid����Ϊ���id���������mcuidx��ʵ�������ڵ��
	----------------------------------------------------------------------*/
	u32 GetMtCount( u32 dwMtId ) const 
	{
		IMcu* pMcu = GetIMcu( CMtUtility::GetMcuId( dwMtId ) );
		return pMcu->GetItemCount();
	}
	
    /*=============================================================================
    �� �� ��:GetMcuList
    ��    ��:��ȡMCU�б�
    ��    ��:TMcu *ptMcu                        [out]   TMcu����
	s32 &nNum                          [in/out]    �����С/ʵ������С
    ע    ��:��
    �� �� ֵ:�ɹ�:TRUE; ��������С: FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.0	Ĳ��ï	�����ӿڣ���д���д���
	2010/11/17	v4.0	Ĳ��ï	��ýڵ��ն���������mcuindex���ϼ��������Ҫ��ô˽ڵ��index������GetMcuIdx�ӿ�
    =============================================================================*/
	BOOL32 GetMcuList(TMcu *ptMcu, s32 &nNum) const
    { 
		s32 nMcuCount = GetMcuNum();
		if (nNum < nMcuCount)
		{
			return FALSE;
		}	
		
		nNum = nMcuCount;
		for ( s32 nIndex = 0; nIndex < nMcuCount; nIndex++ )
		{
            u16 wMcuIdx = 0;
			// ��ȡmcuָ��
			IMcu* pMcu = GetVisableIMcuByIndex( nIndex, wMcuIdx );//m_piMcuIndex->GetItemByIndex( nIndex );
			
			if ( pMcu != NULL )
			{
				// ��ȡ����mcu��cmt�ṹ
				CMt* pMt = pMcu->GetItemData();	
				if ( pMt != NULL )
				{
					ptMcu[nIndex].SetMcuIdx( GetMcuIdx( *pMt ) );
				}			
			}		
		}	
		return TRUE;
	}
	
    /*=============================================================================
    �� �� ��:GetMcuList
    ��    ��:��ȡMCU�б�
    ��    ��:u8 *pbyMcuId                       [out]   McuId����
	s32 &nNum                          [in/out]    �����С/ʵ������С
    ע    ��:��
    �� �� ֵ:�ɹ�:TRUE; ��������С: FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 GetMcuList(u16 *pbyMcuId, s32 &nNum) const
    { 
		s32 nMcuNum = GetMcuNum();
		if (nNum < nMcuNum)
		{
			return FALSE;
		}
		
		nNum = nMcuNum;
		u32 dwCounts = 0;
		for (s32 nIndex = 0; nIndex < nNum; nIndex ++)
		{
            u16 wMcuIdx = 0;
			IMcu* pMcu = GetVisableIMcuByIndex( nIndex, wMcuIdx );
			if ( pMcu != NULL )
			{
				pbyMcuId[dwCounts++] = wMcuIdx;		
			}		
		}
		
		return TRUE;
	}
	
    /*=============================================================================
    �� �� ��:GetMcuNum
    ��    ��:��ȡMCU����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:MCU����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    s32 GetMcuNum(void) const 
	{ 
		return m_dwMcuNbr;
		//	return m_piMcuIndex->GetItemCount();
	}
	
    /*=============================================================================
    �� �� ��:SetAllMtInfo
    ��    ��:����TConfAllMtInfo
    ��    ��:const TConfAllMtInfo &tConfAllMtInfo   [in]    TConfAllMtInfo
    ע    ��:
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void SetAllMtInfo(const TConfAllMtInfo &tConfAllMtInfo)
    { 
		m_tConfAllMtInfo = tConfAllMtInfo; 
	}
	
    /*=============================================================================
    �� �� ��:GetConfAllMtInfo
    ��    ��:��ȡTConfAllMtInfo
    ��    ��:��
    ע    ��:��
    �� �� ֵ:const TConfAllMtInfo
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    const TConfAllMtInfo& GetConfAllMtInfo(void) const
    { 
		return m_tConfAllMtInfo; 
	}
	
    /*=============================================================================
    �� �� ��:GetConfAllMtInfo
    ��    ��:��ȡTConfAllMtInfo
    ��    ��:��
    ע    ��:��
    �� �� ֵ:TConfAllMtInfo
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    TConfAllMtInfo& GetConfAllMtInfo(void)
    { 
		return m_tConfAllMtInfo; 
	}
	
	/*=============================================================================
    �� �� ��:GetConfAllMtInfo
    ��    ��:��ȡTConfAllMtInfo
    ��    ��:��
    ע    ��:��
    �� �� ֵ:TConfAllMtInfo
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    TConfAllMcuInfo& GetConfAllMcuInfo(void)
    { 
		return m_tConfAllMcuInfo;
	}
	
    /*=============================================================================
    �� �� ��:SetLocalMcuIpAddr
    ��    ��:���ñ���MCU��IP��ַ
    ��    ��:u32 dwMcuIpAddr                    [in]    ����MCU��IP��ַ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void SetLocalMcuIpAddr(u32 dwMcuIpAddr)
    { 
		m_dwLocalMcuIp = dwMcuIpAddr;
	}
	
    /*=============================================================================
    �� �� ��:SetMcuMediaSrc
    ��    ��:����MCUý��Դ
    ��    ��:const TMcu &tMcu                   [in]    TMcu
	const TMt &tMt                     [in]    ý��Դ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    inline void SetMcuMediaSrc(const TMcu &tMcu, const TMt &tMt)
    { 
		SetMcuMediaSrc( tMcu.GetMcuIdx(), tMt );	
	}
	
    /*=============================================================================
    �� �� ��:SetMcuMediaSrc
    ��    ��:����MCUý��Դ
    ��    ��:u16 byMcuId                         [in]    McuId
	const TMt &tMt                     [in]    ý��Դ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
    2006/09/05  4.0     ���    ���⴦��, ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void SetMcuMediaSrc(u16 byMcuId, const TMt &tMt)
	{
		IMcu* pMcu = GetIMcu( byMcuId );
		if ( pMcu != NULL )
		{
			CMt* pMt = pMcu->GetItemData();
			if ( pMt != NULL )
			{
				pMt->SetMediaSrc( tMt );
			}	
		}
	}
	
    /*=============================================================================
    �� �� ��:GetMcuMediaSrc
    ��    ��:��ȡMCUý��Դ
    ��    ��:const TMcu &tMcu                   [in]    MCU
    ע    ��:��
    �� �� ֵ:MCUý��Դ
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
	�мǣ���������TMcu��Ҫ��ʾ��������Ҫ��mcu����Ϣ���������ϼ�mcu����Ϣ�����龡����������һ���ӿ�
    =============================================================================*/
    const TMt& GetMcuMediaSrc(const TMcu &tMcu) const
	{
		return GetMcuMediaSrc( tMcu.GetMcuIdx() );
	}
	
    /*=============================================================================
    �� �� ��:GetMcuMediaSrc
    ��    ��:��ȡMCUý��Դ
    ��    ��:u8 byMcuId                         [in]    McuId
    ע    ��:��
    �� �� ֵ:MCUý��Դ
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    const CMt& GetMcuMediaSrc(u16 byMcuId) const;
	
	
    /*=============================================================================
    �� �� ��:SetMcuLockStatus
    ��    ��:����MCU����״̬
    ��    ��:const TMcu &tMcu                   [in]    MCU
	u8 byLockStatus                    [in]    ����״̬
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void SetMcuLockStatus(const TMcu &tMcu, u8 byLockStatus)
    { 
		IMcu* pMcu = GetIMcu( tMcu.GetMcuIdx() );
		if ( pMcu != NULL )
		{	
			CMt* pMt = pMcu->GetItemData();
			if ( pMt != NULL )
			{
				pMt->SetLockStatus( byLockStatus );
			}		
		}
	}
	
    /*=============================================================================
    �� �� ��:IsMcuLocked
    ��    ��:MCU�Ƿ�����
    ��    ��:const TMcu &tMcu                   [in]    MCU
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsMcuLocked(const TMcu &tMcu) const
    { 
		return IsMcuLocked( tMcu.GetMcuIdx() );
	}
	
    /*=============================================================================
    �� �� ��:IsMcuLocked
    ��    ��:MCU�Ƿ�����
    ��    ��:u8 byMcuId                         [in]    McuId
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsMcuLocked(u16 byMcuId) const;
	
    /*=============================================================================
    �� �� ��:SetMt
    ��    ��:�����ն������ṹ
    ��    ��:const CMt &cMt                     [in]    �ն������ṹ
    ע    ��:��
    �� �� ֵ:�ɹ�����TRUE; ʧ�ܷ���FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 SetMt(const CMt &cMt);
	
    /*=============================================================================
    �� �� ��:SetMt
    ��    ��:�����ն���չ�ṹ
    ��    ��:const TMtExt &tMtExt               [in]    �ն���չ�ṹ
    ע    ��:��
    �� �� ֵ:�ɹ�����TRUE; ʧ�ܷ���FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
    =============================================================================*/
    BOOL32 SetMt(const TMtExtCur &tMtExt);
	
	
    /*=============================================================================
    �� �� ��:SetMt
    ��    ��:�����ն�״̬
    ��    ��:const TMtStatus &tMtStatus         [in]    �ն�״̬
    ע    ��:��
    �� �� ֵ:����CMt���m_tMtStatus��tMtStatus��һ��:TRUE; ����:FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
    =============================================================================*/
    BOOL32 SetMt(const TMtStatus &tMtStatus);
	
	
    /*=============================================================================
    �� �� ��:GetMt
    ��    ��:��ȡ�ն�
    ��    ��:const TMt &tMt                     [in]    �ն�
    ע    ��:��
    �� �� ֵ:�ն������ṹ
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    CMt& GetMt(const TMt &tMt) const;
	
	CMt& GetMt( u16 dwMcuId, u8 wEqpId) const;
	
	CMt& GetMt( u32 dwMtId ) const;
	
    /*=============================================================================
    �� �� ��:GetLocalMcu
    ��    ��:��ȡ��ǰMCU�������ն˽ṹ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��ǰMCU�����ն˽ṹ
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    CMt& GetLocalMcu(void) 
    { 
		// ���ر���mcu
		return m_cLocalMcu;
	}
	
    /*=============================================================================
    �� �� ��:GetMcu
    ��    ��:����TMcu��ȡ��Ӧ��CMt
    ��    ��:const TMcu &tMcu                   [in]    MCU�ṹ
    ע    ��:��
    �� �� ֵ:CMt
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    CMt& GetMcu(const TMcu &tMcu) const
	{
		return GetMcu( tMcu.GetMcuIdx() );
	}
	
    /*=============================================================================
    �� �� ��:GetMcu
    ��    ��:����McuId��ȡ��Ӧ��CMt
    ��    ��:u8 byMcuId                         [in]    McuId
    ע    ��:��
    �� �� ֵ:CMt
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    CMt& GetMcu(u16 byMcuId) const;
	
    /*=============================================================================
    �� �� ��:IsMtInConf
    ��    ��:�ն��Ƿ��������б���
    ��    ��:const TMt &tMt                     [in]    �ն˻����ṹ
    ע    ��:����TConfAllMtInfo���ж�
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsMtInConf(const TMt &tMt) const
    {
		return m_tConfAllMtInfo.MtInConf( tMt.GetMcuIdx(), tMt.GetEqpId() );
	}
	
	BOOL32 IsMtInConf( u16 dwMcuId, u8 wEqpId ) const
    {
		return m_tConfAllMtInfo.MtInConf( dwMcuId, wEqpId );
	}
	
	BOOL32 IsMtInConf( u32 dwMtId ) const
    {
		u16 dwMcuId = CMtUtility::GetMcuId( dwMtId );
		u8  wEqpId	= CMtUtility::GetMtId( dwMtId );
		return m_tConfAllMtInfo.MtInConf( dwMcuId, wEqpId );
	}
	
    /*=============================================================================
    �� �� ��:IsMtInConf
    ��    ��:�ն��Ƿ�������б���
    ��    ��:const CMt &cMt                     [in]    �ն������ṹ
    ע    ��:����TConfAllMtInfo���ж�
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsMtJoinConf(const CMt &cMt) const
    { 
		return m_tConfAllMtInfo.MtJoinedConf( cMt.GetMcuIdx(), cMt.GetEqpId() );
	}
	
	BOOL32 IsMtJoinConf( u16 dwMcuId, u8 wEqpId ) const
    {
		return m_tConfAllMtInfo.MtJoinedConf( dwMcuId, wEqpId );
	}
	
	BOOL32 IsMtJoinConf( u32 dwMtId ) const
    {
		u16 dwMcuId = CMtUtility::GetMcuId( dwMtId );
		u8	wEqpId	= CMtUtility::GetMtId( dwMtId );
		return m_tConfAllMtInfo.MtJoinedConf( dwMcuId, wEqpId );
	}
	
    /*=============================================================================
    �� �� ��:IsMtInConf
    ��    ��:�ն��Ƿ�������б���
    ��    ��:const TMt &tMt                     [in]    �ն˻����ṹ
    ע    ��:����TConfAllMtInfo���ж�
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsMtJoinConf(const TMt &tMt) const
    { 
		return m_tConfAllMtInfo.MtJoinedConf( tMt.GetMcuIdx(), tMt.GetEqpId() );
	}
	
	
    /*=============================================================================
    �� �� ��:IsMtAvailable
    ��    ��:�ж��ն��Ƿ�ɲ���
    ��    ��:const CMt &cMt                     [in]    ��Ҫ�жϵ��ն�
	BOOL32 bIncludeMcu                 [in]    �Ƿ������ն�ΪMCU
    ע    ��:��
    �� �� ֵ:�ն��Ǳ������¼�MCU�µ��ն� : TRUE
	�ն��Ǳ���MCU���ϼ�MCU�µ��ն� : FALSE
	�ն����ϼ����¼�MCU : bIncludeMcu
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/24  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsMtAvaliable(const TMt &tMt, BOOL32 bIncludeMcu = TRUE) const;
	
    /*=============================================================================
    �� �� ��:IsMtInSMcu
    ��    ��:�ж��ն��Ƿ����¼�MCU��
    ��    ��:const CMt &cMt                     [in]    ��Ҫ�жϵ��ն�
	TMcu &tMcu                         [out]   �ն������¼�MCU
	BOOL32 bIncludeMcu                 [in]    �Ƿ�����¼�MCU
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/24  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsMtInSMcu(const TMt &tMt, TMcu &tMcu, BOOL32 bIncludeMcu = TRUE) const;
	
	/*---------------------------------------------------------------------
	* �� �� ����IsMtInMMcu
	* ��    �ܣ��ж��ն��ǲ����ϼ�mcu�µ��ն�
	* ����˵����[in] �ն� [out] tMcu�ϼ��ն�
	* �� �� ֵ���Ƿ����
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/07/30	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
    BOOL32 IsMtInMMcu(const TMt &tMt, TMcu &tMcu ) const;
	
    /*=============================================================================
    �� �� ��:GetOnlineMtNum
    ��    ��:��ȡ�����ն�����
    ��    ��:��
    ע    ��:������MCU, ��TConfAllMtInfo����ȡ
    �� �� ֵ:�����ն�����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/24  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    s32 GetOnlineMtNum(void) const;
	
    /*=============================================================================
    �� �� ��:GetOfflineMtNum
    ��    ��:��ȡ�������ն�����
    ��    ��:��
    ע    ��:������MCU, ��TConfAllMtInfo����ȡ
    �� �� ֵ:�������ն�����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/24  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    s32 GetOfflineMtNum(void) const;
	
    /*=============================================================================
    �� �� ��:GetMtNum
    ��    ��:��ȡ�ն��ܸ���
    ��    ��:��
    ע    ��:������MCU, ��TConfAllMtInfo����ȡ
    �� �� ֵ:�ն��ܸ���
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/15  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    s32 GetMtNum(void) const;
	
    /*=============================================================================
    �� �� ��:GetConfCascadeType
    ��    ��:��ȡ���鼶������
    ��    ��:��
    ע    ��:��
    �� �� ֵ:emSingleConf��emTwoLayerMMcuConf��emTwoLayerSMcuConf��emThreeLayerConf
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    s32 GetConfCascadeType(void) const;
	
    /*=============================================================================
    �� �� ��:IsConfCascade
    ��    ��:�����Ƿ���
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsConfCascade(void) const 
	{ 
		return (GetConfCascadeType() != emSingleConf);
	}
	
    /*=============================================================================
    �� �� ��:SetRecEqp
    ��    ��:���û���¼���豸
    ��    ��:const TEqp &tRecEqp                [in]    ¼���豸
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void SetRecEqp(const TEqp &tRecEqp)
	{
		if (!m_tStatus.IsNoRecording())
		{
			m_tRecEqp = tRecEqp;
		}
	}
	
    /*=============================================================================
    �� �� ��:SetPlayEqp
    ��    ��:���û�������豸
    ��    ��:const TEqp &tPlayEqp               [in]    ¼���豸
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void SetPlayEqp(const TEqp &tPlayEqp)
	{
		if (!m_tStatus.IsNoPlaying())
		{
			m_tPlayEqp = tPlayEqp;
		}
	}
	
    /*=============================================================================
    �� �� ��:GetRecEqp
    ��    ��:��ȡ¼���豸
    ��    ��:��
    ע    ��:��
    �� �� ֵ:¼���豸
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    TEqp& GetRecEqp(void) const 
	{ 
		// conf���д���
		return const_cast<TEqp&>(m_tRecEqp); 
	}
	
    /*=============================================================================
    �� �� ��:GetPlayEqp
    ��    ��:��ȡ�����豸
    ��    ��:��
    ע    ��:��
    �� �� ֵ:�����豸
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    TEqp& GetPlayEqp(void) const { return const_cast<TEqp&>(m_tPlayEqp); }
	
    /*=============================================================================
    �� �� ��:GetSerialNo
    ��    ��:��ȡ�������к�
    ��    ��:��
    ע    ��:������SetItemDataʱ�õ�
    �� �� ֵ:�������к�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    u32 GetSerialNo(void) const { return m_dwSN; }
	
    /*=============================================================================
    �� �� ��:SetSerialNo
    ��    ��:���û������к�
    ��    ��:u32 dwSN                           [in]    ���к�
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void SetSerialNo(u32 dwSN) { m_dwSN = dwSN; }
	
    /*=============================================================================
    �� �� ��:IsLockBySelf
    ��    ��:�����Ƿ񱻵�ǰ�������
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsLockBySelf(void) const { return (m_byLockBySelf > 0); }
	
    /*=============================================================================
    �� �� ��:SetLockBySelf
    ��    ��:���û����Ƿ񱻵�ǰ�������
    ��    ��:BOOL32 bLockBySelf                 [in]    BOOL
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void SetLockBySelf(BOOL32 bLockBySelf)
    { m_byLockBySelf = (bLockBySelf ? true : false); }
	
    /*=============================================================================
    �� �� ��:IsConfOngoing
    ��    ��:�Ƿ�ʱ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsConfOngoing(void) const { return m_tStatus.IsOngoing(); }
	
    /*=============================================================================
    �� �� ��:IsConfTemplate
    ��    ��:�Ƿ����ģ��,�Ѿ�����.CConf���Ǽ�ʱ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsConfTemplate(void) const { return m_tStatus.IsTemplate(); }
	
    /*=============================================================================
    �� �� ��:IsConfScheduled
    ��    ��:�Ƿ�ԤԼ����,�Ѿ�����.CConf���Ǽ�ʱ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/16  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsConfScheduled(void) const { return m_tStatus.IsScheduled(); }
	
    /*=============================================================================
    �� �� ��:GetSpeaker
    ��    ��:��ȡ���鷢����
    ��    ��:bMediaSrc		�Ƿ��Ի�ȡMCUý��Դ
    ע    ��:�����������MCU, ����Ҹ�MCU��ý��Դ
    �� �� ֵ:���鷢����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    CMt GetSpeaker( BOOL bMediaSrc = TRUE ) const;
	
    /*=============================================================================
    �� �� ��:GetChairman
    ��    ��:��ȡ������ϯ
    ��    ��:bMediaSrc		�Ƿ��Ի�ȡMCUý��Դ
    ע    ��:�����ϯ��MCU, ����Ҹ�MCU��ý��Դ
    �� �� ֵ:����ע��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
	2011/10/28	v4.1	�޿���	����
    =============================================================================*/
	CMt GetChairman( BOOL bMediaSrc = TRUE ) const;
	
    /*=============================================================================
    �� �� ��:operator==
    ��    ��:�Ƚ������
    ��    ��:const CConf &cConf                 [in]    ����
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    inline BOOL32 operator==(const CConf &cConf) const
    { return (cConf.GetConfId() == m_cConfId); }
	
    /*=============================================================================
    �� �� ��:SetNull
    ��    ��:���
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    inline void SetNull(void);
	
    /*=======================================================================
    �� �� ��:HasGetInitValue
    ��    ��:�жϸû����Ƿ��MCU����������ʼ��Ϣ, 
	��Щ��ϢΪ��������������ն�״̬�������ն˱�����
	�¼�MCU����״̬���¼�MCU��������
    ��    ��:s32 nType                          [in]    �񵽵���Ϣ����
	emLockInfo/
	emAllMtStatus/
	emAllMtAlias/
	emAllMcuMediaSrc/
	emAllSMcuLockStatus/
	emAllSMcuMixParam/
	emAllInfo
    ע    ��:��
    �� �� ֵ:�Ѿ��õ�������TRUE, û�еõ�������FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =======================================================================*/
    BOOL32 HasGetInitValue(s32 nType);
	
    /*=============================================================================
    �� �� ��:SetGetInitValue
    ��    ��:���øû����Ѿ���MCU����������ʼ��Ϣ, 
	��Щ��ϢΪ��������������ն�״̬�������ն˱������¼�MCU����״̬��
	�¼�MCU��������
    ��    ��:s32 nType                          [in]    �񵽵���Ϣ����
	emLockInfo/
	emAllMtStatus/
	emAllMtAlias/
	emAllMcuMediaSrc/
	emAllSMcuLockStatus/
	emAllSMcuMixParam/
	emAllInfo
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    void SetGetInitValue(s32 nType);
	
    /*=============================================================================
    �� �� ��:SetMixParam
    ��    ��:���û�������
    ��    ��:const TMcu &tMcu                   [in]    MCU
	const TMixParam *ptMixParam    [in]    ��������
    ע    ��:�����ñ���
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/13  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void SetMixParam(const TMcu &tMcu, const TMixParam *ptMixParam)
    { 
		// ͨ��mcuid�ҵ�CItemGroup���޸�CMt��Ա����
		SetMixParam( tMcu.GetMcuIdx(), ptMixParam );
	}
	
    /*=============================================================================
    �� �� ��:SetMixParam
    ��    ��:���û�������
    ��    ��:u8 byMcuId                         [in]    McuId
	const TMixParam *ptMixParam    [in]    ��������
    ע    ��:�����ñ���
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/13  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void SetMixParam(u16 byMcuId, const TMixParam *ptMixParam);
	// ͨ��mcuid�ҵ�CItemGroup���޸�CMt��Ա����
	
    /*=============================================================================
    �� �� ��:GetMixParam
    ��    ��:��ȡ��������
    ��    ��:const TMcu &tMcu                   [in]    MCU
    ע    ��:��
    �� �� ֵ:��������
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/13  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    TMixParam& GetMixParam(const TMcu &tMcu) const
    { 
		// ͨ��mcuid�ҵ�CItemGroup������CMt��Ա����
		return GetMixParam(tMcu.GetMcuIdx()); 
	}
	
    /*=============================================================================
    �� �� ��:GetMixParam
    ��    ��:��ȡ��������
    ��    ��:u8 byMcuId                         [in]    McuId
    ע    ��:��
    �� �� ֵ:��������
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/13  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    inline TMixParam& GetMixParam(u16 byMcuId) const;
	
    /*=============================================================================
    �� �� ��:IsConfPartMix
    ��    ��:ָ��MCU�Ƿ񲿷ֻ���
    ��    ��:const TMcu &tMcu                   [in]    MCU
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/13  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsConfPartMix(const TMcu &tMcu) const
    { 
		// ͨ��mcuid�ҵ�CItemGroup���Ƚ�֮
		return IsConfPartMix(tMcu.GetMcuIdx()); 
	}
	
    /*=============================================================================
    �� �� ��:IsConfPartMix
    ��    ��:ָ��MCU�Ƿ񲿷ֻ���
    ��    ��:u8 byMcuId                         [in]    McuId
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/13  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    inline BOOL32 IsConfPartMix(u16 byMcuId) const;
	
    /*=============================================================================
    �� �� ��:IsConfEntireMix
    ��    ��:ָ��MCU�Ƿ�ȫ�����
    ��    ��:const TMcu &tMcu                   [in]    MCU
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/13  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsConfEntireMix(const TMcu &tMcu) const
    { 
		// ͨ��mcuid�ҵ�CItemGroup���Ƚ�֮
		return IsConfEntireMix(tMcu.GetMcuIdx()); 
	}
	
    /*=============================================================================
    �� �� ��:IsConfEntireMix
    ��    ��:ָ��MCU�Ƿ�ȫ�����
    ��    ��:u8 byMcuId                         [in]    McuId
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/13  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    inline BOOL32 IsConfEntireMix(u16 byMcuId) const;
	
    /*=============================================================================
    �� �� ��:IsConfNoMix
    ��    ��:ָ��MCU�Ƿ�û�л���
    ��    ��:const TMcu &tMcu                   [in]    MCU
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/08/04  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 IsConfNoMix(const TMcu &tMcu) const
    { return IsConfNoMix(tMcu.GetMcuIdx()); }
	
    /*=============================================================================
    �� �� ��:IsConfNoMix
    ��    ��:ָ��MCU�Ƿ�û�л���
    ��    ��:u8 byMcuId                         [in]    McuId
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/08/04  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    inline BOOL32 IsConfNoMix(u16 byMcuId) const;
	
    /*=============================================================================
    �� �� ��:GetMixMember
    ��    ��:��ȡ������Ա
    ��    ��:TMt *ptMt                          [out]       ������Ա
	s32 &nMtNum                        [in/out]    �����С
    ע    ��:�����¼�, ֻ���ն�, ����MCU
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/13  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void GetMixMember(TMt *ptMt, s32 &nMtNum) ;
	
    /*=============================================================================
    �� �� ��:GetLocalMixNum
    ��    ��:��ȡ����������Ա����
    ��    ��:��
    ע    ��:�����¼�MCU
    �� �� ֵ:����������Ա����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/08/07  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    s32 GetLocalMixNum(void) const;
	
    /*=============================================================================
    �� �� ��:GetSMcuMixNum
    ��    ��:��ȡ�¼�MCU����������ն˸���
    ��    ��:const TMcu &tMcu                   [in]    MCU
    ע    ��:��
    �� �� ֵ:�ն˸���
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/13  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    s32 GetSMcuMixNum(const TMcu &tMcu) const
    { return GetSMcuMixNum(tMcu.GetMcuIdx()); }
	
    /*=============================================================================
    �� �� ��:GetSMcuMixNum
    ��    ��:��ȡ�¼�MCU����������ն˸���
    ��    ��:u8 byMcuId                         [in]    McuId
    ע    ��:��
    �� �� ֵ:�ն˸���
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/13  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    s32 GetSMcuMixNum(u16 byMcuId) const;
	
    /*=============================================================================
    �� �� ��:GetLocalConfIdx
    ��    ��:��ȡ����������
    ��    ��:��
    ע    ��:��
    �� �� ֵ:����������
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/02  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    u8 GetLocalConfIdx(void)
    { 
		// conf���д���
		return m_tConfAllMtInfo.m_tLocalMtInfo.GetConfIdx(); 
	}
	
	/*---------------------------------------------------------------------
	* �� �� ����UpdateMtStatus
	* ��    �ܣ�ͨ������״̬���µ�ǰ���ն�״̬
	* ����˵����PeriEqpStatus& tPeriEqpStatus	[in]	����״̬
				BOOL32 bReset = FALSE			[in]	�Ƿ�����
				* �� �� ֵ��
				* �޸ļ�¼��
				* ����			�汾		�޸���		�޸ļ�¼
				* 2010/12/28	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	void UpdateMtStatus( TPeriEqpStatus& tPeriEqpStatus, BOOL32 bReset = FALSE );
	
    /*=============================================================================
    �� �� ��:GetMMcuSpeaker
    ��    ��:��ȡ�㲥ͨ���ն�
    ��    ��:CMt &cMt                           [out]   �㲥ͨ���ն�
    ע    ��:�����б��뺬���ϼ�MCU
    �� �� ֵ:�㲥ͨ�����ն�:TRUE, �㲥ͨ��û���ն�:FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/11  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 GetMMcuSpeaker(CMt &cMt) const;
	
    /*=============================================================================
    �� �� ��:GetMMcuSpeaker
    ��    ��:��ȡ�㲥ͨ���ն�
    ��    ��:TMt &tMt                           [out]   �㲥ͨ���ն�
    ע    ��:�����б��뺬���ϼ�MCU
    �� �� ֵ:�㲥ͨ�����ն�:TRUE, �㲥ͨ��û���ն�:FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/11  4.0     ���    ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 GetMMcuSpeaker(TMt &tMt) const;
	
    /*=============================================================================
    �� �� ��:GetVideoToMMcuMt
    ��    ��:��ȡ�ش�ͨ���ն�
    ��    ��:TMt &tMt                           [out]   �ش�ͨ���ն�
    ע    ��:�����б��뺬���ϼ�MCU
    �� �� ֵ:�ش�ͨ�����ն�:TRUE, �ش�ͨ��û���ն�:FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/08/11  4.0     ���    ����
    2005/08/23  4.0     ���    �ش�ͨ��������VMP, ȥ������ΪCMt�Ľӿ�
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    BOOL32 GetVideoToMMcuMt(TMt &tMt) const;
	
    /*=============================================================================
    �� �� ��:SetLastRateReqTime
    ��    ��:������һ�η������������ն����ʵ�ʱ��
    ��    ��:u32 dwTime         [in] ʱ��
    ע    ��:
    �� �� ֵ:
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/01/10  4.0     ����  ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    void SetLastRateReqTime(u32 dwTime) { m_dwLastRateReqTime = dwTime; }
	
    /*=============================================================================
    �� �� ��:GetLastRateReqTime
    ��    ��:��ȡ��һ�η������������ն����ʵ�ʱ��
    ��    ��:
    ע    ��:
    �� �� ֵ:
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/01/10  4.0     ����  ����
	2010/07/16	v4.1	Ĳ��ï	�����ӿڣ���д���д���
    =============================================================================*/
    u32 GetLastRateReqTime() const { return m_dwLastRateReqTime; }
	
	void SetApplySpeaker( TMt *pMtList, u32 dwMtNum );
	
    void AddApplySpeaker( TMt *pMtList, u32 dwMtNum );
	
    inline void SetPwdEntered(BOOL32 bEntered = TRUE) { m_bPwdEntered = bEntered; }
	
    inline BOOL32 IsPwdEntered() const { return m_bPwdEntered; }

	//���û᳡Ԥ�������־ //modified by twb 2012.11.20
	inline void SetReqConfSchemaFlag(BOOL32 bReqed = TRUE ){ m_bReqConfSchema = bReqed; }

	//�Ƿ�������᳡Ԥ�� //modified by twb 2012.11.20
	inline BOOL32 IsReqedConfShema() const { return m_bReqConfSchema; }

	//�Ƿ����ʹ�û᳡Ԥ��
	inline void SetHaveSchemaFlag(BOOL32 bCanUse = TRUE) { m_bHaveSchema = bCanUse; }

	inline BOOL32 GetHaveSchemaFlag() const { return m_bHaveSchema; }
	
	/*---------------------------------------------------------------------
	* �� �� ����GetVisableIMcuByIndex
	* ��    �ܣ���mcuindex���б�����ȥ��ȡmcu��ָ��
	* ����˵����[in]wIndex ����
	* �� �� ֵ������mcu�ڵ��ָ��
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/11/18	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	IMcu* GetVisableIMcuByIndex( u16 wIndex, u16 &wMcuIdx ) const;
	
	/*---------------------------------------------------------------------
	* �� �� ����GetVisableIMcuByIndex
	* ��    �ܣ���mcuindex���б�����ȥ��ȡmcu��ָ��
	* ����˵����[in]wIndex ����
	* �� �� ֵ������mcu�ڵ��ָ��
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/11/18	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	IMcu* GetVisableIMcuByIndex( u16 wIndex ) const;
	
	/*---------------------------------------------------------------------
	* �� �� ����HasSameRoot
	* ��    �ܣ���������ն��Ƿ�߱�ͬһ�����ڵ�
	* ����˵����[in] t1 ��һ���ն� 
				[in] t2 �ڶ����ն�
				[out] wIndex	���ڵ��Index������mcu
				* �� �� ֵ���Ƿ�߱���ͬ�ĸ��ڵ�
				* �޸ļ�¼��
				* ����			�汾		�޸���		�޸ļ�¼
				* 2011/08/03	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	BOOL	HasSameRoot( TMt t1, TMt t2, u16& wIndex );
	
	/*---------------------------------------------------------------------
	* �� �� ����GetMtLevel
	* ��    �ܣ����һ��TMt���ڵĲ� ��19200���ǵ�1�㣬�Դ�����
	* ����˵����[in] tMt �ն�
				[out] wlevel ���ڲ�
				* �� �� ֵ���Ƿ��ҵ�
				* �޸ļ�¼��
				* ����			�汾		�޸���		�޸ļ�¼
				* 2011/08/03	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	BOOL	GetMtLevel( TMt tMt, u16& wLevel );
	BOOL	GetMtLevel( u16 wIndex, u16& wLevel );

	//  ����/�������б�
	vector<TMt>*    GetVecApplySpeaker(); 
	
	TMt	GetMtApplySpeaker(u8 byIdx);

	//  �������״̬
	void SetBasicVCCStatus(CBasicVCCStatus& cBasicVCCStatus);
	CBasicVCCStatus* GetBasicVCCStatus();  

	//  ����������û���
	s8*	 GetLockUserName();
	void SetLockUsername(u8 byLockUserName[MAXLEN_PWD + 1]);

	//  ��������Ļ��IP
	void SetLockMcsIp(u32 dwValue);
	u32 GetLockMcsIp();

	//added by twb.2013.3.4 start

	// ���»���ϳ������ն�״̬
	BOOL32 UpdateVmpParam(u8 byEqpId, TVMPParam_25Mem* ptVmpParam, const s8* pchVmpAlias = NULL, u8 bReplaceEqpId = 255);
	// �������ϳ������ն�״̬
	BOOL32 RemoveVmpParam(u8 byEqpId);

	//���»���ϳ�������
	void  UpdateVmpEqpAlias(const s8* pchEqpAlias, u8 byEqpId, TplArray<TPeriEqpStatus>& tplVmpArray);

	// ����¼�MCU����ý��Դ������״̬
	BOOL32 CheckSMcuAndAddMtStatus(CMt &cMt, u8 byEqpId);
	// ����¼�MCU����ý��Դ�����״̬
	BOOL32 CheckSMcuAndClearMtStatus(CMt &cMt, u8 byEqpId);
	// �滻�ն�״̬
	void ReplaceMediaSrcMtVmpStatus(CMt& cMcu, CMt& cOldMediaMt, CMt& cNowMediaMt);
	// ��ȡ�㲥�Ļ���ϳ���
	u8 GetBrdstVmpEqpId();
	// ��ȡVmp�б�
	MAPVMPPARAMINFO* GetVmpList();
	
	// ���˫���ն�
	CMt GetDualStreamMt();

private:
	void UpdateVmpStatus(TVMPParam_25Mem* ptVmpParam, u8 byEqpId);
	void RemoveVmpStatus(TVMPParam_25Mem* pTVMPParam_25Mem, u8 byEqpId);

	void UpdateVmpStatus(CMt& cMt, u8 byEqpId);
	void RemoveVmpStatus(CMt& cMt, u8 byEqpId);
	//added by twb.2013.3.4 end
    
private:
	//����ϳ�����Ϣ
	map<u8, TVmpParamInfo> m_mapVmpInfo;

	u32             m_dwLocalMcuIp;                     //  ����MCUIP
    CMt             m_cLocalMcu;                        //  ����MCU

	u32             m_dwGetInitValue;   //  ���ڼ�¼�Ƿ�õ�����ʼ��״̬����Ҫ������MCUʱ�õ�

	u8              m_byLockBySelf;     //  �����Ƿ񱻱��������

	u32             m_dwLockMcsIP;      //  ��������Ļ��IP
	s8              m_abyLockUserName[MAXLEN_PWD + 1];  //  ����������û���

	BOOL32          m_bPwdEntered;      //  ��¼�û��Ƿ���������룬��Ҫ����MCUҪ���û�����������¼
	BOOL32			m_bReqConfSchema;	//  ��¼�����Ƿ��Ѿ�������᳡Ԥ�� modified by twb 2012.11.20

	BOOL32			m_bHaveSchema;		//	�����Ƿ���Ԥ��

	u32             m_dwSN;             //  �������

	CBasicVCCStatus m_cVcsConfStatus;   //  �������״̬

	vector<TMt>     m_vctApplySpeaker;  //  ����/�������б�

    TEqp            m_tRecEqp;          //  ����¼���豸
    TEqp            m_tPlayEqp;         //  ��������豸
	
    u32             m_dwLastRateReqTime;//  ��¼��һ�η������������ն����ʵ�ʱ��
public:
	void SetHduBatchPollState(THduPollSchemeInfo& tHduPollSchemeInfo)
	{
		m_tHduPollSchemeInfo = tHduPollSchemeInfo;
	}
	THduPollSchemeInfo& GetHduBatchPollStatus()
	{
		return m_tHduPollSchemeInfo;
	}
private:
	THduPollSchemeInfo m_tHduPollSchemeInfo;	//����ǽ������ѯ��Ϣ
	
	//Ԥ����Ϣ
public:
	vector <TVCSSchema*> m_vctpTVcsSchema;
	//TVCSSchema* m_ptCurSchema;
	void GetSchema( const s8* strSchemaName, TVCSSchema ** pptSchema )
	{
		
		for ( u32 nIdx = 0; nIdx< m_vctpTVcsSchema.size(); nIdx ++ )
		{
			TVCSSchema* ptSchema = m_vctpTVcsSchema[ nIdx ];
			if ( strcmp( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, strSchemaName ) == 0 )
			{
				*pptSchema = m_vctpTVcsSchema[ nIdx ];
				return;
			}
		}
	}
	
	TVCSSchema* GetSchema( const s8* strSchemaName )
	{
		TVCSSchema* ptSchema = NULL;
		
		for ( u32 nIdx = 0; nIdx< m_vctpTVcsSchema.size(); nIdx ++ )
		{
			if ( strcmp( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, strSchemaName ) == 0 )
			{
				ptSchema = m_vctpTVcsSchema[ nIdx ];
				break;
			}
		}
		
		return ptSchema;
	}
	
	BOOL32 IsSchemaExist( const s8* strSchemaName )
	{		
		for ( u32 nIdx = 0; nIdx< m_vctpTVcsSchema.size(); nIdx ++ )
		{
			if ( strcmp( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, strSchemaName ) == 0 )
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	
	void PopUpSchema( TVCSSchema *ptSchema )
	{
		for ( u32 nIdx = 0; nIdx< m_vctpTVcsSchema.size(); nIdx ++ )
		{
			if ( strcmp( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, ptSchema->m_pbySchemaName ) == 0 )
			{
				//����Ҫ��ɾ��ָ��
				delete m_vctpTVcsSchema[ nIdx ];
				m_vctpTVcsSchema[ nIdx ] = NULL;
				m_vctpTVcsSchema.erase( m_vctpTVcsSchema.begin() + nIdx );
			}
		}
	}
	
	void SubstituteSchemaName( s8* pbyOldName, s8* pbyNewName )
	{
		for ( u32 nIdx = 0; nIdx< m_vctpTVcsSchema.size(); nIdx ++ )
		{
			if ( strcmp( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, pbyOldName ) == 0 )
			{
				strcpy( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, pbyNewName );
				break;
			}
		}
	}
	
	void SubstituteSchema( TVCSSchema * const ptSchema )
	{
		BOOL32 bFind = FALSE;

		for ( u32 nIdx = 0; nIdx< m_vctpTVcsSchema.size(); nIdx ++ )
		{
			if ( strcmp( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, ptSchema->m_pbySchemaName ) == 0 )
			{
				bFind = TRUE;
				//����Ҫ��ɾ��ָ��
				if ( ptSchema != m_vctpTVcsSchema[ nIdx ] )
				{
					delete m_vctpTVcsSchema[ nIdx ];
					m_vctpTVcsSchema[ nIdx ] = ptSchema;
				}
				
			}
		}

		if (!bFind)
		{
			m_vctpTVcsSchema.push_back( ptSchema );
		}
	}
public:
	//added by spriner 20100518 �¼�mcu֧�ֶ�ش����б�
	//���ﱣ�����֧�ֶ�ش���mcu ID
	TMcu m_atMcu[MAXNUM_CONF_MT];//֧�ֶ�ش����¼�mcu
	//�ı��Ƿ�֧�ֶ�ش���mcuID byCascade Ϊ0��ʱ��ɾ����Ϊ1��ʱ������
	/*---------------------------------------------------------------------
	* �� �� ����SetMcuMutilChannelStatus
	* ��    �ܣ�����mcu�Ƿ��Ƕ�ش�
	* ����˵����[in] wMcuIdx mcu������ [in]bOpen �Ƿ��
	* �� �� ֵ��
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/10/11	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	void SetMcuMutilChannelStatus( const TMt &tMt, BOOL32 bOpen );
	
	/*---------------------------------------------------------------------
	* �� �� ����GetMcuMutilChannelStatus
	* ��    �ܣ����mcu�Ķ�ش�״̬
	* ����˵����
	* �� �� ֵ��
	* �޸ļ�¼��
	* ����			�汾		�޸���		�޸ļ�¼
	* 2010/10/11	v1.0		Ĳ��ï		����
	----------------------------------------------------------------------*/
	BOOL32 GetMcuMutilChannelStatus( const TMt &tMt );
	BOOL32 GetMcuMutilChannelStatus( u16 wMcuIdx );
	
	void PrintBasicVcsStatus();

protected:
	// ����ϳɵ�ͨ����ѯ
	TVmpPollParam m_tVmpPollParam;
public:
	TVmpPollParam& GetVmpPollParam(){ return m_tVmpPollParam; }
	void SetVmpPollParam( TVmpPollParam& tVmpPollParam )
	{
		m_tVmpPollParam = tVmpPollParam;
	}
};

//CConfTable��������ʽ��������MCU�����м�ʱ/ԤԼ�����б�
class CConfTable
{
public:
    CConfTable();
    CConfTable(const CConfTable &cConfTable);
	
    ~CConfTable() {}
	
    /*=============================================================================
    �� �� ��:operator=
    ��    ��:��ֵ
    ��    ��:const CConfTable &cConfTable       [in]    �����б�
    ע    ��:��
    �� �� ֵ:�����б�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    const CConfTable& operator=(const CConfTable &cConfTable);
	
    /*=============================================================================
    �� �� ��:operator[]
    ��    ��:���ݻ�����������������
    ��    ��:const CConfId &cConfId             [in]    �����
    ע    ��:��
    �� �� ֵ:����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    CConf& operator[](const CConfId &cConfId);
	
    /*=============================================================================
    �� �� ��:operator[]
    ��    ��:���ݻ�����������������
    ��    ��:u32 dwConfSN                       [in]    ����������
    ע    ��:��
    �� �� ֵ:����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    CConf& operator[](u32 dwConfSN);
	
    /*=============================================================================
    �� �� ��:GetAt
    ��    ��:��ȡ����
    ��    ��:const CConfId &cConfId             [in]    �����
	CConf &cConf                       [out]   ����
    ע    ��:��
    �� �� ֵ:�������: TRUE; ������: FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    BOOL32 GetAt(const CConfId &cConfId, CConf &cConf) const;
	
    /*=============================================================================
    �� �� ��:GetAt
    ��    ��:��ȡ����
    ��    ��:u32 dwConfSN                       [in]    ����������
	CConf &cConf                       [out]   ����
    ע    ��:��
    �� �� ֵ:�������: TRUE; ������: FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    BOOL32 GetAt(u32 dwConfSN, CConf &cConf) const;
	
    /*=============================================================================
    �� �� ��:SetAt
    ��    ��:���û����
    ��    ��:CConf &cConf                       [in]    ����
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    void SetAt(CConf &cConf);
	
    /*=============================================================================
    �� �� ��:operator-=
    ��    ��:�ӻ������ɾ��һ������
    ��    ��:const CConfId &cConfId             [in]    �����
    ע    ��:��
    �� �� ֵ:�����б�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    CConfTable& operator-=(const CConfId &cConfId);
	
    /*=============================================================================
    �� �� ��:IsExist
    ��    ��:�жϻ����ڻ�������Ƿ����
    ��    ��:const CConfId &cConfId             [in]    �����
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsExist(const CConfId &cConfId) const;
	
    /*=============================================================================
    �� �� ��:IsExist
    ��    ��:�жϻ����ڻ�������Ƿ����
    ��    ��:u32 dwConfSN                        [in]    ����������
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsExist(u32 dwConfSN) const;
	
    /*=============================================================================
    �� �� ��:IsEmpty
    ��    ��:�жϻ�������Ƿ�Ϊ��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsEmpty(void) const { return (m_nConfNum == 0); }
	
    /*=============================================================================
    �� �� ��:ClearContent
    ��    ��:���������е�������
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
	void ClearContent(void);
	
    /*=======================================================================
    �� �� ��:GetNextNoInitValueConfId
    ��    ��:�õ���һ����û�еõ���ʼ��Ϣ�Ļ���ID, ��ʼ��Ϣ����������
	�ն�״̬�б����������¼�MCU����״̬
    ��    ��:��
    ע    ��:��
    �� �� ֵ:�����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =======================================================================*/
    const CConfId GetNextNoInitValueConfId(void);
	
    /*=======================================================================
    �� �� ��:GetNewConfSN
    ��    ��:�õ���������к�,ÿ�ε��ö���õ�һ���µĺ���
    ��    ��:��
    ע    ��:��
    �� �� ֵ:�µ����к�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =======================================================================*/
    static u32 GetNewConfSN(void) { return ++s_dwConfSN; }
	
    /*=============================================================================
    �� �� ��:GetAllConfId
    ��    ��:��ȡ���л����
    ��    ��:CConfId *pcConfId                  [out]   ���������
	s32 nNum                           [in/out]    �����С/�������
    ע    ��:���۳ɹ����, �����С�����óɻ������
    �� �� ֵ:�����СС�ڻ������: FALSE; �ɹ�: TRUE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    BOOL32 GetAllConfId(CConfId *pcConfId, s32 &nNum);
	
    /*=============================================================================
    �� �� ��:GetAllConfSN
    ��    ��:��ȡ���л���������
    ��    ��:u32 *pdwConfSN                     [out]   ��������������
	s32 nNum                           [in/out]    �����С/�������
    ע    ��:���۳ɹ����, �����С�����óɻ������
    �� �� ֵ:�����СС�ڻ������: FALSE; �ɹ�: TRUE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/06/03  4.0     ���    ����
    =============================================================================*/
    BOOL32 GetAllConfSN(u32 *pdwConfSN, s32 nNum);
	
public:
    CConf       m_acConf[MAXNUM_MCU_CONF];          //  ���л����б�
    s32         m_nConfNum;                         //  �������
	
private:
    u8          m_abyConfExist[MAXNUM_MCU_CONF];    //  �������Ƿ��л���
    static u32  s_dwConfSN;                         //  ����������

	CCriticalSection	m_criLock;					//	��ֵ��
};

// ���������
class CVcsGroup
{
public:
    CVcsGroup()
    {
        ZeroMemory( &m_tVcsGroupInfo, sizeof(m_tVcsGroupInfo) );
    }
	
    void AddEntry( TVCSEntryInfo &tEntry )
    {
        m_vctVcsEntry.push_back( tEntry );
        m_tVcsGroupInfo.dwEntryNum = m_vctVcsEntry.size();
    }
    void DelEntry( TVCSEntryInfo &tEntry )
    {
		vector<TVCSEntryInfo>::iterator itr = m_vctVcsEntry.begin();
		for ( ; itr != m_vctVcsEntry.end();  )
		{
			if ( strcmp( tEntry.achEntryName, (*itr).achEntryName ) == 0 && tEntry.byEntryType == (*itr).byEntryType )
			{
				m_vctVcsEntry.erase( itr );
				continue;
			}
            itr++;
		}
		
        m_tVcsGroupInfo.dwEntryNum = m_vctVcsEntry.size();
    }
    inline CVcsGroup& operator=( const CVcsGroup &cGroup )
    {
        strcpy( m_tVcsGroupInfo.achGroupName, cGroup.m_tVcsGroupInfo.achGroupName );
        m_tVcsGroupInfo.dwEntryNum = cGroup.m_tVcsGroupInfo.dwEntryNum;
        m_vctVcsEntry = cGroup.m_vctVcsEntry;
		
        return (*this);
    }
	
    TVCSGroupInfo   m_tVcsGroupInfo;
    vector<TVCSEntryInfo>   m_vctVcsEntry;
};

// �������
// --------------------------------------------------------------------

// ¼������á�ר������VCS����ģ���¼���
typedef struct tagRecDeploy
{
	TEqp	tEqp; //�豸
	TRecStartPara tRecStartPara; // 
}TRecDeploy,*PTRecDeploy;

typedef struct tagMonitorUnion
{
	u32 dwIp;
	u32	dwBasePort;
}TMonitorUnion,*PTMonitorUnion; 

//����ģ��/ԤԼ����
class CTemSchConf : public TConfInfo
{
public:
    CTemSchConf() : m_tplMt(0, MTTABLE_STEP), m_bExist(FALSE), m_bIsHasVmpSubChnnl(FALSE), m_tplSubMcu(0, MTTABLE_STEP), m_wPollTime(6)
		,m_byAutoFlag(0){}
    CTemSchConf(const CTemSchConf &cTemSchConf);
    ~CTemSchConf();
	
    CTemSchConf& operator=(const CTemSchConf &cTemSchConf);
	
    /*=============================================================================
    �� �� ��:SetNull
    ��    ��:����ģ���ÿ�
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/11/22  4.0     ���    ����
    =============================================================================*/
    inline void SetNull(void);
	
    /*=============================================================================
    �� �� ��:IsExist
    ��    ��:�����Ƿ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:����:TRUE; ������: FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/11/22  4.0     ���    ����
    =============================================================================*/
    BOOL32 IsExist(void) const { return m_bExist; }
	
    /*=============================================================================
    �� �� ��:SetConfInfo
    ��    ��:���û�����Ϣ
    ��    ��:const TConfInfo &tConfInfo         [in]    ������Ϣ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/11/22  4.0     ���    ����
    =============================================================================*/
    inline void SetConfInfo(const TConfInfo &tConfInfo)
    {
        *dynamic_cast<TConfInfo*>(this) = tConfInfo;
        m_bExist = TRUE;
    }
	
    /*=============================================================================
    �� �� ��:PrintInfo
    ��    ��:��ӡ
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/11/23  4.0     ���    ����
    =============================================================================*/
    void PrintInfo(void);
	
	
    /*=============================================================================
    �� �� ��:GetAutoFlag
    ��    ��:��ȡ�Զ����ò���
    ��    ��:��
    ע    ��: vcs�Զ����
    �� �� ֵ: �Զ���� = VCS_GROUPCHAIRMAN_MODE 
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/04/20  4.0     ��ȫ    ����
    =============================================================================*/
	u8   GetAutoFlag(){ return m_byAutoFlag; }
	
	/*=============================================================================
    �� �� ��:SetAutoFlag
    ��    ��:u8 byFlag ��ȡ�Զ����ò���
    ��    ��:�Զ���� = VCS_GROUPCHAIRMAN_MODE 
    ע    ��: vcs�Զ����
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2011/04/20  4.0     ��ȫ    ����
    =============================================================================*/
	void SetAutoFlag( u8 byFlag ) { m_byAutoFlag = byFlag; }
	
public:
	BOOL32                  m_bIsHasVmpSubChnnl;
    BOOL32                  m_bExist;       //����ģ���Ƿ����
    TplArray<TAddMtInfo>    m_tplMt;        //�ն��б�
	TMtAlias				m_tMtAliasBack; //���ر����ն�   
    // ����@2006.02.14 ����ǽģ��֧�ֶ������ǽ
    TMultiTvWallModule      m_tTWModules;  //����ǽģ��
	TMcsVmpModule           m_tVmpModuleEx;
	THduVmpTotalEx          m_tHduVmpTotalEx;
	
    //׼��δ��֧�ֶ໭�����ǽģ��
    //TVmpModule            mtVmpTwModule;  /�໭�����ǽģ��
	
    // �������ר��
    TplArray<TAddMtInfo>    m_tplSubMcu;    // �¼�MCU�б�
    vector<CVcsGroup>       m_vctGroup;     // ����������б�
    THDTvWall               m_tHdTvwall;    // �����ն���Ϊ����ǽ
    vector<THduModChnlInfo> m_vctHduModChn; // Hduͨ����Ϣ
    
    u16                     m_wPollTime;    //��ѯʱ��
	
	TMonitorUnion			m_tMonitorUnion;//�����������
	TRecDeploy				m_tRecDeploy;	//¼�������

	TConfInfoEx				m_tConfInfoEx;	//V4R7����������Ϣ

private:
	u8                      m_byAutoFlag;   // 7:�Զ����
};

//ԤԼ����/����ģ���б�
//ע��: �洢δ������, ������Ҫѭ������
class CConfTemSchTable
{
public:
    CConfTemSchTable() {};
    CConfTemSchTable(const CConfTemSchTable &cConfTemSchTable);
    ~CConfTemSchTable() {};
	
    CConfTemSchTable& operator=(const CConfTemSchTable &cConfTemSchTable);
	
    /*=============================================================================
    �� �� ��:ReleaseConf
    ��    ��:���ָ������
    ��    ��:const CConfId &cConfId             [in]    �����
	u8 byTakeMode  [in]    CONF_TAKEMODE_SCHEDULED/CONF_TAKEMODE_TEMPLATE
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/11/23  4.0     ���    ����
    =============================================================================*/
    BOOL32 ReleaseConf(const CConfId &cConfId, u8 byTakeMode);
	
    /*=============================================================================
    �� �� ��:PrintInfo
    ��    ��:��ӡ
    ��    ��:u8 byTakeMode      [in]    CONF_TAKEMODE_SCHEDULED/CONF_TAKEMODE_TEMPLATE
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/11/23  4.0     ���    ����
    =============================================================================*/
    void PrintInfo(u8 byTakeMode);
	
    /*=============================================================================
    �� �� ��:Clear
    ��    ��:���
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/11/24  4.0     ���    ����
    =============================================================================*/
    void Clear(void);
	
public:
    CTemSchConf m_acConfTemplate[MAXNUM_MCU_CONF];  //����ģ������
    CTemSchConf m_acScheduleConf[MAXNUM_MCU_CONF];  //ԤԼ��������
};

//CUserTable�û���������һ��TplArray������õ��û���Ϣ
class CUserTable
{
public:
    CUserTable() { Clear(); }
	
    /*=============================================================================
    �� �� ��:CUserTable
    ��    ��:�������캯��
    ��    ��:const CUserTable &cUserTable       [in]    Ҫ���Ƶ��û��б�
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	CUserTable(const CUserTable &cUserTable);
	
    virtual ~CUserTable() { Clear(); }
	
    /*=============================================================================
    �� �� ��:IsEmpty
    ��    ��:�ж��û����Ƿ�Ϊ��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 IsEmpty(void) const;
	
    /*=============================================================================
    �� �� ��:IsExist
    ��    ��:�жϸ����û��Ƿ��ڱ��д���
    ��    ��:LPCTSTR pszUserName                [in]    �û���
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 IsExist( const s8* pszUserName ) const;
	
    /*=============================================================================
    �� �� ��:operator=
    ��    ��:���������=
    ��    ��:const CUserTable &cUserTable       [in]    ���ڸ�ֵ���û���
    ע    ��:��
    �� �� ֵ:this
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
    const CUserTable& operator=(const CUserTable &cUserTable);
	
    /*=============================================================================
    �� �� ��:operator[]
    ��    ��:���������[]
    ��    ��:s32 nIndex                         [in]    �û����б��е�����
    ע    ��:��
    �� �� ֵ:const CUserFullInfo&   ����ָ�����û���Ϣ
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	const CExUsrInfo& operator[](s32 nIndex) const;
	
	const CExUsrInfo& GetUserAt(s32 nIndex) const;
	
    /*=============================================================================
    �� �� ��:GetUser
    ��    ��:ȡ��ָ�����Ƶ��û���Ϣ
    ��    ��:LPCTSTR pszUserName                [in]    Ҫ��ȡ���û�������
	CUserFullInfo * const pcUserInfo   [out]   �����û���Ϣ
    ע    ��:��
    �� �� ֵ:�ɹ�����TRUE, �û������ڶ�ȡʧ�ܷ���FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 GetUser( const s8* pszUserName, CExUsrInfo * const pcUserInfo ) const;
	
    /*=============================================================================
    �� �� ��:SetUser
    ��    ��:���б��������û�������û���������ֱ�Ӽ��룬����û��Ѵ��ڣ�������û���Ϣ
    ��    ��:const CUserFullInfo &cUserInfo     [in]    Ҫ������û���Ϣ
    ע    ��:��
    �� �� ֵ:TRUE��ʾ�ɹ���FALSE��ʾʧ��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 SetUser(const CExUsrInfo &cUserInfo);
	
    /*=============================================================================
    �� �� ��:DelUser
    ��    ��:ɾ��ָ���û�
    ��    ��:LPCTSTR lptrUserName               [in]    Ҫɾ�����û�
    ע    ��:��
    �� �� ֵ:�ɹ�����TRUE���û������ڻ�ʧ�ܷ���FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 DelUser(const s8* lptrUserName);
	
    /*=============================================================================
    �� �� ��:GetUserNum
    ��    ��:�õ���ǰ�û����е��û�����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:�û��ĸ���
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	s32 GetUserNum(void) const;
	
    /*=============================================================================
    �� �� ��:GetAllUser
    ��    ��:�õ����������û�����Ϣ
    ��    ��:CUserFullInfo *pcUserArray         [out]   �û���Ϣ����
	s32 &nNum                          [in/out]����������С/ʵ�������û�����
    ע    ��:��
    �� �� ֵ:�ɹ�����TRUE��ʧ�ܷ���FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 GetAllUser(CExUsrInfo *pcUserArray, s32 &nNum);
	
    /*=============================================================================
    �� �� ��:Clear
    ��    ��:��������б�
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
    inline void Clear(void) { m_tplUserFullInfo.Clear(); }
	
    /*=============================================================================
    �� �� ��:PrintInfo
    ��    ��:�����е��û���Ϣ��ӡ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
    void PrintInfo(void);
	
protected:
	TplArray<CExUsrInfo> m_tplUserFullInfo;
	
	//#ifdef _DEBUG
	//public:
	//    /*=============================================================================
	//    �� �� ��:Dump
	//    ��    ��:��ӡ����ĳ�Ա������Ϣ
	//    ��    ��:CDumpContext &dc                   [in]    ��ӡͷ
	//    ע    ��:��
	//    �� �� ֵ:��
	//    -------------------------------------------------------------------------------
	//    �޸ļ�¼:
	//    ��      ��  �汾    �޸���  �޸�����
	//    2005/04/26  4.0     ���    ����
	//    =============================================================================*/
	//    virtual void Dump(CDumpContext &dc) const;
	//#endif  //  _DEBUG
};

class CVcsUserTable
{
public:
    CVcsUserTable() { Clear(); }
	
    /*=============================================================================
    �� �� ��:CUserTable
    ��    ��:�������캯��
    ��    ��:const CUserTable &cUserTable       [in]    Ҫ���Ƶ��û��б�
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	CVcsUserTable(const CVcsUserTable &cUserTable);
	
    virtual ~CVcsUserTable() { Clear(); }
	
    /*=============================================================================
    �� �� ��:IsEmpty
    ��    ��:�ж��û����Ƿ�Ϊ��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 IsEmpty(void) const;
	
    /*=============================================================================
    �� �� ��:IsExist
    ��    ��:�жϸ����û��Ƿ��ڱ��д���
    ��    ��:LPCTSTR pszUserName                [in]    �û���
    ע    ��:��
    �� �� ֵ:BOOL
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 IsExist(const s8* pszUserName) const;
	
    /*=============================================================================
    �� �� ��:operator=
    ��    ��:���������=
    ��    ��:const CUserTable &cUserTable       [in]    ���ڸ�ֵ���û���
    ע    ��:��
    �� �� ֵ:this
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
    const CVcsUserTable& operator=(const CVcsUserTable &cUserTable);
	
    /*=============================================================================
    �� �� ��:operator[]
    ��    ��:���������[]
    ��    ��:s32 nIndex                         [in]    �û����б��е�����
    ע    ��:��
    �� �� ֵ:const CUserFullInfo&   ����ָ�����û���Ϣ
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	const CVCSUsrInfo& operator[](s32 nIndex) const;
	
	const CVCSUsrInfo& GetUserAt(s32 nIndex) const;
	
    /*=============================================================================
    �� �� ��:GetUser
    ��    ��:ȡ��ָ�����Ƶ��û���Ϣ
    ��    ��:const s8* pszUserName                [in]    Ҫ��ȡ���û�������
	CUserFullInfo * const pcUserInfo   [out]   �����û���Ϣ
    ע    ��:��
    �� �� ֵ:�ɹ�����TRUE, �û������ڶ�ȡʧ�ܷ���FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 GetUser( const s8* pszUserName, CVCSUsrInfo * const pcUserInfo) const;
	
    /*=============================================================================
    �� �� ��:SetUser
    ��    ��:���б��������û�������û���������ֱ�Ӽ��룬����û��Ѵ��ڣ�������û���Ϣ
    ��    ��:const CUserFullInfo &cUserInfo     [in]    Ҫ������û���Ϣ
    ע    ��:��
    �� �� ֵ:TRUE��ʾ�ɹ���FALSE��ʾʧ��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 SetUser(const CVCSUsrInfo &cUserInfo);
	
    /*=============================================================================
    �� �� ��:DelUser
    ��    ��:ɾ��ָ���û�
    ��    ��:const s8* lptrUserName               [in]    Ҫɾ�����û�
    ע    ��:��
    �� �� ֵ:�ɹ�����TRUE���û������ڻ�ʧ�ܷ���FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 DelUser( const s8* lptrUserName );
	
    /*=============================================================================
    �� �� ��:GetUserNum
    ��    ��:�õ���ǰ�û����е��û�����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:�û��ĸ���
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	s32 GetUserNum(void) const;
	
    /*=============================================================================
    �� �� ��:GetAllUser
    ��    ��:�õ����������û�����Ϣ
    ��    ��:CUserFullInfo *pcUserArray         [out]   �û���Ϣ����
	s32 &nNum                          [in/out]����������С/ʵ�������û�����
    ע    ��:��
    �� �� ֵ:�ɹ�����TRUE��ʧ�ܷ���FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 GetAllUser(CVCSUsrInfo *pcUserArray, s32 &nNum);
	
    /*=============================================================================
    �� �� ��:Clear
    ��    ��:��������б�
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
    inline void Clear(void) { m_tplUserFullInfo.Clear(); }
	
    /*=============================================================================
    �� �� ��:PrintInfo
    ��    ��:�����е��û���Ϣ��ӡ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
    void PrintInfo(void);
	
protected:
	TplArray<CVCSUsrInfo> m_tplUserFullInfo;
	
	//#ifdef _DEBUG
	//public:
	//    /*=============================================================================
	//    �� �� ��:Dump
	//    ��    ��:��ӡ����ĳ�Ա������Ϣ
	//    ��    ��:CDumpContext &dc                   [in]    ��ӡͷ
	//    ע    ��:��
	//    �� �� ֵ:��
	//    -------------------------------------------------------------------------------
	//    �޸ļ�¼:
	//    ��      ��  �汾    �޸���  �޸�����
	//    2005/04/26  4.0     ���    ����
	//    =============================================================================*/
	//    virtual void Dump(CDumpContext &dc) const;
	//#endif  //  _DEBUG
};

//���������û������Ϣ,����Ϊһ��tplarray
class CUserGroupTable
{
public:
	CUserGroupTable();
	virtual ~CUserGroupTable();
    /*�������� CUserGroupTable
    ��    �ܣ� �������캯��
    ��    ���� const CUserGroupTable& rhs
    �� �� ֵ�� 
    ˵    ���� */
	CUserGroupTable(const CUserGroupTable& rhs);
    /*�������� operator=
    ��    �ܣ� ��ֵ����
    ��    ���� const CUserGroupTable &rhs
    �� �� ֵ�� CUserGroupTable& 
    ˵    ���� */
	CUserGroupTable& operator=(const CUserGroupTable &rhs);
    /*�������� AddUserGroup
    ��    �ܣ� ����һ���û�����Ϣ,����û�����Ϣ�Ѿ����ڵĻ��͸���
    ��    ���� const TUsrGrpInfo& tUserGroupInfo
    �� �� ֵ�� BOOL     �����Ϊ������,����TRUE, �����Ϊ�Ǹ���,����FALSE
    ˵    ���� */
	BOOL AddUserGroup( const TUsrGrpInfo& tUserGroupInfo );
    /*�������� DelUserGroup
    ��    �ܣ� ɾ��һ���û������Ϣ
    ��    ���� u8 byGroupId		[in] �û����ID
    �� �� ֵ�� BOOL     �ɹ�����TRUE,ʧ�ܷ���FALSE
    ˵    ���� */
	BOOL DelUserGroup( u8 byGroupId );
    /*�������� GetUserGroupNum
    ��    �ܣ� ��ȡ�û�����Ϣ����Ŀ
    ��    ���� 
    �� �� ֵ�� s32		�û�����Ϣ����Ŀ
    ˵    ���� */
	s32 GetUserGroupNum() const
	{
		return m_tplUserGroupInfo.Size();
	}
    /*�������� ModifyUserGroup
    ��    �ܣ� �޸��û�����Ϣ
    ��    ���� const TUsrGrpInfo& tUserGroupInfo	[in]�û�����Ϣ
    �� �� ֵ�� BOOL			����޸ĳɹ��ͷ���TRUE,���򷵻�False
    ˵    ���� */
	BOOL ModifyUserGroup( const TUsrGrpInfo& tUserGroupInfo );
    /*�������� GetUserGroup
    ��    �ܣ� �����û���ID�����û�����Ϣ
    ��    ���� u8 byGroupId			[in]�û���ID
    �� �� ֵ�� const TUsrGrpInfo&		�û�����Ϣ
    ˵    ���� */
	const TUsrGrpInfo& GetUserGroup( u8 byGroupId ) const;
    /*�������� GetAllUserGroup
    ��    �ܣ� ����ȫ�����û�����Ϣ
    ��    ����  TUsrGrpInfo* ptUserGroup			[in/out]������������ʼ��ַ
	s32& nNum							[in/out]��������Ĵ�С/����ʵ�ʷ��صĴ�С
    �� �� ֵ�� BOOL			���������ȫ����Ϣ����ΪTrue������ΪFalse
    ˵    ���� */
	BOOL GetAllUserGroup( TUsrGrpInfo* ptUserGroup, s32& nNum ) const;
    /*�������� GroupClear
    ��    �ܣ� �û�����Ϣ���
    ��    ���� 
    �� �� ֵ�� void 
    ˵    ���� */
	void GroupClear() { m_tplUserGroupInfo.Clear(); } 
    /*�������� GroupIsExist
    ��    �ܣ� ָ���û���ID���û�����Ϣ�Ƿ����
    ��    ���� u8 byGroupId			[in]�û����ID
    �� �� ֵ�� BOOL 
    ˵    ���� */
	BOOL GroupIsExist( u8 byGroupId ) const;
    /*�������� GetUserGroupAt
    ��    �ܣ� ������tplarray�������������û������Ϣ
    ��    ���� u32 dwIndex			[in]������Ϣ
    �� �� ֵ�� const TUsrGrpInfo&	�û�����Ϣ
    ˵    ���� */
	const TUsrGrpInfo& GetUserGroupAt( u32 dwIndex ) const;
    /*�������� operator[]
    ��    �ܣ� ����[]�±����������GetUserGroupAt()�Ĺ���һ��
    ��    ���� s32 dwIndex			[in]������Ϣ
    �� �� ֵ�� TUsrGrpInfo&			�û�����Ϣ
    ˵    ���� */
	TUsrGrpInfo& operator[](s32 dwIndex);
    /*�������� operator[]
    ��    �ܣ� ����[]�±����������GetUserGroupAt()�Ĺ���һ��(const �汾)
    ��    ���� s32 dwIndex			[in]������Ϣ
    �� �� ֵ�� const TUsrGrpInfo&	�û�����Ϣ
    ˵    ���� */
	const TUsrGrpInfo& operator[](s32 dwIndex )const ; 
	
    /*�������� GrpPrintInfo
    ��    �ܣ� ��ӡ�ڲ�����Ϣ
    ��    ���� void
    �� �� ֵ�� void 
    ˵    ���� */
	void GrpPrintInfo( void ) const;
	
protected:
	TplArray<TUsrGrpInfo>	m_tplUserGroupInfo;		//�����û����б�
};

//����ͳһ�����û����û������Ϣ,�ϸ����˵������has a�Ĺ�ϵ, ����ʹ��
//Mutil-inheritance ��Ϊ��ֱ��ʹ�����ǵķ���
class CUserManage : public CUserTable,
public CUserGroupTable

{
public:
	CUserManage();
	~CUserManage();
    /*�������� GetGroupAllUser
    ��    �ܣ� �����û���ID��������û������������û�����Ϣ
    ��    ����  u8 byGrpId				[in]�û���ID��Ϣ
	CExUsrInfo *pcUserInfo	[in/out]�û���Ϣָ��
	s32 &nNum				[in/out]��������ָ��Ĵ�С/ʵ�ʷ��ص���Ŀ
    �� �� ֵ�� BOOL		�õ�ȫ����Ϣ����True,������������̫С,���·��ز�����Ϣ��ΪFALSE
    ˵    ���� */
	BOOL GetGroupAllUser( u8 byGrpId, CExUsrInfo *pcUserInfo, s32 &nNum ) const ;
    /*�������� CUserManage
    ��    �ܣ� �������캯��
    ��    ���� const CUserManage& rhs
    �� �� ֵ�� 
    ˵    ���� */
	CUserManage( const CUserManage& rhs );
    /*�������� operator=
    ��    �ܣ� ���ظ�ֵ�����
    ��    ���� const CUserManage& rhs
    �� �� ֵ�� CUserManage& 
    ˵    ���� */
	CUserManage& operator=(const CUserManage& rhs);
	
    /*�������� Print
    ��    �ܣ� ��ӡ�ڲ���Ϣ
    ��    ���� void
    �� �� ֵ�� void 
    ˵    ���� */
	void Print(void) const; 
	
private:
/*�������� Init
��    �ܣ� ��ʼ����Ϣ
��    ���� const CUserManage& rhs		[in]�����һ���û���������Ϣ
�� �� ֵ�� void 
    ˵    ���� */
	void Init(const CUserManage& rhs);
};

class CVcsUserManage : public CVcsUserTable,
public CUserGroupTable

{
public:
	CVcsUserManage();
	~CVcsUserManage();
    /*�������� GetGroupAllUser
    ��    �ܣ� �����û���ID��������û������������û�����Ϣ
    ��    ����  u8 byGrpId				[in]�û���ID��Ϣ
	CExUsrInfo *pcUserInfo	[in/out]�û���Ϣָ��
	s32 &nNum				[in/out]��������ָ��Ĵ�С/ʵ�ʷ��ص���Ŀ
    �� �� ֵ�� BOOL		�õ�ȫ����Ϣ����True,������������̫С,���·��ز�����Ϣ��ΪFALSE
    ˵    ���� */
	BOOL GetGroupAllUser( u8 byGrpId, CVCSUsrInfo *pcUserInfo, s32 &nNum ) const ;
    /*�������� CUserManage
    ��    �ܣ� �������캯��
    ��    ���� const CUserManage& rhs
    �� �� ֵ�� 
    ˵    ���� */
	CVcsUserManage( const CVcsUserManage& rhs );
    /*�������� operator=
    ��    �ܣ� ���ظ�ֵ�����
    ��    ���� const CUserManage& rhs
    �� �� ֵ�� CUserManage& 
    ˵    ���� */
	CVcsUserManage& operator=(const CVcsUserManage& rhs);
	
    /*�������� Print
    ��    �ܣ� ��ӡ�ڲ���Ϣ
    ��    ���� void
    �� �� ֵ�� void 
    ˵    ���� */
	void Print(void) const; 
	
private:
/*�������� Init
��    �ܣ� ��ʼ����Ϣ
��    ���� const CUserManage& rhs		[in]�����һ���û���������Ϣ
�� �� ֵ�� void 
    ˵    ���� */
	void Init(const CVcsUserManage& rhs);
};

//CPeriTable��MCU����ͳһ����Ϊһ��TplArray
class CPeriTable
{
public:
    CPeriTable();
	
    /*=============================================================================
    �� �� ��:ClearContent
    ��    ��:���������������
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	void ClearContent(void);
	
    /*=============================================================================
    �� �� ��:SetAt
    ��    ��:�����ˢ������״̬
    ��    ��:const TPeriEqpStatus &tPeriEqpStatus   [in]    ����״̬
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	void SetAt(const TPeriEqpStatus &tPeriEqpStatus);
	
    /*=============================================================================
    �� �� ��:GetAt
    ��    ��:��ȡָ������״̬
    ��    ��:const TEqp &tEqp                   [in]    ָ������
	TPeriEqpStatus &tPeriEqpStatus     [out]   ����״̬
    ע    ��:��
    �� �� ֵ:�ɹ�����TRUE, ʧ�ܷ���FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
	BOOL32 GetAt(const TEqp &tEqp, TPeriEqpStatus &tPeriEqpStatus) const;
	
    /*=============================================================================
    �� �� ��:SetEqpIP
    ��    ��:��������IP
    ��    ��:const TEqp &tEqp                   [in]    ָ������
	s32 dwIP                           [in]    �����IP
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
    void SetEqpIP(const TEqp &tEqp, u32 dwIP);
	
    /*=============================================================================
    �� �� ��:GetEqpIP
    ��    ��:���������IP
    ��    ��:const TEqp &tEqp                   [in]    ָ������
    ע    ��:��
    �� �� ֵ:����IP
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/04/26  4.0     ���    ����
    =============================================================================*/
    u32 GetEqpIP(const TEqp &tEqp) const;
	
	
    /*�������� SetDcsIP
    ��    �ܣ� ����DCS��IP��ַ
    ��    ����  const TEqp &tEqp  [in] DCS���ն˱�ʾ
	u32 dwIP			  [in] Dcs��IP��ַ
    �� �� ֵ�� void 
    ˵    ���� */
	void SetDcsIP( const TEqp &tEqp, u32 dwIP );
    /*�������� GetDcsIP
    ��    �ܣ� ���dcs��IP��ַ 
    ��    ���� const TEqp &tEqp
    �� �� ֵ�� u32 
    ˵    ���� */
	u32 GetDcsIP( const TEqp &tEqp );
    /*�������� SetDcsStatus
    ��    �ܣ� ����DCS��״̬
    ��    ���� const TPeriDcsStatus &tDcsStatus  [in] DCS״̬
    �� �� ֵ�� void 
    ˵    ���� */
	void SetDcsStatus( TPeriDcsStatus &tDcsStatus );
    /*�������� GetDcsStatus
    ��    �ܣ� ��ȡDCS��״̬ 
    ��    ����  const TEqp &tEqp				[in] DCS�ն˱�ʶ
	TPeriEqpStatus &tDcsStatus		[out] Dcs��״̬
    �� �� ֵ�� BOOL
    ˵    ���� */
	BOOL GetDcsStatus( const TEqp &tEqp, TPeriDcsStatus &tDcsStatus );
	
    /*=============================================================================
    �� �� ��:PrintInfo
    ��    ��:�����е������ӡ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/05/19  4.0     ���    ����
    =============================================================================*/
    void PrintInfo(void) const;
	
public:
    u32     m_adwEqpIP[MAXNUM_MCU_PERIEQP];     //  ����IP
	u32		m_adwDcsIP[MAXNUM_MCU_DCS];
	
    TplArray<TPeriEqpStatus> m_tplRecorder;     //  ���浱ǰMCU����¼�����״̬
    TplArray<TPeriEqpStatus> m_tplMixer;        //  ���浱ǰMCU���л�����״̬
    TplArray<TPeriEqpStatus> m_tplVmper;        //  ���浱ǰMCU������Ƶ������״̬
    TplArray<TPeriEqpStatus> m_tplTvWall;       //  ���浱ǰMCU���е���ǽ״̬
    TplArray<TPeriEqpStatus> m_tplHDTvWall;     //  ���浱ǰMCU���и������ǽ״̬
    TplArray<TPeriEqpStatus> m_tplAdapter;      //  ���浱ǰMCU��������������״̬
    TplArray<TPeriEqpStatus> m_tplPrsPerCher;   //  ���浱ǰMCU�����ش���״̬
    TplArray<TPeriEqpStatus> m_tplMultiTvWall;  //  ���浱ǰMCU���ж໭�����ǽ״̬
	// lrf DCS Status [6/12/2006]
	TplArray<TPeriDcsStatus> m_tplDcs;			//	���浱ǰMCU����DCS״̬
};

// ��¼����ǽ��ѯ��Ϣ
class CTwPollParam  
{
public:
	CTwPollParam();
	virtual ~CTwPollParam();
	/*====================================================================
	�� �� ���� SetTwPollParam
	��    �ܣ� ��ȡĳ��ͨ���ϵ���ѯȫ����
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� TTvWallPollParam&	tPollParam	Ҫ���õ���ѯ����
	�� �� ֵ�� void
	--------------------------------------------------------------------
	�޸ļ�¼��
	��  ��	        �汾		�޸���		�߶���       �޸�����
	2011/11/08      1.0			�޿���					 ����
	======================================================================*/
	void SetTwPollParam( TTvWallPollParam &tPollParam );
	
	/*====================================================================
	�� �� ���� GetChnlPollParam
	��    �ܣ� ��ȡĳ��ͨ���ϵ���ѯȫ����
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� 
	[in]	u8					byEqpId		�豸��
	[in]	u8					byChnId		ͨ����
	[out]	TTvWallPollParam&	tPollInfo	��ѯ��Ϣ
	�� �� ֵ�� BOOL 
	--------------------------------------------------------------------
	�޸ļ�¼��
	��  ��	        �汾		�޸���		�߶���       �޸�����
	2011/11/08      1.0			�޿���                   ����
	======================================================================*/
	BOOL GetChnPollParam( u8 byEqpId, u8 byChnId, TTvWallPollParam &tPollParam );
	
	/*====================================================================
	�� �� ���� GetChnlPollParam
	��    �ܣ� ��ȡĳ��ͨ���ϵ���ѯȫ����
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� 
	[in]	u16					byChnIID	ͨ�����豸��
	[out]	TTvWallPollParam&	tPollInfo	��ѯ��Ϣ
	�� �� ֵ�� BOOL 
	--------------------------------------------------------------------
	�޸ļ�¼��
	��  ��	        �汾		�޸���		�߶���       �޸�����
	2010/01/13      4.0			�½���                   ����
	2011/11/08		4.1			�޿���					 �޸�
	======================================================================*/
	BOOL GetChnPollParam( u16 wChnIID, TTvWallPollParam &tPollParam );
	
	/*====================================================================
	�� �� ���� GetChnPollState
	��    �ܣ� ��ȡͨ����ѯ״̬
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� 
	[in]	u8	byEqpId		������
	[in]	u8	byChnId		ͨ�����
	�� �� ֵ�� u8	��ѯ״̬
	POLL_STATE_NONE		ֹͣ
	POLL_STATE_PAUSE		��ͣ
	POLL_STATE_NORMAL	������
	--------------------------------------------------------------------
	�޸ļ�¼��
	��  ��	        �汾		�޸���		�߶���       �޸�����
	2011/11/08		4.1			�޿���					 ����
	======================================================================*/
	u8 GetChnPollState( u8 byEqpId, u8 byChnId);
	
	/*====================================================================
	�� �� ���� GetChnPollState
	��    �ܣ� ��ȡͨ����ѯ״̬
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� 
	[in]	u8	wChnIID		�ն���Ϣ
	�� �� ֵ�� u8	��ѯ״̬
	POLL_STATE_NONE		ֹͣ
	POLL_STATE_PAUSE		��ͣ
	POLL_STATE_NORMAL	������
	--------------------------------------------------------------------
	�޸ļ�¼��
	��  ��	        �汾		�޸���		�߶���       �޸�����
	2011/11/08		4.1			�޿���					 ����
	======================================================================*/
	u8 GetChnPollState( u16 wChnIId);
	
	/*====================================================================
	�� �� ���� IsMtInPollChn
	��    �ܣ� �ж��ն��Ƿ���������ѯ���б���
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� 
	[in]	TMt&	tMt		�ն���Ϣ
	�� �� ֵ�� BOOL
	--------------------------------------------------------------------
	�޸ļ�¼��
	��  ��	        �汾		�޸���		�߶���       �޸�����
	2010/01/13      4.0			�½���                   ����
	2011/11/08		4.1			�޿���					 ������ֹͣ����ѯ
	======================================================================*/
	BOOL IsMtInPollChn(const TMt &tMt);
	
	/*====================================================================
	�� �� ���� Clear
	��    �ܣ� ���ĳ���������ѯ��Ϣ��ȫ����ѯ��Ϣ
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� [in]	u8	byConfIdx	����Idx
	�� �� ֵ�� 
	--------------------------------------------------------------------
	�޸ļ�¼��
	��  ��	        �汾		�޸���		�߶���       �޸�����
	2011/11/08		4.1			�޿���					 �޸�
	======================================================================*/
	void Clear( u8 byConfIdx = 0 ); 
	
	/*====================================================================
	�� �� ���� GerAllPollMt
	��    �ܣ� ��ȡ����������ѯ�б�����ն�
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ���� [out] vector<TMtPollParam>& vctAllPollParam ������ѯ�ն��б�
	�� �� ֵ�� BOOL
	--------------------------------------------------------------------
	�޸ļ�¼��
	��  ��	        �汾		�޸���		�߶���       �޸�����
	2011/11/08		4.1			�޿���					 ������ֹͣ����ѯ
	======================================================================*/
	void GetAllPollMt(vector<TMtPollParam>& vctAllPollParam);
	
	/*====================================================================
	�� �� ���� Print
	��    �ܣ� ��ӡ��ѯ��Ϣ
	�㷨ʵ�֣� 
	ȫ�ֱ����� 
	��    ����
	�� �� ֵ�� 
	--------------------------------------------------------------------
	�޸ļ�¼��
	��  ��	        �汾		�޸���		�߶���       �޸�����
	2011/11/08		4.1			�޿���					 �޸�
	======================================================================*/
	void Print();
	
private:
	mapTwPollParam m_mapTwPollParam;
};

//  ����Ϣ
typedef struct tagNull
{
    tagNull()
    {
        tNullMt.SetNull();
		cNullMt.SetNull();
        ZeroMemory(&cNullUser, sizeof (cNullUser));
        ZeroMemory(&cNullVcsUser, sizeof (cNullVcsUser));
		ZeroMemory(&tNullUserGrp, sizeof(tNullUserGrp) );
        cNullMtAlias.SetNull();
        cNullConfId.SetNull();
        ZeroMemory(&cNullMixParam, sizeof (cNullMixParam));
    }
    TMt         tNullMt;
    CMt         cNullMt;
    CConfId     cNullConfId;
    CConf       cNullConf;
    CExUsrInfo   cNullUser;
    CVCSUsrInfo cNullVcsUser;
	TUsrGrpInfo		tNullUserGrp;
    TMtAlias    cNullMtAlias;
    TMixParam cNullMixParam;
} TNull;

extern TNull g_tNull;

//CMsgHeadInfo��װ��MCU OSPͨ����Ϣ����Ϣͷ
class CMsgHeadInfo
{
public:
    CMsgHeadInfo() : m_wSerialNo( 0 ),
		m_byMCUID( LOCAL_MCUIDX ),
		m_bySessionID( 0 ),
		m_cConfId( g_tNull.cNullConfId ),
		m_wEventID( 0 ),
		m_wDelayTime( DELAY_TIME_DEFAULT ),
		m_wLength( 0 ),
		m_byChannelIndex( 0 ),
		m_wMask( 0 ),
		m_byTotalPktNum(0),
		m_byCurPktIdx(0),
		m_byEqpId(0){}
	
    CMsgHeadInfo( u16 wEventId, u16 wDelayTime, u16 wLen,
		const CConfId *pcConfId = NULL );
	
	//CMsgHeadInfo����Щλ��Ч
#define MASK_CHNINDEX			(u16)0x0001
#define MASK_MCUID				(u16)0x0002
#define MASK_MTID				(u16)0x0004
#define MASK_SESSIONID			(u16)0x0008
#define MASK_CONFID				(u16)0x0010
#define MASK_EVENTID			(u16)0x0020
#define MASK_DELAYTIME			(u16)0x0040
#define MASK_LENGTH				(u16)0x0080
#define MASK_TOALPKTNUM			(u16)0x0100
#define MASK_CURPKTIDX			(u16)0x0200
#define MASK_EQPID				(u16)0x0400
	
public:
/*=============================================================================
�� �� ��:+=
��    ��:������Ϣͷ
��    ��:CMsgHeadInfo &cLeft                [in/out]    ������Ϣͷ
CMsgHeadInfo &cRight               [in]        ������Ϣͷ
ע    ��:��
�� �� ֵ:��
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/07/05  4.0     ���    ����
    =============================================================================*/
    friend CMsgHeadInfo& operator+=(CMsgHeadInfo &cLeft, CMsgHeadInfo &cRight);
	
    /*=============================================================================
    �� �� ��:IncreaseSerialNo
    ��    ��:�ݼ���Ϣ��ˮ��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    void IncreaseSerialNo(void) { m_wSerialNo++; }
	
    /*=============================================================================
    �� �� ��:GetSerialNo
    ��    ��:�õ���ˮ��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��ǰ��Ϣ��ˮ��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    u16 GetSerialNo(void) const { return m_wSerialNo; }
	
    /*=============================================================================
    �� �� ��:SetMCUID
    ��    ��:�趨Ŀ��MCUID������λ��Ч
    ��    ��:u16 wMcuId                         [in]    MCU ID
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    void SetMCUID(u8 byMcuId) { m_byMCUID = byMcuId; AddMask(MASK_MCUID); }
	
    /*=============================================================================
    �� �� ��:GetMCUID
    ��    ��:�õ�Ŀ��MCUID
    ��    ��:��
    ע    ��:��
    �� �� ֵ:Ŀ��MCU ID
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    u8 GetMCUID(void) { return ( Avail(MASK_MCUID) ? m_byMCUID : 0 ); }
	
    /*=============================================================================
    �� �� ��:SetSessionID
    ��    ��:�趨�Ựʵ����
    ��    ��:u8 bySessionId                     [in]    �Ựʵ����
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    void SetSessionID(u8 bySessionId)
    { m_bySessionID = bySessionId; AddMask(MASK_SESSIONID); }
	
    /*=============================================================================
    �� �� ��:GetSessionID
    ��    ��:�õ��Ựʵ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:�Ựʵ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    u8 GetSessionID(void) { return (Avail(MASK_SESSIONID) ? m_bySessionID : 0); }
	
    /*=============================================================================
    �� �� ��:SetConfID
    ��    ��:���û����
    ��    ��:CConfId cConfId                    [in]    �����
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    void SetConfID(CConfId cConfId) { m_cConfId = cConfId; AddMask(MASK_CONFID); }
	
    /*=============================================================================
    �� �� ��:GetConfID
    ��    ��:�õ������
    ��    ��:��
    ע    ��:��
    �� �� ֵ:�����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    CConfId& GetConfID(void)
    { return (Avail(MASK_CONFID) ? m_cConfId : g_tNull.cNullConfId); }
	
    /*=============================================================================
    �� �� ��:SetEventID
    ��    ��:�����¼���
    ��    ��:u16 wEventId                       [in]    �¼���
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    void SetEventID(u16 wEventId) { m_wEventID = wEventId; AddMask(MASK_EVENTID); }
	
    /*=============================================================================
    �� �� ��:GetEventID
    ��    ��:�õ��¼���
    ��    ��:��
    ע    ��:��
    �� �� ֵ:�¼���
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    u16 GetEventID(void) { return (Avail(MASK_EVENTID) ? m_wEventID : 0); }
	
    /*=============================================================================
    �� �� ��:SetDelayTime
    ��    ��:����MCU�ȴ��ظ�ʱ��
    ��    ��:u16 wDelayTime                     [in]    �ȴ�ʱ��
    ע    ��:MCUʵ���Ǹ��ݸ�ʱ���Ƿ����0���ж���Ϣ��������Ϣ����������Ϣ��
	��˷���REQ��Ϣ������벻Ϊ0������CMD��Ϣ�������Ϊ0
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    void SetDelayTime(u16 wDelayTime)
    { m_wDelayTime = wDelayTime; AddMask(MASK_DELAYTIME); }
	
    /*=============================================================================
    �� �� ��:GetDelayTime
    ��    ��:��ȡ��ʱʱ��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��ʱʱ��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    u16 GetDelayTime(void) { return (Avail(MASK_DELAYTIME) ? m_wDelayTime : 0); }
	
    /*=============================================================================
    �� �� ��:SetLength
    ��    ��:������Ϣ�峤��
    ��    ��:u16 wMsgLen                        [in]    ��Ϣ�峤��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    void SetLength(u16 wMsgLen) { m_wLength = wMsgLen; AddMask(MASK_LENGTH); }
	
    /*=============================================================================
    �� �� ��:GetLength
    ��    ��:������Ϣ�峤��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��Ϣ�峤��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    u16 GetLength(void) { return (Avail(MASK_LENGTH) ? m_wLength : 0); }
	
    /*=============================================================================
    �� �� ��:SetChannelIndex
    ��    ��:����ͨ����(�������ʱ�Ƚ���Ҫ)
    ��    ��:u8 byChannelIndex                  [in]    ͨ����
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    void SetChannelIndex(u8 byChannelIndex)
    { m_byChannelIndex = byChannelIndex; AddMask(MASK_CHNINDEX); }
	
    /*=============================================================================
    �� �� ��:GetChannelIndex
    ��    ��:�õ�ͨ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:ͨ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    u8 GetChannelIndex(void)
    { return (Avail(MASK_CHNINDEX) ? m_byChannelIndex : 0); }
	/*=============================================================================
    �� �� ��:SetTotalPktNum
    ��    ��:�����ܰ���
    ��    ��:u8 byTotalPktNum                  [in]    �ܰ���
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2009/12/23  4.0     �½���    ����
    =============================================================================*/
    void SetTotalPktNum(u8 byTotalPktNum)
    { m_byTotalPktNum = byTotalPktNum; AddMask(MASK_TOALPKTNUM); }
	
    /*=============================================================================
    �� �� ��:GetChannelIndex
    ��    ��:�õ��ܰ���
    ��    ��:��
    ע    ��:��
    �� �� ֵ:ͨ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2009/12/23  4.0     �½���    ����
    =============================================================================*/
    u8 GetTotalPktNum(void)
    { return (Avail(MASK_TOALPKTNUM) ? m_byTotalPktNum : 0); }
	/*=============================================================================
    �� �� ��:SetCurPktIdx
    ��    ��:���õ�ǰ����(��0��ʼ)
    ��    ��:u8 byCurPktIdx                  [in]    ��ǰ����
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2009/12/23  4.0     �½���    ����
    =============================================================================*/
    void SetCurPktIdx(u8 byCurPktIdx)
    { m_byCurPktIdx = byCurPktIdx; AddMask(MASK_CURPKTIDX); }
	
    /*=============================================================================
    �� �� ��:GetCurPktIdx
    ��    ��:�õ���ǰ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:ͨ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2009/12/23  4.0     �½���    ����
    =============================================================================*/
    u8 GetCurPktIdx(void)
    { return (Avail(MASK_CURPKTIDX) ? m_byCurPktIdx : 0); }

	void SetEqpId(u8 byEqpId) { m_byEqpId = byEqpId; AddMask(MASK_EQPID); }
	u8	 GetEqpId(void) 
	{ return (Avail(MASK_EQPID) ? m_byEqpId : 0); }
	
private:
/*=============================================================================
�� �� ��:AddMask
��    ��:������Чλ
��    ��:u8 byMask                          [in]    ��Чλ
ע    ��:��
�� �� ֵ:��
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/07/05  4.0     ���    ����
    =============================================================================*/
    void AddMask(u16 wMask) { m_wMask = m_wMask | wMask; }
	
    /*=============================================================================
    �� �� ��:Avail
    ��    ��:�ж�λ�Ƿ���Ч
    ��    ��:u8 byByte                          [in]    ����λ
    ע    ��:��
    �� �� ֵ:�Ƿ���Ч
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2005/07/05  4.0     ���    ����
    =============================================================================*/
    BOOL32 Avail(u16 wByte) { return ( ( wByte & m_wMask ) == wByte ); }
	
private:
    u16	    m_wSerialNo;            // ��Ϣ��ˮ��
	// modify by muxingmao ��������
    u16	    m_byMCUID;              // ��λĿ��MCU 
    u8	    m_bySessionID;          // �Ựʵ����
    CConfId	m_cConfId;              // �����
    u16	    m_wEventID;             // �¼���
    u16	    m_wDelayTime;           // �ȴ�ʱ�ӣ���λ���룩
    u16	    m_wLength;              // ��Ϣ�峤��
    u8	    m_byChannelIndex;       // ͨ��������
	u8      m_byTotalPktNum;        //�ܰ�����������Ҫ�а����͵���Ϣ��
    u8      m_byCurPktIdx;          //��ǰ����������0��ʼ��
    u16	    m_wMask;               // ��Ϣͷ�е���Щ��Ϣ����Ч��
	u8		m_byEqpId;				// EQPID
	
};

//#ifdef _DEBUG
///*=======================================================================
//�� �� ��:Dump
//��    ��:��ӡ��ĳ�Ա������Ϣ
//��    ��:����˵��
//ע    ��:��
//�� �� ֵ:��
//-------------------------------------------------------------------------
//�޸ļ�¼:
//��      ��  �汾    �޸���  �޸�����
//2004/03/19  3.0     ���ǿ  ����
//=======================================================================*/
//void Dump(const TMt &tMt,                       CDumpContext &dc);
//void Dump(const TMtExt &tMtExt,                 CDumpContext &dc);
//void Dump(const TMtStatus &tMtStatus,           CDumpContext &dc);
//void Dump(const TMtAlias &tMtAlias,             CDumpContext &dc);
//void Dump(const TTransportAddr &tTransportAddr, CDumpContext &dc);
//void Dump(const CUserFullInfo &cUserFullInfo,   CDumpContext &dc);
//#endif  //  _DEBUG

#ifdef WIN32
#pragma pack( push )
#pragma pack( 1 )
#endif
//8000H MCU ���ַӳ��ṹ��
struct TDMZAddress
{	
	TDMZAddress()
	{
		memset(this, 0, sizeof(TDMZAddress));
	}
	
    BOOL32 IsDMZAddressUse() const 
	{
		return (m_byUse == 1);
	}
	u32 GetDMZAddressIP() const 
	{
		return ntohl(m_dwAddressIp);
	}
	
	void SetDMZAddressIP(u32 dwAddressIP)
	{
		if (dwAddressIP != 0)
		{
			m_byUse		  = 1;
			m_dwAddressIp = htonl(dwAddressIP);
		}
		else
		{
			m_byUse		  = 0;
			m_dwAddressIp = 0;
		}
	}

protected:
	u8  m_byUse;         //0 ��ʾ�����ã�1 ��ʾ����
	u32 m_dwAddressIp;   //��ӳ���ַ
	
};

// MCUע��GK������֤
struct TMcuReGKPassword
{
	
	TMcuReGKPassword()
	{
		m_byUse = 0;
		memset(m_abyMcuReGKPassword, 0, sizeof(m_abyMcuReGKPassword));
	}
	~TMcuReGKPassword()
	{
		m_byUse = 0;
	}
	
	BOOL32 IsPasswordUse() const
	{
		return (m_byUse == 1);
	}
	const s8* GetMcuReGkPassword() const 
	{
		return m_abyMcuReGKPassword;
	}
	
    void SetMcuReGKPassword( const s8* abyMcuReGKPassword )
	{
		if (abyMcuReGKPassword != NULL)
		{
			if (0 == strlen(abyMcuReGKPassword))
			{
				m_byUse = 0;
				memset(m_abyMcuReGKPassword, 0, sizeof(m_abyMcuReGKPassword));
			}
			else
			{
				m_byUse = 1;
				memcpy(m_abyMcuReGKPassword, abyMcuReGKPassword, sizeof(m_abyMcuReGKPassword));	

				if (strlen(abyMcuReGKPassword) >= MCU_RE_GK_PASSWORD_LEN)
				{/*������ַ�����>=64ʱ*/
					m_abyMcuReGKPassword[MCU_RE_GK_PASSWORD_LEN - 1] = '\0';
				}
				
			}
		}		
	}


protected:
	u8  m_byUse;									   //0 ��ʾ�����ã�1 ��ʾ����
	s8  m_abyMcuReGKPassword[MCU_RE_GK_PASSWORD_LEN];  //DMZ ��ַ
};
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
//lvguanjiao 2013 0329 MCU һЩ�������õĲ��䣨�˽ṹ��Ĵ�����Ϊ�˼�����ǰMCU�汾��
//���MCU�˲�����洢�����ݣ���Ϊ����洢
struct TMcuGeneralCfgImp
{

	TMcuReGKPassword		  m_tMcuReGKPassword; //MCU ע��GK����
	TDMZAddress				  m_tDMZAddress;      //8000H MCU ���ַӳ��

	TMcuGeneralCfgImp()
	{
		memset(this, 0, sizeof(TMcuGeneralCfgImp));
	}
	void SetMcuReGKPassword(TMcuReGKPassword &tMcuReGKPassword)
	{
		m_tMcuReGKPassword = tMcuReGKPassword;
	}
	void SetDMZAddress(TDMZAddress& tDMZAddress)
	{
		m_tDMZAddress = tDMZAddress;
	}

};
#ifdef WIN32
#pragma pack( pop )
#endif

#define  MAX_REMARK_LENGTH 55
//����IP��Ϣ
struct TIpInfo
{
protected:
	u32 m_dwIpAddr;                        //Ip��ַ
    u32 m_dwSubnetMask;                    //��������
    u32 m_dwGWIpAddr;                      //���� Ip��ַ
	s8	m_aszRemark[MAX_REMARK_LENGTH];     //��ע

public:
	BOOL32 operator == (TIpInfo tIpInfo)
	{
		if (strcmp(this->GetRemark(), tIpInfo.GetRemark()) != 0)
		{
			return FALSE;
		}
		

		if (this->GetIpAddr() != tIpInfo.GetIpAddr() ||
			this->GetSubNetMask() != tIpInfo.GetSubNetMask() ||
			this->GetGWIpAddr() != tIpInfo.GetGWIpAddr())
		{
			return FALSE;
		}
		return TRUE;
	}

	TIpInfo(void) { memset(this, 0, sizeof(TIpInfo)); }
    
    void    SetIpAddr(u32 dwIpAddr) { m_dwIpAddr = htonl(dwIpAddr); }     //host order
    u32     GetIpAddr(void)  { return ntohl(m_dwIpAddr); }
    void    SetSubNetMask(u32 dwMask) { m_dwSubnetMask = htonl(dwMask); }
    u32     GetSubNetMask(void) { return ntohl(m_dwSubnetMask); }
    void    SetGWIpAddr(u32 dwGWIpAddr) { m_dwGWIpAddr = htonl(dwGWIpAddr); }   //host order
    u32     GetGWIpAddr(void) { return ntohl(m_dwGWIpAddr); }
	void    SetRemark(LPCSTR lpszRemark)
    {
        if(NULL != lpszRemark)
        {
            strncpy(m_aszRemark, lpszRemark, sizeof(m_aszRemark));
            m_aszRemark[sizeof(m_aszRemark) - 1] = '\0';
        }        
    }
    LPCSTR GetRemark(void) const { return m_aszRemark; }
};

#define  INNER_GK   0
#define  OUTER_GK   1
#define  VALID_GK   2
//GK��Ϣ
struct TGkInfo
{
protected:
	u8     m_byEnableInnerGK;
	u8     m_byInnerGKPort;
	u32    m_dwInnerGKIp;
	u8     m_byTypeOfGk;
	u8     m_byEnableRegGk;

public:
	TGkInfo()
	{
		m_byEnableRegGk = 0;
		m_byEnableInnerGK = 0;
		m_byInnerGKPort = 0;
		m_byTypeOfGk = 0;
		m_dwInnerGKIp = 0;
	}
	//����GK���������
	void SetEnableInnerGK(u8 bEnableInnerGk)
	{
		m_byEnableInnerGK = bEnableInnerGk;
	}
	u8 GetEnableInnerGk()
	{
		return m_byEnableInnerGK;
	}

	//����gk�˿�
	void SetInnerGKPort(u8 byInnerGkPort)
	{
		m_byInnerGKPort = byInnerGkPort;
	}
	u8 GetInnerGKPort()
	{
		return m_byInnerGKPort;
	}

	//����GK��IP
	void SetInnerGKIp(u32 dwInnerGKIp)
	{
		m_dwInnerGKIp = htonl(dwInnerGKIp);
	}
	u32 GetInnerGKIp()
	{
		return ntohl(m_dwInnerGKIp);
	}

	//ע��GK����
	void SetGkType(u8 byGkType)
	{
		m_byTypeOfGk = byGkType;
	}
	u8 GetGkType()
	{
		return m_byTypeOfGk;
	}

	//�Ƿ�ע��GK
	void SetEnableGkReg(u8 byEnableGkReg)
	{
		m_byEnableRegGk = byEnableGkReg;
	}

	u8 GetEnableGkReg()
	{
		return m_byEnableRegGk;
	}

};

#define  MAX_IPNUM_OF_ONE_NET   16
#define  MAX_NETPORT_NUM        3
//�������ڵ���Ϣ�������Ƿ�����DMZ��ַ�����IP��ַ
struct TNetCardInfo
{
protected:
	BOOL32  m_bUseDMZ;
	u32     m_dwDMZIp;
	TIpInfo m_aIpInfo[MAX_IPNUM_OF_ONE_NET];
	u8      m_byIpNum;
public:
	TNetCardInfo()
	{
		memset(this, 0, sizeof(TNetCardInfo));
	}
	BOOL32 IsEnableDMZ()
	{
		return m_bUseDMZ;
	}
	void SetEnableDMZ(BOOL32 bEnableDMZ)
	{
		m_bUseDMZ = bEnableDMZ;
	}
	void SetDMZIp(u32 dwIpAddr)
	{
		m_dwDMZIp = htonl(dwIpAddr);
	}
	u32 GetDMZIp()
	{
		return ntohl(m_dwDMZIp);
	}
	u8 GetNumOfTIpInfo()
	{
		return m_byIpNum;
	}
	void SetNumOfTIpInfo(u8 byNum)
	{
		m_byIpNum = byNum;
	}
	TIpInfo* GetIpInfo(u8 byIndex)
	{
		if (byIndex >= MAX_IPNUM_OF_ONE_NET)
		{
			return NULL;
		}
		return &m_aIpInfo[byIndex];
	}
	BOOL32 IsIpExist(TIpInfo tIpInfo)
	{
		s32 nIndex = 0;
		for (nIndex = 0; nIndex < m_byIpNum; nIndex++)
		{
			if (m_aIpInfo[nIndex].GetIpAddr() == tIpInfo.GetIpAddr())
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	BOOL32 IsIpExist(TIpInfo tIpInfo, u8& byIdx)
	{
		s32 nIndex = 0;
		for (nIndex = 0; nIndex < m_byIpNum; nIndex++)
		{
			if (m_aIpInfo[nIndex].GetIpAddr() == tIpInfo.GetIpAddr())
			{
				byIdx = nIndex;
				return TRUE;
			}
		}
		return FALSE;
	}
	BOOL32 AddIpInfo(TIpInfo& tIpInfo)
	{
		if (m_byIpNum >= MAX_IPNUM_OF_ONE_NET || tIpInfo.GetIpAddr() == 0)
		{
			return FALSE;
		}
		
		else
		{
			s32 nIndex = 0;
			for (nIndex = 0; nIndex < m_byIpNum; nIndex++)
			{
				if (m_aIpInfo[nIndex].GetIpAddr() == tIpInfo.GetIpAddr())
				{
					return FALSE;
				}
			}
			m_aIpInfo[m_byIpNum++] = tIpInfo;
			return TRUE;
		}
	}
	BOOL32 ModifIpInfo(u8 byIndex, TIpInfo& tIpInfo)
	{
		if (byIndex > MAX_IPNUM_OF_ONE_NET)
		{
			return FALSE;
		}
		
		else
		{
			s32 nIndex = 0;
			for (nIndex = 0; nIndex < m_byIpNum; nIndex++)
			{
				if (tIpInfo == m_aIpInfo[nIndex] && byIndex != nIndex)
				{
					return FALSE;
				}
			}
			m_aIpInfo[byIndex] = tIpInfo;
			return TRUE;
		}	
	}
	BOOL32 DelIpInfo(u8 byIndex)
	{
		if (byIndex >= MAX_IPNUM_OF_ONE_NET || m_byIpNum == 0)
		{
			return FALSE;
		}
		else
		{
			u8 byLoop = 0;
			for(byLoop = byIndex; byLoop < m_byIpNum; byLoop ++)
			{				
				m_aIpInfo[byLoop] = m_aIpInfo[byLoop + 1];
			}
			m_byIpNum--;
			return TRUE;
		}
	}
};

#define  MIX_NET       0 
#define  WAN_NET       1
#define  LAN_NET       2 
#define  VALID_NET     3
//�µĽṹ����������mcu���õ���Ϣ
struct TMcsMcuGeneralCfg
{
protected:
	//8000I��������
	BOOL32			   m_bEnablePortBackUp;				//�Ƿ����ڱ���
	BOOL32			   m_bEnableSipService;				//�Ƿ�����SIP����
	TIpInfo			   m_tSipService;					//SIP������Ϣ
	u8				   m_byInterface;					//��ǰʹ�õ�����
	TDMZAddress		   m_tDmzAddress;					//DMZ��Ϣ      
	TNetCardInfo       m_atNetCardInfo[MAX_NETPORT_NUM];//������Ϣ
	u8                 m_byTypeofNet;					//��ѡ�������ͣ��繫����˽��...
	TGkInfo            m_tGkInfo;                       //GK��Ϣ
	u8                 m_bySipPort;                     //SIP������ʹ�õ�����           
	u8                 m_byDebugMode;                   //�Ƿ�������ģʽ��0��������1����     

public:
	TMcuGeneralCfgImp  m_tMcuGeneralCfgImp;				//GKע��������Ϣ��DMZ��ַ��Ϣ
	TMcuNetCfg		   m_tNetCardCfg;					//8000E������Ϣ
	TMcuGeneralCfg     m_tGeneral;						//mcu����������Ϣ
	TMcuEqpCfg         m_tEqp;							//�������ڵ�������Ϣ

	void Clear()
	{
		m_byDebugMode = 0;
		m_bEnablePortBackUp = 0;
		m_bEnableSipService = 0;
		m_byInterface = 0;
		m_byTypeofNet = 2;
		m_bySipPort = 0;
		ZeroMemory( &m_tMcuGeneralCfgImp, sizeof(TMcuGeneralCfgImp) );
		ZeroMemory( &m_tNetCardCfg, sizeof( TMcuNetCfg ) );
		ZeroMemory( &m_tGeneral, sizeof( TMcuGeneralCfg ) );
		ZeroMemory( &m_tEqp, sizeof( TMcuEqpCfg ) );
		ZeroMemory( &m_tSipService, sizeof( TIpInfo ) );
		ZeroMemory( &m_tDmzAddress, sizeof(TDMZAddress) );
		ZeroMemory( &m_atNetCardInfo, sizeof(TNetCardInfo) * MAX_NETPORT_NUM);
		ZeroMemory( &m_tGkInfo, sizeof(TGkInfo));
	}

	TMcsMcuGeneralCfg()
	{
		Clear();
	}

	//8000I�����ӿ�

	//���½ӿڸ�mcu basic����ʹ��
	//���õ���ģʽ
	void SetDebugMode(u8 byDebugMode)
	{
		m_byDebugMode = byDebugMode;
	}
	
	//��ȡ����ģʽ
	u8 GetDebugMode()
	{
		return m_byDebugMode;
	}

	//��ȡ����SIP IP
	u32 GetInnerSipIp()
	{
		return m_tSipService.GetIpAddr();
	}
	
	//��ȡmcu IP
	u32 GetMcuIp()
	{
		TIpInfo* ptIpInfo = m_atNetCardInfo[0].GetIpInfo(0);
		if (ptIpInfo == NULL)
		{
			return 0;
		}
		return ptIpInfo->GetIpAddr();	
	}

	//����Ϊ���ڡ�mcu������Ҫʹ�õĽӿ�
	//Ѱ�Ҷ�ӦIP���ڵ����ڣ���������.��������GK������SIP
	u8 GetNetPortOfIp(u32 dwIp)
	{
		u8 byIndex = 0;
		for (byIndex = 0; byIndex < MAX_NETPORT_NUM; byIndex++)
		{
			//��������
			TNetCardInfo tNetCardInfo = m_atNetCardInfo[byIndex];
			u8 byIdx = 0;
			for (byIdx = 0; byIdx < MAX_IPNUM_OF_ONE_NET; byIdx++)
			{
				//����ÿ��IP
				TIpInfo* ptIpInfo = tNetCardInfo.GetIpInfo(byIdx);
				if (ptIpInfo != NULL)
				{
					if (dwIp == ptIpInfo->GetIpAddr())
					{
						return byIndex;
					}
				}			
			}
		}
		return 0;
	}

	//TNewNetCfg����mcu�ཻ����ʹ�õĽṹ��
	//��TNewNetCfg�е����ݱ��浽TMcsMcuGeneralCfg��
	void SetCfgInfo(TNewNetCfg tNewNetCfg)
	{
		//����ѡ��
		m_byTypeofNet = tNewNetCfg.GetNetworkType();

		//���ڱ��� or 3����
		if (tNewNetCfg.GetUseModeType() == MODETYPE_BAK)
		{
			m_bEnablePortBackUp = TRUE;
		}
		else
		{
			m_bEnablePortBackUp = FALSE;
		}

		//����0 1 2�����ݴ���
		u32 dwIndex = 0;
		//һ��3������
		for (dwIndex = 0; dwIndex < MAX_NETPORT_NUM; dwIndex++)
		{
			//�������ڵ���Ϣ
			TEthCfg tEthCfg;
			tNewNetCfg.GetEthCfg(dwIndex, tEthCfg);
			TNetCardInfo* ptNetCardInfo = &m_atNetCardInfo[dwIndex];
			//DMZ��ַ
			if (tEthCfg.IsUseDMZ())
			{
				ptNetCardInfo->SetEnableDMZ(TRUE);
				ptNetCardInfo->SetDMZIp(tEthCfg.GetDMZIpAddr());
			}
			else
			{
				ptNetCardInfo->SetEnableDMZ(FALSE);
				ptNetCardInfo->SetDMZIp(tEthCfg.GetDMZIpAddr());
			}
			
			//IP��Ϣ,��16��
			u32 dwIdx = 0;
			for (dwIdx = 0; dwIdx < MAX_IPNUM_OF_ONE_NET; dwIdx++)
			{
				//ÿ��IP����Ϣ
				TNetCfg tNetCfg;
				tEthCfg.GetNetCfg(dwIdx, tNetCfg);
				TIpInfo tIpInfo; 
				tIpInfo.SetIpAddr(tNetCfg.GetIpAddr());
				tIpInfo.SetSubNetMask(tNetCfg.GetIpMask());
				tIpInfo.SetGWIpAddr(tNetCfg.GetGwIp());
				tIpInfo.SetRemark(tNetCfg.GetNotes());
				if (dwIdx == 0 && ptNetCardInfo->GetNumOfTIpInfo() != 0)
				{
					ptNetCardInfo->ModifIpInfo(0, tIpInfo);
				}
				else
				{
					ptNetCardInfo->AddIpInfo(tIpInfo);
				}
				
			}
		}
		

		//sip����
		m_bEnableSipService = tNewNetCfg.IsUseSip();
		u32 dwIp = tNewNetCfg.GetSipIpAddr();
		if (m_bEnableSipService)
		{
			m_bEnableSipService = TRUE;
			m_bySipPort = GetNetPortOfIp(dwIp);
			m_tSipService.SetIpAddr(dwIp);
		}
	}

	//���������õ�TNewNetCfg��
	void SetCfgInfoForMcu(TNewNetCfg &tNewNetCfg)
	{
		//����ѡ��
		enNetworkType enType = (enNetworkType)m_byTypeofNet;
		tNewNetCfg.SetNetworkType(enType);
		//���ڱ��� or 3����
		if (m_bEnablePortBackUp)
		{
			tNewNetCfg.SetUseModeType(MODETYPE_BAK);
		}
		else
		{
			tNewNetCfg.SetUseModeType(MODETYPE_3ETH);
		}
		//sip����
		if (m_bEnableSipService)
		{
			tNewNetCfg.SetIsUseSip(TRUE);
			tNewNetCfg.SetSipIpAddr(m_tSipService.GetIpAddr());
		}
		else
		{
			tNewNetCfg.SetIsUseSip(FALSE);
			tNewNetCfg.SetSipIpAddr(0);
		}
		//����0 1 2�����ݴ���
		u32 dwIndex = 0;
		//һ��3������
		for (dwIndex = 0; dwIndex < MAX_NETPORT_NUM; dwIndex++)
		{
			//�������ڵ���Ϣ
			TEthCfg tEthCfg;
			TNetCardInfo tNetCardInfo = m_atNetCardInfo[dwIndex];
			//DMZ��ַ
			if (tNetCardInfo.IsEnableDMZ())
			{
				tEthCfg.SetDMZIpAddr(tNetCardInfo.GetDMZIp());
			}
			else
			{
				tEthCfg.SetDMZIpAddr(0);
			}

			//IP��Ϣ,��16��
			u32 dwIdx = 0;
		
			for (dwIdx = 0; dwIdx < MAX_IPNUM_OF_ONE_NET; dwIdx++)
			{
				//ÿ��IP����Ϣ
				TNetCfg tNetCfg;
				TIpInfo* ptIpInfo = tNetCardInfo.GetIpInfo(dwIdx);
				if (ptIpInfo != NULL)
				{
					tNetCfg.SetNetRouteParam(ptIpInfo->GetIpAddr(), ptIpInfo->GetSubNetMask(), ptIpInfo->GetGWIpAddr());
					tNetCfg.SetNotes(ptIpInfo->GetRemark());
				}
				tEthCfg.SetNetCfg(dwIdx, tNetCfg);
			}
			tNewNetCfg.SetEthCfg(dwIndex, tEthCfg);
		}
	}

	//8000I������GK GKע�����Ϣ
	//���������õ�TMcuGeneralCfg��
	void SetGeneralCfgForMcu(TMcuGeneralCfg &tMcuGeneralCfg)
	{
		//����GK����
		if (m_tGkInfo.GetEnableInnerGk() != 0)
		{
			u32 dwIp = m_tGkInfo.GetInnerGKIp();
			tMcuGeneralCfg.m_tGkProxyCfg.SetGkUsed(TRUE);
			tMcuGeneralCfg.m_tGkProxyCfg.SetGkIpAddr(dwIp);
		}
		else
		{
			tMcuGeneralCfg.m_tGkProxyCfg.SetGkUsed(FALSE);
			tMcuGeneralCfg.m_tGkProxyCfg.SetGkIpAddr(0);
		}	
		//��ʱĬ�Ϸſ����ô������������ʹ��   lbg 2013.11.21
		tMcuGeneralCfg.m_tGkProxyCfg.SetProxyUsed(1);
	}

	//��TMcuGeneralCfg��GK��Ϣ��������
	void SetGeneralCfg(TMcuGeneralCfg tMcuGeneralCfg)
	{
		if (tMcuGeneralCfg.m_tGkProxyCfg.IsGkUsed())
		{
			m_tGkInfo.SetEnableInnerGK(TRUE);
			u32 dwIp = tMcuGeneralCfg.m_tGkProxyCfg.GetGkIpAddr();
			m_tGkInfo.SetInnerGKIp(dwIp);
			u8 byNetPort = GetNetPortOfIp(dwIp);
			m_tGkInfo.SetInnerGKPort(byNetPort);
		}
		else
		{
			m_tGkInfo.SetEnableInnerGK(FALSE);
			m_tGkInfo.SetInnerGKIp(0);
			m_tGkInfo.SetInnerGKPort(0);
		}

		u32 dwIpGkReg = tMcuGeneralCfg.m_tNet.GetGkIpAddr();
		if (dwIpGkReg == 0)
		{
			m_tGkInfo.SetEnableGkReg(FALSE);
			m_tGkInfo.SetGkType(VALID_GK);
		}
		else
		{
			m_tGkInfo.SetEnableGkReg(TRUE);
			TIpInfo tIpInfo;
			tIpInfo.SetIpAddr(dwIpGkReg);
			if (IsIpInfoExist(tIpInfo))
			{
				m_tGkInfo.SetGkType(INNER_GK);
			}
			else
			{
				m_tGkInfo.SetGkType(OUTER_GK);
			}
		}
	}

	//�Ƿ��Ѵ��ڸ�IP��3�������е�IP�����ظ�
	BOOL32 IsIpInfoExist(TIpInfo tIpInfo)
	{
		u8 byIndex = 0;
		for (byIndex = 0; byIndex < MAX_NETPORT_NUM; byIndex++)
		{
			//each netport
			if (m_atNetCardInfo[byIndex].IsIpExist(tIpInfo))
			{
				return TRUE;			
			}		
		}
		return FALSE;
	}

	//������IP�ĸ���
    void GetNumOfIp(u8 &byNum)
	{
		u8 byIndex = 0;
		for (byIndex = 0; byIndex < MAX_NETPORT_NUM; byIndex++)
		{
			TNetCardInfo tNetCardInfo = m_atNetCardInfo[byIndex];
			u8 byIdx = 0;
			for (byIdx = 0; byIdx < MAX_IPNUM_OF_ONE_NET; byIdx++)
			{
				TIpInfo* ptIpInfo = tNetCardInfo.GetIpInfo(byIdx);
				if (ptIpInfo != NULL)
				{
					u32 dwIp = ptIpInfo->GetIpAddr();
					if (dwIp != 0)
					{
						byNum++;
					}
				}
			}
		}
	}

	//��IP�Ƿ��Ѵ���
	BOOL32 IsIpInfoExist(TIpInfo tIpInfo, u8 byNetPort, u8 byIdx)
	{
		u8 byIndex = 0;
		for (byIndex = 0; byIndex < MAX_NETPORT_NUM; byIndex++)
		{
			//each netport
			u8 byIdx2 = 0;
			if (m_atNetCardInfo[byIndex].IsIpExist(tIpInfo, byIdx2))
			{
				if (byIndex != byNetPort || byIdx != byIdx2)
				{
					return TRUE;
				}			
			}		
		}
		return FALSE;
	}

	//������������
	void SetTypeofNet(u8 byType)
	{
		m_byTypeofNet = byType;
	}

	//��ȡ��������
	u8 GetTypeofNet()
	{
		return m_byTypeofNet;
	}

	//�Ƿ����ڱ���
	BOOL32 IsEnablePortBackUp()
	{
		return m_bEnablePortBackUp;
	}

	//�����Ƿ����ڱ���
	void SetEnablePortBackUp(BOOL32 bEnablePortBackUp)
	{
		m_bEnablePortBackUp = bEnablePortBackUp;
	}
	
	//���õ���������Ϣ
	BOOL32 SetOneNetCardInfo(u8 byNetPort, TNetCardInfo tNetCardInfo)
	{
		if (byNetPort >= MAX_NETPORT_NUM)
		{
			return FALSE;
		}
		else
		{
			m_atNetCardInfo[byNetPort] = tNetCardInfo;

			TIpInfo *ptIpInfo = tNetCardInfo.GetIpInfo(0);
			if(ptIpInfo == NULL)
			{
				return FALSE;
			}		
			m_tEqp.SetMcuIpAddr(ptIpInfo->GetIpAddr());
			m_tEqp.SetMcuSubNetMask(ptIpInfo->GetSubNetMask());
			m_tEqp.SetGWIpAddr(ptIpInfo->GetGWIpAddr());

			m_tNetCardCfg.SetMcuEqpCfg(byNetPort, m_tEqp);
			return TRUE;
		}		
	}

	//��ȡ����������Ϣ
	TNetCardInfo* GetOneNetCardInfo(u8 byNetPort)
	{
		if (byNetPort >= MAX_NETPORT_NUM )
		{
			return NULL;
		}
		else
		{
			return &m_atNetCardInfo[byNetPort];
		}
	}

	//�����ϰ汾�ĵ������ڵ���Ϣ����Ҫ��Ϊ�˼����ϰ汾����lib��������ݵ�ת��
	void SetMcuEqpCfg(u8 byInterface, TMcuEqpCfg tMcuEqpCfg)
	{
		//��mcu���õĵط�����ֱ����m_tEqp
		m_tEqp = tMcuEqpCfg;
		m_byInterface = 0;
		TNetCardInfo* ptNetCardInfo = GetOneNetCardInfo(byInterface);
		if (ptNetCardInfo == NULL)
		{
			return;
		}

		TIpInfo* ptIpInfo = ptNetCardInfo->GetIpInfo(0);
		if (ptIpInfo == NULL)
		{
			return;
		}

		ptIpInfo->SetIpAddr(tMcuEqpCfg.GetMcuIpAddr());
		ptIpInfo->SetSubNetMask(tMcuEqpCfg.GetMcuSubNetMask());
		ptIpInfo->SetGWIpAddr(tMcuEqpCfg.GetGWIpAddr());

		ptNetCardInfo->AddIpInfo(*ptIpInfo);
		SetOneNetCardInfo(byInterface, *ptNetCardInfo);
	}

	//��ȡ�ϰ汾�ĵ������ڵ���Ϣ
    TMcuEqpCfg* GetMcuEqpCfg(u8 byInterface, BOOL32 bNewVersion)
	{		
		return &m_tEqp;			
	}

	//���½ṹ���е����ݱ��浽�ϵĽṹ��TMCUEQPCFG��,����ֱ����GET��������SET����
	void SetMcuEqpCfgForOldVersion(u8 byInterface, BOOL32 bNewVersion)
	{
		TNetCardInfo *ptNetCardInfo = GetOneNetCardInfo(byInterface);
		if (ptNetCardInfo == NULL)
		{
			return;
		}

		TIpInfo *ptIpInfo = ptNetCardInfo->GetIpInfo(0);
		if(ptIpInfo == NULL)
		{
			return;
		}

		m_tEqp.SetMcuIpAddr(ptIpInfo->GetIpAddr());
		m_tEqp.SetMcuSubNetMask(ptIpInfo->GetSubNetMask());
		m_tEqp.SetGWIpAddr(ptIpInfo->GetGWIpAddr());
	}

	//�����ϰ汾��������Ϣ������0����
	void SetMcuNetCfg(TMcuNetCfg tMcuNetCfg)
	{
		//��mcu���õĵط�����ֱ����m_tMcuNetCfg
		m_tNetCardCfg = tMcuNetCfg;
		m_byInterface = tMcuNetCfg.GetInterface();
		s32 nIndex = 0;
		for (nIndex = 0; nIndex < MAXNUM_ETH_INTERFACE; nIndex++)
		{
			SetMcuEqpCfg(nIndex, tMcuNetCfg.m_atMcuEqpCfg[nIndex]);
		}
	}

	//��ȡ�ϰ汾��������Ϣ
	TMcuNetCfg* GetMcuNetCfg()
	{		
		return &m_tNetCardCfg;
	}

	//���½ṹ���е����ݱ��浽�ϵĽṹ��TNETCARDCFG��,����ֱ����GET��������SET����
	void SetMcuNetCfgForOldVersion()
	{
		m_tNetCardCfg.SetInterface(m_byInterface);
		s32 nIndex = 0;
		for (nIndex = 0; nIndex < MAXNUM_ETH_INTERFACE; nIndex++)
		{
			TMcuEqpCfg* ptMcuEqpCfg = GetMcuEqpCfg(nIndex);
			m_tNetCardCfg.SetMcuEqpCfg(nIndex, *ptMcuEqpCfg);
		}
	}
	
	//�����ϰ汾��DMZ��Ϣ
	void SetDmzIp(TDMZAddress tDMZAddress)
	{
		m_tDmzAddress = tDMZAddress;
		m_byInterface = 0;
		TNetCardInfo* ptNetCardInfo = GetOneNetCardInfo(m_byInterface);
		if (ptNetCardInfo == NULL)
		{
			return;
		}
		
		ptNetCardInfo->SetEnableDMZ(tDMZAddress.IsDMZAddressUse());
		ptNetCardInfo->SetDMZIp(tDMZAddress.GetDMZAddressIP());
		SetOneNetCardInfo(0, *ptNetCardInfo);
	}

	//��ȡ�ϰ汾��DMZ��Ϣ
	TDMZAddress* GetDmzAddress()
	{
		return &m_tDmzAddress;		
	}

	//���½ṹ���е����ݱ��浽�ϵĽṹ��TDMZAddress��,����ֱ����GET��������SET����
	void SetDmzAddressForOldVersion()
	{
		TNetCardInfo *ptNetCardInfo = GetOneNetCardInfo(m_byInterface);
		if (ptNetCardInfo == NULL)
		{
			return;
		}
		m_tDmzAddress.SetDMZAddressIP(ptNetCardInfo->GetDMZIp());
	}

	//�Ƿ�����SIP����
	BOOL32 IsEnableSipService()
	{
		return m_bEnableSipService;
	}

	void SetEnableSipService(BOOL32 bEnableSipService)
	{
		m_bEnableSipService = bEnableSipService;
	}

	void SetSipPort(u8 byPort)
	{
		m_bySipPort = byPort;
	}
	u8 GetSipPort()
	{
		return m_bySipPort;
	}
	void SetSipService(TIpInfo tIpInfo)
	{
		m_tSipService = tIpInfo;
	}

	TIpInfo* GetSipService()
	{
		return &m_tSipService;
	}

	//GK��Ϣ
	void SetGkInfo(TGkInfo tGkInfo)
	{
		m_tGkInfo = tGkInfo;
	}
	TGkInfo* GetGkInfo()
	{
		return &m_tGkInfo;
	}


	//TMcuGeneralCfgImp�����нӿ���ȡ
	//����mcuע��GK������Ϣ(�ṹ��)
	void SetMcuReGKPassword(TMcuReGKPassword &tMcuReGKPassword)
	{
		m_tMcuGeneralCfgImp.m_tMcuReGKPassword = tMcuReGKPassword;
	}

	//����DMZ��ַ��Ϣ���ṹ�壩
	void SetDMZAddress(TDMZAddress& tDMZAddress)
	{
		m_tMcuGeneralCfgImp.m_tDMZAddress = tDMZAddress;
	}

	//�Ƿ�����GK����
	BOOL32 IsPasswordUse() const
	{
		return m_tMcuGeneralCfgImp.m_tMcuReGKPassword.IsPasswordUse();
	}

	//����GKע������
	void SetMcuReGKPassword( const s8* abyMcuReGKPassword )
	{
		m_tMcuGeneralCfgImp.m_tMcuReGKPassword.SetMcuReGKPassword(abyMcuReGKPassword);
	}

	//��ȡGKע������
	const s8* GetMcuReGkPassword() const 
	{
		return m_tMcuGeneralCfgImp.m_tMcuReGKPassword.GetMcuReGkPassword();
	}

	//�Ƿ�ʹ��DMZ��ַ
	BOOL32 IsDMZAddressUse() 
	{
		return m_atNetCardInfo[m_byInterface].IsEnableDMZ();
	}

	//����DMZ ip��ַ
	void SetDMZAddressIP(u32 dwAddressIP)
	{
		TDMZAddress tDmzAddress;
		tDmzAddress.SetDMZAddressIP(dwAddressIP);
		SetDmzIp(tDmzAddress);
	}

	//��ȡDMZ ip��ַ
	u32 GetDMZAddressIP() const
	{
		return m_tDmzAddress.GetDMZAddressIP();
	}

	//TMcuNetCfg�����нӿ���ȡ
	//��ȡ���ڽӿ�
	u8 GetInterfaceForNetCfg( void ) const
	{
		return m_tNetCardCfg.GetInterface();
	}

	//�������ڽӿ�
	void SetInterfaceForNetCfg( u8 byInterface)
	{
		m_tNetCardCfg.SetInterface(byInterface);
	}

	//��ȡ�������õĻ�����Ϣ
	TMcuEqpCfg *GetMcuEqpCfg(u8 byIdx)
	{
		return m_tNetCardCfg.GetMcuEqpCfg(byIdx);
	}

	//TMcuEqpCfg�����нӿ���ȡ
	//����IP��ַ
	void SetMcuIpAddr(u32 dwIpAddr)
	{
		m_tEqp.SetMcuIpAddr(dwIpAddr);
	}

	//��ȡIP��ַ
	u32 GetMcuIpAddr()
	{
		return m_tEqp.GetMcuIpAddr();
	}

	//������������
	void SetMcuSubNetMask(u32 dwMask)
	{
		m_tEqp.SetMcuSubNetMask(dwMask);
	}

	//��ȡ��������
	u32 GetMcuSubNetMask()
	{
		return m_tEqp.GetMcuSubNetMask();
	}

	//��������
	void SetGWIpAddr(u32 dwGWIpAddr)
	{
		m_tEqp.SetGWIpAddr(dwGWIpAddr);
	}

	//��ȡ����
	u32 GetGWIpAddr()
	{
		return m_tEqp.GetGWIpAddr();
	}

	//���ò��
	void SetLayer(u8 byLayer)
	{
		m_tEqp.SetLayer(byLayer);
	}

	//��ȡ���
	u8 GetLayer()
	{
		return m_tEqp.GetLayer();
	}

	//���òۺ�
	void SetSlot(u8 bySlotId)
	{
		m_tEqp.SetSlot(bySlotId);
	}

	//��ȡ�ۺ�
	u8 GetSlot()
	{
		return m_tEqp.GetSlot();
	}

	//��������
	void SetInterfaceForEqpCfg(u8 byInterface)
	{
		m_tEqp.SetInterface(byInterface);
	}

	//��ȡ����
	u8 GetInterfaceForEqpCfg()
	{
		return m_tEqp.GetInterface();
	}

};

// ���浥��MCU�����ý��滯��Ϣ
class CMcuCfg : public TMcuExt
{
public:
/*=============================================================================
�� �� ��:CMcuCfg
��    ��:
��    ��:
ע    ��:��
�� �� ֵ:
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/09/09	4.0		����	����
2006/02/17  4.0     ����  �ӽ����ƶ���lib����
    =============================================================================*/
	CMcuCfg()
    {
        Clear();
    }
    ~CMcuCfg()
    {
        Clear();
    }
	
    // �� TMcuExt �ṹ����
    CMcuCfg& operator =(const TMcuExt &tMcuExt);
	
	CMcuCfg& operator =(const TMcuEqpCfg &tEqp);
	
	CMcuCfg& operator =(const TMcuGeneralCfg &tGeneral);
	CMcuCfg& operator =(const TMcuNetCfg &tNetCardCfg);
	CMcuCfg& operator =(const CMcuCfg& cMcuCfg);
	
	void SetBrdCfg(s32 nNum, CBrdCfg* pcBrdCfg) ;
	void SetRecCfg(s32 nNum, TEqpRecCfgInfo* pcRecCfg) ;
	void SetVrsCfg(s32 nNum, TEqpVrsRecCfgInfo* pcVrsCfg);
    void SetHDBasCfg(s32 nNum, TEqpBasHDCfgInfo* pcHDBasCfg);
    void SetHDUStyleCfg(s32 nNum, THduStyleCfgInfo* pcHduStyleCfg);
	
    // ����MCU�ϱ����µ���״̬�����û���ҵ��򷵻�FALSE
    BOOL32 UpdateBrdStatus(TBoardStatusNotify* const ptStatus) ;
    
	void SetInited(BOOL32 bInit = TRUE)
	{
		m_bInit = bInit;
	}
    BOOL32 IsInited()
    {
        return m_bInit;
    }
    void Clear()
    {
        ZeroMemory(this, sizeof(TMcuExt));
		m_tMcsMcuGeneralCfg.Clear();
        m_tplBrdCfg.Clear();
        m_tplRecCfg.Clear();
        m_tplHDBasCfg.Clear();
        SetInited(FALSE);
    }
	
    // ��ӡ������Ϣ
	void PrintInfo();

	//�Ƿ����õ���ģʽ
	void SetEnableDebug(u8 byEnableDebug)
	{
		m_byEnableDebug = byEnableDebug;
	}
	u8 GetEnableDebug()
	{
		return m_byEnableDebug;
	}
	TMcsMcuGeneralCfg* GetMcsMcuGeneralCfg()
	{
		return &m_tMcsMcuGeneralCfg;
	}
    
public:
    BOOL32              m_bInit;           // FALSE ��ʾ��Mcu��δ��ȡ��Ϣ
 
	TMcsMcuGeneralCfg   m_tMcsMcuGeneralCfg;
    TplArray<THduStyleCfgInfo> m_tplHduStyleCfgInfo; //xts20090120HDUԤ��������Ϣ	
    TplArray<CBrdCfg>   m_tplBrdCfg;    
    TplArray<TEqpRecCfgInfo> m_tplRecCfg;   // ¼�������
	TplArray<TEqpVrsRecCfgInfo> m_tplVrsCfg;   // Vrs����
    TplArray<TEqpBasHDCfgInfo> m_tplHDBasCfg;   // ����BAS������ 
	TplArray<TCRIBrdExInfo>   m_tplCRIBrdExInfo; //CRI�������Ȩ��Ϣ

protected:
	u8 m_byEnableDebug;    //�Ƿ�ʹ�õ���ģʽ
};





///////////////////////////////////////////////////////////////////////////////
// CMtUtility

/*=============================================================================
�� �� ��:GetMcuId
��    ��:����u16�õ�McuID
��    ��:u16 wID                            [in]    u16
ע    ��:��
�� �� ֵ:McuID
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
=============================================================================*/
inline u16 CMtUtility::GetMcuId(u32 wID)
{
    return HIWORD16(wID);
}

/*=============================================================================
�� �� ��:GetMtId
��    ��:����u16�õ�MtID
��    ��:u16 wID                            [in]    u16
ע    ��:��
�� �� ֵ:MtID
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
=============================================================================*/
inline u8 CMtUtility::GetMtId(u32 wID)
{
    return (u8)LOWORD16(wID);
}

/*=============================================================================
�� �� ��:GetwID
��    ��:��TMtת��Ϊu16
��    ��:const TMt &tMt                     [in]    TMt
ע    ��:��
�� �� ֵ:�ն˱�ʶ��
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
=============================================================================*/
inline u32 CMtUtility::GetwID(const TMt &tMt)
{
    return MAKEDWORD((u16)tMt.GetMtId(), tMt.GetMcuIdx());
}

/*=============================================================================
�� �� ��:GetwID
��    ��:��McuID��MtID���ת��Ϊu16
��    ��:u8 byMcuId                         [in]    McuID
u8 byMtId                          [in]    MtID
ע    ��:��
�� �� ֵ:�ն˱�ʶ��
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
=============================================================================*/
inline u32 CMtUtility::GetwID(u16 byMcuId, u8 byMtId)
{
    return MAKEDWORD(byMtId, byMcuId);
}

/*=============================================================================
�� �� ��:IsMcu
��    ��:�ж��ն��Ƿ���MCU
��    ��:const TMt &tMt                     [in]    �ն�
ע    ��:��
�� �� ֵ:BOOL
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
=============================================================================*/
inline BOOL32 CMtUtility::IsMcu(const TMt &tMt)
{
    u8 byMtType = tMt.GetEqpType();
	//	tMt.GetType() == TYPE_MT
	//       &&
    if (  (byMtType == MT_TYPE_MMCU
		|| byMtType == MT_TYPE_SMCU
		|| byMtType == MT_TYPE_LOCALMCU) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
�� �� ��:IsSMcu
��    ��:�ж��ն��Ƿ����¼�MCU
��    ��:const TMt &tMt                     [in]    �ն�
ע    ��:��
�� �� ֵ:BOOL
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
=============================================================================*/
inline BOOL32 CMtUtility::IsSMcu(const TMt &tMt)
{
    if (tMt.GetType() == TYPE_MT && tMt.GetEqpType() == MT_TYPE_SMCU)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
�� �� ��:IsMMcu
��    ��:�ж��ն��Ƿ����ϼ�MCU
��    ��:const TMt &tMt                     [in]    �ն�
ע    ��:��
�� �� ֵ:BOOL
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
=============================================================================*/
inline BOOL32 CMtUtility::IsMMcu(const TMt &tMt)
{
    if (tMt.GetType() == TYPE_MT && tMt.GetEqpType() == MT_TYPE_MMCU)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
�� �� ��:IsLocalMcu
��    ��:�ж��ն��Ƿ��Ǳ���MCU
��    ��:const TMt &tMt                     [in]    �ն�
ע    ��:��
�� �� ֵ:BOOL
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/05/13  4.0     ���    ����
=============================================================================*/
inline BOOL32 CMtUtility::IsLocalMcu(const TMt &tMt)
{
    if ( tMt.GetEqpId() == 0 && tMt.GetMcuIdx() == LOCAL_MCUIDX )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*====================================================================
�� �� ���� IsSMt
��    �ܣ� �ж��Ƿ����¼����ն�
�㷨ʵ�֣� ��Ϊ�����ϼ��ͱ���֮�����е��ն˶����¼����ն�
P.S �㷨��Ч,���ǲ�׼ȷ,��������ȷ����ѯ���ն��ǻ����е��ն�
ȫ�ֱ����� 
��    ���� const TMt &tMt				[in]��Ҫ�жϵ�Mt
const TMt &tMMcu			[in]�ϼ�MCU
�� �� ֵ�� inline BOOL32 
--------------------------------------------------------------------
�޸ļ�¼��
��  ��	      �汾		    �޸���		�߶���    �޸�����
2006/5/29      4.0		    �����                   ����
======================================================================*/
// inline BOOL32 CMtUtility::IsSMt( const TMt &tMt, const TMt &tMMcu )
// {
// 	if ( tMt.IsLocal() )
// 	{
// 		return FALSE;
// 	}
// 	
// 	u16 byMcuId = tMt.GetMcuIdx();
// 	//�ж��Ƿ����ϼ�mcu�µ��ն� 
// 	if ( byMcuId == tMMcu.GetMcuIdx() )
// 	{
// 		return FALSE;
// 	}
// 	
// 	return TRUE;
// }

/*====================================================================
�� �� ���� IsMMt
��    �ܣ� �ж��Ƿ����ϼ����ն� 
�㷨ʵ�֣� ��Ч���ǲ�׼ȷ,�������û���ȷ���ն��ڻ�����
ȫ�ֱ����� 
��    ���� const TMt &tMt				[in]��Ҫ�жϵ�Mt
const TMt &tMMcu			[in]�ϼ�MCU,���û���ǿ�ֵ
�� �� ֵ�� inline BOOL32 
--------------------------------------------------------------------
�޸ļ�¼��
��  ��	      �汾		    �޸���		�߶���    �޸�����
2006/5/29      4.0		    �����                   ����
======================================================================*/
// inline BOOL32 CMtUtility::IsMMt( const TMt &tMt, const TMt &tMMcu )
// {
// 	if ( tMt.IsLocal() )
// 	{
// 		return FALSE;
// 	}
// 	
// 	u16 byMcuId = tMt.GetMcuIdx();
// 	if ( byMcuId != tMMcu.GetMcuIdx() )
// 	{
// 		return FALSE;
// 	}
// 	return TRUE;
// }


///////////////////////////////////////////////////////////////////////////////
// CBrdCfg

/*=============================================================================
�� �� ��:operator=
��    ��:��ֵ������
��    ��:��
ע    ��:��
�� �� ֵ:this
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/09/01  4.0     ���    ����
=============================================================================*/
inline CBrdCfg& CBrdCfg::operator=(const TBrdCfgInfo &tBrdCfg)
{
    if ( this == &tBrdCfg )
    {
        return (*this);
    }
	
    memcpy( dynamic_cast<TBrdCfgInfo*>(this), &tBrdCfg, sizeof (TBrdCfgInfo) );
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
// CMt

/*=============================================================================
�� �� ��:operator=
��    ��:ȫ����ֵ
��    ��:const CMt& cMt                     [in]    ȫ���ն���Ϣ
ע    ��:��
�� �� ֵ:this
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
=============================================================================*/
inline CMt& CMt::operator=(const CMt &cMt)
{
    if (this == &cMt)
    {
        return (*this);
    }
	
    memcpy(this, &cMt, sizeof (CMt));

    return (*this);
}

/*=============================================================================
�� �� ��:operator=
��    ��:�����ṹ��ֵ
��    ��:const TMt& tMt                     [in]    �����ն���Ϣ
ע    ��:��
�� �� ֵ:this
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
=============================================================================*/
inline CMt& CMt::operator=(const TMt &tMt)
{
    if (this == &tMt)
    {
        return (*this);
    }
	
    SetMt(tMt);
    return (*this);
}

/*=============================================================================
�� �� ��:operator=
��    ��:���ֽṹ��ֵ
��    ��:const TMtExt &tMtExt               [in]    �����ն���Ϣ
ע    ��:��
�� �� ֵ:this
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
2012/05/29  4.0     ��̫��  �޸�
=============================================================================*/
inline CMt& CMt::operator=(const TMtExtCur &tMtExt)
{
	m_tMtExt = tMtExt;

	if ( strcmp( tMtExt.GetAlias(),"(none)") == 0 )
	{
		
		m_tMtExt.SetAlias("");
	}

    SetMt(tMtExt);
    SyncAliasFromExt();
	
    return (*this);
}

/*=============================================================================
�� �� ��:operator=
��    ��:���ֽṹ��ֵ
��    ��:const TMtStatus &tMtStatus         [in]    �����ն���Ϣ
ע    ��:��
�� �� ֵ:this
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/06/14  4.0     ���    ����
=============================================================================*/
inline CMt& CMt::operator=(const TMtStatus &tMtStatus)
{
    SetMtStatus(tMtStatus);
    return (*this);
}

/*=============================================================================
�� �� ��:operator==
��    ��:�Ƚϲ�����
��    ��:const TMtStatus &tMtStatus         [in]    TMtStatus
ע    ��:��
�� �� ֵ:BOOL
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/27  4.0     ���    ����
=============================================================================*/
inline BOOL32 CMt::operator==(const TMtStatus &tMtStatus) const
{
    if ( GetMcuIdx() == tMtStatus.GetMcuIdx()
        && GetMtId() == tMtStatus.GetMtId() )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
�� �� ��:operator==
��    ��:�Ƚϲ�����
��    ��:const TMtExt &tMtExt               [in]    TMtExt
ע    ��:��
�� �� ֵ:BOOL
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/05/08  4.0     ���    ����
=============================================================================*/
inline BOOL32 CMt::operator==(const TMtExtCur &tMtExt) const
{
    if ( GetMcuIdx() == tMtExt.GetMcuIdx()
        && GetMtId() == tMtExt.GetMtId() )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
�� �� ��:operator==
��    ��:�Ƚϲ�����
��    ��:const TMt &tMt                     [in]    TMt
ע    ��:��
�� �� ֵ:BOOL
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/05/09  4.0     ���    ����
=============================================================================*/
inline BOOL32 CMt::operator==(const TMt &tMt) const
{
    if ( (*dynamic_cast<TMt*>(const_cast<CMt*>(this))) == tMt)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
�� �� ��:IsNull
��    ��:�ж��ն��Ƿ�Ϊ��
��    ��:��
ע    ��:��
�� �� ֵ:BOOL
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/05/09  4.0     ���    ����
=============================================================================*/
inline BOOL32 CMt::IsNull(void) const
{
    return dynamic_cast<TMt&>(const_cast<CMt&>(*this)).IsNull();
}

/*=============================================================================
�� �� ��:GetwID
��    ��:��ȡ16λ�ն˱�ʶ��
��    ��:��
ע    ��:��
�� �� ֵ:�ն˱�ʶ��
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
=============================================================================*/
inline u32 CMt::GetwID(void) const
{
    return CMtUtility::GetwID(GetMcuIdx(), GetMtId());
}

/*=============================================================================
�� �� ��:SetMt
��    ��:����TMt
��    ��:const TMt &tMt                     [in]    TMt
ע    ��:��
�� �� ֵ:��
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/26  4.0     ���    ����
2013/07/03	4.0
=============================================================================*/
inline void CMt::SetMt(const TMt &tMt)
{
	*static_cast<TMt*>( this ) = tMt;
	*static_cast<TMt*>( &m_tMtExt ) = tMt;
	*static_cast<TMt*>( &m_tMtStatus ) = tMt;
	*static_cast<TMt*>( &m_tMtVersion ) = tMt;
}

/*=============================================================================
�� �� ��:SetMt
��    ��:����TMt
��    ��:const TMt &tMt                     [in]    TMt
ע    ��:��
�� �� ֵ:��
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/26  4.0     ���    ����
=============================================================================*/

/*=============================================================================
�� �� ��:SetMtStatus
��    ��:�����ն�״̬
��    ��:const TMtStatus &tMtStatus         [in]    �ն�״̬
ע    ��:��
�� �� ֵ:����m_tMtStatus��tMtStatus��һ��:TRUE; ����:FALSE
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/04/25  4.0     ���    ����
=============================================================================*/
inline BOOL32 CMt::SetMtStatus(const TMtStatus &tMtStatus)
{
    BOOL32 bRet = (memcmp(&m_tMtStatus, &tMtStatus, sizeof (TMtStatus)) != 0);
    m_tMtStatus = tMtStatus;
    if (tMtStatus.m_tRecState.IsNoRecording())
    {
        ZeroMemory(&m_tRecEqp, sizeof (m_tRecEqp));
        ZeroMemory(&m_tRecProg, sizeof (m_tRecProg));
    }
    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// CSingleMcuInfo

///////////////////////////////////////////////////////////////////////////////
// CConf

/*=============================================================================
�� �� ��:SetNull
��    ��:���
��    ��:��
ע    ��:��
�� �� ֵ:��
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/06/03  4.0     ���    ����
=============================================================================*/
inline void CConf::SetNull(void)
{

	TConfInfo::Reset();
	//    ZeroMemory(&m_tTwModule,                    sizeof (m_tTwModule));
	//    ZeroMemory(&m_tVmpModule,                   sizeof (m_tVmpModule));
    ZeroMemory(m_abyLockUserName,               sizeof (m_abyLockUserName));
    ZeroMemory(&m_tRecEqp,                      sizeof (m_tRecEqp));
    ZeroMemory(&m_tPlayEqp,                     sizeof (m_tPlayEqp));
	ZeroMemory(m_atMcu, sizeof(m_atMcu) );
	
    m_dwGetInitValue    = 0;
    m_byLockBySelf      = 0;
    m_dwLockMcsIP       = 0;
    m_bPwdEntered       = FALSE;
	m_bReqConfSchema	= FALSE; //modified by twb 2012.11.20
	m_bHaveSchema		= TRUE;
    m_dwSN              = 0;
    m_dwLastRateReqTime = 0;
	
    m_cVcsConfStatus.RestoreStatus();
	if ( m_piMcu != NULL )
	{
		m_piMcu->DeleteAllItem();
	}
	else
	{
		m_piMcu = new CItemGroup<CMt, u32>;
	}
	
	
	/*	if ( m_piMcuIndex != NULL )
	{
	m_piMcuIndex->DeleteAllItem();
	}
	else
	{
	m_piMcuIndex = new CItemGroup<CMt,u32>();
	}
	*/
    TMtExtCur tMtExt;

    tMtExt.SetMcuIdx( LOCAL_MCUIDX );
	tMtExt.SetEqpId( 0 );
    tMtExt.SetMtType( MT_TYPE_LOCALMCU );
    m_cLocalMcu = tMtExt;
	m_cLocalMcu.SetOnline(TRUE);
	
	m_dwMcuNbr = 0;
	m_vctApplySpeaker.clear();	
}

/*=============================================================================
�� �� ��:GetMixParam
��    ��:��ȡ��������
��    ��:u8 byMcuId                         [in]    McuId
ע    ��:��
�� �� ֵ:��������
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/06/13  4.0     ���    ����
=============================================================================*/
inline TMixParam& CConf::GetMixParam(u16 byMcuId) const
{
    if (byMcuId == LOCAL_MCUIDX)
    {
        return const_cast<TMixParam&>(m_tStatus.m_tMixParam);
    }
    else
    {
		IMcu* pMcu = GetIMcu( byMcuId );
		if ( pMcu == NULL )
		{
			return g_tNull.cNullMixParam;
		}
		CMt* pMt = pMcu->GetItemData();	
		if ( pMt == NULL )
		{
			return g_tNull.cNullMixParam;
		}
		
        return pMt->GetMixParam();
    }
}

/*=============================================================================
�� �� ��:IsConfEntireMix
��    ��:ָ��MCU�Ƿ�ȫ�����
��    ��:u8 byMcuId                         [in]    McuId
ע    ��:��
�� �� ֵ:BOOL
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/06/13  4.0     ���    ����
=============================================================================*/
inline BOOL32 CConf::IsConfEntireMix(u16 byMcuId) const
{
    if (byMcuId == LOCAL_MCUIDX)
    {
        return m_tStatus.IsAutoMixing();
		//        return (m_tStatus.m_tConfMode.IsDiscussMode()
		//                && !m_tStatus.m_tConfMode.IsMixSpecMt());
    }
    else
    {
		TMixParam tMixParam = GetMixParam( byMcuId );
		return    tMixParam.GetMode() == mcuWholeMix
			|| tMixParam.GetMode() == mcuVacWholeMix;
		
		//        return (m_cMcuTable.GetMixParam(byMcuId).m_byMixMode == mcuWholeMix);
    }
}

/*=============================================================================
�� �� ��:IsConfPartMix
��    ��:ָ��MCU�Ƿ񲿷ֻ���
��    ��:u8 byMcuId                         [in]    McuId
ע    ��:��
�� �� ֵ:BOOL
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/06/13  4.0     ���    ����
=============================================================================*/
inline BOOL32 CConf::IsConfPartMix(u16 byMcuId) const
{
    if (byMcuId == LOCAL_MCUIDX)
    {
        return m_tStatus.IsSpecMixing();
		//        return (m_tStatus.m_tConfMode.IsDiscussMode()
		//                && m_tStatus.m_tConfMode.IsMixSpecMt());
    }
    else
    {
		//        return (m_cMcuTable.GetMixParam(byMcuId).m_byMixMode == mcuPartMix);
		TMixParam tMixParam = GetMixParam( byMcuId );
		return    tMixParam.GetMode() == mcuPartMix
			|| tMixParam.GetMode() == mcuVacPartMix;
    }
}

/*=============================================================================
�� �� ��:IsConfNoMix
��    ��:ָ��MCU�Ƿ�û�л���
��    ��:u8 byMcuId                         [in]    McuId
ע    ��:��
�� �� ֵ:BOOL
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2006/08/04  4.0     ���    ����
=============================================================================*/
inline BOOL32 CConf::IsConfNoMix(u16 byMcuId) const
{
    if (byMcuId == LOCAL_MCUIDX)
    {
        return m_tStatus.IsNoMixing();
        //return ( ! m_tStatus.m_tConfMode.IsDiscussMode() );
    }
    else
    {
		TMixParam tMixParam = GetMixParam( byMcuId );
		return    tMixParam.GetMode() == mcuNoMix
			|| tMixParam.GetMode() == mcuVacMix;
    }
}


/*=============================================================================
�� �� ��:SetNull
��    ��:����ģ���ÿ�
��    ��:��
ע    ��:��
�� �� ֵ:��
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2005/11/22  4.0     ���    ����
=============================================================================*/
inline void CTemSchConf::SetNull(void)
{
	TConfInfo::Reset();
    m_bExist = FALSE;
	m_bIsHasVmpSubChnnl = FALSE;
	m_tVmpModuleEx.EmptyMember();
	m_tTWModules.Clear();
    m_tplMt.Clear();
	
    m_tplSubMcu.Clear();
	m_tMtAliasBack.SetNull();
	m_vctGroup.clear();

    m_tHdTvwall.SetNull();
    m_vctHduModChn.clear();
	m_tConfInfoEx.Clear();
	m_tHduVmpTotalEx.Empty();
    m_wPollTime = 0;
}

#define INVALID_INDEX 0xFFFFFFFF

class CMcuAddrBook
{
public:
    // ��ʼ���ɲ���ֵ
    CMcuAddrBook() :m_tplEntry(128, 32),
		m_tplGroup(16, 4)
	{		
	}
    ~CMcuAddrBook()
    {
        Clear();
    }
	
    /*=============================================================================
    �� �� ��:Clear
    ��    ��:��յ�ַ��
    ��    ��:��
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/
	void Clear()
	{
		m_tplEntry.Clear();
		m_tplGroup.Clear();
	}
	
    /*=============================================================================
    �� �� ��:AddEntry
    ��    ��:���ַ��������Ŀ
    ��    ��:TMcuAddrEntry& tEntry          [in]    Ҫ���ӵ���Ŀ
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/
	void AddEntry(TMcuAddrEntryCur& tEntry);
	
    /*=============================================================================
    �� �� ��:DelEntry
    ��    ��:�ӵ�ַ��ɾ����Ŀ
    ��    ��:u32 dwIndex                    [in]    ��ɾ����Ŀ��EntryIdx
    ע    ��:��
    �� �� ֵ:TRUEΪ�ɹ�ɾ����FALSEû���ҵ�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/
	BOOL32 DelEntry(u32 dwIndex);
    
    /*=============================================================================
    �� �� ��:ModifyEntry
    ��    ��:�ӵ�ַ���޸���Ŀ
    ��    ��:TMcuAddrEntry& tEntry          [in]    ���޸ĵ���Ŀ��������EntryIdx����
    ע    ��:��
    �� �� ֵ:TRUEΪ�ɹ��޸ģ�FALSEû���ҵ�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/
	BOOL32 ModifyEntry(TMcuAddrEntryCur& tEntry);
	
    /*=============================================================================
    �� �� ��:AddGroup
    ��    ��:���ַ��������
    ��    ��:TMcuAddrGroup& tGroup          [in]    Ҫ���ӵ���
    ע    ��:��
    �� �� ֵ:��
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/
	void AddGroup(TMcuAddrGroupCur& tGroup);
	
    /*=============================================================================
    �� �� ��:DelGroup
    ��    ��:�ӵ�ַ��ɾ����
    ��    ��:u32 dwIndex                    [in]    ��ɾ�����EntryIdx
    ע    ��:��
    �� �� ֵ:TRUEΪ�ɹ�ɾ����FALSEû���ҵ�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/    
	BOOL32 DelGroup(u32 dwIndex);
	
    /*=============================================================================
    �� �� ��:ModifyGroup
    ��    ��:�ӵ�ַ���޸���
    ��    ��:TMcuAddrGroup& tGroup          [in]    ���޸ĵ��飬������EntryIdx����
    ע    ��:��
    �� �� ֵ:TRUEΪ�ɹ��޸ģ�FALSEû���ҵ�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/    
	BOOL32 ModifyGroup(TMcuAddrGroupCur& tGroup);
	
    /*=============================================================================
    �� �� ��:AddEntryToGroup
    ��    ��:���ַ����������Ŀ
    ��    ��:TMcuAddrGroup& tGroup          [in]    ���ӵ���Ŀ��Ϣ��������EntryIdx����
    ע    ��:��
    �� �� ֵ:TRUEΪ�ɹ����ӣ�FALSEû���ҵ�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/   
	BOOL32 AddEntryToGroup(TMcuAddrGroupCur& tGroup);
	
    /*=============================================================================
    �� �� ��:DelEntryFromGroup
    ��    ��:�ӵ�ַ����ɾ����Ŀ
    ��    ��:TMcuAddrGroup& tGroup          [in]    ɾ������Ŀ��Ϣ��������EntryIdx����
    ע    ��:��
    �� �� ֵ:TRUEΪ�ɹ����ӣ�FALSEû���ҵ�
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/   
	BOOL32 DelEntryFromGroup(TMcuAddrGroupCur& tGroup);
	
    /*=============================================================================
    �� �� ��:GetEntryCount
    ��    ��:��ȡ��ַ����Ŀ��
    ��    ��:
    ע    ��:��
    �� �� ֵ:��Ŀ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/  
	u32  GetEntryCount()
    {
        return m_tplEntry.Size();
    }
	
    /*=============================================================================
    �� �� ��:GetGroupCount
    ��    ��:��ȡ��ַ������
    ��    ��:
    ע    ��:��
    �� �� ֵ:��Ŀ����
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/  
	u32  GetGroupCount()
    {
        return m_tplGroup.Size();
    }
	
    /*=============================================================================
    �� �� ��:IsEntryExist
    ��    ��:�жϵ�ַ����Ŀ�Ƿ���ڣ�����EntryIdx����
    ��    ��:TMcuAddrEntry& tEntry        [in]    Ŀ����Ŀ
    ע    ��:��
    �� �� ֵ:TRUE���ڣ�FALSE������
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/ 
	BOOL32 IsEntryExist(TMcuAddrEntryCur& tEntry) ;
	
    /*=============================================================================
    �� �� ��:IsGroupExist
    ��    ��:�жϵ�ַ�����Ƿ���ڣ�����EntryIdx����
    ��    ��:TMcuAddrGroup& tGroup        [in]    Ŀ����  
    ע    ��:��
    �� �� ֵ:TRUE���ڣ�FALSE������
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/ 
	BOOL32 IsGroupExist(TMcuAddrGroupCur& tGroup) ;
	
    /*=============================================================================
    �� �� ��:IsEntryExistInGroup
    ��    ��:�жϵ�ַ����Ŀ�Ƿ���������У�����EntryIdx����
    ��    ��:
    ע    ��:��
    �� �� ֵ:TRUE���ڣ�FALSE������
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/ 
	BOOL32 IsEntryExistInGroup(TMcuAddrEntryCur& tEntry, TMcuAddrGroupCur& tGroup);
	
    /*=============================================================================
    �� �� ��:GetEntryByIndex
    ��    ��:������Ŀ��������
    ��    ��:u32 dwIndex                    [in]    ��Ŀ����
    ע    ��:��
    �� �� ֵ:TMcuAddrEntry����������ڣ���EntryIdxΪ INVALID_INDEX
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/ 
    TMcuAddrEntryCur GetEntryByIndex(u32 dwIndex);
	
    /*=============================================================================
    �� �� ��:GetGroupByIndex
    ��    ��:������Ŀ��������
    ��    ��:u32 dwIndex                    [in]    ������
    ע    ��:��
    �� �� ֵ:TMcuAddrGroup����������ڣ���EntryIdxΪ INVALID_INDEX
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/ 
    TMcuAddrGroupCur GetGroupByIndex(u32 dwIndex);
	
    /*=============================================================================
    �� �� ��:GetEntryByName
    ��    ��:������Ŀ���Ʋ���
    ��    ��:LPCTSTR lpszName               [in]    ��Ŀ����
	TMcuAddrEntry* ptEntry         [out]   ���ص���Ŀ
    ע    ��:��
    �� �� ֵ:u32 ��Ŀ���ڵ�������ַ�����û���ҵ����� INVALID_INDEX
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/ 
    u32 GetEntryByName(const s8* lpszName, TMcuAddrEntryCur* ptEntry);
	
    /*=============================================================================
    �� �� ��:GetFirstEntry
    ��    ��:���ҵ�ַ�������ϵ�һ����Ŀ
    ��    ��:TMcuAddrEntry* ptEntry         [out] �����Ŀ
    ע    ��:��
    �� �� ֵ:������ҳɹ�����0(�����ϵ�һ��)��ptEntry �����
	���򷵻�INVALID_INDEX�����EntryIdxΪ INVALID_INDEX             
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/ 
    u32 GetFirstEntry(TMcuAddrEntryCur* ptEntry);
	
    /*=============================================================================
    �� �� ��:GetNextEntry
    ��    ��:�����û�Ҫ����������ϵ�dwCurIndex+1��ʼ��dwCount����Ŀ
    ��    ��:u32 dwCurIndex                 [in]  ��ʼ��Ŀ������λ�ã���������λ�õ���Ŀ
	TMcuAddrEntry* ptEntryTable    [out] �����Ŀ�������������
	u32& dwCount                   [in/out] �������Ŀ������ͬʱ���ؽ����ʵ����������
    ע    ��:��
    �� �� ֵ:u32 ��������1�������ϣ��򷵻����һ����Ŀ������λ�ã�
	ptEntryTable ����䣬dwCount ����ʵ���������
	���򷵻�INVALID_INDEX��dwCount ���� 0          
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/ 
    u32 GetNextEntry(u32 dwCurIndex, TMcuAddrEntryCur* ptEntryTable, u32& dwCount);
	
    /*=============================================================================
    �� �� ��:GetFirstGroup
    ��    ��:���ҵ�ַ�������ϵ�һ����
    ��    ��:TMcuAddrGroup* ptGroup         [out] �����
    ע    ��:��
    �� �� ֵ:������ҳɹ�����0(�����ϵ�һ��)��ptGroup �����
	���򷵻�INVALID_INDEX�����EntryIdxΪ INVALID_INDEX             
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/ 
    u32 GetFirstGroup(TMcuAddrGroupCur* ptGroup);
	
    /*=============================================================================
    �� �� ��:GetNextGroup
    ��    ��:�����û�Ҫ����������ϵ�dwCurIndex+1��ʼ��dwCount����
    ��    ��:u32 dwCurIndex                 [in]  ��ʼ�������λ�ã���������λ�õ���
	TMcuAddrEntry* ptEntryTable    [out] �����������������
	u32& dwCount                   [in/out] �������������ͬʱ���ؽ����ʵ����������
    ע    ��:��
    �� �� ֵ:u32 ��������1�������ϣ��򷵻����һ���������λ�ã�
	ptEntryTable ����䣬dwCount ����ʵ���������
	���򷵻�INVALID_INDEX��dwCount ���� 0          
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/02/23  4.0     ����  ����
    =============================================================================*/ 
    u32 GetNextGroup(u32 dwCurIndex, TMcuAddrGroupCur* ptGroupTable, u32& dwCount);
	
    // ���Դ�ӡ��Ϣ
    void PrintInfo();
	
public:
	TplArray<TMcuAddrEntryCur> m_tplEntry;
	TplArray<TMcuAddrGroupCur> m_tplGroup;
};

// ��������Ϣ
typedef struct tagCallGroupInfo
{
    tagCallGroupInfo()
    {
        ZeroMemory( m_achGroupName, sizeof( m_achGroupName ) );
    }
	
    s8     m_achGroupName[MAX_VCSGROUPNAMELEN]; // �������;
    vector<TMt> m_vctMts;       //���ں��е��ն�  
	
}TCallGroupInfo, *PTCallGroupInfo;

//�����ն���Ϣ
typedef struct tagHideMtInfo 
{
public:
    tagHideMtInfo( u32 dwIP = 0, LPCTSTR strName = _T("") )
    {
        m_dwMtIP = dwIP;
        SetMtName( strName );
    }
    tagHideMtInfo( const tagHideMtInfo& tMtInfo )
    {
		//         m_dwMtIP = tMtInfo.m_dwMtIP;
		//         memcpy( m_strMtName, tMtInfo.m_strMtName, MAX_VCSGROUPNAMELEN );
        if ( this == &tMtInfo )
        {
            return ;
        } 
        *this = tMtInfo;
    }
    ~tagHideMtInfo(){}
	
    void SetIP( u32 dwIP ){ m_dwMtIP = dwIP; }
    u32 GetIP(){ return  m_dwMtIP; }
	
    void SetMtName( LPCTSTR pstrName ) { _tcscpy( m_strMtName, pstrName ); }
    LPCTSTR GetMtName(){ return m_strMtName; }
    
private:
    u32 m_dwMtIP;     //������
    TCHAR m_strMtName[VCS_MAXLEN_ALIAS];
	
}THideMtInfo, *PTHideMtInfo;

typedef struct tagChnInfo 
{
	u8		m_byDevId;             //ͨ�������豸ID
	u8		m_byChnId;
	
	tagChnInfo( u8 byDevId=0, u8 byChnlId=0 ){ m_byDevId = byDevId; m_byChnId = byChnlId; }
	void ResetChnl(){ ZeroMemory(this, sizeof(tagChnInfo)); }
	
	void SetChnl( u8 byDevId, u8 byChnlId ){ m_byDevId = byDevId; m_byChnId = byChnlId; }
	
}TChnlInfo, *PTChnlInfo;

typedef struct tagVcsTwChnItem 
{
	TChnlInfo		m_tChnl;			//����ǽ����
	u8				m_bySeq;			//ͨ�����к�
	BOOL32			m_bActive;			//ͨ���Ƿ񼤻�
	tagVcsTwChnItem(){ ZeroMemory(this, sizeof(tagVcsTwChnItem) ); }
}TVcsTwChnItem;

typedef struct tagDataPtr 
{
	tagDataPtr( u8 *pData = NULL, u32 nLen = 0){ m_pData = pData; m_nLen = nLen; }
	void SetNull(){ m_pData = NULL; m_nLen = 0; }
	void Free(){ DEL_PTR(m_pData); m_nLen = 0; }
	
	u8 *m_pData;
	u32 m_nLen;
}TDataPtr;

//---------------------------------------------------------------------
// �����������
typedef struct tagMonitorUnionInfo
{
	CMt tMt;
}TMonitorUnionInfo;


#define MAX_MSG_BODY_LEN	100

/*---------------------------------------------------------------------
* ��	����CMonitorUnionMsgHeader
* ��    �ܣ���VCS��������Ϣͷ
* ����˵����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/08/11	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/
class CMonitorUnionMsgHeader
{
public:
	CMonitorUnionMsgHeader() : m_wEvent(0), m_wBodyLen(0){}
	u8 m_wEvent;
	u32 m_wBodyLen;
	u8 m_wBody[MAX_MSG_BODY_LEN];
public:
	void SetEvent( u8 wEvent ){ m_wEvent = wEvent; }
	u8 GetEvent(){ return m_wEvent; }
	BOOL32 SetBody( void* pBody, u32 dwBodyLen )
	{
		if ( dwBodyLen > MAX_MSG_BODY_LEN )
		{
			return FALSE;
		}
		if ( pBody == NULL || dwBodyLen == 0  )
		{
			return TRUE;
		}
		memcpy( m_wBody, pBody, dwBodyLen );
		m_wBodyLen = dwBodyLen;
		return TRUE;
	}
};

/*---------------------------------------------------------------------
* ��	����tagMtReqJionConfInfo
* ��    �ܣ��ն���������vcs�������������Ϣ
* ����˵����
* �޸ļ�¼��
* ����			�汾		�޸���		�޸ļ�¼
* 2010/11/12	v1.0		Ĳ��ï		����
----------------------------------------------------------------------*/


//�ն������������  ��Ϣ�壺byEncrypt(1-����,0-������)+
//bySrcDriId(u8)+TMtAlias(tFstAlias)+TMtAlias(tMtH323Alias)+
//TMtAlias(tMtE164Alias)+TMtAlias(tMtAddr)+U16(wCallRate)+u8(bySrcSsnId)
//#pragma pack(1) 
typedef struct tagMtReqJionConfInfo
{
	//���ڻ�ȡ���ֵ�˳��  tFstAlias-��tMtE164Alias-�� tMtH323Alias-�� tMtAddr
	u8			byEncrypt;				// �Ƿ����
	u8			bySrcDriId;
	TMtAlias	tFstAlias;
	TMtAlias	tMtH323Alias;
	TMtAlias	tMtE164Alias;
	TMtAlias	tMtAddr;
	u16			wCallRate;
	u8			bySrcSsnId;
}TMtReqJionConfInfo; 




#endif      //  _MCSSTRUCT_20050422_H_