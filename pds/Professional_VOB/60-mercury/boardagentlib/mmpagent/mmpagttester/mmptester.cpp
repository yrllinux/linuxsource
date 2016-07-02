#include "mmpagent.h"

void main()
{
	if( ! OspInit(TRUE) )
    {
		printf("OspInit Error!\n");
        return;
    }

	OspOpenLogFile("mmpAgent.log", 1000, 10);


	InitMmpAgent();

	getchar();

    Sleep(0xFFFFFFFF);
	return;
}