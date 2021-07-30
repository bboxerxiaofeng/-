#include "sql.h"

CTcpServer Tcpserver;


struct st_person stperson;
vector<struct st_person> V_st_person;
CLogFile LogFile;

char Str_Read[1024];
char Str_Write[1024];



void FathEXIT(int sig);  // 父进程退出函数
void ChidEXIT(int sig);  // 子进程退出函数

bool business_main(char *Str_Read,char *Str_Write); // 服务端处理主函数
bool Identity(char *Str_Read,char *Str_Write); // 账号密码认证子函数
bool Change_password(char *Str_Read,char *Str_Write);

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
    case 1:
        Identity(Str_Read,Str_Write); break; // 身份认证
    case 2:
        Change_password(Str_Read,Str_Write); break; // 更改密码
    case 3:
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
        if( (strcmp(name,V_st_person[ii].name)==0) )
            if( strcmp(password,V_st_person[ii].password)==0 )
            { 
                sprintf(Str_Write,"<retcode>1</retcode><id>%ld</id>",V_st_person[ii].id);
                return true;
            }
    }

    sprintf(Str_Write,"<retcode>0</retcode>");

    return false;

}


bool Change_password(char *Str_Read,char *Str_Write)
{
    int password_id;
    char new_password[30];
    memset(new_password,0,sizeof(new_password));

    GetXMLBuffer(Str_Read,"new_password",new_password);
    GetXMLBuffer(Str_Read,"id",&password_id);

    printf("new_password_before_oracle-%s\n",new_password);
    if(update_oracle(new_password,password_id)==true)
    {
        sprintf(Str_Write,"<retcode>1</retcode>");
        return true;
    }
    else
    {
        sprintf(Str_Write,"<retcode>0</retcode>");
        return false;
    }

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
