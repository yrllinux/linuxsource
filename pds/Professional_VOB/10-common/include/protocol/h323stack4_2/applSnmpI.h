#ifndef __APPLSNMPI_H
#define __APPLSNMPI_H
#include <snmpI.h>

#ifdef __cplusplus
extern "C" {
#endif

RVAPI
void RVCALLCONV applSnmpInstanceInit(IN h341ApplHandle hApp);

RVAPI
void RVCALLCONV applSnmpInstanceEnd();


#ifdef __cplusplus
}
#endif

#endif