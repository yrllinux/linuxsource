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
 *  Module name              : $RCSfile: ds.h,v $   $Revision: 1.2 $
 *
 *  Last update              : $Date: 2003/05/15 19:53:15 $ UTC
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

#if !defined(_DS_)
#define _DS_

/*------------------------------- Includes ----------------------------------*/

#include <etilib/etiTypes.h>
#include <libdev/etiProcessor.h>
#include "libdev/mapca/dscsm_piomap.h"

#ifndef  DS_BRINGUP
#include <libdev/etiDataStreamer.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------- Constants ---------------------------------*/

#if !defined(S_OK)
#define S_OK 0x0
#endif /* S_OK */
#if !defined(S_FALSE)
#define S_FALSE 0x1
#endif /* S_FALSE */

#if !defined(S_TRUE)
#define S_TRUE  (S_FALSE+1)
#endif /* S_TRUE */
#if !defined(S_ERROR)
#define S_ERROR (S_OK-1)	/* Make it negative */
#endif /* S_ERROR */

/*
 * Error codes
*/
#if !defined(STATUS_ETI_ERROR)
#define STATUS_ETI_ERROR(u)   ((u)|0x80100000)
#endif /* STATUS_ETI_ERROR */

/*
 * Error codes for standalone environment
 */
#define E_DS_NOT_SUPPORTED_FEATURE               STATUS_ETI_ERROR( 128 )
#define E_DS_ILLEGAL_ARGUMENT                    STATUS_ETI_ERROR( 129 )
#define E_DS_ILLEGAL_CHANNEL                     STATUS_ETI_ERROR( 130 )
#define E_DS_ILLEGAL_COMMAND                     STATUS_ETI_ERROR( 131 )
#define E_DS_ILLEGAL_PARAMETER                   STATUS_ETI_ERROR( 132 )
#define E_DS_NOT_OPENED_BUFFER                   STATUS_ETI_ERROR( 133 )
#define E_DS_NOT_OPENED_CHANNEL                  STATUS_ETI_ERROR( 134 )
#define E_DS_NOT_OPENED_PATH                     STATUS_ETI_ERROR( 135 )
#define E_DS_OUT_OF_CHANNELS                     STATUS_ETI_ERROR( 136 )
#define E_DS_OUT_OF_BUFFERS                      STATUS_ETI_ERROR( 137 )
#define E_DS_TIMEOUT                             STATUS_ETI_ERROR( 138 )
#define E_DS_RESOURCE_UNAVAILABLE                STATUS_ETI_ERROR( 139 )
#define E_DS_INT_ROUTE_ERROR                     STATUS_ETI_ERROR( 140 )


/*
 * Value to request a default value for a particular member of request.
 */
#define DS_DEFAULT                               0x7fffffff	/* Maximum positive 32-bit integer */

/*
 * Specifies type of descriptor.
 */
#define DS_DESCRIPTOR_FORMAT_1                   0x0
#define DS_DESCRIPTOR_FORMAT_2                   0x1
#define DS_DESCRIPTOR_CONTROL                    0x2

/*
// Control descriptor arg: descAccessMode, add by xiang
*/

#define DS_CTRL_DESC_ARG_DESC_FETCH_MASK	    0x3
#define DS_CTRL_DESC_ARG_DESC_FETCH_NC	        0x0
#define DS_CTRL_DESC_ARG_DESC_FETCH_CNA	        0x1
#define DS_CTRL_DESC_ARG_DESC_FETCH_CA	        0x2
#define DS_CTRL_DESC_ARG_DESC_FETCH_CAW	        0x3


/*
 * Data and Descriptor access modes
 */
#define DS_DESC_DATA_ACCESS_MASK	             0x60
#define DS_DESC_DATA_ACCESS_NC	                 0x00
#define DS_DESC_DATA_ACCESS_CNA	                 0x20
#define DS_DESC_DATA_ACCESS_CA	                 0x40
#define DS_DESC_DATA_ACCESS_CAW	                 0x60

#define DS_ACCESS_MODE_NON_COHERENT              0x0
#define DS_ACCESS_MODE_COHERENT_NO_ALLOCATE      0x1
#define DS_ACCESS_MODE_COHERENT_ALLOCATE         0x2
#define DS_ACCESS_MODE_COHERENT_ALLOCATE_MASKED  0x3

/*
 * Descriptor types
 */
