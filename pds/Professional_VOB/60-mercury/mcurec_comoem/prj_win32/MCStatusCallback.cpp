// MCStatusCallback.cpp : Implementation of CMCStatusCallback
#include "stdafx.h"
#include "Mcu.h"
#include "MCStatusCallback.h"
#include "mcu2recitf.h"
#include "ConfData.h"

/////////////////////////////////////////////////////////////////////////////
// CMCStatusCallback

static CMCStatusCallback *g_pcCBMCStatus = NULL;

CMCStatusCallback::CMCStatusCallback()
{
    g_pcCBMCStatus = this;
}

// 处理录像库的回调
void CBRecStart( TRecProperty &tRecProperty, TConfProperty &tConfProperty, u8 &byErrId )
{
    RecComLog(LOGLV_INFO, "CBRecStart\n");
    //save the property
    g_cConfData.SetAt(tConfProperty.GetIndex(), 
                      &tConfProperty);

    g_cConfData.SetAt(tConfProperty.GetIndex(), 
                      &tRecProperty);

    //Notify the user
    if (g_pcCBMCStatus)
    {
        RecComLog(LOGLV_WARN, "Fire_OnStatus for ConfRec.%d Start\n",tConfProperty.GetIndex() );

        RecComLog(LOGLV_WARN, "ConfRec.%d information:\n", tConfProperty.GetIndex());
        RecComLog(LOGLV_WARN, "\t Conf name: %s\n", tConfProperty.GetConfName());
        RecComLog(LOGLV_WARN, "\t Record name: %s\n", tConfProperty.GetRecName());
        RecComLog(LOGLV_WARN, "\t Bitrate: %d\n", tConfProperty.GetTotalBitRate());

        RecComLog(LOGLV_WARN, "\t Stream Number: %d\n", tRecProperty.GetTotalStreamNum());
        for (int i = 0; i < tRecProperty.GetTotalStreamNum(); i++)
        {
            TStreamParam *ptParam = tRecProperty.GetStreamParam(i);
            RecComLog(LOGLV_WARN, "\t Stream.%d:\n",i);
            RecComLog(LOGLV_WARN, "\t\t Type: %d \n", ptParam->GetStreamType());
            RecComLog(LOGLV_WARN, "\t\t Bitrate: %d \n", ptParam->GetBitrate());
            if (ptParam->GetStreamType() == emStreamTypeAudio)
            {
                RecComLog(LOGLV_WARN, "\t\t Audio Format: 0x%x \n", ptParam->GetAudFormat());
                RecComLog(LOGLV_WARN, "\t\t Audio Sample/s: %d \n", ptParam->GetAudSamplesPerSec());
                RecComLog(LOGLV_WARN, "\t\t Audio Bit/s: %d \n", ptParam->GetAudBitsPerSample());
                RecComLog(LOGLV_WARN, "\t\t Audio Chan Num: %d \n", ptParam->GetAudChnlNum());
                RecComLog(LOGLV_WARN, "\t\t Audio Block Aligh: %d \n", ptParam->GetAudBlockAlignment());
                RecComLog(LOGLV_WARN, "\t\t Audio Spec Data (Len: %d) 0x%.2x... \n", 
                                      ptParam->GetAudCodecSpecificDataSize(),
                                      ptParam->GetAudCodecSpecificDataSize() > 0 ? *ptParam->GetAudCodecSpecificData() : 0 );
            }
            else if (ptParam->GetStreamType() == emStreamTypeVideo ||
                     ptParam->GetStreamType() == emStreamTypeDStream)
            {
                RecComLog(LOGLV_WARN, "\t\t Video Format: 0x%x \n", ptParam->GetVidFormat());
                RecComLog(LOGLV_WARN, "\t\t Video Fps: %d \n", ptParam->GetVidFramesPerSec());
                RecComLog(LOGLV_WARN, "\t\t Video Size: %d*%d \n", ptParam->GetVidImageWidth(), ptParam->GetVidImageHeight());
                RecComLog(LOGLV_WARN, "\t\t Video Bitdepth: %d \n", ptParam->GetVidBitCounts());
                RecComLog(LOGLV_WARN, "\t\t Video Image Size: %d \n", ptParam->GetVidImageSize());
                RecComLog(LOGLV_WARN, "\t\t Video Horz PPM: %d \n", ptParam->GetVidHorizontalPixelsPerMeter());
                RecComLog(LOGLV_WARN, "\t\t Video Vert PPM: %d \n", ptParam->GetVidVerticalPixelsPerMeter());
                RecComLog(LOGLV_WARN, "\t\t Video ColorsUsedCount: %d \n", ptParam->GetVidColorsUsedCount());
                RecComLog(LOGLV_WARN, "\t\t Video ImportantColorsCount: %d \n", ptParam->GetVidImportantColorsCount());
            }
            else
            {
                RecComLog(LOGLV_INFO, "\t\t Unknow Media\n");
            }
        }

        TRecConfStatus tStatus;
        tStatus.SetIndex(tConfProperty.GetIndex());
        tStatus.SetRecState(emRecStateRecording);
        g_pcCBMCStatus->Fire_OnStatus((long)&tStatus);
    }

    byErrId = 0;
    return;
}

void CBRecStop(DWORD dwIndex, u8 &byErr)
{
    RecComLog(LOGLV_INFO, "CBRecStop\n");
    //save the property
    g_cConfData.Reset(dwIndex);

    //Notify the user
    if (g_pcCBMCStatus)
    {
        RecComLog(LOGLV_WARN, "Fire_OnStatus for ConfRec.%d Stop\n", dwIndex);
        TRecConfStatus tStatus;
        tStatus.SetIndex(dwIndex);
        tStatus.SetRecState(emRecStateStop);
        g_pcCBMCStatus->Fire_OnStatus((long)&tStatus);
    }

    byErr = 0;
    return;
}

void CBRecPause(DWORD dwIndex, u8 &byErr)
{
    RecComLog(LOGLV_INFO, "CBRecPause\n");
    //set the property
    TRecConfStatus tStatus;
    tStatus.SetIndex(dwIndex);
    tStatus.SetRecState(emRecStatePaused);
        
    //Notify the user
    if (g_pcCBMCStatus)
    {
        RecComLog(LOGLV_WARN, "Fire_OnStatus for ConfRec.%d Paused\n", dwIndex);
        
        g_pcCBMCStatus->Fire_OnStatus((long)&tStatus);
    }

    byErr = 0;
}

void CBRecResume(DWORD dwIndex, u8 &byErr)
{
    RecComLog(LOGLV_INFO, "CBRecResume\n");
    //set the property
    TRecConfStatus tStatus;
    tStatus.SetIndex(dwIndex);
    tStatus.SetRecState(emRecStateRecording);
    
    //Notify the user
    if (g_pcCBMCStatus)
    {
        RecComLog(LOGLV_WARN, "Fire_OnStatus for ConfRec.%d Resumed\n", dwIndex);
        g_pcCBMCStatus->Fire_OnStatus((long)&tStatus);
    }

    byErr = 0;
}