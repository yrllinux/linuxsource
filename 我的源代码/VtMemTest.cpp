#include <iostream>
#include <vector>
using namespace std;

typedef unsigned long   u32;

//Çå³ývectorÄÚ´æ
template<typename T> void VectorRelease(std::vector<T> &vc)
{
	std::vector<T> vctmp;
	vctmp.swap(vc);
}

typedef std::vector<u32> VtMemTest;

int main(int argc, char *argv[])
{
	VtMemTest m_VtMemTest;
	
	for(u32 i = 0; i < 100; i++)
	{
		//m_VtMemTest.resize(0xf);
		m_VtMemTest.push_back(i);
		//::VectorRelease(m_VtMemTest);		
	}	

	printf("The Memory size:%lld\n", m_VtMemTest.capacity() * sizeof( u32 ));
	
	VtMemTest::iterator iter = m_VtMemTest.begin();
		
	while( iter != m_VtMemTest.end() )
	{
		printf(" %ld\n", (u32)*iter);

		iter++;
	}
	
	return 0;
}
