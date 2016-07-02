#include <stdio.h>

typedef struct link{
	int data;
	struct link *next;
}link;

link *reverse2(link * head)
{
	link *pre = NULL;
	link *back= NULL;
	
	if(head == NULL || head->next == NULL)
		return head;
	while(head)
	{
		back=head->next;
		head->next=pre;
		pre=head;
		head=back;
	}
	return pre; 
}

link *reverse(link * head)
{
	link *newhead;
	if(head == NULL || head->next == NULL)
		return head;
	newhead = reverse(head->next);
	head->next->next = head;
	head->next = NULL;
	
	return newhead; 
}

link *reverse1(link * head)
{
	link *prev=NULL;
	link *next=NULL;
	
	while(head)
	{
		printf("test\n");
		next=head->next;
		head->next=prev;
		prev=head;
		head=next;
	}
	
	return prev; 
}

int main(int argc, char *argv[])
{
	link a,b,c,d;
	a.next = &b; a.data = 1;
	b.next = &c; b.data = 2;
	c.next = &d; c.data = 3;
	d.next = NULL;d.data =4;
	link *tmp;
	tmp = reverse2(&a);
	while(tmp)	
		printf("%d ",tmp->data),tmp=tmp->next;	
	return 0;
}
