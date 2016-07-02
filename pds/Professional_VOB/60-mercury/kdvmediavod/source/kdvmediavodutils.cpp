#include "kdvmediavod.h"
// #include "kdvsys.h"
#include "rpctrl.h"
#include "kdvmediavodinstance.h"


#ifdef _VXWORKS_
#include "dirent.h"
#endif //_VXWORKS_


#define IsScreenLog(n) ((n) <= g_nScreenLogLevel)
#define IsFileLog(n) ((n) <= g_nFileLogLevel)

s32  g_nScreenLogLevel = 3;
s32  g_nFileLogLevel = 3;
FILE* g_pLogFile = NULL;


API void setvodlog(s32 nSrceenLevel, s32 nFileLevel)
{    
    g_nScreenLogLevel = nSrceenLevel;
    g_nFileLogLevel = nFileLevel;

}

s8* GetLogTypeName(s32 nLevel)
{
    s8* p;
    switch(nLevel)
    {
    case LOG_DEBUG:
        p = "DEBUG";
    	break;

    case LOG_EXP:
        p = "ERROR";
        break;

    default:
        p = "  ";
        break;
    }

    return p;
}

void VodLog(s32 nLevel, s8 * pchFormat,...)
{
    s8 szBuf[512];
	memset(szBuf, 0, sizeof(szBuf));
    va_list arg;
    s32 nLen = 0;
    BOOL32 bSrceenLog = IsScreenLog(nLevel);
    BOOL32 bFileLog = IsFileLog(nLevel);

    if (bSrceenLog || bFileLog)
    {
        time_t now;
        struct tm t;
        
        time(&now);
        t = *localtime(&now);

        nLen = sprintf(szBuf, "%2.2u:%2.2u:%2.2u----[%s] ",
                        t.tm_hour, t.tm_min, t.tm_sec, GetLogTypeName(nLevel));

        va_start(arg,pchFormat);
        nLen += vsprintf(szBuf+nLen, pchFormat, arg);
        va_end(arg);
    }

    if (bSrceenLog)
    {
        OspPrintf(TRUE, FALSE, szBuf);
    }

    if (bFileLog)
    {
        if (g_pLogFile == NULL)
        {
            g_pLogFile = fopen("vodlog.txt", "w");
            if (NULL == g_pLogFile)
            {
                OspPrintf(TRUE, FALSE, "Create LogFile Failed.\n");
            }
        }

        if (NULL != g_pLogFile)
        {
            fwrite(szBuf, 1, nLen, g_pLogFile);
            fflush(g_pLogFile);
        }
    }
   

    return;    
}


BOOL32 IsValidPlayFile(s8* pszFileName)
{
    
    u32 nNameLen = strlen(pszFileName);
    if (nNameLen < 4)
    {
        return FALSE;
    }
    
    if(!(pszFileName[nNameLen-4] == '.' && 
        (pszFileName[nNameLen-3] == 'a' || pszFileName[nNameLen-3] == 'A')&&
        (pszFileName[nNameLen-2] == 's' || pszFileName[nNameLen-2] == 'S')&& 
        (pszFileName[nNameLen-1] == 'f' || pszFileName[nNameLen-1] == 'F')))
    {
        return FALSE; 
    }
    
    return TRUE;
}

   


u16 GetAbsolutenessPath(s8* pszFilePath, s8* pbyAbsPath, u32 dwAbsPathSize)
{
    if (dwAbsPathSize < strlen(g_tVodConfig.m_achRootDir))
    {
        return ERROR_MEDIAVOD_PARAM;
    }

    s32 nLen = strlen(g_tVodConfig.m_achRootDir);
    memcpy(pbyAbsPath, g_tVodConfig.m_achRootDir, nLen);
    pbyAbsPath += nLen;
    nLen = strlen(pszFilePath);

    memcpy(pbyAbsPath, pszFilePath, nLen);
    pbyAbsPath[nLen] = '\0';

    return MEDIAVOD_NO_ERROR;
}