#define DS_DESC_TYPE_MASK			             0x7
#define DS_DESC_TYPE_1				             0x0
#define DS_DESC_TYPE_2				             0x1
#define DS_DESC_TYPE_CTRL			             0x2

/*
 * Descriptor control bits
 * - halt-on-completion bit
 * - inhibitFlush bit
 * - inhibit-prefetch bit
 * - last-descriptor (halt plus no-prefetch)
 */
#define DS_DESC_HALT_BIT_MASK	                 0x8
#define DS_DESC_HALT_BIT                         0x8

#define DS_DESC_INHIBIT_FLUSH_MASK               0x1000
#define DS_DESC_INHIBIT_FLUSH                    0x1000

#define DS_DESC_HALT_MASK	                     (DS_DESC_HALT_BIT_MASK|DS_DESC_INHIBIT_FLUSH_MASK)
#define DS_DESC_HALT	                         (DS_DESC_HALT_BIT|DS_DESC_INHIBIT_FLUSH)

#define DS_DESC_INHIBIT_PREFETCH_MASK	         0x10
#define DS_DESC_INHIBIT_PREFETCH	             0x10

#define DS_DESC_LAST_MASK	                     (DS_DESC_HALT_MASK|DS_DESC_INHIBIT_PREFETCH_MASK)
#define DS_DESC_LAST	                         (DS_DESC_HALT|DS_DESC_INHIBIT_PREFETCH)

/*
 * Macros for Very-backword compatibility (Used in the legacy Vlx driver)
 */
#define DS_FMT1_HALT                             (DS_DESC_TYPE_1 | DS_DESC_HALT)
#define DS_FMT1_LAST                             (DS_DESC_TYPE_1 | DS_DESC_LAST)

/*
 * DsCtrl*() commands
 */
#define DS_CTRL_SET_DESC_ACCESS_MODE             0x0001

#define DS_CTRL_SET_WAY_MASK                     0x0002
#define DS_CTRL_SET_BASE_ADDR                    0x0004
#define DS_CTRL_ADD_BASE_ADDR                    0x0008
#define DS_CTRL_SET_INTRDS0_HANDLER              0x0010
#define DS_CTRL_SET_INTRDS1_HANDLER              0x0020
#define DS_CTRL_SET_INTRBUF_HANDLER              0x0040
#define DS_CTRL_SET_INTRTLB_HANDLER              0x0080
#define DS_CTRL_RESET_CACHE_LOAD_CHANNEL         0x0100
#define DS_CTRL_RESET                            0x0200
#define DS_CTRL_SET_INHIBIT_FLUSH                0x0400
#define DS_CTRL_SET_HALT_POLL_INTERVAL           0x0800          // obsolete
#define DS_CTRL_SET_POLL_INTERVAL                0x0800
#define DS_CTRL_SET_DSBC_SAFE_MODE               0x1000
#define DS_CTRL_SET_DSTLB_STATE                  0x2000
#define DS_CTRL_SET_BRIEF_INIT                   0x4000
#define DS_CTRL_RESET_CHUNK_COUNT                0x8000

#define DS_CTRL_CLEAR_INTRDS0_HANDLER            0x0010000
#define DS_CTRL_CLEAR_INTRDS1_HANDLER            0x0020000
#define DS_CTRL_CLEAR_INTRTLB_HANDLER            0x0040000
#define DS_CTRL_CLEAR_INTRBUF_HANDLER            0x0080000

#define DS_CTRL_SET_BIT_121                      0x40000000
#define DS_CTRL_SET_TLB_MODE                     0x80000000

/*
 * Alignment requirements
 */
#define DS_DESCRIPTOR_ALIGN                      16
#define DS_DESCRIPTOR_FORMAT_2_ARG_ALIGN         16
#define DS_CONTROL_DESCRIPTOR_ALIGN              16

/*
 * Timeout (used for DsWaitHalted)
 */
#define DS_TIMEOUT_INDEFINITE                    0x7fffffffffffffff
#define DS_TIMEOUT_NONE                          0x0


#if !defined(NULL)
#define NULL  Null
#endif


/*--------------------------------- Macros ----------------------------------*/

#define DsGetAddressOfChannel(id, member) &PIO8( DSCSMOffset + DSCSMChannelOffset(id) + DSCSMChannel##member##Offset)

#define ReadDSRegister(address) (*(volatile __uncached__ UInt64 *)(address))

/*------------------------------ Device Types -------------------------------*/

