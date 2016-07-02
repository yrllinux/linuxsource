#include "kdvmediavod.h"
//#include "kdvsys.h"
#include "rpctrl.h"
#include "kdvmediavodinstance.h"
#include "stdio.h"
#include "mcuver.h"

#define VOD_VERSION             (const s8* )"kdvmediavod 40.01.01.04.130513"

int g_nLanguageType;
// 退出信号量
SEMHANDLE g_hSemQuit;

//读取配置文件
void ReadConfigFile(TVodConfig* ptVodConfig) 
{
    s8 szData[256];
    
    BOOL32 bRet;
    bRet = GetRegKeyString(CONFIG_FILE_NAME,
                "SERVERCONFIG",  
                "SERVERIP", 
                "127.0.0.1",
                szData,
		        sizeof(szData));

    if (!bRet)
    {
        // if file not exist, create it
        FILE* f;
        f = fopen(CONFIG_FILE_NAME, "r");
        if (NULL == f)
        {
            f = fopen(CONFIG_FILE_NAME, "w");
        }
        
        if (NULL != f)
        {
            fclose(f);
        }

        SetRegKeyString(CONFIG_FILE_NAME,
                "SERVERCONFIG",  
                "SERVERIP", 
                "127.0.0.1");
		GetRegKeyString(CONFIG_FILE_NAME,
                "SERVERCONFIG",  
                "SERVERIP", 
                "127.0.0.1",
                szData,
		        sizeof(szData));
    }

    ptVodConfig->m_dwLocalIP = inet_addr(szData);
    
    s32 dwPort;
    
    bRet = GetRegKeyInt(CONFIG_FILE_NAME,
                "SERVERCONFIG",  
                "SERVERPORT", 
                6692,
                (s32*)&dwPort);
    if (!bRet)
    {
       SetRegKeyInt(CONFIG_FILE_NAME,
                "SERVERCONFIG",  
                "SERVERPORT", 
                6692);
	   GetRegKeyInt(CONFIG_FILE_NAME,
                "SERVERCONFIG",  
                "SERVERPORT", 
                6692,
                (s32*)&dwPort);
    }

	ptVodConfig->m_wVodListenPort = (u16)dwPort;
    
    bRet = GetRegKeyInt(CONFIG_FILE_NAME,
                "SERVERCONFIG",  
                "SENDSTARTPORT", 
                5000,
                (s32*)&dwPort);

    if (!bRet)
    {
        
        SetRegKeyInt(CONFIG_FILE_NAME,
                "SERVERCONFIG",  
                "SENDSTARTPORT", 
                5000);
		GetRegKeyInt(CONFIG_FILE_NAME,
                "SERVERCONFIG",  
                "SENDSTARTPORT", 
                5000,
                (s32*)&dwPort);
    }
    
	ptVodConfig->m_wSendPort = (u16)dwPort;
    
    bRet = GetRegKeyString(CONFIG_FILE_NAME,
                "SERVERCONFIG",  
                "ROOTDIR", 
                "c:\\",
                ptVodConfig->m_achRootDir,
                sizeof(ptVodConfig->m_achRootDir));
    if (!bRet)
    {
        SetRegKeyString(CONFIG_FILE_NAME,
                "SERVERCONFIG",  
                "ROOTDIR", 
                "c:\\");
		GetRegKeyString(CONFIG_FILE_NAME,
                "SERVERCONFIG",  
                "ROOTDIR", 
                "c:\\",
                ptVodConfig->m_achRootDir,
                sizeof(ptVodConfig->m_achRootDir));
    }
	
	// 增加从配置文件里读取MTU大小, zgc, 2007-04-02
	u16 wMTUSize = 0;
	bRet = GetRegKeyInt(CONFIG_FILE_NAME,
				"SERVERCONFIG", 
				"MTUSIZE",
				1500,
				(s32*)&wMTUSize );
	if( !bRet )
	{
		SetRegKeyInt(CONFIG_FILE_NAME,
				"SERVERCONFIG",
				"MTUSIZE",
				1500);
		GetRegKeyInt(CONFIG_FILE_NAME,
				"SERVERCONFIG", 
				"MTUSIZE",
				1500,
				(s32*)&wMTUSize );
	}
	ptVodConfig->m_wMTUSize = wMTUSize;
}

