#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#define PORT 9527
void SysError(char* str)
{
  perror(str);
  exit(-1);
}
int main(int argc,char* argv[])
{
  int cfd;
  int ret;
  char buf[4096];
  int serverIp;
  ret=inet_pton(AF_INET,"127.0.0.1",(void*)&serverIp);  //转换字节序
  if(ret==-1)
  {
    SysError("inet_pton");
  }
  struct sockaddr_in serverAddr;   //设定服务器地址
  serverAddr.sin_family=AF_INET;
  serverAddr.sin_port=htons(PORT);
  serverAddr.sin_addr.s_addr=serverIp;
  cfd=socket(AF_INET,SOCK_STREAM,0);
  if(cfd==-1)
  {
    SysError("socket error");
  }
  ret=connect(cfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));  //连接服务器
  if(ret==-1)
  {
    SysError("connect error");
  }
  //写入小写
  while(1)
  {
    scanf("%s",buf);
    ret=write(cfd,buf,strlen(buf));
    if(ret==-1)
    {
      SysError("write error");
    }
    ret=read(cfd,buf,sizeof(buf));
    if(ret==-1)
    {
      SysError("read error");
    }
    printf("%s\n",buf);
  }
  close(cfd);
    return 0;
}