#ifdef ETI_PLATFORM_VxWorks
#include <types/vxTypesOld.h>
#else 
typedef UInt32       UINT32;

typedef Int32        INT32;
typedef UInt16       UINT16;

typedef UInt8        UINT8;
typedef UInt8        TBOOL;
typedef UInt32        BOOL;
#endif

typedef UInt64       UINT64;
typedef Int64        SINT64;
typedef Int64         INT64;

typedef Int          SCODE;
typedef Int32        SINT32;
typedef Int16        SINT16;


typedef UInt32      *PUINT32;
typedef UInt16      *PUINT16;
typedef UInt8       *PUINT8;

/*
 * DS_TIME: Data streamer time value in 27MHz ticks
 */
typedef SINT64        DS_TIME;


/*
 *  DS Identifier
 *  Note:
 *     - DSChannel is actually a pointer (address) to the "Kick" register of a channel.
 *     - DSBuffer value can be used to program an I/O module to tell it what buffer it should talk to.
 */
typedef SINT32        DS_CHANNEL;
typedef UINT32        DS_BUFFER;
typedef UINT32        DS_PATH;



/*
 * DS_CHANNEL_INFO: Data Streamer Channel Information
 *   This structure includes all the attributes a DS channel can have.
*/
typedef struct {
    DS_CHANNEL  id;
    SINT32      transferBurstSize;
    SINT32      minInterburstDelay;
    SINT32      dtsPriority;
    SINT32      channelPriority;
    SINT32      wayMask;
    SINT32      descriptorFetchMode;
    SINT32      tlbMode;                 // 0 for disabled, 1 for enabled, 
    PUINT8      base;
} DS_CHANNEL_INFO, *PDS_CHANNEL_INFO;


/*
 * DS_BUFFER_INFO: Data Streamer Buffer Information
 *   This structure includes all the attributes a DS buffer can have.
 */
typedef struct {
    DS_BUFFER  id;
    PUINT8     start;
    SINT32     bufferSize;
    SINT32     srcChunkSize;
    SINT32     dstChunkSize;
} DS_BUFFER_INFO, *PDS_BUFFER_INFO;


/*
 * DS_PATH_INFO: Data Streamer Path Information
 *   This structure includes all the attributes a DS path can have.
 */
typedef struct {
    DS_PATH          id;
    TBOOL            isSrcIO;
    TBOOL            isDstIO;
    DS_CHANNEL_INFO  srcCh;
    DS_CHANNEL_INFO  dstCh;
    DS_BUFFER_INFO   buf;
    UINT32           flags;
} DS_PATH_INFO, *PDS_PATH_INFO;


/*
 * _DS_DESCRIPTOR_CONTROL: union for specifying 'control' member  of descriptors.
 */
union _DS_DESCRIPTOR_CONTROL {
    struct _bits {
        UINT16 type:             3; /*  2:0 descriptor format type
                                    //        000 format 1
                                    //        001 format 2
                                    //        010 control
                                    */
        UINT16 haltOnCompletion: 1; /*  3     halt at the end of desc. */
        UINT16 inhibitPrefetch:  1; /*  4     inhibit prefetch when halt (used only if haltOnCompletion=1) */
        UINT16 dataAccessMode:   2; /*  6: 5  data read/write mode */
        UINT16 raiseInterrupt0:  1; /*  7     raise interrupt 0 on completion of this desc. */
        UINT16 raiseInterrupt1:  1; /*  8     raise interrupt 1 on completion of this desc.*/
        UINT16 forceData:        1; /*  9     data (0 or 1) to place in output stream */
        UINT16 forceEnable:      1; /* 10     set to force 0's or 1's to output stream */
        UINT16 setSemaphore:     1; /* 11     set to force an extra byte to be written as an EOS marker.  */
        UINT16 inhibitFlush:     1; /* 12     set with halt on source channel to suppress fhlush request. */
        UINT16 PAD13_15:         3; /* 15: 13 unused */
    } bits;
    UINT16 all;
};


