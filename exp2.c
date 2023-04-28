#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_CAP 10 //缓冲区最大容量
#define ISR_NUM 5 //指令个数
#define MAX_INPUT 50 //输入区最大容量
#define COLOR_NONE "\033[0m" //表示清除前面设置的格式
#define RED "\033[1;31;40m" //40表示背景色为黑色, 1 表示高亮
struct PCB
{
    char name[20];
    char state;
    int WaitReason;
    int breakpoint;
}producer,consumer,*currentPC;//定义生产者，消费者PCB和指针
    int s1, s2; //s1:当前资源可存量 s2：当前资源可取量
    int PA[ISR_NUM], PB[ISR_NUM]; //用于存放指令标号
    int PC; //程序计数器 
    char Buffer[MAX_CAP]; //缓冲区
    char input[MAX_INPUT]={0};//输入暂存
    int input_p=0;
    int Buf_front = 0; //缓冲区头指针
    int Buf_rear = 0;//缓冲区尾指针
    char S; //存放产品
    int OUTPUT = 0;//输出消费的产品
    int fetch_done=0;//标志生产者是否已经从键盘将产品全部取到缓冲区
void Init(){
    int i;
    strcpy(producer.name,"producer");
    strcpy(consumer.name,"consumer");
    for(i=0;i<5;i++){
        PA[i]=i;
        PB[i]=i;
    }
    for(i=0;i<MAX_CAP;i++)
        Buffer[i]='#';
    PC=0;
    producer.state='R';
    consumer.state='R';
    producer.breakpoint=0;
    consumer.breakpoint=0;
    producer.WaitReason=-1;
    consumer.WaitReason=-1;
    
    s1=10;s2=0;//资源初始化
}
void P(int s,int s_type){
        if(s_type==1)//如果对S1操作
         {
            s1--;
            s=s1;
         }
         else {//如果对S2操作
            s2--;
            s=s2;
         }
         if(s<0){//信号量小于0，设置为等待
            currentPC->state='W';
            currentPC->WaitReason=s_type;
            printf("等待原因为:S%d\n",currentPC->WaitReason);
         }
         else {
            currentPC->state='R';
         }
}
void V(int s,int s_type){
    if(s_type==1)//如果对S1操作
    {
        s1++;
        s=s1;
    }
    else {//如果对S2操作
        s2++;
        s=s2;
    }
    if(s<=0){//如果信号量小于等于0，选择一个进程释放
        if (producer.WaitReason == s_type)
		{
			producer.state = 'R';
			producer.WaitReason = -1;
			printf("【提示】释放一个producer\n");
		}
 
		if (consumer.WaitReason == s_type)
		{
			consumer.state = 'R';
			consumer.WaitReason = -1;
			printf("【提示】释放一个consumer\n");
		}
    }
    else {
        printf("无进程释放\n");
		currentPC->state = 'R';//将现有进程置为就绪态
    }
}
void print_Buffer(){//打印缓冲区
    int i;
    for(i=0;i<MAX_CAP;i++)
        printf("%c ",Buffer[i]);
    printf("\n");
}
void print_PCB(){//打印PCB
    printf("Name:%20s%20s\n",producer.name,consumer.name);
    printf("State:%17c%17c\n",producer.state,consumer.state);
    printf("Wait Reason:");
    if(producer.WaitReason!=-1)
        printf("%11c%d",'S',producer.WaitReason);
    else printf("%12s","NULL");
    if(consumer.WaitReason!=-1)
        printf("%15c%d",'S',consumer.WaitReason);
    else printf("%16s","NULL");
    printf("\nBreak Point:%12d%16d\n",producer.breakpoint,consumer.breakpoint);

}
void produce(char s)//produce指令
{
    S=s;
}
void consum(int output){//consume指令
    printf("消费者消费了一个产品:%c\n",output);
}
void input_char(){//从键盘读取一组字符
    char s;
    printf("请输入一组字符,以‘#’结束:");
    while((s=getchar())!='#')
        {
        if (s != ' ')
        {
            input[input_p] = s;
            input_p++;
        }
        }
    input[input_p]='#';
    input_p=0;
}
void running()//模拟处理器执行
{
    int q,j=-1,i;
    i=PC;
    if (currentPC->name[0] == 'p') //选中生产者
	{
		j = PA[i];
		q = 1; //标识符
	}
	if (currentPC->name[0] == 'c')  //选中消费者
	{
		j = PB[i];
		q = 2;
	}
	PC = i + 1;
	//执行过程
	//char S;//临时存放生产商品
	int s=0;
	switch (j)
	{	
		case 0:
			if (q == 1)  //生产者执行【produce】
			{
                if(fetch_done)
                {
                    printf(RED"已经没有产品可以取了\n"COLOR_NONE);
                    PC=PC-1;
                    break;
                }
				printf("【提示】生产者取一件产品:");
				s=input[input_p++];
				printf("%c\n",s);
                if(input[input_p]=='#')
                    fetch_done=1;
                produce(s); // 暂存商品
            }
			else //消费者【P(S2)】
			{
 
				P(s2, 2); //2表示操作S2
			}
		break;
		case 1:
			if (q == 1) //生产者【P(s1)】
			{ 
				P(s1, 1);
			}
			else //消费者【GET】
			{
				printf("【提示】消费者取一个产品");
			
				OUTPUT = Buffer[Buf_rear];
				printf(":%c\n",OUTPUT);
				Buffer[Buf_rear] = '#';
				Buf_rear=(Buf_rear+1)%MAX_CAP;
				currentPC->state = 'R'; //设置为就绪态
 
			}
		break;
		case 2:
			if (q == 1) //生产者【PUT】
			{
				printf("【提示】生产者放入一个商品");
				printf(":%c\n",S); 
				//producerList[pflag] = S; //将产品存入生产者数组
				Buffer[Buf_front] = S;
				//pflag = (pflag + 1) % Total;
				Buf_front = (Buf_front + 1) % MAX_CAP;
				currentPC->state = 'R';//设置为就绪态
 
			}
			else //消费者【V(s1)】
			{
				V(s1, 1);
			}
			break;
		case 3:
			if (q 	== 1) //生产者【V(s2)】
			{
				V(s2, 2);
			}
			else //消费者【consume】--打印
			{
 
				consum(OUTPUT);
			}
			break;
		case 4:
			if (q == 1) //生产者【goto 0】
			{
				PC = 0;
			}
			else //消费者【goto 0】
			{
				PC = 0;
			}
			break;
 
	}		
	if (q==0&&j==4)//判断生产者进程是否运行结束
	{
		printf("【提示】生产者程序已执行完\n");
	}
    
}
void Scheduling()//处理器调度程序
{
    int choice;
    while(producer.state == 'R' || consumer.state == 'R')
    {
        printf("【提示】请选择本次执行的进程(1为生产者，2为消费者):");
        scanf("%d",&choice);
        printf("\n");
        while((choice==1 && producer.state!='R')||(choice==2 && consumer.state!='R'))
        {
            if(choice==1)
                {
                    printf("生产者进程被挂起,请重新选择:");
                    scanf("%d",&choice);
                    printf("\n");
                }
            else 
            {
                    printf("消费者进程被挂起,请重新选择:");
                    scanf("%d",&choice);
                    printf("\n");
            }
        }
        if(choice==1)
        {
            if(producer.state=='R')
                currentPC=&producer;
            else 
                printf("生产者进程被挂起,无法执行\n");
        }
        else if(choice==2)
        {
            if(consumer.state=='R')
                currentPC=&consumer;
            else printf("消费者进程被挂起,无法执行\n");
        }
        else
        {
            printf("输入有误,请重新输入\n");
            continue;
        }
        PC=currentPC->breakpoint;
        running();
        currentPC->breakpoint=PC;
        printf("【提示】当前缓冲区为:");
        print_Buffer();
        printf("\n【提示】当前S1值为:%d 当前S2值为:%d\n\n", s1,s2);
        print_PCB();
        printf("----------------------------------------------\n");
    }
}
int main(){
    Init();
    input_char();
    
     while(1)
         Scheduling();
    
}
