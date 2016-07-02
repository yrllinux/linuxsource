#include <stdio.h>

#define N 1024
#define Head 1
#define Tail 0	

struct BinaryTreeNode
{
   int m_nValue;
   BinaryTreeNode* m_pLeft;
   BinaryTreeNode* m_pRight;
};

struct BinaryTreeNode BaseBinaryTreeNode;

int main(int argc, char *argv[])
{
	struct BinaryTreeNode *pBinaryTreeNode = &BaseBinaryTreeNode; 
	int i = 0;

	for(i=0; i<N; i++)
	{
		if(Head)
		{
			pBinaryTreeNode->m_pLeft->m_nValue = Head;
			pBinaryTreeNode = pBinaryTreeNode->m_pLeft;
		}
		else
		{
			pBinaryTreeNode->m_pRight->m_nValue = Tail;
			pBinaryTreeNode = pBinaryTreeNode->m_pRight;			
		}
	}	
	
 	queue<BinaryTreeNode *> q;  
    q.push(BaseBinaryTreeNode);  
    while(!q.empty())  
    {  
        BinaryTreeNode * pNode = q.front();  
        q.pop();  
        Visit(pNode); // ·ÃÎÊ½Úµã  
        if(pNode->m_pLeft != NULL)  
            q.push(pNode->m_pLeft);  
        if(pNode->m_pRight != NULL)  
            q.push(pNode->m_pRight);  
    }  
	return 0;
}
