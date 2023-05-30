#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "wrap.h"
#include <netinet/in.h>
#include <ctype.h>
#define PORT 9527
int main()
{
  int lfd,cfd;
  int client[FD_SETSIZE]; //设置数组存放已连接的文件描述符
  char buf[16],rbuf[1500];
  lfd=Socket(AF_INET,SOCK_STREAM,0);
  struct sockaddr_in serverAddr,clientAddr;
  serverAddr.sin_family=AF_INET;   //初始化服务器地址结构
  serverAddr.sin_port=htons(PORT);
  serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
  socklen_t clientLen;
  //设置端口复用
  int opt = 1;
  setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  Bind(lfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
  Listen(lfd,128);
  //初始化读文件描述符集
  fd_set rset,oldset;
  FD_ZERO(&oldset);
  FD_SET(lfd,&oldset);
  int maxfd=lfd;      //最大文件描述符
  int maxi=-1;//用作client[]存放数据的最大下标
  memset(client,-1,sizeof(client)); 
  while(1)
  {
    rset=oldset;
    int n=select(maxfd+1,&rset,NULL,NULL,NULL);    //select模式监听
    if(n==-1)
    {
      perror("select error");
      exit(-1);
    }
    else if(n==0)
    {
      continue;
    }
    else if(n>0)
    {
      //已连接队列中提取通信套接字
      if(FD_ISSET(lfd,&rset))
      {
        cfd=Accept(lfd,(struct sockaddr*)&clientAddr,&clientLen);
        printf("connected from %s to %d\n",inet_ntop(AF_INET,
              &clientAddr.sin_addr.s_addr,buf,sizeof(buf)),ntohs(clientAddr.sin_port));     //显示连接用户
        if(maxi<1022)     //用数组存放已提取的通信文件描述符
        {
          client[++maxi]=cfd;
        }
        else
        {
          printf("the connection limit was reached\n");    //连接超出限制时
        }
        FD_SET(cfd,&oldset);

        if(cfd>maxfd)   
          maxfd=cfd;

        if(--n==0)
          continue;
      }
      //遍历查看哪些cfd文件描述符发生变化
      for(int i=0;i<=maxi;i++)
      {
        int ret=0;
        if(FD_ISSET(client[i],&rset))
        {
          ret=Read(client[i],rbuf,sizeof(rbuf));
          if(ret<0)
          {
            perror("Read error");

          }
          else if(ret==0)
          {
            printf("client close\n");
            //当有客户端断开连接时，将相应的文件描述符移除数组   
            if(i<maxi)      
            {
              close(client[i]);
              dup2(client[maxi],client[i]);
            }
            close(client[maxi]);
            FD_CLR(client[i],&oldset);
            --maxi;
          }
          else if(ret>0)
          {
            for(int i=0;i<ret;i++)
            {
              rbuf[i]=toupper(rbuf[i]);
            }
            ret=Write(client[i],rbuf,ret);
            if(ret<0)
            {
              perror("Write error");
            }
          }
          if(--n==0)
            break;
        }
      }
    }
  }
  Close(lfd);
  return 0;
}
