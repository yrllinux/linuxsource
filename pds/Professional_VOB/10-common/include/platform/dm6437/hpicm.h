#ifndef __INC_KDC_HPI_COMMUNICATION_H__
#define __INC_KDC_HPI_COMMUNICATION_H__

#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */



/*****************************************************
 *ENUM definition
 *****************************************************/
typedef enum{
	HPICM_EVENT_CHAN_CREATE,
	HPICM_EVENT_CHAN_DESTROY,
	HPICM_EVENT_CHAN_FLUSH
}HPICM_EVENT_T;

typedef enum
{
	PPC_TO_DSP = 0,
	DSP_TO_PPC
}TRANS_DIR_T;

typedef enum
{
	STAT_READ = 0,
	STAT_WRITE 
}TRANS_MODE_T;

/* hpi communication init/exit API */
/*initialize hpicm module, sedid is the SEDID of memory which hpicm use */
l32 hpicm_init(l32 segid);

/*destroy hpicm module*/
l32 hpicm_exit(void);
/* data io API */

/*request some memory to write*/
l32 hpicm_write_req(l32 chanid, u8 ** data, u32 len);
/*notify hpicm that write operation finished*/
l32 hpicm_write_fin(l32 chanid, u32 len);

/*request some content to read*/
l32 hpicm_read_req(l32 chanid, u8 ** data, u32 * len);

/*notify hpicm that read operation finished*/
l32 hpicm_read_fin(l32 chanid, u32 len);

int hpicm_rxchnl_data_num_get(l32 chanid, u32 * num);

/* HOST->DSP event API */
int hpicm_set_event_cb(HPICM_EVENT_T event, Fxn cb_routine, void * data);

/* LOG API */
l32 hpicm_log(s8 * fmt, ...);
l32 hpicm_klog(s8 * fmt, ...);
/* dump API */
l32 hpicm_mem_dump_set(u32 index, u32 value);

l32 hpicm_func_register(u8 id, Fxn routine, void * data, l32 force);

int hpi_get_stats(u32 chan, u32 direction, u32 mode, u32 *byte_count, u32 *times, u32 *error_times, u32 *error_type);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif



