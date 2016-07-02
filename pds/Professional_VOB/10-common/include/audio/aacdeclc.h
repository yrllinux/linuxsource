#ifndef __AACDEC_H__
#define __AACDEC_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "algorithmtype_ti.h"
#define MAX_CHANNELS        2
#define MAX_SYNTAX_ELEMENTS 48
#define MAX_WINDOW_GROUPS    8
#define MAX_SFB             51
typedef struct NeAACDecFrameInfo
{
     u32 bytesconsumed;
     u32 samples;
     u32 samplerate;
     u8 channel_position[8];
     u8 channels;
     u8 error;    
     u8 sbr;
    /* MPEG-4 ObjectType */
     u8 object_type;

    /* AAC header type; MP4 will be signalled as RAW also */
    u8 header_type;

    /* multichannel configuration */
    u8 num_front_channels;
    u8 num_side_channels;
    u8 num_back_channels;
    u8 num_lfe_channels;
    
    
} NeAACDecFrameInfo;

typedef struct NeAACDecConfiguration
{   
    /*unsigned int*/ u32 defSampleRate;
    /*u8*/ u8 defObjectType;
    /*u8*/ u8 outputFormat;
    /*u8*/ u8 downMatrix;
    /*u8*/ u8 useOldADTSFormat;
    /*u8*/ u8 dontUpSampleImplicitSBR;
} NeAACDecConfiguration, *NeAACDecConfigurationPtr;

typedef struct
{
    u16 syncword;
    u16 aac_frame_length;
    u16 adts_buffer_fullness;
    u16 crc_check;
    u8 id;
    u8 layer;
    u8 protection_absent;
    u8 profile;
    u8 sf_index;
    u8 private_bit;
    u8 channel_configuration;
    u8 original;
    u8 home;
    u8 emphasis;
    u8 copyright_identification_bit;
    u8 copyright_identification_start;  
    u8 no_raw_data_blocks_in_frame;   
    /* control param */
    u8 old_format;
} adts_header;  //9 bytes

typedef struct _bitfile
{
    /* bit input */
    u32 bufa;
    u32 bufb;
    u32 bits_left;
    u32 buffer_size; /* size of the buffer in bytes */
    u32 bytes_used;   
    u32 *tail;
    u32 *start;
    void *buffer;
    u8 no_more_reading;
    u8 error;
} bitfile;

typedef struct
{
    l32  ctrl1;
    l32  ctrl2;        
    u8 band_top[17];
    u8 pce_instance_tag;
    u8 excluded_chns_present;
    u8 prog_ref_level;
    u8 dyn_rng_sgn[17];
    u8 present;
    u8 exclude_mask[MAX_CHANNELS];   
    u8 dyn_rng_ctl[17];
    u8 num_bands;
    u8 additional_excluded_chns[MAX_CHANNELS];    
} drc_info;

typedef l32 complex_t[2];

typedef struct
{   
    complex_t work[512];
    complex_t *tab;
    u16 n;
    u16 ifac[15];
} cfft_info;

typedef struct {
	complex_t Z1[512];
    cfft_info cfft;
    complex_t *sincos;
    u16 N;
} mdct_info;

typedef struct
{
    const l32 *long_window[2];
    const l32 *short_window[2];
    mdct_info  mdct256;
    mdct_info  mdct2048;
} fb_info;

typedef struct
{
    u8 front_element_is_cpe[16];
    u8 front_element_tag_select[16];
    u8 side_element_is_cpe[16];
    u8 side_element_tag_select[16];
    u8 back_element_is_cpe[16];
    u8 back_element_tag_select[16];
    u8 lfe_element_tag_select[16];
    u8 assoc_data_element_tag_select[16];
    u8 cc_element_is_ind_sw[16];
    u8 valid_cc_element_tag_select[16];
    u8 sce_channel[16];
    u8 cpe_channel[16];
    u8 comment_field_data[257];
    u8 element_instance_tag;
    u8 object_type;
    u8 sf_index;
    u8 num_front_channel_elements;
    u8 num_side_channel_elements;
    u8 num_back_channel_elements;
    u8 num_lfe_channel_elements;
    u8 num_assoc_data_elements;
    u8 num_valid_cc_elements;
    u8 mono_mixdown_present;
    u8 mono_mixdown_element_number;
    u8 stereo_mixdown_present;
    u8 stereo_mixdown_element_number;
    u8 matrix_mixdown_idx_present;
    u8 pseudo_surround_enable;
    u8 matrix_mixdown_idx;    
    u8 channels;
    u8 comment_field_bytes;
    /* extra added values */
    u8 num_front_channels;
    u8 num_side_channels;
    u8 num_back_channels;
    u8 num_lfe_channels;
    
} program_config;

