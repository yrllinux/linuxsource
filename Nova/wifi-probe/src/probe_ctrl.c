#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>  /*open*/

#include "osal_api.h"
#include "wifiprobe.h"
#include "ap_log.h"

#define CM_CTRL_BUFF_SIZE      128
#define CM_MAX_CMDTBL_ARGS      3


typedef struct cm_ctrl_cmd{
	char *name;
    INT16 args;
	void (*f)(INT32, void *, void *, void*);
    void *help;
}CM_CTRL_CMD;

unsigned int g_uiLogMoudleMask = DEFAULT_LOG_MOUDLES_MASK;


/*declare*/
void do_help(INT32 idx, void *arg1, void *arg2, void*arg3);

/*SHOW*/
int taskShow(int taskid)
{
    int status;
    int failTaskGetcount = 0;
    int totalfail = -999;
    int i;
    char buf[16];
    OSAL_task_info_t task_prop;


    printf("\nTID     NAME         THREAD ID     CALLER    STACK SIZE     STACK POINT     PRIORITY\n");
    printf("\n%6s%6s%11s%8s%12s%12s%10s\n", "TID", "NAME", "THREAD ID", "CALLER", "STACK SIZE", "STACK POINT", "PRIORITY");
    if(taskid >= OSAL_MAX_TASKS)    /*show all*/
    {
        for(i=0; i< OSAL_MAX_TASKS; i++)
        {
            memset((void*)&task_prop, 0, sizeof(OSAL_task_info_t));
            if(OSAL_ERR_INVALID_ID == OSAL_TaskGetInfo(i, &task_prop))
                continue;
            
            printf("%6d%6s%11x%8d%12x%12p%10d\n", i, task_prop.name, task_prop.pthreadID, 
                    task_prop.creator, task_prop.stack_size, task_prop.stack_point, task_prop.priority);    
        }
    }
    else
    {

        memset((void*)&task_prop, 0, sizeof(OSAL_task_info_t));
        status = OSAL_TaskGetInfo(taskid, &task_prop);
        if(status != OSAL_SUCCESS)
            failTaskGetcount++;
        else
            printf("%02d      %s     %20x    %02d     %08x    %08p     %02d\n", taskid, task_prop.name, task_prop.pthreadID, 
                    task_prop.creator, task_prop.stack_size, task_prop.stack_point, task_prop.priority);
    }
    


    if (failTaskGetcount != 0)
        printf("Task Get Failed Count: %d\n", failTaskGetcount);
    
    return totalfail;

}/* end TestTasks */


void do_taskShow(INT32 idx, void *arg1, void *arg2, void*arg3)
{
    int tasknums=0;
    
    if(idx==0)
        return ;
    
    tasknums = atoi((char*)arg1);

    taskShow(tasknums);
	
}

void do_taskDelete(INT32 idx, void *arg1, void *arg2, void*arg3)
{
    int tasknums=0;
    
    if(idx==0)
        return ;
    
    tasknums = atoi((char*)arg1);

    OSAL_TaskDelete(tasknums);
	
}

extern void bufferQueueShow(void);

void do_buffShow(INT32 idx, void *arg1, void *arg2, void*arg3)
{
    
    if(idx!=0)
        return ;
    
    bufferQueueShow();
	
}

UINT32 testMsgType = MSG_SUBTYPE0_0;
extern INT32 OSAL_QueueApi_uninit(UINT32 qid);

void do_msgSend(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	INT32 retcode;
	MSG_HEAD msg;
	UINT32 qid;
	
	if(idx == 0)
	{
		memset((void *)&msg, 0, sizeof(MSG_HEAD));
		retcode = OSAL_QueueGetIdByName(&qid, QUEUE_UART_TX_NAME);
		if(retcode != OSAL_SUCCESS)
		{
			printf("OSAL_QueueGetIdByName err=%d\n", retcode);
			return ;
		}
		
		msg.destQID = qid;
		msg.msgType = testMsgType ++;
		msg.option = MSG_NORMAL;
		msg.flags = MSG_FLAGS_NONE;
		msg.msgPara1 = 0xdeaddead;
		msg.msgPara2.pAddedMsg = (void *)0x12345678;
		msg.msgLen = 30;
		retcode =  OSAL_msgSend(&msg, NO_WAIT);
		if(OSAL_SUCCESS != retcode)
			printf("sendmsg failure.ret = %d, type =%x\n", retcode,msg.msgType);
	}
	
}

void do_msgSendprio(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	INT32 retcode;
	MSG_HEAD msg;
	UINT32 qid;

	if(idx == 0)
	{
		memset((void *)&msg, 0, sizeof(MSG_HEAD));
		
		retcode = OSAL_QueueGetIdByName(&qid, QUEUE_CONFIG_NAME);
		if(retcode != OSAL_SUCCESS)
			return ;
		
		msg.destQID = qid;		
		msg.msgType = testMsgType ++;
		msg.option = MSG_URGENT;
		msg.flags = MSG_FLAGS_NONE;
		msg.msgPara1 = 0x88889999;
		msg.msgPara2.pAddedMsg = (void *)0x88889999;
		msg.msgLen = 40;
		retcode =  OSAL_msgSend(&msg, NO_WAIT);
		if(OSAL_SUCCESS != retcode)
			printf("sendmsg failure.ret = %d\n", retcode);
	}

	
}

