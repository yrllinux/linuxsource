#ifdef __cplusplus
extern "C" {
#endif



#ifndef SSSIZE_H
#define SSSIZE_H

#ifdef __cplusplus
extern "C" {
#endif

RVAPI int RVCALLCONV
ssSizeCurProtocols(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeMaxProtocols(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeCurProcs(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeMaxProcs(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeCurEvents(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeMaxEvents(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeCurTimers(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeMaxTimers(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeCurChannels(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeMaxChannels(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeCurMessages(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeMaxMessages(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeCurChanDescs(HSSAPP hSSApp);

RVAPI int RVCALLCONV
ssSizeMaxChanDescs(HSSAPP hSSApp);

#ifdef __cplusplus
}
#endif


#endif

#ifdef __cplusplus
}              
#endif



