#ifndef __INC_KDC_HPI_GLOBAL_H__
#define __INC_KDC_HPI_GLOBAL_H__

#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */


#define HPI_VER_MAIN 0
#define HPI_VER_SUB1 1
#define HPI_VER_SUB2 2
#define HPI_VERSION (HPI_VER_MAIN << 16 | HPI_VER_SUB1 << 8 | HPI_VER_SUB2)




/***********************************************************
 *            mark zone definition                         *
 ***********************************************************/

#define HPI_MAGIC_HOST	0x18539427
#define HPI_MAGIC_DSP	0x87492765


/* mkz opration type */
enum _HPIC_MARK_OPR_TYPE{
	_HPIC_RD_REQ = 0,/* HOST read from SLAVE, data directory SLAVE=>HOST */
	_HPIC_RD_FIN,/* HOST finish reading from SLAVE, data directory SLAVE=>HOST */
	_HPIC_WT_REQ,/* write SLAVE, data directory HOST=>SLAVE  */
	_HPIC_WT_FIN,/* writing SLAVE finished, data directory HOST=>SLAVE  */
	_HPIC_IOCTL,
	_HPIC_MARK_OPR_MAX
};

enum _HPIC_BLOCK_OPR_TYPE{	/* the opr for block operation */
	_HPIC_BLOCK_CREATE_CHAN = 0,
	_HPIC_BLOCK_DESTROY_CHAN,
	_HPIC_BLOCK_TEST_LOOP_START,
	_HPIC_BLOCK_TEST_LOOP_STOP,
	_HPIC_BLOCK_CALL_FUNCTION
	
};

/* mkz return value */
typedef enum{
	MKZ_RET_OK = 0,
	MKZ_RET_PRESET = -1,
	MKZ_RET_NENOU = -2	/* data size not enough */
}MKZ_RET;

typedef struct{
#if (__linux__)			/*this is linux(now support PPC(big endian only))*/
/* PPC is big endian, use reversed structure */
	u32 size:12;		/* channel size, unit 16kByte, range 16k-65520kBytes */
	u32 maxiosz:8;		/* max io size, unit 16kbytes, range 16k-4080k */
	u32 cap:2;		/* channel R/W cap */
	u32 type:2;            	/* channel stream/datagram type */
	u32 overwrite:1;	/* channel data overwritable */
	u32 rsv:7;		/* reserved data field */
#else				
/* DSP is little endian */
	u32 rsv:7;		/* reserved data field */
	u32 overwrite:1;	/* channel data overwritable */
	u32 type:2;            	/* channel stream/datagram type */
	u32 cap:2;		/* channel R/W cap */
	u32 maxiosz:8;		/* max io size, unit 16kbytes, range 16k-4080k */
	u32 size:12;		/* channel size, unit 16kByte, range 16k-65520kBytes */
#endif
}mkz_chan_attr;

/* mkz struct */
typedef struct STRUCT_HPIC_MARK_BASE{
	u32 rsv:16;		/* do use the first 16 bit, some time HPI get the first 16 bit wrong*/
	u32 cmd:8;		/* enum _HPIC_MARK_REQ_TYPE */
	u32 chanid:8;		/* channel id */
	union{
		u32 len_req;	/* the length(unit:byte) read/write request  */
		u32 len_ret;	/* the length(read/write request) return */
		int ret;	/* the return value for command operation */

	}
#if (!__linux__)
	retval
#endif
	;
	union{
		u8 * pbuf;
		u32 arg;
	}
#if (!__linux__)
param
#endif
;
	u32 magic;
}_HPIC_MARK_BASE;
#define _HPIC_MARK_BASE_ADDR	0x80000010

/*	for avoiding hpi channel lost packet,
	add the ppc to dsp packet length check,
	dsp will set the length in specical address, and ppc will read it then check
	if check is failed, ppc will re-send the packet again.
	Noted: If PPC re-send the packet twice and the check is not passed, it will return error*/
#define PPC_2_DSP				0x1
#define DSP_2_PPC				0x2
#define HPI_CHECK_PPC_W_LEN 	0x800000a0
#define HPI_CHECK_PPC_R_LEN 	0x800000a4
#define HPI_CHECK_VERSION_ADD	0x800000b0
#define HPI_VERSION_DEBUG		0x20090603


#define _HPI_DEFAULT_CHAN_BUFSZ 0x80000 /* default channel buffer size:512k */
#define _HPI_DEFAULT_CHAN_IOSZ 0x10000 /* default channel max io unit size:64k */
#define _HPI_DEFAULT_BLOCK_MODE 0 /* default block mode , not block */
#define _HPI_DEFAULT_BLOCK_TIMEOUT 100 /* default timeout 1s */

#define _HPIC_MAX_CHAN_NUM 64	/* the max number of HPI channel */

typedef enum _HPIC_IO_CAP_ENUM{
	_HPIC_IO_CAP_RONLY = 0x1,
	_HPIC_IO_CAP_WONLY = 0x2,
	_HPIC_IO_CAP_RW = 0x3
}_HPIC_IO_CAP;

typedef enum _HPIC_IO_TYPE_ENUM{
	_HPIC_IO_TYPE_STREAM = 0,
	_HPIC_IO_TYPE_DATAGRAM
}_HPIC_IO_TYPE;


/* channel sttr */
typedef struct HPI_ATTR{
	u32 size;		/* channel's buffer size */
	u32 maxiosz;		/* channel's max data IO unit size*/
	u32 overwrite;		/* only avail for stream IO */
	u32 cap;		/* _HPIC_IO_CAP */
	u32 type;		/* _HPIC_IO_TYPE */
}hpi_attr;


enum HPI_ERRNO{
	EHPI_NONE = 0,
	EHPI_PERM = 10,
	EHPI_NAVAIL,
	EHPI_NEXIST,
	EHPI_EXIST,
	EHPI_NOMEM,
	EHPI_TIMEOUT,
	EHPI_FULL,
	EHPI_BUFSMALL,
	EHPI_INTERNAL=255
};

#ifdef __cplusplus
}
#endif   /* __cplusplus */


#endif