/*=============================================================================
函 数 名： MoveToSectionStart
功    能： 将游标移到指定的段名下一行开始处
算法实现： 
全局变量： 
参    数： FILE *stream, Profile名柄   
           const s8* lpszSectionName, Profile中的段名
           BOOL32 bCreate, 如果没有该段名是否创建
返 回 值： 返回偏移量Offset，如果失败返回-1
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/31   4.0			周广程                  创建
=============================================================================*/
s32	MoveToSectionStart( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate )
{
	char    achRow[MAX_LINE_LEN + 1];
    char    achSectionCopy[MAX_SECTIONNAME_LEN + 1];
    u32  dwLen;
    s8*   lpszReturn;

    ASSERT( strlen( lpszSectionName ) <= MAX_SECTIONNAME_LEN );

    /* move from the beginning */
    if ( fseek( stream, 0, SEEK_SET ) != 0 )
    {
        return( -1 );
    }

    /* copy the section name and change it */
    strncpy( achSectionCopy, lpszSectionName, MAX_SECTIONNAME_LEN );
    achSectionCopy[MAX_SECTIONNAME_LEN] = '\0';
    Trim( achSectionCopy );
    StrUpper( achSectionCopy );

    do
    {
        /* if error or arrive at the end of file */
		memset( achRow, 0, sizeof(achRow) );
        if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
        {
            if( bCreate && feof( stream ) )    /* create the section */
            {
                fputs( (const s8*)STR_RETURN, stream );
                fputs( (const s8*)"[", stream );
                fputs( (const s8*)lpszSectionName, stream );
                fputs( (const s8*)"]", stream );
                fputs( (const s8*)STR_RETURN, stream );
                fseek( stream, 0, SEEK_END );
                return( ftell( stream ) );
            }
            else
            {
                return( -1 );
            }
        }

        /* eliminate the return key */
        if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
        {
            lpszReturn[0] = '\0';
        }
        else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
        {
            lpszReturn[0] = '\0';
        }
        Trim( achRow );
        dwLen = strlen( achRow );
        
        /* must be a section */
        if( dwLen <= 2 || achRow[0] != '[' || achRow[dwLen - 1] != ']' )
        {
            continue;
        }
        
        /* verify the section name */
        strncpy( achRow, achRow + 1, dwLen - 2 );
        achRow[dwLen - 2] = '\0';
        Trim( achRow );
        StrUpper( achRow );
        if( strcmp( achRow, achSectionCopy ) == 0 )    /* identical */
        {
            return( ftell( stream ) );
        }
    } while( TRUE );
}

