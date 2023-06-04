#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include "wrap.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <fcntl.h>
#define PORT 9527
int main()
{
  int lfd,cfd;
  char buf[4];
  struct sockaddr_in serverAddr,clientAddr;
  lfd=Socket(AF_INET,SOCK_STREAM,0);
  //初始化服务器地址结构
  serverAddr.sin_family=AF_INET;
  serverAddr.sin_port=htons(PORT);
  serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
  //设置端口复用
  int opt = 1;
  setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  Bind(lfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
  Listen(lfd,128);

  int epfd=epoll_create(1);
  if(epfd==-1)
  {
    perror("epfd error");
    exit(-1);
  }
  struct epoll_event ev,evs[1024];
  //上树监听套接字
  ev.data.fd=lfd;
  ev.events=EPOLLIN;
  int ret=epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ev);
  if(ret==-1)
  {
    perror("epoll_ctl error");
    exit(-1);
  }

  while(1)
  {//启用epoll模式监听
    int n=epoll_wait(epfd,evs,1024,-1);
    if(n<0)
    {
      perror("epoll_wait error");
      break;
    }
    else if(n==0)
    {
      continue;
    }
    else
    {
      for(int i=0;i<n;i++)
      {
        if(evs[i].data.fd==lfd && evs[i].events&EPOLLIN)
        {
          socklen_t clientLen;
          char ip[16];
          cfd=Accept(lfd,(struct sockaddr*)&clientAddr,&clientLen);
          printf("connect ip is %s,port is%d\n",  //显示连接的客户端
              inet_ntop(AF_INET,&clientAddr.sin_addr.s_addr,ip,16),ntohs(clientAddr.sin_port));
          //设置cfd为非阻塞
          int flags=fcntl(cfd,F_GETFL);
          flags|=O_NONBLOCK;
          fcntl(cfd,F_SETFL,flags);
          //上树通讯连接套接字
          ev.data.fd=cfd;
          ev.events=EPOLLIN | EPOLLET;  //设置监听cfd为边沿触发
          ret=epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev);
          if(ret==-1)
          {
            perror("epoll_ctl error");
            continue;
          }
        }
        else if(evs[i].events & EPOLLIN)
        {
          while(1)
          {

            ret=Read(evs[i].data.fd,buf,sizeof(buf));
            if(ret<0)
            {
              if(errno==EAGAIN) //数据读取完毕，跳出循环
              {
                break;
              }
              perror("read error");  //读取出错时
              Close(evs[i].data.fd);
              epoll_ctl(epfd,EPOLL_CTL_DEL,evs[i].data.fd,&evs[i]);
              break;
            }
            else if(ret==0)
            {//客户端关闭下数cfd
              printf("client close\n");
              epoll_ctl(epfd,EPOLL_CTL_DEL,evs[i].data.fd,&evs[i]);
              Close(evs[i].data.fd);
              break;
            }
            else
            {//处理数据
              for(int i=0;i<ret;i++)
              {
                buf[i]=toupper(buf[i]);
              }
              ret=Write(evs[i].data.fd,buf,ret);
              if(ret==-1)
              {
                perror("write error");
              }
            }
          }
        }
      }
    }
  }
  return 0;
}