void do_msgDelete(INT32 idx, void *arg1, void *arg2, void*arg3)
{
    int qid=0;
    
    if(idx==0)
        return ;
    
    qid = atoi((char*)arg1);

    OSAL_QueueApi_uninit(qid);
	
}

extern 	int TestSems(void);

void do_semtest1(INT32 idx, void *arg1, void *arg2, void*arg3)
{

    TestSems();
	
}

extern 	int TestMutexes(void);

void do_muxtest1(INT32 idx, void *arg1, void *arg2, void*arg3)
{

    TestMutexes();
	
}

extern 	void semShow(UINT32 semID);

void do_semShow(INT32 idx, void *arg1, void *arg2, void*arg3)
{
    int qid=0;
    
    if(idx==0)
        return ;
    
    qid = atoi((char*)arg1);

    semShow(qid);
	
}

void do_muxShow(INT32 idx, void *arg1, void *arg2, void*arg3)
{
    int qid=0;
    
    if(idx==0)
        return ;
    
    qid = atoi((char*)arg1);

    muxShow(qid);
	
}

void do_semGive(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	UINT32 semID=99;
	INT32 retcode = OSAL_SUCCESS;
	
	retcode = OSAL_SemaphoreGetIdByName(&semID, "PARSE SEM");
	if(retcode != OSAL_SUCCESS){
		printf("OSAL_SemaphoreGetIdByName err: %d\n", retcode);
		return ;
	}

	printf("before OSAL_SemaphoreGive\n");
   retcode =  OSAL_SemaphoreGive(semID);
	
	printf("OSAL_SemaphoreGive1 semID=%d: retcode = %d\n", semID, retcode);

	
	//retcode =  OSAL_SemaphoreGive(semID);
	
	//printf("OSAL_SemaphoreGive2 semID=%d: retcode = %d\n", semID, retcode);
	
}

void do_semFlush(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	UINT32 semID=99;
	INT32 retcode = OSAL_SUCCESS;
	
	retcode = OSAL_SemaphoreGetIdByName(&semID, "PARSE SEM");
	if(retcode != OSAL_SUCCESS){
		printf("OSAL_SemaphoreGetIdByName err: %d\n", retcode);
		return ;
	}

	printf("before OSAL_SemaphoreFlush\n");
   retcode =  OSAL_SemaphoreFlush(semID);
	
	printf("OSAL_SemaphoreFlush1 semID=%d: retcode = %d\n", semID, retcode);

	
	//retcode =  OSAL_SemaphoreFlush(semID);
	
	//printf("OSAL_SemaphoreFlush2 semID=%d: retcode = %d\n", semID, retcode);
	
}

void do_stat(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	dump_probe_stat();
}

void do_speed(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	dump_speed();
}



void do_timer1(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	timer1_test();
}

void do_timerShow(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	timerShow();
}

void do_timerDelete(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	timerDelete();
}
void do_timerStop(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	timerStop();
}


void do_macfiltertime(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	UINT32 filtertime;
	if(idx == 0)
		return;
    filtertime = (UINT32)atoi((char*)arg1);
	set_mac_report_time(filtertime);
}

void do_channelscan(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	UINT32 chmask;
	if(idx == 0)
		return;
	chmask = (UINT32)strtol(arg1, NULL, 16);
	printf("setting chmask=%x\n", chmask);
	set_scan_channel(chmask);
}
void do_channelswtime(INT32 idx, void *arg1, void *arg2, void*arg3)
{
	UINT32 swtime;
	if(idx == 0)
		return;
	
    swtime = (UINT32)atoi((char*)arg1);
	if(swtime == 0)
		swtime = 5;
	set_scan_swtime(swtime);
}

CM_CTRL_CMD cmCtrlCmdTbl[]={
	{"help", 0, do_help, ""},
    {"taskShow",1, do_taskShow, "task show [tid]"},
	{"taskDelete",1, do_taskDelete, "task delete [tid]"},    
	{"buffShow",0, do_buffShow, "buffer show"},    
	{"msgSend",1, do_msgSend, "msgSend test"},    
	{"msgPrio",0, do_msgSendprio, "msgSend prio test"},	  
	{"msgDelete",1, do_msgDelete, "msgDelete [qid]"},   
	{"semShow",1, do_semShow, "semShow [semid]"},	
	{"semtest1",0, do_semtest1, "semtest1"},	
	{"semGive",0, do_semGive, "semGive"},	
	{"semFlush",0, do_semFlush, "semFlush"},
	{"muxShow",0, do_muxShow, "muxShow"},	
	{"muxtest1",0, do_muxtest1, "muxtest1"},
	{"stat",0,do_stat, "show probe stat"},
	{"speed",0,do_speed,"show probe speed"},
	{"timer1",0,do_timer1,"test timer1"},
	{"timerShow",0,do_timerShow,"show timer"},
	{"timerDelete",0,do_timerDelete,"test delete timer"},
	{"timerStop",0,do_timerStop,"test stop timer"},
	{"macfiltertime",1,do_macfiltertime,"macfiltertime <time>"},
	{"channelscan",1,do_channelscan,"channelscan <chmask>"},
	{"channelswtime",1,do_channelswtime,"channelswtime <swtime>"},

	{NULL, 0, NULL, NULL}
};