/*=============================================================================
函 数 名： MoveToSectionEnd
功    能： 将游标移到指定的段名的下一段开始处或是文件结尾
算法实现： 
全局变量： 
参    数： FILE *stream, Profile名柄   
           const s8* lpszSectionName, Profile中的段名
           BOOL32 bCreate, 如果没有该段名是否创建
返 回 值： 返回偏移量Offset，如果失败返回-1
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/31   4.0			周广程                  创建
=============================================================================*/
s32	MoveToSectionEnd( FILE *stream, const s8* lpszSectionName, BOOL32 bCreate )
{
	char    achRow[MAX_LINE_LEN + 1];
    u32		dwLen;
    s8*		lpszReturn;

	/* get section start offset */
	s32 nStartOffset = MoveToSectionStart( stream, lpszSectionName, bCreate );
	if ( -1 == nStartOffset )
	{
		OspPrintf( TRUE, FALSE, "[MoveToSectionEnd] Move to section %s start failed!\n", lpszSectionName );
		return nStartOffset;
	}

	/* move from the beginning */
    if ( fseek( stream, nStartOffset, SEEK_SET ) != 0 )
    {
        return( -1 );
    }

// 	s32 nScanOffset = nStartOffset;
	do
    {
		s32 nScanOffset = nStartOffset;
        /* if error or arrive at the end of file */
		memset( achRow, 0, sizeof(achRow) );
        if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
        {
            if ( feof(stream) != 0 )
			{
				/* eliminate the return key */
				if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
				{
					lpszReturn[0] = '\0';
				}
				else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
				{
					lpszReturn[0] = '\0';
				}
				Trim( achRow );
				dwLen = strlen( achRow );
        
				/* must be a section */
				if( dwLen <= 2 || achRow[0] != '[' || achRow[dwLen - 1] != ']' )
				{
					return ftell(stream);
				}
				else
				{
					return nScanOffset;
				}
			}
			else
			{
				return (-1);
			}
        }

        /* eliminate the return key */
		if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
		{
			lpszReturn[0] = '\0';
		}
		else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
		{
			lpszReturn[0] = '\0';
		}
		Trim( achRow );
		dwLen = strlen( achRow );

		/* must be a section */
		if( dwLen <= 2 || achRow[0] != '[' || achRow[dwLen - 1] != ']' )
		{
			nScanOffset = ftell(stream);
			continue;
		}
		else
		{
			return nScanOffset;
		}

    } while( TRUE );
}

/*=============================================================================
函 数 名： DelSection
功    能： 删除指定段名下的内容
算法实现： 
全局变量： 
参    数： FILE *stream, Profile名柄   
const s8* lpszSectionName, Profile中的段名
返 回 值： 返回偏移量Offset，如果失败返回-1
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/31   4.0			周广程                  创建
=============================================================================*/
s32	DelSection( FILE *stream, const s8* lpszSectionName )
{
    if ( stream == NULL || lpszSectionName == NULL )
    {
        return -1;
    }
    fseek( stream, 0, SEEK_SET );
    s32 nSecStartOffset = MoveToSectionStart( stream, lpszSectionName, FALSE );
    if ( -1 == nSecStartOffset )
    {
        OspPrintf( TRUE, FALSE, "[DelSection] Move to section %s start failed!\n", lpszSectionName );
        return -1;
    }
    OspPrintf( TRUE, FALSE,  "[DelSection] Sec %s start offset: %d\n", lpszSectionName, nSecStartOffset );
    s32 nSetEndOffset = MoveToSectionEnd( stream, lpszSectionName, FALSE );
    if ( -1 == nSetEndOffset )
    {
        OspPrintf( TRUE, FALSE,  "[DelSection] Move to section %s end failed!\n", lpszSectionName );
        return -1;
    }
    OspPrintf( TRUE, FALSE,  "[DelSection] Sec %s end offset: %d\n", lpszSectionName, nSetEndOffset );
    u32 dwLen = nSetEndOffset - nSecStartOffset;
    if ( 0 == dwLen )
    {
        return ftell(stream);
    }
    
    fseek( stream, 0, SEEK_END );
    u32 dwCpyLen = ftell(stream) - nSetEndOffset;
    
    s8 *lpszCpyBuf = (s8*)malloc( dwCpyLen + dwLen );
    if ( lpszCpyBuf == NULL )
    {
        return -1;
    }
    memset( lpszCpyBuf, 0, dwCpyLen + dwLen );
    fseek( stream, nSetEndOffset, SEEK_SET );
    dwCpyLen = fread( lpszCpyBuf, sizeof( char ), dwCpyLen, stream );
    fseek( stream, nSecStartOffset, SEEK_SET );
    fwrite( lpszCpyBuf, sizeof( char ), dwCpyLen+dwLen, stream );
    
    free(lpszCpyBuf);
    return ftell(stream);
}

