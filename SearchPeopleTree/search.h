#ifndef _SEARTCH_H_
#define _SEARTCH_H_

#include "type.h"

#define MIN(x, y)	x <= y ? x : y

struct TPeoPleMiaoShu
{
	s8 m_sMiaoshu[NAME_LEN];

public:
	TPeoPleMiaoShu(){Init();}
	void Init(){memset(this, 0, sizeof(TPeoPleMiaoShu));}
	void SetMiaoshu(const s8 *sMiaoshu, u8 byLen){if(sMiaoshu != NULL){memcpy(m_sMiaoshu, sMiaoshu, MIN(byLen, NAME_LEN));}}
	s8 *GetMiaoshu(){return m_sMiaoshu;}
};

struct TPeopleTree
{
	s8 m_sName[NAME_LEN];					//用户名；
	s8 m_sSheng[PLACE_LEN];					//省
	s8 m_sShi[PLACE_LEN];					//市
	s8 m_sXian[PLACE_LEN];					//县
	s8 m_sJie[PLACE_LEN];					//街道
	s8 m_sSex[NAME_LEN];					//性别（男/女）
	s8 m_sAge[NAME_LEN];					//年龄
	s8 m_sAliens[NAME_LEN];					//昵称
	s8 m_sPhone[NUM_LEN];					//电话
	s8 m_sQQ[NUM_LEN];						//QQ
	s8 m_sCompany[PLACE_LEN];				//工作单位
	s8 m_sZhiwu[PLACE_LEN];					//职务
	s8 m_sPeoPleMiaoShu[NAME_LEN];			//个人描述
	
public:
	TPeopleTree(){Init();}
	~TPeopleTree(){Init();}
	void Init(){memset(this, '\0', sizeof(TPeopleTree));}
	void SetName(const s8 *sName, u8 byNameLen){if(sName != NULL){memcpy(m_sName, sName, MIN(byNameLen, NAME_LEN));}}
	s8 *GetName(){return m_sName;}
	void SetSheng(const s8 *sSheng, u8 byShengLen){if(sSheng != NULL){memcpy(m_sSheng, sSheng, MIN(byShengLen, PLACE_LEN));}}
	s8 *GetSheng(){return m_sSheng;}
	void SetShi(const s8 *sShi, u8 byShiLen){if(sShi != NULL){memcpy(m_sShi, sShi, MIN(byShiLen, PLACE_LEN));}}
	s8 *GetShi(){return m_sShi;}
	void SetXian(const s8 *sXian, u8 byXianLen){if(sXian != NULL){memcpy(m_sXian, sXian, MIN(byXianLen, PLACE_LEN));}}
	s8 *GetXian(){return m_sXian;}
	void SetJie(const s8 *sJie, u8 byJieLen){if(sJie != NULL){memcpy(m_sJie, sJie, MIN(byJieLen, PLACE_LEN));}}
	s8 *GetJie(){return m_sJie;}
	void SetSex(const s8 *sSex, u8 bySexLen){if(sSex != NULL){memcpy(m_sSex, sSex, MIN(bySexLen, NAME_LEN));}}
	s8 *GetSex(){return m_sSex;}
	void SetAge(const s8 *sAge, u8 byAgeLen){if(sAge != NULL){memcpy(m_sAge, sAge, MIN(byAgeLen, NAME_LEN));}}
	s8 *GetAge(){return m_sAge;}
	void SetAliens(const s8 *sAliens, u8 byAliensLen){if(sAliens != NULL){memcpy(m_sAliens, sAliens, MIN(byAliensLen, NAME_LEN));}}
	s8 *GetAliens(){return m_sAliens;}
	void SetPhone(const s8 *sPhone, u8 byPhoneLen){if(sPhone != NULL){memcpy(m_sPhone, sPhone, MIN(byPhoneLen, NUM_LEN));}}
	s8 *GetPhone(){return m_sPhone;}
	void SetQQ(const s8 *sQQ, u8 byQQLen){if(sQQ != NULL){memcpy(m_sQQ, sQQ, MIN(byQQLen, NUM_LEN));}}
	s8 *GetQQ(){return m_sQQ;}
	void SetCompany(const s8 *sCompany, u8 byCompanyLen){if(sCompany != NULL){memcpy(m_sCompany, sCompany, MIN(byCompanyLen, PLACE_LEN));}}
	s8 *GetCompany(){return m_sCompany;}
	void SetZhiwu(const s8 *sZhiwu, u8 byZhiwuLen){if(sZhiwu != NULL){memcpy(m_sZhiwu, sZhiwu, MIN(byZhiwuLen, PLACE_LEN));}}
	s8 *GetZhiwu(){return m_sZhiwu;}
	void SetPeoPleMiaoShu(const s8 *sPeoPleMiaoShu, u8 byPeoPleMiaoShuLen){if(sPeoPleMiaoShu != NULL){memcpy(m_sPeoPleMiaoShu, sPeoPleMiaoShu, MIN(byPeoPleMiaoShuLen, NAME_LEN));}}
	s8 *GetPeoPleMiaoShu(){return m_sPeoPleMiaoShu;}
	
	void Print()
	{
		printf("姓名：%s\n性别：%s\n年龄：%s\n电话：%s\nQQ：%s\n工作单位：%s\n职务：%s\n住址：%s%s%s%s\n标签：\n",
			m_sName, m_sSex, m_sAge, m_sPhone, m_sQQ, m_sCompany, m_sZhiwu, m_sSheng, m_sShi, m_sXian, m_sJie, m_sPeoPleMiaoShu);
	}
};

#endif //_SEARTCH_H_