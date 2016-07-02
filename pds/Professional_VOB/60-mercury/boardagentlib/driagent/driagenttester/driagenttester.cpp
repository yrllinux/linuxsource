#include "driagent.h"

void main()
{
	if( ! OspInit(TRUE) )
    {
		printf("OspInit Error!\n");
        return;
    }

	OspOpenLogFile("DriAgent.log", 1000, 10);

	InitDriAgent();

	getchar();
	return;
}