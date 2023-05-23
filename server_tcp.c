#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include  <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#define PORT 9527
void SysError(char* str)
{
  perror(str);
  exit(-1);
}

int main()
{
  int lfd,cfd;
  int ret;
  char buf[4096];
  socklen_t clitAddrLen;
  struct sockaddr_in serverAddr,clitAddr;
  //初始化服务器地址
  serverAddr.sin_port=htons(PORT);
  serverAddr.sin_family=AF_INET;
  serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
  lfd=socket(AF_INET,SOCK_STREAM,0);
  if(lfd==-1)
  {
    SysError("socket error");
  }
  ret=bind(lfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
  if(ret==-1)
  {
    SysError("bind error");
  }
  ret=listen(lfd,128);
  if(ret==-1)
  {
    SysError("listen error");
  }
  cfd=accept(lfd,(struct sockaddr*)&clitAddr,&clitAddrLen);
  {
    if(cfd==-1)
    {
      SysError("accept error");
    }
  }
  //小写转换大写
  while(1)
  {
    ret=read(cfd,buf,4096);
    if(ret==-1)
    {
      SysError("read error");
    }
    int i=0;
    for(i=0;i<ret;i++)
    {
      buf[i]=toupper(buf[i]);
    }
    ret=write(cfd,buf,ret);
    if(ret==-1)
    {
      SysError("write error");
    }
  }
  close(lfd);
  close(cfd);
  return 0;
}
