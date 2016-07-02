/**
*** Copyright (c) Equator Technologies, Inc. Unpublished
***
*** Equator Technologies, Inc.
*** Proprietary & Confidential
***
*** This source code and the algorithms implemented therein constitute
*** confidential information and may comprise trade secrets of Equator
*** or its associates, and any use thereof is subject to the terms and
*** conditions of the Non-Disclosure Agreement pursuant to which this
*** source code was originally received.
**/

/*
 *  Module name              : $RCSfile: ds_desc_api.h,v $   $Revision: 1.1 $
 *
 *  Last update              : $Date: 2003/04/12 00:01:58 $ UTC
 *
 *  Description              :
 *     
 *  Legacy Datastreamer wrapper.
 *
 *  This provides mimimum functionality to support the old DS API. Its primary goal
 *  is to enable codecs to be ported quickly with the intention that legacy DS API
 *  calls will be systematically changed to the V6 API. Hence, the supported API is
 *  based on the common function calls used in existing codecs. These have the
 *  following characteristics:
 *    - Paths are Memory-to-Memory, Memory-to-GBin or GBout-to-memory
 *    - Interrupts are not used (codecs poll the "halt" bit)
 *    - Codec create their own descriptor chains.
 */

#if !defined(_DS_DESC_)
#define _DS_DESC_

/*------------------------------- Includes ----------------------------------*/

#include "ds.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------ Device Types -------------------------------*/

#define DS_DESC_API_COUNT            1
#define DS_DESC_API_CONTROL_FIELD    2
#define DS_DESC_API_PITCH            3
#define DS_DESC_API_WIDTH            4
#define DS_DESC_API_ADDR             5
#define DS_DESC_API_FORMAT2          1
#define DS_DESC_API_NOT_FORMAT2      0
#define DS_DESC_API_LINKABLE_FORMAT2 1
#define DS_DESC_API_NOT_LINKABLE_FORMAT2 0

/*------------------------------- API Functions -----------------------------*/
DS_DESCRIPTOR * DsDescAllocate( int number_of_decriptors);

SCODE DsDescFree( DS_DESCRIPTOR *the_descriptor_to_free );

SCODE DsStopPath(DS_PATH path);

void DsDescSetGenericCtrl( DS_CONTROL_DESCRIPTOR  *the_descriptor,
                        DS_DESCRIPTOR             *the_next_descriptor,
                        unsigned short             options,
                        void                      *addr,
                        unsigned short             channel_control,
                        unsigned short             transfer_information);
                        
void DsDescSetGenericFmt1( DS_DESCRIPTOR    *the_descriptor,
                        DS_DESCRIPTOR       *the_next_descriptor,
                        unsigned short       options,
                        void                *addr,
                        unsigned short       width,
                        unsigned short       count,
                        short                pitch );


void DsDescSetupDSBuff( DS_PATH_INFO   *dsPathRequest, 
                        int             bufferSize,
                        int             srcChunkSize,
                        int             dstChunkSize,
                        int             isSrcIO, 
                        int             isDstIO,
                        int             flags );


void DsDescSetupDSChans(DS_PATH_INFO    *dsPathRequest,
                        int              srcChTransferBurstSize,
                        int              srcChMinInterburstDelay,
                        int              srcChDtsPriority,
                        int              srcChChannelPriority,
                        int              srcChWayMask,
                        void            *srcChBase,
                        int              srcChDescriptorFetchMode,
                        int              dstChTransferBurstSize,
                        int              dstChMinInterburstDelay,
                        int              dstChDtsPriority,
                        int              dstChChannelPriority,
                        int              dstChWayMask,
                        void            *dstChBase,
                        int              dstChDescriptorFetchMode);
                        

void DsDescSet1DFmt1( DS_DESCRIPTOR    *the_descriptor,
                      DS_DESCRIPTOR    *the_next_descriptor,
                      unsigned short    options,
                      void             *addr,
                      unsigned short    length );


void DsDescSetupDSChansSrcIo( DS_PATH_INFO  *dsPathRequest,
                              int            dstChTransferBurstSize,
                              int            dstChMinInterburstDelay,
                              int            dstChDtsPriority,
                              int            dstChChannelPriority,
                              int            dstChWayMask,
                              void          *dstChBase,
                              int            dstChDescriptorFetchMode);


void DsDescSetupDSChansDstIo( DS_PATH_INFO  *dsPathRequest,
                              int            srcChTransferBurstSize,
                              int            srcChMinInterburstDelay,
                              int            srcChDtsPriority,
                              int            srcChChannelPriority,
                              int            srcChWayMask,
                              void          *srcChBase,
                              int            srcChDescriptorFetchMode);

/*------------------------------- Unsupported -------------------------------*/
/*
 * Legacy Types/Functions that are either not required or supported.
 */ 
#ifdef NOT_REQUIRED
void DsDescAPIInit( void );

DS_DESCRIPTOR * DsDescAllocate( int number_of_decriptors );

DS_DESCRIPTOR * DSDescriptorAllocateFmt1( void );

SCODE DsDescFree( DS_DESCRIPTOR *the_descriptor_to_free );

void ** DsDescFmt2AddrListAllocate( int num_addresses );

SCODE DsDescFmt2AddrListFree( void **addr_list_to_free );


void 
DsDescSetGenericFmt2(
    DS_DESCRIPTOR  *the_descriptor,
    DS_DESCRIPTOR  *the_next_descriptor,
    unsigned short options,
    void           **addr_list,
    unsigned short width,
    unsigned short number_of_addresses,
    unsigned short is_last
);

void 
DsDescSetGenericCtrl(
     DS_CONTROL_DESCRIPTOR  *the_descriptor,
     DS_DESCRIPTOR  *the_next_descriptor,
     unsigned short options,
     void           *addr,
     unsigned short channel_control,
     unsigned short transfer_information
);

void 
DsDescLink(
     DS_DESCRIPTOR  *the_first_descriptor,       
     DS_DESCRIPTOR  *the_second_descriptor,
     int            is_src_format_2,
     void           **addr_list,
     unsigned short number_of_addresses
);

void 
DsDescModifyFmt1(
     DS_DESCRIPTOR  *the_descriptor,       
     int            field_to_modify,
     int            modification_value,
     int            is_src_format_2,
     int            is_linkable,
     void           **addr_list,
     unsigned short number_of_addresses
);




void 
DsDescChainPrint(
     DS_DESCRIPTOR *the_descriptor,       
     int number_of_links
);

#endif  // NOT_REQUIRED


#ifdef __cplusplus
}
#endif

#endif /* !_DS_DESC_ */
