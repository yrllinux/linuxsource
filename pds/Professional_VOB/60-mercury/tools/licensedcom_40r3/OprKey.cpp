// OprKey.cpp : Implementation of COprKey
#include "stdafx.h"
#include "Mculicense.h"
#include "OprKey.h"
#include "osp.h"
#include "mcuconst.h"
#include "kdvencrypt.h"


/////////////////////////////////////////////////////////////////////////////
// COprKey

#define SUCCESS_KDVLICENSE              (u32)0 // 成功
#define ERR_KDVLICENSE_AUTH_FAILED      (u32)1 // 授权码验证失败
#define ERR_KDVLICENSE_GETPARA_FAILED   (u32)2 // 详细授权信息提取失败
#define ERR_KDVLICENSE_INVALID_DEVICEID (u32)3 // 设备ID非法
#define ERR_KDVLICENSE_INVALID_NUM      (u32)4 // 接入个数非法
#define ERR_KDVLICENSE_INVALID_DATE     (u32)5 // 截至日期非法
#define ERR_KDVLICENSE_INVALID_PRODUCT  (u32)6 // 产品类型非法
#define ERR_KDVLICENSE_WRITEKEY_FAILED  (u32)7 // 写licensekey文件失败
#define ERR_KDVLICENSE_UNKNOWN          (u32)100 // 不明原因的错误

//缺省的授权码 （klms.kedacom.com进行md5运算后结果32位结果）
#define DEFAULT_KDVLICENSE_AUTHCODE      (LPCSTR)"b2ae4e5afc3619dae447fd23dc0605b2"

//文件生成路径
#define DEFAULT_KEY_PATHNAME             (LPCSTR)"c:\\KedaLicenseKey\\"

//当前版本号
#define CURRENT_VERSION                 (u16)44

struct TLicenseParam 
{
    TLicenseParam(void) { memset(this, 0, sizeof(TLicenseParam)); }

	u16 wAuthNum;           //授权个数
	s8  szAuthName[256];    //许可证编码,作为用户名
    s8  szDeviceID[32];     //设备号, 即mac addr
    TKdvTime tExpireDate;   //失效日期
    u8  byProductID;        //产品类型, T120 / MCU
    u16 wVersion;           //版本号
};

/*=============================================================================
函 数 名： GenEncodeData
功    能： 
算法实现： 
全局变量： 
参    数： u8 *pOutBuf
           u16 wInBufLen
           u8 *pInBuf
           BOOL32 bDirect
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/10/19  4.0			许世林                  创建
=============================================================================*/
void GenEncodeData(u8 *pOutBuf, u16 wInBufLen, u8 *pInBuf, BOOL32 bDirect)
{
    s8 achIV[MAX_IV_SIZE] = {0};
	
    s8 achKey[32] = {0};
	strncpy( achKey, KEDA_AES_KEY, sizeof(KEDA_AES_KEY) );// 保持key为16的倍数
    u16 wKeyLen = strlen(achKey);

    s32 nRet;
	if( bDirect ) // eccrypt 
	{
		nRet = KdvAES( achKey, wKeyLen, MODE_CBC, DIR_ENCRYPT, achIV,
					   pInBuf, wInBufLen, pOutBuf );
	}
	else  // decrypt
	{
		nRet = KdvAES( achKey, wKeyLen, MODE_CBC, DIR_DECRYPT, achIV,
					   pInBuf, wInBufLen, pOutBuf );
	}

	return;
}

