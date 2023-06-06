#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
int main(int argc,char* agrv[])
{
  struct sockaddr_in serverAddr,clientAddr;
  int ret=0;
  char ip[16];
  char buf[1500];
  memset(buf,0,sizeof(buf));
  socklen_t clientLen=sizeof(clientAddr);
  int lfd=socket(AF_INET,SOCK_DGRAM,0);
  if(lfd==-1)
  {
    perror("socket error");
    exit(-1);
  }

  serverAddr.sin_family=AF_INET;
  serverAddr.sin_port=htons(9527);
  serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
  ret=bind(lfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
  if(ret==-1)
  {
    perror("bind error");
    exit(-1);
  }
  while(1)
  {
    ret=recvfrom(lfd,buf,sizeof(buf),0,(struct sockaddr*)&clientAddr,&clientLen);
    if(ret<0)
    {
      perror("recvfrom error");
      exit(-1);
    }
    else
    {
      printf("connect ip is%s,port if%d\n",inet_ntop(AF_INET,&clientAddr.sin_addr.s_addr
            ,ip,sizeof(ip)),ntohs(clientAddr.sin_port));
      for(int i=0;i<ret;i++)
      {
        buf[i]=toupper(buf[i]);
      }
      ret=sendto(lfd,buf,ret,0,(struct sockaddr*)&clientAddr,sizeof(clientAddr));
      if(ret==-1)
      {
        perror("sendto error");
        exit(-1);
      }
    }
  }
  return 0;
}
