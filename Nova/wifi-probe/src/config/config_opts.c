#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "common_types.h"
#include "config_intf.h"
#include "wifiprobe.h"
#include "ap_log.h"

#define CONFIG_QUEUE_NUMS	20
#define DEFAULT_CONFIG_FILE_PATH	"/tmp/probeconfig.ini"
#define DEFAULT_WIFI_IF		"ath0"

typedef struct config_cmd{
	char * name;
	void (*f)(char *);
}CONFIG_CMD;

extern WIFI_PROBE_T gWifiProbe;

unsigned int g_uiChanMask = 0;
extern char g_acSniffDevName[16];	/* 设备名称最大长度为16 */


void get_hwversion(char *hwver, int len)
{
	if(NULL == hwver)
		return ;

	strcpy(hwver, "hw1.0");
	//memset(hwver,0x55,len);
}

void get_swversion(char *hwver, int len)
{
	if(NULL == hwver)
		return;

	
	strcpy(hwver, "sw2.0");
	//memset(hwver,0x33,len);
}

UINT32 get_mac_report_time(void)
{
	UINT32 reporttime;
	
	OSAL_MutSemTake(configDB.dblock);
	reporttime = configDB.report_time_per_mac;
	OSAL_MutSemGive(configDB.dblock);
	return reporttime;
}
	
void set_mac_report_time(UINT32 reporttime)
{
	OSAL_MutSemTake(configDB.dblock);
	configDB.report_time_per_mac = reporttime;
	OSAL_MutSemGive(configDB.dblock);
}

void set_scan_channel(UINT32 channelMask)
{
	OSAL_MutSemTake(configDB.dblock);
	
	if(configDB.scan_ch_mask != channelMask)
	{
		configDB.settingchange = 1;
		configDB.scan_ch_mask = channelMask;
	}
	
	OSAL_MutSemGive(configDB.dblock);
}

void set_scan_swtime(UINT32 swtime)
{
	OSAL_MutSemTake(configDB.dblock);
	if(configDB.ch_switch_time != swtime)
	{
		configDB.settingchange = 1;
		configDB.ch_switch_time = swtime;
	}
	
	OSAL_MutSemGive(configDB.dblock);
}

BOOL is_setting_change_and_clear(void)
{
	BOOL ret = FALSE;
	OSAL_MutSemTake(configDB.dblock);
	if(configDB.settingchange == 1)
	{
		configDB.settingchange = 0;
		ret = TRUE;
	}
	OSAL_MutSemGive(configDB.dblock);
	return ret;
}

UINT32 get_channel_caps(void)
{
	return DEFAULT_SCAN_CH;
}

void set_auto_scan_ch_mask(UINT32 channel)
{
	OSAL_MutSemTake(configDB.dblock);
	configDB.auto_scan_ch_mask |= (1 << channel);
	OSAL_MutSemGive(configDB.dblock);
}

void zero_auto_scan_ch_mask(void)
{
	OSAL_MutSemTake(configDB.dblock);
	configDB.auto_scan_ch_mask = 0;
	OSAL_MutSemGive(configDB.dblock);
}

UINT32 get_auto_scan_ch_mask(void)
{
	UINT32 auto_chmask=0;
	OSAL_MutSemTake(configDB.dblock);
	auto_chmask = configDB.auto_scan_ch_mask;
	OSAL_MutSemGive(configDB.dblock);
	return auto_chmask;
}

static const char meta[] = "\000123456 \b\t\n78\r90abcdefABCDE\033F:,.";

static char hide_meta(char c)
{
  unsigned int i;

  for (i = 0; i < (sizeof(meta) - 1); i++)
    if (c == meta[i])
      return (char)i;
  
  return c;
}


static int parse_mac_format(char * string, char *mac)
{
	char *start, *next;
	int i;
	
	if((NULL == string) || (NULL == mac))
		return -1;

	memset(mac, 0, 6);
	
	start = string;
	for(i = 0; i<6; i++){
		
		next = strchr(start, ':');
		if(next)
		{
			start[next - start]=0;
			next++;
		}
		
		mac[i] = strtol(start, NULL, 16);
		//printf("mac=%x\n", mac[i]);
		start = next;
	}
	

	return 1;
}

