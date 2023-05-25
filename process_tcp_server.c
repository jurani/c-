#include "wrap.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#define PORT 9527

void ProcessWait(int sig)
{
  int ret;
  pid_t pid;
  while(1)
  {
    ret=waitpid(-1,NULL,WNOHANG);  
    if(ret<=0)  //当子进程已回收完或没有子进程回收时
    {
      break;
    }
    else
    {
      printf("child wait:%d\n",pid);
    }
  }
}

int main(int argc,char* argv[])
{  
  //设置阻塞SIGCHLD信号
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set,SIGCHLD);
  sigprocmask(SIG_BLOCK,&set,NULL);
  int lfd,cfd;
  socklen_t clientLen;
  char len[16];
  //初始化服务器地址结构
  struct sockaddr_in serverAddr,clientAddr;
  serverAddr.sin_family=AF_INET;
  serverAddr.sin_port=htons(9527);
  serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
  lfd=Socket(AF_INET,SOCK_STREAM,0);
  Bind(lfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
  Listen(lfd,128);

  while(1)
  {
    cfd=Accept(lfd,(struct sockaddr*)&clientAddr,&clientLen);
    printf("已连接新用户ip为ip:%s,端口为%d\n",inet_ntop(AF_INET,
          &clientAddr.sin_addr.s_addr,len,sizeof(len)),ntohs(clientAddr.sin_port));  //显示连接客户端IP和端口
    pid_t fd=fork();
    if(fd<0)
    {
      perror("ford error");
      exit(-1);
    }
    else if(fd==0)  //子进程用以通信
    {
      Close(lfd);
      int ret;
      char buf[1024];
      memset(buf,0,sizeof(buf)); 
      while(1)   //处理数据
      {
        ret=Read(cfd,buf,sizeof(buf));
        if(ret==-1)
        {
          perror("Read error");
          exit(-1);
        }
        else if(ret==0)  //客户端断开连接时
        {
          printf("client close\n");
          close(cfd);
          exit(0);
        }
        else if(ret>0)
        {
          int i=0;
          for(i=0;i<ret;i++)
          {
            buf[i]=toupper(buf[i]);
          }
          ret=Write(cfd,buf,ret);

        }
      }
      Close(cfd);
    }
    else
    {
      Close(cfd);
      //捕抓SIGCHLD,用来回收子进程
      struct sigaction act;
      act.sa_flags=0;
      sigemptyset(&act.sa_mask);
      act.sa_handler=ProcessWait;
      //解除阻塞
      sigdelset(&set,SIGCHLD);
      sigprocmask(SIG_BLOCK,&set,NULL);
    }
  }
  Close(lfd);
  return 0;
}

