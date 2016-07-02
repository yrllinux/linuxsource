#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "common_types.h"
#include "config_intf.h"
#include "ap_log.h"
#include "err_code.h"

typedef struct config_cmd{
	char * name;
	void (*f)(char *);
}CONFIG_CMD;

CONFIG_DB_T configDB;

static const char meta[] = "\000123456 \b\t\n78\r90abcdefABCDE\033F:,.";

static char hide_meta(char c)
{
  unsigned int i;

  for (i = 0; i < (sizeof(meta) - 1); i++)
    if (c == meta[i])
      return (char)i;
  
  return c;
}

static void cfg_parse_netip_mode(char *string)
{
	if(NULL == string)
		return ;
	
	configDB.netip_mode = atoi(string);
}

static void cfg_parse_netip_ip(char *string)
{
	if(NULL == string)
		return ;

	sprintf(configDB.netip_ip, "%s", string);
}

static void cfg_parse_netip_mask(char *string)
{
	if(NULL == string)
		return ;

	sprintf(configDB.netip_mask, "%s", string);
}

static void cfg_parse_netip_dgw(char *string)
{
	if(NULL == string)
		return ;

	sprintf(configDB.netip_dgw, "%s", string);
}

static void cfg_parse_netip_dns(char *string)
{
	if(NULL == string)
		return ;

	sprintf(configDB.netip_dns, "%s", string);
}

static void cfg_parse_netport_type(char *string)
{
	if(NULL == string)
		return ;
	
	configDB.netport_type = atoi(string);
}

static void cfg_parse_serial_rate(char *string)
{
	if(NULL == string)
		return ;
	
	configDB.serial_rate = atoi(string);
}

static void cfg_parse_serial_db(char *string)
{
	if(NULL == string)
		return ;
	
	configDB.serial_db = atoi(string);
}

static void cfg_parse_serial_sb(char *string)
{
	if(NULL == string)
		return ;
	
	configDB.serial_sb = atoi(string);
}

static void cfg_parse_serial_parity(char *string)
{
	if(NULL == string)
		return ;
	
	configDB.serial_parity = atoi(string);
}

static void cfg_parse_socket_mode(char *string)
{
	if(NULL == string)
		return ;
	
	configDB.socket_mode = atoi(string);
}

static void cfg_parse_socket_proto(char *string)
{
	if(NULL == string)
		return ;
	
	configDB.socket_proto = atoi(string);
}

static void cfg_parse_socket_cs(char *string)
{
	if(NULL == string)
		return ;
	
	configDB.socket_cs = atoi(string);
}

static void cfg_parse_socket_host(char *string)
{
	if(NULL == string)
		return ;

	sprintf(configDB.socket_host, "%s", string);
}

static void cfg_parse_socket_cport(char *string)
{
	if(NULL == string)
		return ;
	
	configDB.socket_cport = atoi(string);
}

static void cfg_parse_socket_sport(char *string)
{
	if(NULL == string)
		return ;
	
	configDB.socket_sport = atoi(string);
}

static void cfg_parse_socket_timeout(char *string)
{
	if(NULL == string)
		return ;
	
	configDB.socket_timeout = atoi(string);
}

CONFIG_CMD gConfigCmd[]={
	{"NETIP_MODE", cfg_parse_netip_mode},
	{"NETIP_IP", cfg_parse_netip_ip},
	{"NETIP_MASK", cfg_parse_netip_mask},
	{"NETIP_DGW", cfg_parse_netip_dgw},
	{"NETIP_DNS", cfg_parse_netip_dns},	
	{"NETPORT_TYPE", cfg_parse_netport_type},
	
	{"SERIAL_RATE", cfg_parse_serial_rate},
	{"SERIAL_DB", cfg_parse_serial_db},
	{"SERIAL_SB", cfg_parse_serial_sb},
	{"SERIAL_PARITY", cfg_parse_serial_parity},
	
	{"SOCKET_MODE", cfg_parse_socket_mode},	
	{"SOCKET_PROTO", cfg_parse_socket_proto},
	{"SOCKET_CS", cfg_parse_socket_cs},	
	{"SOCKET_HOST", cfg_parse_socket_host},
	{"SOCKET_CPORT", cfg_parse_socket_cport},	
	{"SOCKET_SPORT", cfg_parse_socket_sport},
	{"SOCKET_TIMEOUT", cfg_parse_socket_timeout},	
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
  
	if ((fd = fopen(file, "r")) != NULL)
	{
		memset(buff, 0, sizeof(buff));
		while (fgets(buff, sizeof(buff), fd))
		{
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
				
			if ((p=strchr(start, '=')))
			{
				/* allow spaces around "=" */
				for (arg = p+1; *arg == ' '; arg++);

				for (; p >= start && (*p == ' ' || *p == '='); p--)
					*p = 0;
			}

			for (i = 0; gConfigCmd[i].name; i++)
			{
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

void print_config_info(void)
{
	printf( "netip_mode:\t%d\nnetip_ip:\t%s\nnetip_mask:\t%s\nnetip_dgw:\t%s\nnetip_dns:\t%s\n"
			"netport_type:\t%d\nserial_rate:\t%d\nserial_db:\t%d\nserial_sb:\t%d\nserial_parity:\t%d\n"
			"socket_mode:\t%d\nsocket_proto:\t%d\nsocket_cs:\t%d\nsocket_host:\t%s\n"
			"socket_cport:\t%d\nsocket_sport:\t%d\nsocket_timeout:\t%d\n",
			configDB.netip_mode, configDB.netip_ip, configDB.netip_mask, configDB.netip_dgw, 
			configDB.netip_dns, configDB.netport_type, configDB.serial_rate, configDB.serial_db,
			configDB.serial_sb, configDB.serial_parity, configDB.socket_mode, configDB.socket_proto,
			configDB.socket_cs, configDB.socket_host, configDB.socket_cport, configDB.socket_sport,
			configDB.socket_timeout );	
}

INT32 CONFIG_init(char *configfile)
{
	INT32 retcode;
	UINT32 tid;

	memset((void *)&configDB, 0, sizeof(configDB));

	/*parse config file*/
	read_cfg_file(configfile);
	
	return 0;
}

static void usage(void)
{
	fprintf(stderr, "Usage: serial-wifi [<options> ...]\n\n"
        "Following options are available:\n"
        "        -f        config file.\n");
 
	exit(1);
}

#if 0
int main(int argc,char **argv)
{
	int ch;
	char *configFile = NULL;

	while ((ch = getopt(argc, argv,	"f:")) != -1)
	{
		switch (ch)
		{
			case 'f':
				configFile = optarg;
				break;
			default:
				usage();
		}
	}

	CONFIG_init(configFile);

	print_config_info();
}
#endif

