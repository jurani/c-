#include <stdio.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include "wrap.h"
int main()
{
  struct sockaddr_un serverAddr,clientAddr;
  char buf[1500];
  memset(buf,0,sizeof(buf));
  int cfd=Socket(AF_UNIX,SOCK_STREAM,0);
  //初始话客户端地址结构
  clientAddr.sun_family=AF_UNIX;
  strcpy(clientAddr.sun_path,"CLIENT_SOCK");
  socklen_t len=offsetof(struct sockaddr_un,sun_path)+strlen(clientAddr.sun_path);
  unlink("CLIENT_SOCK"); // 删除同名套接字,防止bind失败
  Bind(cfd,(struct sockaddr*)&clientAddr,len);
  //初始化服务器地址结构
  serverAddr.sun_family=AF_UNIX;
  strcpy(serverAddr.sun_path,"SERVER_SOCK");
  len=offsetof(struct sockaddr_un,sun_path)+strlen(serverAddr.sun_path);

  int ret=connect(cfd,(struct sockaddr*)&serverAddr,len);
  if(ret==-1)
  {
    perror("connect error");
    exit(-1);
  }
  while(fgets(buf,sizeof(buf),stdin)!=NULL)
  {
    ret=write(cfd,buf,strlen(buf));
    if(ret==-1)
    {
      perror("write error");
      exit(-1);
    }
    ret=read(cfd,buf,sizeof(buf));
    if(ret==-1)
    {
      perror("read error");
      exit(-1);
    }
    ret=write(STDOUT_FILENO,buf,ret);
    if(ret==-1)
    {
      perror("write error");
      exit(-1);
    }
  }
  close(cfd);
  return 0;
}