/*=============================================================================
函 数 名： TrimCfgFile
功    能： 
算法实现： 
全局变量： 
参    数： const s8* lpszCfgFilePath
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/3   4.0			周广程                  创建
=============================================================================*/
BOOL32 TrimCfgFile( const s8* lpszCfgFilePath )
{
    FILE *stream = fopen( lpszCfgFilePath, "rb+" );
    if( stream == NULL )
    {
        return( FALSE );
    }
    fseek( stream, 0, SEEK_END );
    u32 dwFileLen = ftell( stream );
    s8 *lpszFileBuf = (s8*)malloc( dwFileLen+3 );
	if ( NULL == lpszFileBuf)
	{
		return FALSE;
	}
    memset( lpszFileBuf, 0 , dwFileLen );
    
    s8 *lpszTemp = lpszFileBuf;
    s8 *lpszReturn = NULL;
    u32 dwLen = 0;
    u32 dwTrimLen = 0;
    char achRow[MAX_LINE_LEN + 1];
    fseek( stream, 0, SEEK_SET );
    do 
    {
        memset( achRow, 0, sizeof(achRow) );
        if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
        {
            if ( 0 == feof(stream) )
            {
                return FALSE;
            }
        }
        
        /* eliminate the return key */
        if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
        {
            lpszReturn[0] = '\0';
            lpszReturn[1] = '\0';
        }
        else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
        {
            lpszReturn[0] = '\0';
        }
        Trim( achRow );
        dwLen = strlen( achRow );
        
        if ( dwLen > 0 )
        {
            sprintf( lpszTemp, "%s", achRow );
            lpszTemp += dwLen;
            sprintf( lpszTemp, "%s", "\n" );
            lpszTemp += strlen("\n");
        }
        
    } while( 0 == feof(stream) );
    
    dwTrimLen = lpszTemp - lpszFileBuf;
    
    fclose( stream );
    stream = fopen( lpszCfgFilePath, "w" );
	if ( NULL == stream)
	{
		return FALSE;
	}
    fwrite( lpszFileBuf, sizeof(s8), dwTrimLen, stream );
    
    free( lpszFileBuf );
    fclose( stream );
    
    return TRUE;
}

