#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/un.h>
#include "wrap.h"
#include <stddef.h>
#include <ctype.h>
int main()
{
  struct sockaddr_un addr,clientAddr;
  int ret=0;
  int lfd,cfd;
  char buf[1500];
  memset(buf,0,sizeof(buf));
  lfd=socket(AF_UNIX,SOCK_STREAM,0);
  //初始化地址结构
  addr.sun_family=AF_UNIX;
  strcpy(addr.sun_path,"SERVER_SOCK");
  socklen_t len=offsetof(struct sockaddr_un,sun_path)+strlen(addr.sun_path);
  unlink("SERVER_SOCK"); //删除同名套接字，防止bing失败
  bind(lfd,(struct sockaddr*)&addr,len);
  listen(lfd,20);
  cfd=accept(lfd,(struct sockaddr*)&clientAddr,&len);
  while(1)
  {
    int n=read(cfd,buf,sizeof(buf));
    if(n<0)
    {
      perror("read perror");
      exit(-1);
    }
    else if(n==0)
    {
      printf("connect close\n");
      break;
    }
    else
    {
      for(int i=0;i<n;i++)
      {
        buf[i]=toupper(buf[i]);
      }
      n=write(cfd,buf,n);
      if(n==-1)
      {
        perror("write error");
        exit(-1);
      }
    }
  }
  close(cfd);
  close(lfd);
  return 0;
}