static void cfg_parse_mac(char *string)
{
	int i;
	
	if(NULL == string)
		return ;
	
	parse_mac_format(string, gWifiProbe.config.probe_fixed_mac[gWifiProbe.config.probe_fixed_mac_nums].mac);
	printf("%02x:%02x:%02x:%02x:%02x:%02x\n",gWifiProbe.config.probe_fixed_mac[gWifiProbe.config.probe_fixed_mac_nums].mac[0],
					gWifiProbe.config.probe_fixed_mac[gWifiProbe.config.probe_fixed_mac_nums].mac[1],\
					gWifiProbe.config.probe_fixed_mac[gWifiProbe.config.probe_fixed_mac_nums].mac[2],\
					gWifiProbe.config.probe_fixed_mac[gWifiProbe.config.probe_fixed_mac_nums].mac[3],\
					gWifiProbe.config.probe_fixed_mac[gWifiProbe.config.probe_fixed_mac_nums].mac[4],\
					gWifiProbe.config.probe_fixed_mac[gWifiProbe.config.probe_fixed_mac_nums].mac[5]);
	
	gWifiProbe.config.probe_fixed_mac_nums++;

	printf("config : %d\n", gWifiProbe.config.probe_fixed_mac_nums);

}

static void cfg_parse_channel(char *string)
{
	char *start,*next, *next2;
	unsigned chn_mask =0;
	int chn,chn2;
	int i;
	
	if(NULL == string)
		return ;

	//printf("%s\n",string);


	start = string;

	while(start){
		
		next = strchr(start, ',');
		if(next){
			start[next - start]=0;
			next++;
		}

		/*parse -*/
		next2 = strchr(start, '-');
		if(next2)
		{
			start[next2 - start]=0;
			chn = atoi(start);
			next2++;
			chn2 = atoi(next2);
		}
		else{
			chn = atoi(start);
			chn2 =chn;
		}
			

		//printf("ch1=%d, chn2=%d\n", chn,chn2);
		if((chn!=0) && (chn2!=0) && (chn2>=chn)){
			for(i = 0; i<= (chn2-chn); i++)
				chn_mask |= 1 << (i + chn -1);
		}
		chn2 = 0;
		chn = 0;
		start = next;
	}

	gWifiProbe.config.chn_mask = chn_mask;
	
	printf("chn_mask=%x\n", gWifiProbe.config.chn_mask);
	
}

static void cfg_parse_swtime(char *string)
{
	if(NULL == string)
		return ;

	gWifiProbe.config.chn_scan_time = atoi(string);

	printf("channel switch time=%d\n", gWifiProbe.config.chn_scan_time);

}

static void cfg_parse_qqport(char *string)
{
	char *start,*next;
	int qqport;
	int qqnums=0;
	
	if(NULL == string)
		return ;

	//printf("%s\n",string);

	start = string;

	while(start){
		next = strchr(start, ',');
		if(next){
			start[next - start]=0;
			next++;
		}

		qqport = atoi(start);
		if(qqport !=0 ){
			gWifiProbe.config.qqport[qqnums] = qqport;
			//printf("qqport =%d\n",gProbeConfig.qqport[qqnums]);
			qqnums++;
		}

		if(qqnums >= MAX_QQ_PORTS)
			break;
		start = next;
	}
	
}

static void cfg_parse_slottime(char *string)
{
	if(NULL == string)
		return ;
	
	gWifiProbe.config.report_time = atoi(string);
	printf("mac catch slot time =%d\n", gWifiProbe.config.report_time);
}


CONFIG_CMD gConfigCmd[]={
	{"MAC", cfg_parse_mac},
	{"CHANNEL", cfg_parse_channel},
	{"SWTIME", cfg_parse_swtime},
	{"QQPORT", cfg_parse_qqport},
	{"QQPORT", cfg_parse_qqport},	
	{"SLOTTIME", cfg_parse_slottime},
	{NULL, NULL}
};

