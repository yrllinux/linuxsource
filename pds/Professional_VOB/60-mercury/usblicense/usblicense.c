#include "eleses.h"
#include "resource.h"

#define BUFF	pucInBuff
void main()
{
	unsigned short uslen = _get_input_len();	// input buffer length

	// to do: you can input your code here

	int i=0;
	char *pbuf = "kdvx86series";
	char byIsOk = 1;

	for( i = 0;i< 12;i++ )
	{
		if( BUFF[i] != pbuf[i] )
		{
			byIsOk = 0;
			break;
		}	
	}

	// set output buffer 
	if( byIsOk )
	{
		BUFF[0] = 'y';
		BUFF[1] = 'e';
		BUFF[2] = 's';
		BUFF[3] = 0;
	}

	// set output buffer 
	_set_output(BUFF,uslen);
	
	// exit program			
	_exit(0);
}
