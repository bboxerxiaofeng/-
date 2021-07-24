#include <iostream>
#include "/home/xiaofeng/Test/Model/freecplus_20200926/freecplus/_freecplus.h"

CTcpClient TcpClient;
char str_write[101];
char str_read[101];

bool Identity();
bool business_0();

int main()
{
    if(TcpClient.ConnectToServer("192.168.198.129",5005)==0) { printf("ConnectToServer fail\n"); return false; };

    int ii;

    Identity();


    /*
    for(ii=0;ii<5;ii++)
    {
      memset(str_write,0,sizeof(str_write));
      sprintf(str_write,"这是第%d条数据",ii);
      if(TcpClient.Write(str_write)==false) { printf("Write false\n"); return false; }

      memset(str_read,0,sizeof(str_read));
      if(TcpClient.Read(str_read,20)==false) { printf("Read false\n"); return false; }
      printf("%s\n",str_read);
      sleep(1);
    }
    for(int ii; ii<100; ii++)
    {
        business_0();
        sleep(30);
    }
     */   
    return 0;
}

bool Identity()
{
    char name[30]; char password[30];
    memset(name,0,sizeof(name)); memset(password,0,sizeof(password));

    printf("请输入要登录的账号和密码(以空格隔开):\n");

    scanf("%s %s",name,password);
    printf("name=%s password=%s",name,password);

    // 发送端，发送的数据首先包含的是确定哪个业务的消息包 
    snprintf(str_write,100,"<business>0</business><name>%s</name><password>%s</password>",name,password);
    printf("%s",str_write);
    TcpClient.Write(str_write);

    // 接收端，对发送到服务器的数据进行解析判断，如果正确，服务端会给到接收正确的信号 
    int iretcode;
    TcpClient.Read(str_read,20);
    GetXMLBuffer(str_read,"retcode",&iretcode);
    if(iretcode)
        printf("登录成功\n");
    else
        printf("登录失败\n");
}

bool business_0()
{
    snprintf(str_write,100,"<business>business_0</business>");
    TcpClient.Write(str_write);

    int iretcode;
    TcpClient.Read(str_read,20);
    GetXMLBuffer(str_read,"retcode",&iretcode);
}


