#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define ProcessNum 5
typedef struct pcb
{
    char name[20];
    int time_left;//进程剩余运行时间
    int time_require;//进程需要运行的时间
    int priority;//优先级
    char status;//状态
    struct pcb *next;//指向下一个PCB
    /* data */
}PCB;
int priority_num[ProcessNum]={1,3,2,5,4};
int time_require_num[ProcessNum]={4,8,10,3,11};
PCB *head;
void init();
void run();
void sort();
int run_once(PCB *p);
void del(PCB *p);
void print_queue();
int main(){
    init();
    run();
    return 0;
}
void init(){
    int n=ProcessNum;
    int i;
	PCB *p;
    //创建链表
	head=(PCB*)malloc(sizeof(PCB));
	p=head;
	for(i=0;i<n;i++){
		
		PCB *t=(PCB*)malloc(sizeof(PCB));
		p->next=t;
		p=t;
	}
	p->next=NULL;
    //遍历链表，给name赋值
    PCB *head1=head; 
    i=1;
    while(head1->next)
    {
    	sprintf(head1->next->name,"%s%d","PCB",i);
    	head1=head1->next;
    	i++;
	}
    //遍历链表，给priority赋值
	i=0;
	head1=head;
	while(head1->next){
		head1->next->priority=priority_num[i];
		head1=head1->next;
		i++;
	}
	//遍历链表，给time_require赋值
	i=0;
	head1=head;
	while(head1->next){
		head1->next->time_require=head1->next->time_left=time_require_num[i];
		head1=head1->next;
		i++;
	}
	//给status赋值
	for(head1=head;head1->next;head1=head1->next){
		head1->next->status='R';
	}
	sort();//按priority从大到小排序

	
}
void sort(){//冒泡排序，按priority从大到小排序
	PCB *head1;
	PCB *head2;
	for(head1=head;head1->next;head1=head1->next)
		for(head2=head1->next->next;head2!=NULL;head2=head2->next){
			if(head1->next->priority<head2->priority)
				{
					PCB *t=head1->next;
					head1->next=t->next;
					t->next=head2->next;
					head2->next=t;
				}
		}
	// for (head1 = head; head1->next; head1 = head1->next)
	// {
	// 	printf("%d ", head1->next->priority);
		
	// }printf("\n");
	// for (head1 = head; head1->next; head1 = head1->next)
	// {
	// 	printf("%d ", head1->next->time_left);
		
	// }printf("\n");
}
void run(){//运行处理器调度程序
	PCB *head1;
	while(head->next)//当链表不为空时运行处理器调度程序
	{
		
		head1=head;
		while (head1->next)
		{
				if (head1->next->status == 'R')
				{
					if (run_once(head1))
					{
						del (head1);
						print_queue();
						printf("\n");
					}
					else
						{head1 = head1->next;
						print_queue();
						printf("\n");}
				}
		}

		
	}
	printf("运行结束");
}
int run_once(PCB* p){//运行某个PCB
	int end=0;
	if (p->next->priority != 0)//运行某个PCB时，priority减1，最小值为0
		p->next->priority--;
	printf("当前运行进程:%s\n", p->next->name);
	if (--p->next->time_left == 0)//时间为0则表示运行结束，status设为E
	{
		p->next->status = 'E';
		end=1;
		//delete(p);
	}
	
	return end;
}
void print_queue(){//打印队列
	PCB *head1;
	if(head->next)
	printf("进程队列:");
	for(head1=head;head1->next;head1=head1->next)//遍历链表打印
	{	
		if(head1->next->time_left)
		{
			printf("%s(p=%d,s=%c,t=%d)", head1->next->name, head1->next->priority, head1->next->status, head1->next->time_left);
			if (head1->next->next&&head1->next->next->time_left)
				printf("->");
		}
		
		
	}
	
}
void del(PCB *p){//如果进程剩余时间为0，就将PCB从队列中删除
	PCB *t=p->next;
	p->next=t->next;
	free(t);
}