/*=============================================================================
函 数 名： GenLicense
功    能： 
算法实现： 
全局变量： 
参    数： CString &csLicenseKey
           CString &csLicenseContent
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/10/19  4.0			许世林                  创建
=============================================================================*/
u32 GenLicense(CString &csLicenseKey, CString &csLicenseContent)
{   
    TLicenseParam tLicenseParam;

    //解析licensecontent
    if (csLicenseKey.IsEmpty() || csLicenseContent.IsEmpty())
    {
        return ERR_KDVLICENSE_AUTH_FAILED;
    }

	s8  achSeps[] = "$"; //$ as seperators
	s8 *pchToken  = NULL;
    s8  achbuf[2048] = {0};
	strcpy(achbuf, csLicenseContent);    
		
	// 1. 设备id
	pchToken = strtok(achbuf, achSeps);
    if (NULL == pchToken)
    {
        return ERR_KDVLICENSE_GETPARA_FAILED;
    }
    else
    {
        strcpy(tLicenseParam.szDeviceID, pchToken);
    }
	
    // 2. 接入个数
    pchToken = strtok(NULL, achSeps);
    if (NULL == pchToken)
    {
        return ERR_KDVLICENSE_GETPARA_FAILED;
    }
    else
    {
        s32 nNum = atoi(pchToken);
        if (nNum <= 0 || nNum > MAXNUM_MCU_MT)
        {
            return ERR_KDVLICENSE_INVALID_NUM;
        }        
        tLicenseParam.wAuthNum = (u16)nNum;
    }

    // 3. 截止日期
    pchToken = strtok(NULL, achSeps);
    if (NULL == pchToken)
    {
        return ERR_KDVLICENSE_GETPARA_FAILED;
    }
    else
    {
        s8 achTmp[32];
        strcpy(achTmp, pchToken);
        s8 achSeps[] = "-";
        pchToken = strtok(achTmp, achSeps);
        if (NULL == pchToken)
        {
            return ERR_KDVLICENSE_INVALID_DATE;
        }
        u16 wYear = atoi(pchToken);
        if (wYear >= 2038)
        {
            return ERR_KDVLICENSE_INVALID_DATE;
        }
        pchToken = strtok(NULL, achSeps);
        if (NULL == pchToken)
        {
            return ERR_KDVLICENSE_INVALID_DATE;
        }
        u8 byMonth = atoi(pchToken);
        if (byMonth > 12)
        {
            return ERR_KDVLICENSE_INVALID_DATE;
        }
        pchToken = strtok(NULL, achSeps);
        if (NULL == pchToken)
        {
            return ERR_KDVLICENSE_INVALID_DATE;
        }
        u8 byDay = atoi(pchToken);
        if (byDay > 31)
        {
            return ERR_KDVLICENSE_INVALID_DATE;
        }
        
        tLicenseParam.tExpireDate.SetYear(wYear);
        tLicenseParam.tExpireDate.SetMonth(byMonth);
        tLicenseParam.tExpireDate.SetDay(byDay);      
    }

    //许可证编码   
    strcpy(tLicenseParam.szAuthName, csLicenseKey.SpanExcluding("|"));

    //generator key file
    u8 achEncText[LEN_KEYFILE_CONT] = {0};

	// 加密原始数据中用'|'分隔   
    s8 achSequenceStr[1024] = {0};
	sprintf(achSequenceStr, "%s|%s|%d|%d|%d|%d", tLicenseParam.szAuthName, 
        tLicenseParam.szDeviceID, tLicenseParam.wAuthNum, 
        tLicenseParam.tExpireDate.GetYear(), tLicenseParam.tExpireDate.GetMonth(), 
        tLicenseParam.tExpireDate.GetDay());    
	u16 wSequenceStrLen = strlen(achSequenceStr);	
	while( 0 != wSequenceStrLen % 16 )  // 补足16的倍数
	{
		strcat(achSequenceStr, "|");
		wSequenceStrLen += 1;
	}

    CreateDirectory(DEFAULT_KEY_PATHNAME, NULL);
    
    s8 achKeyFileName[KDV_MAX_PATH] = {0};
    sprintf(achKeyFileName, "%s%s", DEFAULT_KEY_PATHNAME, KEY_FILENAME);
	FILE *pHandler = fopen( achKeyFileName, "wb" );
	if( NULL == pHandler )
	{
		return ERR_KDVLICENSE_WRITEKEY_FAILED;
	}
	
	GenEncodeData( achEncText, wSequenceStrLen, (u8*)achSequenceStr, TRUE);

	s8 szCopyright[128];
	memset( szCopyright, 0 ,sizeof(szCopyright) );
	memcpy( szCopyright, KEDA_COPYRIGHT, strlen(KEDA_COPYRIGHT) );
	
	fwrite( szCopyright, strlen(KEDA_COPYRIGHT), 1, pHandler );
	
	u16 wEncTextLen = htons(LEN_KEYFILE_CONT);
	fwrite( &wEncTextLen, sizeof(wEncTextLen), 1, pHandler );	

	fwrite( achEncText, sizeof(achEncText), 1, pHandler );
	
	fwrite( szCopyright, sizeof(KEDA_COPYRIGHT), 1, pHandler );
	
	memset( szCopyright, 0, sizeof(szCopyright) );
	strcpy( szCopyright, ". Copyright 2003 - 2008." );

	fwrite( szCopyright, strlen( szCopyright ), 1, pHandler );

	fclose( pHandler );
	pHandler = NULL;		

    return SUCCESS_KDVLICENSE;
}

/*=============================================================================
函 数 名： WriteKey
功    能： 
算法实现： 
全局变量： 
参    数： BSTR bsAuthCode              [in]  授权码 32位
           BSTR bsLicenseKey            [in]  许可证编码 36位
           BSTR bsLicenseContent        [in]  license授权信息，具体内容为：设备ID + "$" + 接入数 + "$" + 截止日期
           BSTR *pbsRetId               [out] 返回码           
返 回 值： STDMETHODIMP 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/10/19  4.0			许世林                  创建
=============================================================================*/
STDMETHODIMP COprKey::WriteKey(BSTR bsAuthCode, BSTR bsLicenseKey, BSTR bsLicenseContent, BSTR *pbsRetId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

    CString csRetId;
	CString csAuthCode = bsAuthCode;
	CString csLicenseKey = bsLicenseKey;
	CString csLicenseContent = bsLicenseContent;
	u32 dwRet = SUCCESS_KDVLICENSE;

    //授权码验证
    if (0 != csAuthCode.Compare(DEFAULT_KDVLICENSE_AUTHCODE))
	{
		dwRet = ERR_KDVLICENSE_AUTH_FAILED;
	}
    else
    {        
        dwRet = GenLicense(csLicenseKey, csLicenseContent);        
    }    
	
	//返回值
	csRetId.Format("%d", dwRet);
	*pbsRetId = csRetId.AllocSysString();
	csRetId.ReleaseBuffer();

	if (SUCCESS_KDVLICENSE != dwRet)
	{
		return S_FALSE;
	}
	return S_OK;
}