/*
 * DS_DESCRIPTOR: Data Streamer Descriptor
 *   Note:
 *     - There are three types, i.e. format 1, 2, and control.
 *     - The DS_DESCRIPTOR type defines basically the format 1.
 *       A format 2 descriptor needs an array of DS_DESCRIPTOR_FORMAT_2_ARG
 *       pointed by "next" of the descriptor. The array consists of
 *       up to 16535/4 DS_DESCRIPTOR_FORMAT_2_ARGs.
 *     - A separate type called DS_CONTROL_DESCRIPTOR is defined.
 *       Typecast from DS_DESCRIPTOR to DS_CONTROL_DESCRIPTOR is legitimate.
*/
typedef struct _DS_DESCRIPTOR {
    struct _DS_DESCRIPTOR         *next;
    UINT16                         count;
    union _DS_DESCRIPTOR_CONTROL   control;
    SINT16                         pitch;
    UINT16                         width;
    PUINT8                         address;
} __attribute__((aligned(16))) DS_DESCRIPTOR, *PDS_DESCRIPTOR;


/*
 * DS_DESCRIPTOR_FORMAT_2_ARG: Argument of format 2 descriptor
 *   Note:
 *     - An array of objects of this type is pointed by "next"
 *       of a format 2 descriptor.
 */
typedef struct _DS_DESCRIPTOR_FORMAT_2_ARG {
    PUINT8 address[4];
} DS_DESCRIPTOR_FORMAT_2_ARG, *PDS_DESCRIPTOR_FORMAT_2_ARG;


/*
 * DS_CONTROL_DESCRIPTOR: Control descriptor
 *   Note:
 *     - This type is compatible (i.e. "typecast"able) to DS_DESCRIPTOR
 *      with respect to "next" and "control" members.
 *
*/
union _DS_CONTROL_DESCRIPTOR_COMMAND {
    struct {
        UINT16 setBaseAddr:      1; /*  0      change base address */
        UINT16 addBaseAddr:      1; /*  1      add to the current base */
        UINT16 setDescFetchMode: 1; /*  2      change descriptor fetch mode */
        UINT16 setWayMask:       1; /*  3      change way mask */
        UINT16 PAD1504:         12; /* 15: 4   unused */
    } bits;
    UINT16 all;
};


union _DS_CONTROL_DESCRIPTOR_ARG {
    struct {
        UINT16 descAccessMode:   2; /*  1: 0 descriptor read mode */
        UINT16 unused0:          2; /*  3: 2 unused */
        UINT16 wayMask:          4; /*  7: 4 way mask (used only if setWayMask=1) */
        UINT16 unused1:          8; /* 15: 8 unused */
    } bits;
    UINT16 all;
};


typedef struct _DS_CONTROL_DESCRIPTOR {
    struct _DS_DESCRIPTOR                 *next;
    union _DS_CONTROL_DESCRIPTOR_COMMAND   command;
    union _DS_DESCRIPTOR_CONTROL           control;
    union _DS_CONTROL_DESCRIPTOR_ARG       arg;
    UINT16                                 unused;
    PUINT8                                 address;
} __attribute__((aligned(16))) DS_CONTROL_DESCRIPTOR, *PDS_CONTROL_DESCRIPTOR;


/*
 * DS_CHANNEL_STATUS: Data Streamer Channel Status
 */
typedef struct {
    UINT32          halted:  1;		/* 1 if the channel is halted */
    UINT32          intrDS0: 1;		/* 1 if intrDS0 is raised */
    UINT32          intrDS1: 1;		/* 1 if intrDS1 is raised */
    UINT32          reserved: 29;	/* (reserved, always 0) */
    UINT32          continueCount;	/* Pending continue count */
    UINT32          wayMask;		/* Way mask */
    UINT32          descAccessMode;	/* Descriptor fetch mode */
    PUINT8          baseAddr;		/* Base address */
    PUINT8          dataAddr;		/* Data address channel is working around */
    DS_DESCRIPTOR  *nextDescAddr;   /* Next descriptor address */
} DS_CHANNEL_STATUS, *PDS_CHANNEL_STATUS;
    

/*
 * DS_BUFFER_STATUS: Data Streamer Channel Status
 */
typedef struct {
    UINT32 intrBuf;
    UINT32 inputCount;		/* input byte count since last output */
    UINT32 outputCount;		/* output byte count since last input */
} DS_BUFFER_STATUS, *PDS_BUFFER_STATUS;

/*
// DsStatChannel() commands
*/
#define DS_STAT_CHANNEL_HALTED              0x001
#define DS_STAT_CHANNEL_INTR_DS0            0x002
#define DS_STAT_CHANNEL_INTR_DS1            0x004
#define DS_STAT_CHANNEL_CONTINUE_COUNT		0x008
#define DS_STAT_CHANNEL_WAY_MASK            0x010
#define DS_STAT_CHANNEL_DESC_ACCESS_MODE	0x020
#define DS_STAT_CHANNEL_BASE_ADDR           0x040
#define DS_STAT_CHANNEL_DATA_ADDR           0x080
#define DS_STAT_CHANNEL_NEXT_DESC_ADDR 		0x100