typedef struct
{
    u8 pulse_offset[4];
    u8 pulse_amp[4];
    u8 number_pulse;
    u8 pulse_start_sfb;
    
} pulse_info;

typedef struct
{
    u8 n_filt[8];
    u8 coef_res[8];
    u8 length[8][4];
    u8 order[8][4];
    u8 direction[8][4];
    u8 coef_compress[8][4];
    u8 coef[8][4][32];
} tns_info; 

typedef struct
{
    u16 sect_start[8][15*8];
    u16 sect_end[8][15*8];
    u16 sect_sfb_offset[8][15*8];
    u16 swb_offset[52];
    s16 scale_factors[8][51]; /* [0..255] */
    u8 sect_cb[8][15*8];   
    u8 sfb_cb[8][8*15];
    u8 num_sec[8]; /* number of sections in a group */
    u8 window_group_length[8];
    u8 ms_used[MAX_WINDOW_GROUPS][MAX_SFB];
    u8 max_sfb;
    u8 num_swb;
    u8 num_window_groups;
    u8 num_windows;
    u8 window_sequence;   
    u8 window_shape;
    u8 scale_factor_grouping;     
    u8 global_gain;   
    u8 ms_mask_present;    
    u8 noise_used;
    u8 pulse_data_present;
    u8 tns_data_present;
    u8 gain_control_data_present;
    u8 predictor_data_present;
    pulse_info pul;
    tns_info tns;    
 } ic_stream; /* individual channel stream */

typedef struct
{   
    ic_stream ics1;
    ic_stream ics2;
    s16 paired_channel;
    u8 channel;
    u8 element_instance_tag;
    u8 common_window;    
} element; /* syntax element (SCE, CPE, LFE) */

typedef struct
{
    l32 BStrLength;
    u32 frame;      
    u8 outtime[MAX_CHANNELS][4096];
    u8 intermed[MAX_CHANNELS][4096];    
	l32 temp[2*1024];
	l32 transf_buf[2*1024] ;
	s16 spec_data1[1024] ;
    s16 spec_data2[1024] ;
    l32 *time_out[MAX_CHANNELS];
    l32 *fb_intermed[MAX_CHANNELS];
    void *sample_buffer; 
    /* element_output_channels:
       determines the number of channels the element will output
    */
    u8 element_output_channels[MAX_SYNTAX_ELEMENTS];
    /* element_alloced:
       determines whether the data needed for the element is allocated or not
    */
    u8 element_alloced[MAX_SYNTAX_ELEMENTS];
    /* output data buffer */
    u8 window_shape_prev[MAX_CHANNELS];  
    u8 element_id[MAX_CHANNELS];
    u8 internal_channel[MAX_CHANNELS];
    u16 frameLength;  
     /* alloced_channels:
       determines the number of channels where output data is allocated for
    */
    u8 alloced_channels;
    /* number of elements in current frame */
    u8 fr_ch_ele;
    u8 adts_header_present;
    u8 adif_header_present;     
    u8 sf_index;
    u8 object_type;
    u8 channelConfiguration;    
    /* Program Config Element */
    u8 pce_set;    
    /* Configuration data */  
    u8 postSeekResetFlag;
    u8 downMatrix;     
    u8 upMatrix;
    u8 first_syn_ele;
    u8 has_lfe;
    /* number of channels in current frame */
    u8 fr_channels;
            
    // struct variable
    NeAACDecFrameInfo tFrameInfo;	
    NeAACDecConfiguration config; 
    adts_header adts;
    bitfile ld;
    fb_info  fb;
    drc_info drc;
    program_config pce;
    element chele;
  
} NeAACDecStruct, *NeAACDecHandle;

l32 AACDecInit(void *DecHandle) ;
l32 NeAACDecInit(NeAACDecHandle hDecoder, u8 *buffer, u32 buffer_size );  
l32 NeAACDecInit_ti(NeAACDecHandle hDecoder, u8 * InBuf );  
l32 AACDecLc(void *DecHandle, u8 * InBuf, u8 *OutBuf);
l32 AACDecLc_ti(void *DecHandle,  u8 * InBuf, u8 * OutBuf);
void  GetAacdeclcVersion(s8 *Version, l32 StrLen, l32 * StrLenUsed);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

