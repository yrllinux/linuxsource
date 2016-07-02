//以下示例仅仅是框架，说明父类跟子类之间如何实现多态的； 
#include <iostream>
using namespace std;

class cFather
{
public:
	cFather(){}
	virtual void fuc1()		//注意关键字virtual 
	{
		printf("This is father!\n");
	}	
};

class cSon : public cFather	//注意关键字public 
{
public:
	cSon(){}
	void fuc1()
	{
		printf("This is son!\n");
	}	
};

int main(int argc, char *argv[])
{
	cFather cfa;
	cSon cso;
	
	cfa.fuc1();
	cso.fuc1();
	
	cFather *pcfa = &cso;//new cSon();		//注意此处需要new 
	pcfa->fuc1();
	
	if(pcfa != NULL)		//注意此处需要判空 
	{
//		delete pcfa;
	}
		
	return 0;
}