#define DS_STAT_CHANNEL_ALL (DS_STAT_CHANNEL_HALTED |           \
                             DS_STAT_CHANNEL_INTR_DS0 |		\
                             DS_STAT_CHANNEL_INTR_DS1 |		\
                             DS_STAT_CHANNEL_CONTINUE_COUNT |	\
                             DS_STAT_CHANNEL_WAY_MASK |		\
                             DS_STAT_CHANNEL_DESC_ACCESS_MODE |	\
                             DS_STAT_CHANNEL_BASE_ADDR |	\
                             DS_STAT_CHANNEL_DATA_ADDR |	\
                             DS_STAT_CHANNEL_NEXT_DESC_ADDR)



/*------------------------------- API Functions -----------------------------*/

SCODE DsOpenPath( DS_PATH_INFO *request, DS_PATH_INFO *actual );

SCODE DsCtrlPath( DS_PATH Path, UINT32 Cmd, void *Arg ); /* add by xiang, only DS_CTRL_RESET supported */

SCODE DsClosePath( DS_PATH path );

SCODE DsStopChannel(DS_CHANNEL channel); /* add by xiang */

SCODE DsCtrlChannel( DS_CHANNEL Ch, UINT32 Cmd, void *Arg );

UINT32 DsStatChannel( DS_CHANNEL Ch, UINT32 Cmd, DS_CHANNEL_STATUS *St );

Int DsGetChannelIndexFromDS_CHANNEL(DS_CHANNEL channel);

Int DsGetBufferIndexFromDS_BUFFER(Int id);

#define DsCheckHalted(c)            _DsCheckHalted(c)

SCODE _DsCheckHalted( DS_CHANNEL channel );

#ifdef  DS_BRINGUP
#define DsKick(c,a)                 _DsKick(c,a)

#define DsContinue(c)               _DsContinue(c)

#define DsWaitHalted(c,a)           _DsWaitHalted(c,(DS_TIME)a)

#define DsWaitHaltedNoTimeout(c)    _DsWaitHaltedNoTimeout(c)

SCODE _DsKick( DS_CHANNEL channel, DS_DESCRIPTOR *desc );

SCODE _DsContinue( DS_CHANNEL channel );

SCODE _DsWaitHalted( DS_CHANNEL channel, DS_TIME timeout );

SCODE _DsWaitHaltedNoTimeout( DS_CHANNEL channel );
#else
#define DsKick(c,a)                 (dsStartLw((Int32) c, (dsDescriptor_t *) a), 0)
#define _DsKick(c,a)                (dsStartLw((Int32) c, (dsDescriptor_t *) a), 0)

#define DsContinue(c)               (dsContinueLw((Int32) c), 0)
#define _DsContinue(c)              (dsContinueLw((Int32) c), 0)

#define DsWaitHalted(c,a)           (dsChannelWaitHalted((Int32) c), 0)
#define _DsWaitHalted(c,a)          (dsChannelWaitHalted((Int32) c), 0)

#define DsWaitHaltedNoTimeout(c)    (dsChannelWaitHalted((Int32) c), 0)
#define _DsWaitHaltedNoTimeout(c)   (dsChannelWaitHalted((Int32) c), 0)
#endif 

// New "legacy-type" function to reset a datastreamer path.
SCODE DsResetPath( DS_PATH_INFO * pathInfo );

// New "legacy-type" function to open path with interrupt handlers.
SCODE DsOpenPathWithInterrupts( DS_PATH_INFO *request, DS_PATH_INFO *actual, 
                                void * srcDs0Handler, void * srcDs0Data, void * srcDs1Handler, void * srcDs1Data,
                                void * dstDs0Handler, void * dstDs0Data, void * dstDs1Handler, void * dstDs1Data
                                );                                

// Bringup debug function
void DsDebugDumpChannels(channel);


/*------------------------------- Unsupported -------------------------------*/
/*
 * Legacy Types/Functions that are either not required or supported.
 */ 
#ifdef NOT_REQUIRED

//#include "ds_bit.h"
#include "ds_desc_api.h"

