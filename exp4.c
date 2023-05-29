#include<stdio.h>
#include <string.h>
#define MAXCYLINDERNUM 200//最大柱面数
#define MAXsectorNUM 20//最大扇区数
#define MAXPHYSICALNUM 8//最大物理记录数

struct request_table
{
    char name[10];       //进程名
    int cylinder ; //柱面号
    int sector ; //扇区号
    int record ;   //物理记录号
}Table[100],current;//分别为请求表和当前磁头位置
char direction='U';//U代表里移，D代表外移
int table_num=5;//用于统计当前io请求个数
int first_run=1;
void Init(){//初始化，为了模拟运行先添加几个io请求
    strcpy(current.name,"初始进程");
    current.cylinder = 0;
    current.sector = 0;
    current.record = 0;
    //假定表中已经存在的请求
    strcpy(Table[0].name,"P1");
    Table[0].cylinder = 12;
    Table[0].sector = 2;
    Table[0].record = 2;
    strcpy(Table[1].name,"P2");
    Table[1].cylinder = 54;
    Table[1].sector = 12;
    Table[1].record = 7;
    strcpy(Table[2].name,"P3");
    Table[2].cylinder = 12;
    Table[2].sector = 11;
    Table[2].record = 1;
    strcpy(Table[3].name,"P4");
    Table[3].cylinder = 12;
    Table[3].sector = 1;
    Table[3].record = 1;
    strcpy(Table[4].name,"P5");
    Table[4].cylinder = 12;
    Table[4].sector = 10;
    Table[4].record = 2;
}
void accept_request(){//接受请求
    int select;
    do
    {
         struct request_table temp;//记录临时输入
         
    printf("依次输入：进程名、柱面号、扇区号、物理记录号");
    scanf("%s %d %d %d",temp.name,&temp.cylinder,&temp.sector,&temp.record);
    if (temp.cylinder < MAXCYLINDERNUM && temp.sector < MAXsectorNUM && temp.record < MAXPHYSICALNUM)//检查输入是否合规
        {
            Table[table_num] = temp;
            table_num++;
            printf("登记成功!  1.退出   2.继续");
            scanf("%d",&select);
        }
        else printf("输入错误,请重输, 柱面号需小于200,扇区号需小于20,物理记录号需小于8.");
    }while(select!=1);
   
}
void print_IO_table(){//打印io请求表
    printf("***************************请求I/O表*******************************\n");
    printf("进程名:         柱面号:         扇区号:         物理记录号:\n");
    for(int i=0;i<table_num;i++)
        printf("%4s%16d%17d%17d\n",Table[i].name,Table[i].cylinder,Table[i].sector,Table[i].record);
    printf("***************************请求I/O表*******************************\n\n");
}
void printProcess(){//打印当前运行的请求
    printf("************************选中的进程信息*****************************\n");
    printf("进程名:         柱面号:         扇区号:         物理记录号     方向:\n");
    printf("%4s%16d%17d%14d",current.name,current.cylinder,current.sector,current.record);
    if(direction=='U')printf("%16s","UP");
    else printf("%16s","down");
    printf("\n************************选中的进程信息*****************************\n\n");
}
int has_same_cylinder(){//是否有相同的柱面号,返回在请求表中的位置
    for(int i=0;i<table_num;i++)
        if(Table[i].cylinder==current.cylinder)
            return i;
    return -1;
}
void process(int i){//运行进程，被选者退出IO请求表
    current=Table[i];//修改当前运行的请求
    printProcess();//打印当前运行的请求
    table_num--;//运行了一个请求，总数应当减去1
    for(int j=i;j<table_num;j++)//将运行过的请求从表中删去
        Table[j]=Table[j+1];
}
int spin_shortest(){//在请求表中有相同柱面号的情况下，找出能使旋转距离最短的等待访问者
    int distance=MAXPHYSICALNUM*MAXsectorNUM;//记录最短的旋转距离
    int shortest;//距离能使旋转距离最短的等待访问者的位置
    int diff;//记录请求的位置离当前磁头的距离
    for(int i=0;i<table_num;i++)
    {
        if(Table[i].cylinder==current.cylinder)
        {
            if(Table[i].sector>current.sector)//请求扇区在当前磁头前面，只需减去即可得到距离
                {
                    diff=MAXPHYSICALNUM*Table[i].sector+Table[i].record-MAXPHYSICALNUM*current.sector-current.record;
                    if(diff<distance)
                    {
                        distance=diff;
                        shortest=i;
                    }

                }
            else if(Table[i].sector<current.sector)//请求扇区在当前磁头后面，因为磁盘固定方向旋转，距离计算方式有所不同
                {
                    diff=MAXPHYSICALNUM*MAXsectorNUM-current.sector*MAXPHYSICALNUM-current.record+Table[i].sector*MAXPHYSICALNUM+Table[i].record;
                    if(diff<distance)
                    {
                        distance=diff;
                        shortest=i;
                    }
                }
            else//如果扇区相同，则比较物理记录位置
                {
                    if(Table[i].record>current.record)
                    {
                        diff=Table[i].record-current.record;
                        if(diff<distance)
                        {
                            distance=diff;
                            shortest=i;
                        }
                    }
                    else
                    {
                        diff=MAXPHYSICALNUM*MAXsectorNUM-current.record+Table[i].record;
                        if(diff<distance)
                        {
                            distance=diff;
                            shortest=i;
                        }
                    }
                }
        }
    }
    return shortest;
}
int find_larger_nearest()//UP方向时，找出一个与当前柱面号最近的访问请求
{
    int temp=MAXCYLINDERNUM;
    int final_choice=-1;
    for(int i=0;i<table_num;i++)
    {
        if(Table[i].cylinder>current.cylinder&&Table[i].cylinder<=temp)
        {
            temp=Table[i].cylinder;
            final_choice=i;
        }
    }
    
    return final_choice;
}
int find_smaller_nearest()//Down方向时，找出一个与当前柱面号最近的访问请求
{
    int temp=0;
    int final_choice=-1;
    for(int i=0;i<table_num;i++)
    {
        if(Table[i].cylinder<current.cylinder&&Table[i].cylinder>=temp)
        {
            temp=Table[i].cylinder;
            final_choice=i;
        }
    }
    
    return final_choice;
}
int FirstRun(){//第一次运行时找出与磁头000起始点最近的请求
    int temp=MAXCYLINDERNUM*MAXsectorNUM;
    int final=-1;
    for(int i=0;i<table_num;i++)
        {
            int diff=Table[i].cylinder*MAXsectorNUM+Table[i].sector*MAXPHYSICALNUM+Table[i].record;
            if(diff<temp){
                final=i;
                temp=diff;
            }
                
        }
    return final;
}
void scan()//电梯调度算法
{   
    int final_process=-1;
    if(first_run)
    {
        process(FirstRun());
        first_run=0;
        return;
    }
    if(has_same_cylinder()!=-1)
    {
       final_process=spin_shortest();//如果有当前柱面号相同的访问请求，则找出并执行
        
    }
    else /*如果没有与当前柱面号相同的访问请求，则根据移臂方向来选择，每次总是沿臂移动方向选择
一个与当前柱面号最近的访问请求*/
    {   
        if(direction=='U')//如果方向为UP
            if(find_larger_nearest()!=-1)//沿这个方向有访问请求时
            {
                final_process=find_larger_nearest();
            }
            else//如果沿这个方向没有访问请求时，就改变臂的移动方向。
            {
                direction='D';
                final_process=find_smaller_nearest();
            }
        else//如果方向为Down
        {
            if (find_smaller_nearest() != -1)//沿这个方向有访问请求时
            {
                final_process = find_smaller_nearest();
            }
            else//如果沿这个方向没有访问请求时，就改变臂的移动方向。
            {
                direction = 'U';
                final_process = find_larger_nearest();
            }
        }
    }
    process(final_process);//执行
}
int main(){
    Init();
   
    while(1){
        float select;//记录随机数
        if (table_num)
        {   
            printf("[当前磁头位置]:%d  %d  %d  ",current.cylinder,current.sector,current.record);
            printf("\n[当前IO请求表]\n");
            print_IO_table();
        }
        else
        {
            printf("[执行完毕]\n");
            return 0;
        }
        printf("输入在[0,1]内的随机数:");
        scanf("%f",&select);
        if(select<=0.5)
            accept_request();
        else scan();

    }
}