u16 FindFile(char* path, TVodFileList* ptVodFileList) //查找文件
{

    s8 achRootDir[MEDIAVOD_MAX_PATHFILENAME];
    memset(achRootDir, 0, sizeof(achRootDir));
    u16 dwTemp = 0;
    dwTemp = GetAbsolutenessPath(path,achRootDir,sizeof(achRootDir));
    if (dwTemp != MEDIAVOD_NO_ERROR)
    {
        return ERROR_FILE_PATH;
    }
    
#if defined (_VXWORKS_)   

    printf("filedir: %s\n",achRootDir);
    
    DIR *root;
    root = opendir(achRootDir);
       
    struct dirent *entry;
    s32 FileCount = 0;
    while(entry = readdir(root))
    {

        if (FileCount >= MEDIAVOD_MAX_FILENUM)
        {
            FileCount--;
            OspPrintf(TRUE, FALSE, "文件数大于规定文件数!\n");
            break;
        }

        memcpy(ptVodFileList->m_tVodFileName[FileCount].m_achFileName,entry->d_name,sizeof(entry->d_name));
        char          *filename;
        struct stat   fileStat;
        
        stat (entry->d_name, &fileStat);
        
        if (S_ISDIR (fileStat.st_mode))
        {
            //目录
            ptVodFileList->m_tVodFileName[FileCount].m_byType = 0;
        }
        else
        {
            //文件
            ptVodFileList->m_tVodFileName[FileCount].m_byType = 1;
        }

        FileCount++;
    }
    
    ptVodFileList->m_dwFileNum = FileCount;

#endif // defined (_VXWORKS_)
    
#if defined (WIN32)
    
	//[2013/5/10 zhangli]目录判断长度，去掉非asf格式文件
    u32 i = strlen(achRootDir);
	s8 *pSuffixStr = "*.*";
	if (i + strlen(pSuffixStr) >= MEDIAVOD_MAX_PATHFILENAME)
	{
		OspPrintf(TRUE, FALSE, "file full path name lenght(%d) is longer than %d!\n", i, MEDIAVOD_MAX_PATHFILENAME);
		return ERROR_TOO_LONG_FILENAME;
	}

    if(achRootDir[i-1] == '\\')
    {
		sprintf(achRootDir, "%s%s", achRootDir, pSuffixStr);
    }
    else
    {
		sprintf(achRootDir, "%s\\%s", achRootDir, pSuffixStr);
    }

	HANDLE hFind;
	WIN32_FIND_DATA dataFind;
	BOOL bMoreFiles = TRUE;
    memset(&dataFind,0,sizeof(dataFind));
	hFind = FindFirstFile(achRootDir,&dataFind);
	s32 dirFileCount = 0;
	s32 asfFileCount = 0;
	while(hFind != INVALID_HANDLE_VALUE && bMoreFiles == TRUE)
	{
		if (strlen(dataFind.cFileName) >= MEDIAVOD_MAX_FILENAME)
		{
			bMoreFiles = FindNextFile(hFind, &dataFind);
			continue;
		}

		DWORD sign = dataFind.dwFileAttributes;
		if((sign & FILE_ATTRIBUTE_DIRECTORY))	// dirctory
		{
			dirFileCount++;
		}
		else if (isAsfFile(dataFind.cFileName))	// file & asf file
		{
			asfFileCount++;
		}

		bMoreFiles = FindNextFile(hFind,&dataFind);	
        if((dirFileCount + asfFileCount) >= MEDIAVOD_MAX_FILENUM)
        {
            OspPrintf(TRUE, FALSE, "文件数大于规定文件数!\n");
            break;
        }
	}

	s32 FileNum = 0;
	s32 dirFileNum = 0;
	s32 asfFileNum = 0;
    memset(&dataFind, 0, sizeof(dataFind));    
	hFind = FindFirstFile(achRootDir,&dataFind);
	bMoreFiles = TRUE;
	while(hFind != INVALID_HANDLE_VALUE && bMoreFiles == TRUE)
	{
		if (strlen(dataFind.cFileName) >= MEDIAVOD_MAX_FILENAME)
		{
			bMoreFiles = FindNextFile(hFind, &dataFind);
			continue;
		}

		FileNum = MEDIAVOD_MAX_FILENUM + 1;

		DWORD sign = dataFind.dwFileAttributes;
		if(sign & FILE_ATTRIBUTE_DIRECTORY)//目录
		{
			if (dirFileNum < dirFileCount)
			{
				FileNum = dirFileNum;
				ptVodFileList->m_tVodFileName[FileNum].m_byType = 0;
				dirFileNum++;
			}
		}
		else 
        {
			if (asfFileNum < asfFileCount && isAsfFile(dataFind.cFileName))
			{
				FileNum = dirFileCount + asfFileNum;
				ptVodFileList->m_tVodFileName[FileNum].m_byType = 1; //文件
				asfFileNum++;
			}
        }

		//找到符合条件的文件或者目录
		if (MEDIAVOD_MAX_FILENUM + 1 != FileNum)
		{
			memcpy(ptVodFileList->m_tVodFileName[FileNum].m_achFileName, dataFind.cFileName, strlen(dataFind.cFileName));
			ptVodFileList->m_tVodFileName[FileNum].m_dwFileLen = dataFind.nFileSizeLow;//文件大小
		}
        
		bMoreFiles = FindNextFile(hFind, &dataFind);	
        if ((dirFileNum + asfFileNum) >= MEDIAVOD_MAX_FILENUM || (dirFileNum + asfFileNum) >= (dirFileCount + asfFileCount))
		{
			break;
		}
	}

    ptVodFileList->m_dwFileNum = dirFileCount + asfFileCount;
	OspPrintf(TRUE, FALSE, "Find %d files\n", dirFileCount + asfFileCount);
	FindClose(hFind);
    
#endif // defined (WIN32)
    
	return MEDIAVOD_NO_ERROR;	
}

BOOL32 isAsfFile(const s8 *pFileNme)
{
	if (NULL == pFileNme)
	{
		return FALSE;
	}
	u16 wFileNameLen = strlen(pFileNme);
	if (wFileNameLen <= 4)
	{
		return FALSE;
	}
	if (strcmp(pFileNme+(wFileNameLen-4), ".asf") == 0)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL32 IsVideoType(u8 byMediaType)
{
    if (byMediaType == MEDIA_TYPE_MP4  ||  //mpeg4	
        byMediaType == MEDIA_TYPE_H262 ||  //mpeg2
        byMediaType == MEDIA_TYPE_H264 ||
        byMediaType == MEDIA_TYPE_H261 ||
        byMediaType == MEDIA_TYPE_H263 || 
        byMediaType == MEDIA_TYPE_H263PLUS )
    {
        return TRUE;
	}

    return FALSE;
}



