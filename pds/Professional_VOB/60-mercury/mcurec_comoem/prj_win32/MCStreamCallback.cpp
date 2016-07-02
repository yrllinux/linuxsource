// MCStreamCallback.cpp : Implementation of CMCStreamCallback
#include "stdafx.h"
#include "Mcu.h"
#include "MCStreamCallback.h"
#include "mcu2recitf.h"
#include "ConfData.h"

/////////////////////////////////////////////////////////////////////////////
// CMCStreamCallback
static CMCStreamCallback *g_pcCBMCStream = NULL;

CMCStreamCallback::CMCStreamCallback()
{
    g_pcCBMCStream = this;
}

void CBRecStream( TStreamProperty *ptSProperty, u8 &byErrId )
{
    //save the property
    g_cConfData.SetAt(ptSProperty->GetConfIndex(),
                      ptSProperty->GetStreamIndex(),
                      ptSProperty);

    //Notify the user
    if (g_pcCBMCStream)
    {
        RecComLog(LOGLV_INFO, "CBRecStream conf.%u, strm.%u, tstamp.%u\n",
                  ptSProperty->GetConfIndex(), 
                  ptSProperty->GetStreamIndex(),
                  ptSProperty->GetFrameHead()->GetTStamp());

        g_pcCBMCStream->Fire_OnStreamSample((long)ptSProperty);
    }

    byErrId = 0;
    return;
}