void do_help(INT32 idx, void *arg1, void *arg2, void*arg3)
{
    printf("%s: idx=%d, cmd help: %s\n", __FUNCTION__, idx, cmCtrlCmdTbl[idx].help);
}

void CM_ctrl_handle_cmd(char *buff, INT32 len)
{
    int i,idx=0;
    UINT32 args[CM_MAX_CMDTBL_ARGS + 1];
    int arg_header =0, is_space=0;
    
    if(NULL == buff)
        return ;

    memset((void *)&args[0], 0, sizeof(args));

    /*parse cmd && args.according to SPACE*/
    for(i = 0; i < len; i++)
    {
        if(!isspace(buff[i]))
        {
            if(arg_header == 0)
            {
                /*记录每个字符串的首地址*/
                args[idx] = (UINT32)&buff[i];
                arg_header = 1;
                is_space = 0;
                idx++;
            }
            else
            {
                /*空格到字符的转变,标记新的字符串*/
                if(is_space)
                   arg_header = 0; 
            }
        }
        else
        {
            /*空格标记，并置0*/
            is_space = 1;
            arg_header=0;
            buff[i]=0;
        }
    }

   // for(i = 0; i<idx; i++)
     //   printf("arg%d = %s\n", i, (char*)args[i]);

    /*check args nums*/
    if((idx > (CM_MAX_CMDTBL_ARGS + 1)) || (idx == 0))
        return ;

    idx=idx-1;  /*skip cmd string self.*/

    /*搜索命令执行*/
    for(i =0; cmCtrlCmdTbl[i].name; i++)
    {
        if(strncmp(cmCtrlCmdTbl[i].name, (void *)args[0], strlen(cmCtrlCmdTbl[i].name)) == 0)
        {
            cmCtrlCmdTbl[i].f(idx, (void *)args[1], (void*)args[2], (void*)args[3]);
            break;
        }
    }   
    
}

/*

*/
void *CM_ctrl_handle(void* args)
{
    INT32 readbytes;
    INT32 ctrl_fd;
    UINT32 tid;
    char buff[CM_CTRL_BUFF_SIZE];

    OSAL_TaskRegister();
  
    memset(buff, 0, CM_CTRL_BUFF_SIZE);

    /*一直等待PIPI write open才往下执行。*/
   ctrl_fd = open(DEBUG_CTRL_INTF, O_RDONLY);
    if(ctrl_fd < 0)
    {
        AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CONFIG, "Could not open fifo %s\n", DEBUG_CTRL_INTF);  
        OSAL_TaskExit();
    }
   
    while(1)
    {
        readbytes = read(ctrl_fd, buff, CM_CTRL_BUFF_SIZE);
        if(readbytes == 0){
            OSAL_TaskDelay(100);
            continue;
        }

        buff[readbytes]=0;
      //  printf("read = %d, = %s\n", readbytes, buff);

        CM_ctrl_handle_cmd(buff, readbytes);
        
    }

    return ;
    
}


/*
* 创建命名管道，用来作为调试接口.
*/
INT32 CM_ctrl_init(void)
{
    char *pipe_name = DEBUG_CTRL_INTF;
    int retcode;
    UINT32 tid;
    
    if(access(pipe_name, F_OK) == 0)
    {
        unlink(pipe_name);
    }

    //创建命名管道,阻塞  
    retcode = mkfifo(pipe_name, 0777);  
    if(retcode != 0)  
    {  
        AP_LOG(AP_LOG_ERR, LOG_MOUDLES_CONFIG, "Could not create fifo %s\n", pipe_name);  
        return STATUS_ERROR;  
    }  


	#if 0	
    retcode = OSAL_TaskCreate(&tid, TASK_DEBUG_CTRL_NAME, CM_ctrl_handle, NULL,	\
                    0, TASK_DEBUG_CTRL_PRIO, 0);

    if(retcode != OSAL_SUCCESS)
    {
        printf("OSAL_TaskCreate return =%d\n", retcode);
        return STATUS_ERROR;
    }

	printf("Create Task: %s, tid =%d\n", TASK_DEBUG_CTRL_NAME, tid);
	#endif
	
    return STATUS_SUCCESS;
}



