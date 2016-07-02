
#ifndef SIMBOARDWRAPPER_H
#define SIMBOARDWRAPPER_H
#include <string>
#include <time.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <signal.h>

#include "boardwrapper.h"
#include "kdvsys.h"

const int  NAME_SIZE = 20;
const int  BUF_SIZE = 50;

const std::string profilename = "./brddebug.ini";
const std::string boardposition = "boardposition";
const std::string boardlayer = "layer";
const std::string boardslot = "slot";
const std::string boardtype = "type";

const std::string host = "host";
const std::string hostip = "hostip";
const std::string hostmask = "hostmask";
const std::string hostgateway = "gateway";

const int boardhwver = 4;   // hardware version
const int boardfpgaver = 4; // fpge version
const int bsp15speed = 128;

const std::string osversion = "redhat-aes4";


struct proc_list
{
	pid_t pid;
	pid_t ppid;
	char name[NAME_SIZE];
	char status;
	struct proc_list *next;
};

struct proc_list *inport_list(char *);
void show_info(struct proc_list *);
int read_info(char *,struct proc_list *);
void free_list(struct proc_list *);
int is_num(char *);

int read_info(char *p_filename, char *p_item );

#endif

