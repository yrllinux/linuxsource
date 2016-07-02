
/*

NOTICE:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*/

#ifndef _RV_H323_MIB_STATISTICS_H
#define _RV_H323_MIB_STATISTICS_H


#include "cmmib.h"
#include "pvaltree.h"

#ifdef __cplusplus
extern "C" {
#endif

#define capExchCauseSize    4
#define logChanCauseSize    14


typedef  enum
{
    q931MsgSetup,
    q931MsgCallProceeding,
    q931MsgConnect,
    q931MsgAlerting,
    q931MsgReleaseComplete,
    q931MsgStatus,
    q931MsgFacility,
    q931MsgStatusEnquiry,
    q931MsgProgress,
    q931MsgSetupAck,
    q931MsgInformation,
    q931MsgNotify,
    q931MsgStatSize /* Must be last in list - used to get the length of it */

} q931MsgStatTypeT;




typedef struct
{
    int q931StatisticIn[q931MsgStatSize];
    int q931StatisticOut[q931MsgStatSize];

    int h245ControlChannelNumberOfListenPorts;
    int h245ControlChannelMaxConnections;
    int h245ControlChannelNumberOfListenFails;
    int h245ControlChannelNumberOfActiveConnections;
    int h245ControlChannelMasterSlaveMaxRetries;
    int h245ControlChannelConnectionAttemptsFail;
    int h245ControlChannelNumberOfTunnels;

    int h245ControlChanneMasterSlavelDeterminations;
    int h245ControlChannelMasterSlaveAcks;
    int h245ControlChannelMasterSlaveRejects;
    int h245ControlChannelMasterSlaveReleases;
    int h245ControlChannelNumberOfMasterSlaveInconsistentFieldRejects;


    int h245ControlChannelMasterSlaveMSDRejects;
    int h245ControlChannelMasterSlaveT106Rejects;
    int h245ControlChannelMasterSlaveMaxCounterRejects;



    int h245CapExchangeSets;
    int h245CapExchangeAcks;
    int h245CapExchangeRejects;
    int h245CapExchangeReleases;
    int h245CapExchangeRejectCause[capExchCauseSize];

    int h245LogChanOpenLogChanTotalRequests;
    int h245LogChanOpenLogChanAcks;
    int h245LogChanOpenLogChanConfirms;
    int h245LogChanOpenLogChanRejects;
    int h245LogChanOpenLogChanRejectCause[logChanCauseSize];

    int h245LogChanCloseLogChannels;
    int h245LogChanCloseLogChanAcks;
    int h245LogChanCloseLogChanRequests;
    int h245LogChanCloseLogChanRequestsAcks;
    int h245LogChanCloseLogChanRequestRejects;
    int h245LogChanCloseLogChanRequestReleases;
}h341StatisticParametersT;


typedef  enum
{
    h245CapExchangeRejectUnspecified,
    h245CapExchangeRejectUndefinedTableEntryUsed,
    h245CapExchangeRejectDescriptorCapacityExceeded,
    h245CapExchangeRejectTableEntryCapacityExeeded
}capExchRejectCauseT;



typedef enum
{
    h245LogChanOpenLogChanRejectUnspecified,
    h245LogChanOpenLogChanRejectUnsuitableReverseParameters,
    h245LogChanOpenLogChanRejectDataTypeNotSupported,
    h245LogChanOpenLogChanRejectDataTypeNotAvailable,
    h245LogChanOpenLogChanRejectUnknownDataType,
    h245LogChanOpenLogChanRejectDataTypeALCombinationNotSupported,
    h245LogChanOpenLogChanRejectMulticastChannelNotAllowed,
    h245LogChanOpenLogChanRejectInsuffientBandwdith,
    h245LogChanOpenLogChanRejectSeparateStackEstablishmentFailed,
    h245LogChanOpenLogChanRejectInvalidSessionID,
    h245LogChanOpenLogChanRejectMasterSlaveConflict,
    h245LogChanOpenLogChanRejectWaitForCommunicationMode,
    h245LogChanOpenLogChanRejectInvalidDependentChannel,
    h245LogChansOpenLogChanRejectReplacementForRejected
}logChannelRejectCause;


HSTATISTIC mibCreateStatistic(void);

void mibDestroyStatistic(IN HSTATISTIC statistic);


/************************************************************************
 * addStatistic
 * purpose: Update the statistics information about current state of the
 *          stack. This information is taken from incoming and outgoing
 *          messages.
 * input  : hStatistic  - Statistics information handle
 *          mType       - Type of message to check
 *          hVal        - Value tree of the message to check
 *          vNodeId     - Root ID of the message to check
 *          directionIn - RV_TRUE if this is an incoming message, RV_FALSE if outgoing.
 * output : none
 * return : none
 ************************************************************************/
void addStatistic(
    IN HSTATISTIC       hStatistic,
    IN cmProtocol       mType,
    IN HPVT             hVal,
    IN int              vNodeId,
    IN RvBool           directionIn);


/************************************************************************
 * getStatistic
 * purpose: Get a statistics parameter value for the MIB
 * input  : pStatistic  - Current statistics information
 *          type        - Type of parameter to get
 * output : none
 * return : Parameter's value on success
 *          Negative value on failure
 ************************************************************************/
int getStatistic(IN h341StatisticParametersT* pStatistic, IN mibStatisticParamEnumT type);


#ifdef __cplusplus
}
#endif

#endif  /* _RV_H323_MIB_STATISTICS_H */