/*----- Constants ----- */
/* 
 * Settings for tlbMode
 * NOTE: Choose arbitrary value for the disabled setting so it can be distinguished from failure to set the field.
*/
#define DS_CHAN_DSTLB_ENABLED                    1
#define DS_CHAN_DSTLB_DISABLED                  (0x7B4506CA)


#define DS_PATH_FLAG_CACHE_LOAD_PATH            0x01
#define DS_PATH_FLAG_FORCE_ZERO_PATH            0x02
#define DS_PATH_FLAG_FORCE_ONE_PATH             0x04
#define DS_PATH_FLAG_IOCHAN_PRIORITY_OVERRIDE   0x08
#define DS_PATH_FLAG_MASK                       0x0f

/*
 * DsCtrlInterrupts settings
 */

/* intClass selection
 */
#define DSINT_DS0 0
#define DSINT_DS1 1
#define DSINT_TLB 2
#define DSINT_BUF 3

/* intRoute selection
 * 
 * NOTE: CORE gets mapped to CORE3 to get consecutive
 * numbers for better error detection.
 */
#define DS_CTRL_INTR_SET_CORE   ETISYS_INT_TARGET_CORE3
#define DS_CTRL_INTR_SET_PCIAA  ETISYS_INT_TARGET_PCIAA
#define DS_CTRL_INTR_SET_PCIAB  ETISYS_INT_TARGET_PCIAB
#define DS_CTRL_INTR_CLEAR      (DS_CTRL_INTR_SET_PCIAB+1)

/* NOTE: Define COREx symbols to placate code still using
 * the obsolete DsCtrlInterrupts() call.
 * Use DsRouteInterrupt() instead, or use EtiSys routines
 * for complete control over core interrupt assignment
 * and usage.
 */
#define DS_CTRL_INTR_SET_CORE0   ETISYS_INT_TARGET_CORE3
#define DS_CTRL_INTR_SET_CORE1   ETISYS_INT_TARGET_CORE3
#define DS_CTRL_INTR_SET_CORE2   ETISYS_INT_TARGET_CORE3
#define DS_CTRL_INTR_SET_CORE3   ETISYS_INT_TARGET_CORE3

/* intDefault selection
 * Specify the behavior if an interrupt comes in for a 
 * channel or buffer with no handler
 */
#define DSINT_DEFHAND_IGNORE    0
#define DSINT_DEFHAND_PRINT     1
#define DSINT_DEFHAND_USER_RTN  2


/*
// DsStatBuffer() commands
*/
#define DS_STAT_BUFFER_INTR_BUF             0x00000001
#define DS_STAT_BUFFER_ALL                 (0x40000000|DS_STAT_BUFFER_INTR_BUF)
#define DS_STAT_IO_BUFFER_BYTES_IN_BUFFER  (0x80000000|0x00000001)


/*
//
// Debug setting
//   DS_DEBUG_LEVEL: 0 or 3(default)
//   DS_DEBUG_Ds*: DS_DEBUG_ARG_VALIDATION(level) and/or
//                 DS_DEBUG_STATUS_VALIDATION(level) and/or
//                 DS_DEBUG_DESC_VALIDATION(level)
//
//   Internal macros:
//     DS_DEBUG_ZERO: DS_DEBUG_ARG_VALIDATION(0)|
//                    DS_DEBUG_STATUS_VALIDATION(0)|
//                    DS_DEBUG_DESC_VALIDATION(0)
*/
#define DS_DEBUG_ARG_VALIDATION(level)		((0x01)<<level)
#define DS_DEBUG_STATUS_VALIDATION(level)	((0x08)<<level)
#define DS_DEBUG_DESC_VALIDATION(level)		((0x40)<<level)

#define DS_DEBUG_ZERO (DS_DEBUG_ARG_VALIDATION(0) |\
                       DS_DEBUG_STATUS_VALIDATION(0)|\
                       DS_DEBUG_DESC_VALIDATION(0))

#if !defined(DS_DEBUG_LEVEL)
#  if defined(_hmpv_)
#    define DS_DEBUG_LEVEL 0
#  else
#    define DS_DEBUG_LEVEL 3 /* Prevent inline calls from being included when
                                non-hmpv programs (e.g. WIMSIM) are built */
#  endif
#endif