int read_cfg_file(char* file)
{
  int lineno = 0;
  FILE *fd;
  char *errmess, *p, *arg = NULL, *start;
  size_t len;
  int white, i;
  char buff[512];

  if(NULL == file)
  	return -1;

  gWifiProbe.config.probe_fixed_mac_nums = 0;
  
  if ((fd = fopen(file, "r")) != NULL)
  {
	  memset(buff, 0, sizeof(buff));
	  while (fgets(buff, sizeof(buff), fd)){
		  lineno++;
		  
		  for (white = 1, p = buff; *p; p++)
		  {
				if (*p == '"')
				  {
					memmove(p, p+1, strlen(p+1)+1);
		  
					for(; *p && *p != '"'; p++)
				  {
					if (*p == '\\' && strchr("\"tnebr\\", p[1]))
					  {
						if (p[1] == 't')
					  p[1] = '\t';
						else if (p[1] == 'n')
					  p[1] = '\n';
						else if (p[1] == 'b')
					  p[1] = '\b';
						else if (p[1] == 'r')
					  p[1] = '\r';
						else if (p[1] == 'e') /* escape */
					  p[1] = '\033';
						memmove(p, p+1, strlen(p+1)+1);
					  }
					*p = hide_meta(*p);
				  }
		  
				if (*p == 0) 
				  {
					printf("*p=0\n");
					continue;
				  }
		  
					memmove(p, p+1, strlen(p+1)+1);
				  }
		  
				if (isspace(*p))
				  {
					*p = ' ';
					white = 1;
				  }
				else 
				  {
					if (white && *p == '#')
				  { 
					*p = 0;
					break;
				  }
					white = 0;
				  } 

		  }

	  /* strip leading spaces */
	  for (start = buff; *start && *start == ' '; start++);
		
	  /* strip trailing spaces */
	  for (len = strlen(start); (len != 0) && (start[len-1] == ' '); len--);
	  
	  if (len == 0)
		  continue; 
	  else
		  start[len] = 0;
	  
		
	 // printf("start= %s\n", start);

	  if ((p=strchr(start, '=')))
	  {
		/* allow spaces around "=" */
		for (arg = p+1; *arg == ' '; arg++);
		
		for (; p >= start && (*p == ' ' || *p == '='); p--)
		  *p = 0;
	  }

	 // printf("start1= %s\n", start);
	  //printf("arg=%s\n", arg);

		for (i = 0; gConfigCmd[i].name; i++){
		    if (strcmp(gConfigCmd[i].name, start) == 0)
	      {
	       gConfigCmd[i].f(arg);
			break;
	      }			
		}
		
	  }


	
  }

  fclose(fd);

  return 0;
  
}

/*配置处理函数*/
void * channel_change_handle(void *args)
{
	UINT32 uiCount = 0; /* 需切换的信道个数 */
	int    iLoop = 0;
	int    iChannel = 0;
	char   acCmd[64] = {0};
	char   acDevName[32] = {0};
	UINT32 iChanmask = 0;
	int    iChanScanTime = 0;
	int	   iCfgFlag = 0;
	UINT32 lastch=99, curr_ch;
	UINT32 secs, setting_flags=0;
	UINT32 channel_caps = 0;
	
	channel_caps = get_channel_caps();
	if('0' == g_acSniffDevName[0])
	{
		snprintf(acDevName, "%s", DEFAULT_WIFI_IF);
	}
	else
	{
		snprintf(acDevName, "%s", g_acSniffDevName);
	}

	
	while(1)
	{

		/*获取信道和设置时间*/
		OSAL_MutSemTake(configDB.dblock);
		iChanmask = configDB.scan_ch_mask;
		iChanScanTime = configDB.ch_switch_time;		
		OSAL_MutSemGive(configDB.dblock);

		iLoop = 0;
		
		if((0 == iChanmask) || (CONFIG_AUTO_CH_MASK == iChanmask))
		{
			lastch = 99;/*初始值*/
			if(0 == uiCount % 100)
			{
				iChanmask = channel_caps;
				iChanScanTime = 2;/*开始自动扫描的情况下，每个channel设定为2秒*/
				zero_auto_scan_ch_mask();
				printf("begin auto scan %x:%d\n", iChanmask, iChanScanTime);
				
			}
			else
			{
				/*背景动态扫描*/
				iChanmask = get_auto_scan_ch_mask() & channel_caps;/* g_uiChanMask默认值为1-13信道，扫描，获取包含AP的信道 */
				printf("auto scan mask %x\n", iChanmask);
			}
		}

		uiCount++;
		for(iLoop = 0; iLoop < DEFAULT_CHANNEL_COUNT; ++iLoop)
		{
			if(iChanmask & 0x1)
			{
				curr_ch = iLoop + 1;
				
				/*只有一个channel的情况。*/
				if(lastch != curr_ch)
				{
					snprintf(acCmd, sizeof(acCmd), "iwconfig %s channel %d", acDevName, curr_ch);
					system(acCmd);
					printf("%s\n", acCmd);
				}

				/* 睡眠，等待下次切换 */
				for(secs =0; secs < iChanScanTime; secs ++)
				{
					OSAL_TaskDelay(1000);
					if(is_setting_change_and_clear())/*有新的配置产生*/
					{
						uiCount = 0;
						setting_flags = 1;
						break;
					}
				}

				lastch = curr_ch;

				if(setting_flags == 1)
				{
					setting_flags = 0;
					break;
				}
				
			}
		
			iChanmask = iChanmask >> 1;
		}
		
	}

	return 0;
}


