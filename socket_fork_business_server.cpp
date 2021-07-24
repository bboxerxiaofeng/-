#include <iostream>
#include "/home/xiaofeng/Test/Model/freecplus_20200926/freecplus/_freecplus.h"
#include "_ooci.h"
#include <vector>

CTcpServer Tcpserver;
CLogFile LogFile;
char Str_Read[1024];
char Str_Write[1024];

// 定义用于查询数据的结构，与表中的字段对应
struct st_person
{
  char name[31];     // 用户名称，用char对应oracle的varchar2，注意，表中字段的长度是30，char定义的长度是31，要留C语言的结束符
  char password[31]; // 用户密码
} stperson;

vector<struct st_person> V_st_person;

void FathEXIT(int sig);  // 父进程退出函数
void ChidEXIT(int sig);  // 子进程退出函数

bool business_main(char *Str_Read,char *Str_Write); // 服务端处理主函数
bool Identity(char *Str_Read,char *Str_Write); // 账号密码认证子函数
bool select_oracle(); // oracle查询数据函数

int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        printf("./socket_fork_business_server 5005 /home/xiaofeng/Item/Local_Cloud_server/tmp/socket_fork_server.log\n"); return false;
    }

    for(int ii=0;ii<100;ii++) signal(ii,SIG_IGN); // 屏蔽信号，避免产生僵尸进程

    if(LogFile.Open(argv[2],"a+")==false) { printf("open logfile %s false\n",argv[2]); return false; }

    if(Tcpserver.InitServer(atoi(argv[1]))==false) { LogFile.Write("Tcpserver_init false\n"); return false; }

    signal(SIGINT,FathEXIT); signal(SIGTERM,FathEXIT);

    while(true)
    {
        if(Tcpserver.Accept()==false) { LogFile.Write("Accept false\n"); return false; }

        if(fork()>0) { Tcpserver.CloseClient(); continue; } // 父进程负责接收客户端连接，子进程负责和客户端进行通信

        signal(SIGINT,ChidEXIT); signal(SIGTERM,ChidEXIT);

        Tcpserver.CloseListen();

        LogFile.Write("客户端(%s)已连接。\n",Tcpserver.GetIP());


        while(true)
        {
            memset(Str_Read,0,sizeof(Str_Read));
            if(Tcpserver.Read(Str_Read,10)==false) { LogFile.Write("Read false\n"); break; }

            business_main(Str_Read,Str_Write);

            if(Tcpserver.Write(Str_Write)==false) { LogFile.Write("Write false\n"); break; }

        }
    
        LogFile.Write("客户端已断开\n");
        exit(0);
    }
    return 0;
}

bool business_main(char *Str_Read,char *Str_Write)
{
    int business_number;
    GetXMLBuffer(Str_Read,"business",&business_number);
    switch(business_number)
    {
    case 0:
        Identity(Str_Read,Str_Write); break;
    case 1:
        //business_1(Str_Read,Str_Write); break;
    case 2:
        //business_2(Str_Read,Str_Write); break;
    default:break;
        
    }
    return 0;
}

bool Identity(char *Str_Read,char *Str_Write)
{
    char name[30]; char password[30];
    memset(name,0,sizeof(name));
    memset(password,0,sizeof(password));

    GetXMLBuffer(Str_Read,"name",name);
    GetXMLBuffer(Str_Read,"password",password);

    select_oracle();

    for(unsigned int ii=0; ii<V_st_person.size(); ii++)
    {
        printf("name=%s,password=%s",V_st_person[ii].name,V_st_person[ii].password);
        if( (strcmp(name,V_st_person[ii].name)==0) )
            if( strcmp(password,V_st_person[ii].password)==0 )
            { 
                sprintf(Str_Write,"<retcode>1</retcode>");
                return 0;
            }
    }

    sprintf(Str_Write,"<retcode>0</retcode>");

    return 0;

}

bool select_oracle()
{
  // 数据库连接池
  connection conn;
  printf("111");
  
  // 连接数据库，返回值0-成功，其它-失败
  // 失败代码在conn.m_cda.rc中，失败描述在conn.m_cda.message中。
  if (conn.connecttodb("scott/13011021@snorcl11g_192","Simplified Chinese_China.ZHS16GBK") != 0)
  {
    printf("connect database failed.\n%s\n",conn.m_cda.message); return -1;
  }
  
  // SQL语言操作类
  sqlstatement stmt(&conn);

  // 准备查询数据的SQL，不需要判断返回值
  stmt.prepare("\
    select name,password from person");

  // 为SQL语句绑定输出变量的地址
  stmt.bindout(1, stperson.name,30);
  stmt.bindout(2, stperson.password,30);

  // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
  if (stmt.execute() != 0)
  {
    printf("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return -1;
  }

  while (1)
  {
    // 先把结构体变量初始化，然后才获取记录
    memset(&stperson,0,sizeof(stperson));

    // 获取一条记录，一定要判断返回值，0-成功，1403-无记录，其它-失败
    // 在实际应用中，除了0和1403，其它的情况极少出现。
    if (stmt.next() !=0) break;
    
    // 将获取到的值存放到容器里
    V_st_person.push_back(stperson);

    // 把获取到的记录的值打印出来
    printf("name=%s=,password=%s=\n",stperson.name,stperson.password);
  }

  // 请注意，stmt.m_cda.rpc变量非常重要，它保存了SQL被执行后影响的记录数。
  printf("本次查询了goods表%ld条记录。\n",stmt.m_cda.rpc);
  
  return 0;
}


void FathEXIT(int sig)
{
    if(sig>0)
    {
        signal(sig,SIG_IGN); signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);
    }

    kill(0,15);

    LogFile.Write("父进程退出\n");

    Tcpserver.CloseClient();

    exit(0);
}


void ChidEXIT(int sig)
{
    if(sig>0)
    {
        signal(sig,SIG_IGN); signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);
    }

    LogFile.Write("子进程退出\n");

    Tcpserver.CloseListen();

    exit(0);
    
}