#if !defined(DS_DEBUG_Ds_ALL)
  #define DS_DEBUG_Ds_ALL DS_DEBUG_ARG_VALIDATION(DS_DEBUG_LEVEL)|\
                          DS_DEBUG_STATUS_VALIDATION(DS_DEBUG_LEVEL)|\
                          DS_DEBUG_DESC_VALIDATION(DS_DEBUG_LEVEL)
#endif


#if !defined(DS_DEBUG_DsGetVersion)
  #define DS_DEBUG_DsGetVersion (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsClosePath)
  #define DS_DEBUG_DsClosePath (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsContinue)
  #define DS_DEBUG_DsContinue (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsStopChannel)
  #define DS_DEBUG_DsStopChannel (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsStopPath)
  #define DS_DEBUG_DsStopPath (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsCtrlBuffer)
  #define DS_DEBUG_DsCtrlBuffer (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsCtrlChannel)
  #define DS_DEBUG_DsCtrlChannel (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsCheckHalted)
  #define DS_DEBUG_DsCheckHalted (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsDebugSelect)
  #define DS_DEBUG_DsDebugSelect (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsKick)
  #define DS_DEBUG_DsKick (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsOpenPath)
  #define DS_DEBUG_DsOpenPath (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsStatBuffer)
  #define DS_DEBUG_DsStatBuffer (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsStatChannel)
  #define DS_DEBUG_DsStatChannel (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsWaitHalted)
  #define DS_DEBUG_DsWaitHalted (DS_DEBUG_Ds_ALL)
#endif
#if !defined(DS_DEBUG_DsWaitStopped)
  #define DS_DEBUG_DsWaitStopped (DS_DEBUG_Ds_ALL)
#endif

/*
 * Interrupt handler info structures
 */
typedef void (*DS_INTERRUPT_HANDLER)(int index, void *usr_data);

typedef struct {
    DS_INTERRUPT_HANDLER  handler;
    void                 *usr_data;
    int                   priority;
} DS_INTR_HANDLER_INFO, *PDS_INTR_HANDLER_INFO;


/*----- Macros -----*/
/*
// Id-index conversions
*/
#if 0 // REPLACE WITH FUNCTIONS
#define DsGetChannelIndexFromDS_CHANNEL(id) \
  ((SINT32)((((UINT32)(id))&(((UINT32)(DSCSMSize))-1))/DSCSMChannelElSize))
#define DsGetBufferIndexFromDS_BUFFER(id) ((SINT32)(id))
#endif

/*
// Index-id conversions
*/
#if 0 // REPLACE WITH FUNCTIONS
#define DsGetDS_CHANNELFromChannelIndex(idx) \
  ((DS_CHANNEL)(DsBaseAddress + DSCSMChannelOffset(idx)))
#define DsGetDS_BUFFERFromBufferIndex(idx) ((DS_PATH)(idx))
#endif

/*----- API -----*/
#if !defined(FUNC_GetLastError)
SCODE GetLastError( void );
#define FUNC_GetLastError
#define NEED_FUNC_GetLastError
#endif /* GetLastError */


void * DsMalloc( unsigned int numbytes );
void * DsCalloc( unsigned int nmemb, unsigned int size );
void   DsFree( void *ptr );

#if !defined(FUNC_malloc)
void * malloc( unsigned int numbytes );
void * calloc( unsigned int nmemb, unsigned int size );
void   free( void *ptr );
#define NEED_FUNC_malloc
#endif /* malloc */

UINT32 DsGetVersion( void );
SCODE DsCheckBuffer( DS_BUFFER Buf, UINT32 Cmd, void *Arg );
SCODE DsCtrlBuffer( DS_BUFFER Ch, UINT32 Cmd, void *Arg );


SCODE DsCtrlGlobalSettings( UINT32 Cmd, void *Arg );

/* NOTE: This routine is obsolete. Use DsRouteInterrupt instead
 */
SCODE DsCtrlInterrupts( int DS0Route, int DS1Route, int DSBUFRoute, int DSTLBRoute );
SCODE DsRouteInterrupt( int intClass, int intRoute, int intDefault, DS_INTERRUPT_HANDLER defaultHandler );
/* SCODE DsCtrlPath( DS_PATH Path, UINT32 Cmd, void *Arg ); xiang */
SCODE DsResetPath( DS_PATH Path );
UINT32 DsStatBuffer( DS_BUFFER Buf, UINT32 Cmd, DS_BUFFER_STATUS *St );

#endif // NOT_REQUIRED

/*----------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* !_DS_ */
