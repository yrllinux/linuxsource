#include <stdio.h>         //标准输入输出定义
#include <stdlib.h>        //标准函数库定义
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>          //文件控制定义
#include <termios.h>     //POSIX中断控制定义
//#include <asm/termbits.h>
//#include <asm/ioctls.h>

#include <unistd.h>       //Unix标准函数定义

#include <errno.h>        //错误号定义

#include "uartcom.h"

#include "ap_log.h"


int speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300,//
                    B38400, B19200, B9600, B4800, B2400, B1200, B300,
                  };

int name_arr[] = {115200, 19200, 9600, 4800, 2400, 1200, 300,
                  38400, 19200, 9600, 4800, 2400, 1200, 300,
                 };


INT32 UART_SetOptions(INT32 fd,UART_OPTIONS *setting)  
{  
	INT32   i;  
	struct termios options;  
     
	if(tcgetattr(fd,&options)  !=  0)  
	{  
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"tcgetattr failure.\n");      
		return(-1);   
	}  

	//printf("options.c_cflag = %x\n", options.c_cflag);
	//printf("options.c_lflag = %x\n", options.c_lflag);
	
	//printf("options.c_iflag = %x\n", options.c_iflag);
	//printf("options.c_oflag = %x\n", options.c_oflag);
	
    //设置串口输入波特率和输出波特率  
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
	{  
		if(setting->speed == name_arr[i])  
		{               
			cfsetispeed(&options, speed_arr[i]);   /*input speed.*/
			cfsetospeed(&options, speed_arr[i]);    /*output speed.*/
			printf("speed i=%d\n", i);
			break;
		}  
	}       

	if(i == sizeof(speed_arr) / sizeof(int))
	{
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"invalid speed.\n"); 	 
		return -1;
	}

	
    options.c_cflag |= CLOCAL;  /*Ignore modem status lines*/
    options.c_cflag |= CREAD;  /*Enable receiver*/
    

	/*SETTING FLCTRL*/
    switch(setting->flowctrl)  
    {  
       case NO_FLOWCTRL ://不使用流控制  
			options.c_cflag &= ~CRTSCTS;  
			break;     
       case HW_FLOWCTRL : 
			options.c_cflag |= CRTSCTS;  
			break;  
       case SOFT_FLOWCTRL :
			options.c_cflag |= IXON | IXOFF | IXANY;  
			break;  
		default:
			break;
	}  
	
	/*SETTING DATABITS.*/
    options.c_cflag &= ~CSIZE; 
    switch (setting->databits)  
    {    
		case DATABITS_5    :  
			options.c_cflag |= CS5;  
			break;  
		case DATABITS_6:  
			options.c_cflag |= CS6;  
			break;  
		case DATABITS_7    :      
			options.c_cflag |= CS7;  
			break;  
		case DATABITS_8:      
			options.c_cflag |= CS8;  
			break;    
		default:     
			AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"invalid databits.\n");		
			return (-1);   
    }  
	
	/*SETTING PARITY.*/
    switch (setting->parity)  
    {    
		case PARITY_NONE:  
			options.c_cflag &= ~PARENB;   
			options.c_iflag &= ~INPCK;      
			break;   
		case PARITY_ODD:    
			options.c_cflag |= (PARODD | PARENB);   
			options.c_iflag |= INPCK; /*启用输入奇偶检测*/      
			break;   
		case PARITY_EVEN:   
			options.c_cflag |= PARENB;         
			options.c_cflag &= ~PARODD;         
			options.c_iflag |= INPCK; /*启用输入奇偶检测*/    
			break;   
		default:    
			AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"invalid parity.\n");		
			return (-1);   
    }   

	//printf("options.c_cflag = %x, odd=%x,paren=%x\n",options.c_cflag, PARODD, PARENB);
	/*SETTING STOPBITS*/
    switch (setting->stopbits)  
    {    
		case STOPBITS_1:     
			options.c_cflag &= ~CSTOPB; 
			break;   
		case STOPBITS_2:     
			options.c_cflag |= CSTOPB; 
			break;  
		default:     
			AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"invalid stopbits.\n");		
			return (-1);  
    }  
     
	/*RAW MODE.*/
	options.c_oflag &= ~(OPOST | ONLCR);  
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_iflag &= ~(BRKINT|IXON|ICRNL);
     
    //设置等待时间和最小接收字符  
    options.c_cc[VTIME] = 0; /* 读取一个字符等待1*(1/10)s */    
    options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */  

	//printf("after options.c_cflag = %x\n", options.c_cflag);
	//printf("after options.c_lflag = %x\n", options.c_lflag);
	
	//printf("after options.c_iflag = %x\n", options.c_iflag);
	//printf("after options.c_oflag = %x\n", options.c_oflag);
	
    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读  
    tcflush(fd,TCIFLUSH);  
     
    if(tcsetattr(fd,TCSANOW,&options) != 0)    
	{  
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"UART_SetOptions failure.\n");		
		return (-1);   
	}  
	
    return 0;   
}  

INT32 UART_Open(char* port)  
{  
	INT32 fd;
	
	fd = open(port, O_RDWR|O_NOCTTY|O_NONBLOCK);  
	if (fd < 0)  
	{  
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"UART_Open failure.\n");		
		return -1;  
	}  

	/*SETTING TO BLOCK.*/
	if(fcntl(fd, F_SETFL, 0) < 0)  
	{  
		AP_LOG(AP_LOG_ERR,LOG_MOUDLES_UART,"fcntl failure.\n");		
		return(-1);  
	}       
	else  
	{  
		printf("fcntl=%d\n",fcntl(fd, F_GETFL,0));  
	} 
		 
	//测试是否为终端设备      
	if(0 == isatty(fd))  
	{  
		printf("standard input is not a terminal device,errno=%d\n",errno);  
	}  
	else  
	{  
		printf("isatty success!\n");  
	}          
	
	return fd;  
}

INT32 UART_Close(INT32 fd)
{
	return close(fd);
}

INT32 UART_Flush(INT32 fd)
{
    tcflush(fd, TCIOFLUSH);  
	return 0;
}

INT32 UART_RecvChar(INT32 fd, char *ch, INT32 ms)
{
	int max_fd;
	fd_set readfds;
	struct timeval timeout;
	int rc;

	if(fd < 0)
		return -1;
	

	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	max_fd = fd + 1;

	do{
		timeout.tv_sec = ms/1000;	
		timeout.tv_usec = (ms % 1000)*1000;
		rc = select(max_fd, &readfds,NULL, NULL, &timeout);
	}while((rc == -1) & (errno == EINTR));
	
	if(rc <= 0) 	/*timeout or select err*/
		return -1;

	if(!FD_ISSET(fd, &readfds))
	{
		return -2;	/*has byte but not belong to the fd*/
	}
	
	*ch = 0;
	return read(fd, ch, 1);
	
}


INT32 UART_send(INT32 fd, char *buff, int len)
{

	int bytes, offset=0;;

	if((fd < 0) || (NULL == buff))
		return -1;

	do{
		bytes = write(fd, &buff[offset], len);
		if(bytes <= 0)
		{
			return -1;
		}
		
		len -=bytes;
		offset+=bytes;
	}while(len > 0);
	
	return offset;
	
}