/*=============================================================================
函 数 名： GetSectionKeyArray
功    能： 批量读取某Section的Key值，适用于Key值不定的情况
算法实现： 
全局变量： 
参    数： FILE * stream
           const s8* lpszSectionName    
           s8 **ppKeyArray  // 单项key值的最大长度为MAX_LINE_LEN-2
           u32 dwMaxKeyNum  // 数组能容纳的最大Key数
           u32 &dwOffset  [IN/OUT] // 文件指针相对于该Section起始位置的偏移量
返 回 值： u32: 成功读取到的Key值数量
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/3   4.0			周广程                  创建
=============================================================================*/
u32 GetSectionKeyArray(FILE * stream, const s8* lpszSectionName, s8 **ppKeyArray, u32 dwMaxKeyNum, u32 &dwOffset )
{
    if ( stream == NULL || lpszSectionName == NULL || ppKeyArray == NULL || dwMaxKeyNum == 0 )
    {
        return 0;
    }

    fseek( stream, 0, SEEK_SET );
    s32 nSecStartOffset = MoveToSectionStart( stream, lpszSectionName, TRUE );
    if ( -1 == nSecStartOffset )
    {
        OspPrintf( TRUE, FALSE, "[GetSectionKeyArray] Move to section %s start failed!\n", lpszSectionName );
        return 0;
    }
    OspPrintf( TRUE, FALSE,  "[GetSectionKeyArray] Sec %s start offset: %d\n", lpszSectionName, nSecStartOffset );
    s32 nSetEndOffset = MoveToSectionEnd( stream, lpszSectionName, TRUE );
    if ( -1 == nSetEndOffset )
    {
        OspPrintf( TRUE, FALSE,  "[GetSectionKeyArray] Move to section %s end failed!\n", lpszSectionName );
        return 0;
    }
    OspPrintf( TRUE, FALSE,  "[GetSectionKeyArray] Sec %s end offset: %d\n", lpszSectionName, nSetEndOffset );
    u32 dwLen = nSetEndOffset - nSecStartOffset;
    if ( 0 == dwLen || ( dwLen > 0 && dwOffset >= dwLen ) )
    {
        return 0;
    }

    fseek( stream, nSecStartOffset + dwOffset, SEEK_SET );
    
    char achRow[MAX_LINE_LEN + 1];
    u32  dwKeyNum = 0;
    do
    {
        s8*   lpszReturn = NULL;
        /* if error or arrive at the end of file */
        memset( achRow, 0, sizeof(achRow) );
        if( fgets( achRow, MAX_LINE_LEN, stream ) == NULL )
        {
            return 0;
        }
        
        /* if arrive at a comment or cannot find a '=', continue */
        if( strstr( achRow, PROFILE_COMMENT1 ) == achRow ||
            strstr( achRow, PROFILE_COMMENT2 ) == achRow ||
            strstr( achRow, PROFILE_COMMENT3 ) == achRow ||
            strstr( achRow, PROFILE_COMMENT4 ) == achRow ||
            ( lpszReturn = strchr( achRow, '=' ) ) == NULL )
        {
            continue;
        }

        /* eliminate the return key */
        if( ( lpszReturn = strstr( achRow, "\r\n" ) ) != NULL )
        {
            lpszReturn[0] = '\0';
        }
        else if( ( lpszReturn = strstr( achRow, "\n" ) ) != NULL )
        {
            lpszReturn[0] = '\0';
        }
        dwLen = strlen( achRow );
        
        /* 长度小于等于2，不满足配置文件合法形式的最小长度条件（x=x）*/
        if( dwLen <= 2 )
        {
            continue;
        }
        
        // 找到'='号
        if( ( lpszReturn = strchr( achRow, '=' ) ) != NULL )
        {
            if ( lpszReturn - achRow > MAX_LINE_LEN - 2 )
            {
                continue;
            }
            lpszReturn[0] = '\0';
            Trim( achRow );
//            u8 len = strlen(achRow);
            // 提取key值
            strncpy( ppKeyArray[dwKeyNum], achRow, strlen(achRow) );
            ppKeyArray[dwKeyNum][lpszReturn-achRow] = '\0';
            dwKeyNum++;
        }
        
    } while( dwKeyNum < dwMaxKeyNum && ftell(stream) < nSetEndOffset );

    dwOffset = ftell(stream) - nSecStartOffset;

    return dwKeyNum;
}

//嵌入式下启动的API函数
API s32 vodstart()
{

#if defined(WIN32) && defined(_DEBUG)
    OspInit(TRUE);
#else
    OspInit(FALSE);
#endif

    TVodConfig tVodConfig;
    ReadConfigFile(&tVodConfig);
    
    printf("==================================================\n");
    printf("Wellcome to use kdvmediavod ! \n");
    printf("==================================================\n \n");
    
    
    //Print Config Param
    printf("CONFIG PARAMETERS:\n");
    u8* pIP = (u8*)&tVodConfig.m_dwLocalIP;
    printf("       SERVER IP:%u.%u.%u.%u Port:%u\n",
        pIP[0],pIP[1],pIP[2],pIP[3], tVodConfig.m_wVodListenPort);

	// 2011-11-17 康锐新需求：提示放到窗口开启之后进行，移至kdvmediavodinstance.cpp
	// 	if( !OspIsLocalHost( tVodConfig.m_dwLocalIP ) )
	// 	{
	// 		// 2011-10-14 add by pgf: 新需求：配置文件为非本地IP时，给出窗口警告
	//         if ( g_nLanguageType == LANGUAGE_CHINESE )
	//         {
	//             AfxMessageBox( "读取配置IP错误：本地IP与配置IP不匹配，服务器不能正常工作，请配置为本地IP" );
	//         }
	//         else
	//         {
	// 			// 启动语言，根据系统和语言默认设置而定
	//             AfxMessageBox( "reading config error: local IP doesn't match that from config, cannot connect server, please set it using local IP" );
	//         }
	// 	}
    
	g_tVodConfig = tVodConfig;
    MediaVodInit(&tVodConfig);
    
    return 0;
}

