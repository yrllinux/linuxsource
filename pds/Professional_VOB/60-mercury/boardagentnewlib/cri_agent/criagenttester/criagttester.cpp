#include "criagent.h"

//extern "C" void SetCriAgentDebug();

void userinit()
{
	if( ! OspInit(TRUE) )
    {
		printf("OspInit Error!\n");
        return;
    }

    //SetCriAgentDebug();

	OspOpenLogFile("CriAgent.log", 1000, 10);

	InitCriAgent();

	//getchar();
	return;
}

#ifdef WIN32
void main()
{
	userinit();
}
#endif
/*
void aaaa()
{
	Trim(" 1234 ");
}

void bbbb()
{
//	BrdQueryHWVersion();
}
*/