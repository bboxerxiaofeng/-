#include <iostream>
#include "/home/xiaofeng/Test/Model/freecplus_20200926/freecplus/_freecplus.h"

CTcpClient TcpClient;
char str_write[101];
char str_read[101];

int Identity(); 
bool business_0();
void Client_server(int Server_id);
bool Change_password();

int main()
{
    if(TcpClient.ConnectToServer("192.168.198.129",5005)==0) { printf("ConnectToServer fail\n"); return false; };

    //int ii;
    int Server_id;
    printf("请输入你要进行操作：数字1代表-->账号登录 数字2代表-->修改密码\n");

    scanf("%d",&Server_id);

    Client_server(Server_id);


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

void Client_server(int Server_id)
{
    switch(Server_id)
    {
    case 1:
        Identity(); break;
    case 2:
        Change_password(); break;
    }

}

// 成功返回账号和密码对应的主键
int Identity()
{
    char name[30]; char password[30];
    memset(name,0,sizeof(name)); memset(password,0,sizeof(password));

    printf("请输入要登录的账号和密码(以空格隔开):\n");

    scanf("%s %s",name,password);
    printf("name=%s password=%s",name,password);

    // 发送端，发送的数据首先包含的是确定哪个业务的消息包 
    snprintf(str_write,100,"<business>1</business><name>%s</name><password>%s</password>",name,password);
    TcpClient.Write(str_write);

    // 接收端，对发送到服务器的数据进行解析判断，如果正确，服务端会给到接收正确的信号 
    int iretcode;
    int id;
    TcpClient.Read(str_read,20);
    GetXMLBuffer(str_read,"retcode",&iretcode);
    GetXMLBuffer(str_read,"id",&id);

    if(iretcode) { printf("登录成功\n");  return id; }
    else { printf("登录失败\n"); return false; }
}

bool Change_password()
{
    int Identity_flag;
    char new_password[30]; char new_password_confirm[30];
    memset(new_password,0,sizeof(new_password)); memset(new_password_confirm,0,sizeof(new_password_confirm));

    Identity_flag=Identity();
    if(Identity_flag) 
    {
        while(1)
        {
            printf("请输入新的密码:\n");
            scanf("%s",new_password);
            printf("请再次输入刚才输入的新密码以确保输入的新密码无误\n");
            scanf("%s",new_password_confirm);
            if(strcmp(new_password,new_password_confirm)==0)
            {
                 // 发送端，发送的数据首先包含的是确定哪个业务的消息包 
                 snprintf(str_write,100,"<business>2</business><id>%d</id><new_password>%s</new_password>",Identity_flag,new_password);
                 TcpClient.Write(str_write);
                 
                 // 接收端，对发送到服务器的数据进行解析判断，如果正确，服务端会给到接收正确的信号 
                 int iretcode;
                 TcpClient.Read(str_read,20);
                 GetXMLBuffer(str_read,"retcode",&iretcode);

                 if(iretcode) { printf("密码修改成功\n");  return true; }
                 else { printf("密码修改失败\n"); return false; }
            }
            else { printf("两次输入的密码不一致，请重新输入\n"); }

        }

    }
    return true;

}

bool business_0()
{
    snprintf(str_write,100,"<business>business_0</business>");
    TcpClient.Write(str_write);

    int iretcode;
    TcpClient.Read(str_read,20);
    GetXMLBuffer(str_read,"retcode",&iretcode);
    return 0;
}