API void vodend()
{
    printf("\nKDVVOD: User quit vod, please wait...\n");
	OspSemBCreate(&g_hSemQuit);
	OspSemTake(g_hSemQuit);
	::OspPost(MAKEIID(MEDIAVOD_SERVER_ID, CInstance::DAEMON), MEDIAVOD_QUIT);

	OspSemTake(g_hSemQuit);

    RPRelease();//释放录放像库

	OspSemGive(g_hSemQuit);
	OspSemDelete(g_hSemQuit);
    OspQuit();
}


#if defined(WIN32) && defined(_CONSOLE)
void main(int argc, char *argv[])
{
    vodstart();
    
    s8 command[256] = {0};    
    do 
    {
        scanf("%s",command);
        if ( memcmp(command, "quit", strlen("quit")) == 0 )
        {
            printf("Do you want to quit KDVMEDIAVOD[Y/N]?  ");
            scanf("%s",command);
            if( command[0] == 'y' || command[0] == 'Y' )
                break;
        }
        memset(command, 0, 256);
    } while(TRUE);
    
    printf("\nKDVVOD: User quit vod, please wait...\n");
    
    vodend();
    
}
#endif // defined (WIN32)



API void vodver()
{
    s8 gs_VersionBuf[128] = {0};
    {
        strcpy(gs_VersionBuf, KDV_MCU_PREFIX);

        s8 achMon[16] = {0};
        u32 byDay = 0;
        u32 byMonth = 0;
        u32 wYear = 0;
        s8 achFullDate[24] = {0};

        s8 achDate[32] = {0};
        sprintf(achDate, "%s", __DATE__);
        StrUpper(achDate);

        sscanf(achDate, "%s %d %d", achMon, &byDay, &wYear );

        if ( 0 == strcmp( achMon, "JAN") )		 
            byMonth = 1;
        else if ( 0 == strcmp( achMon, "FEB") )
            byMonth = 2;
        else if ( 0 == strcmp( achMon, "MAR") )
            byMonth = 3;
        else if ( 0 == strcmp( achMon, "APR") )		 
            byMonth = 4;
        else if ( 0 == strcmp( achMon, "MAY") )
            byMonth = 5;
        else if ( 0 == strcmp( achMon, "JUN") )
            byMonth = 6;
        else if ( 0 == strcmp( achMon, "JUL") )
            byMonth = 7;
        else if ( 0 == strcmp( achMon, "AUG") )
            byMonth = 8;
        else if ( 0 == strcmp( achMon, "SEP") )		 
            byMonth = 9;
        else if ( 0 == strcmp( achMon, "OCT") )
            byMonth = 10;
        else if ( 0 == strcmp( achMon, "NOV") )
            byMonth = 11;
        else if ( 0 == strcmp( achMon, "DEC") )
            byMonth = 12;
        else
            byMonth = 0;

        if ( byMonth != 0 )
        {
            sprintf(achFullDate, "%04d%02d%02d", wYear, byMonth, byDay);
            sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
        }
        else
        {
            // for debug information
            sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
        }
    }
    OspPrintf( TRUE, FALSE, "Vod Version: %s\n", gs_VersionBuf );
    OspPrintf(TRUE, FALSE,"Vod Version %s. compile time: %s %s\n",
		      VOD_VERSION, __TIME__, __DATE__ );
}


API void ssnDump(u16 wIndex)
{
    ::OspInstDump(MEDIAVOD_SERVER_ID, wIndex, 0);
}

API void vodhelp()
{
    OspPrintf(TRUE, FALSE, "***********Vod Help Command********\n");
    OspPrintf(TRUE, FALSE, "**  1. vodver()                                    **\n");
    OspPrintf(TRUE, FALSE, "**  2. setvodlog(s32 nSrceenLevel, s32 nFileLevel) **\n");
    OspPrintf(TRUE, FALSE, "**  3. ssnDump(u16 wIndex)                         **\n");
}
