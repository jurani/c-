#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "wrap.h"
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include "assist.h"
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#define _GNU_SOURCE
#define PORT 9527

void response_server_request(int epfd,struct epoll_event* ev);
void send_header(int cfd,int code,char* info,char* filetype,int size);
void send_file(int cfd,char* file,struct epoll_event* ev,int epfd,int flag);

int main(int argc,char* argv[])
{
  //忽略sigpipe信号，防止服务器关闭
  signal(SIGPIPE,SIG_IGN);
  //改变工作目录
  int ret=0;
  char pwd_path[256];
  char* dest_path=getenv("PWD");
  strcpy(pwd_path,dest_path);
  strcat(pwd_path,"/myhttp");
  printf("%s\n",pwd_path);
  ret=chdir(pwd_path);
  if(ret==-1)
  {
    perror("chdir error");
    exit(-1);
  }
  struct sockaddr_in server_addr, client_addr;
  memset(&server_addr,0,sizeof(server_addr));
  int lfd=Socket(AF_INET,SOCK_STREAM,0);
  server_addr.sin_family=AF_INET;
  server_addr.sin_port=htons(PORT);
  server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  //设置端口复用
  int opt = 1;
  setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  Bind(lfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
  Listen(lfd,128);

  int epfd=epoll_create(1);
  if(epfd==-1)
  {
    perror("epoll_create error");
    exit(-1);
  }
  struct epoll_event ev,evs[1024];
  ev.data.fd=lfd;
  ev.events=EPOLLIN;
  ret=epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ev);
  if(ret==-1)
  {
    perror("epoll_ctl error");
    exit(-1);
  }

  while(1)
  {
    int n=epoll_wait(epfd,evs,1024,-1);
    if(n<0)
    {
      perror("epoll_wait error");
      exit(-1);
    }
    else
    {
      int i=0;
      for(i=0;i<n;++i)
      {
        if(evs[i].data.fd==lfd && evs[i].events&EPOLLIN)
        {
          char ip[16];
          memset(&client_addr,0,sizeof(client_addr));
          socklen_t client_len=sizeof(client_addr);
          int cfd=Accept(evs[i].data.fd,(struct sockaddr*)&client_addr,&client_len);
          printf("connect ip is %s,port is %d\n",inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,
                ip,sizeof(ip)),ntohs(client_addr.sin_port));
          //设置cfd非阻塞
          int flag=fcntl(cfd,F_GETFL);
          flag=flag | O_NONBLOCK;
          fcntl(cfd,F_SETFL,flag);
          ev.data.fd=cfd;
          ev.events=EPOLLIN;
          ret=epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev);


        }
        else if(evs[i].events & EPOLLIN)
        {
          response_server_request(epfd,&evs[i]);
        }
      }
    }
  }
  return 0;
}

void response_server_request(int epfd,struct epoll_event* ev)
{
  //读取请求行
  char buf[1500];
  char tmp[1024];
  int n=Readline(ev->data.fd,buf,sizeof(buf));
  if(n==0)
  {
    printf("recv error or client colsei\n");
    close(ev->data.fd);
    epoll_ctl(epfd,EPOLL_CTL_DEL,ev->data.fd,ev);
    return;
  }
  while(Readline(ev->data.fd,tmp,sizeof(tmp))>0);

  //解析请求行
  char type[256];
  char resource[256];
  char protocol[256];
  sscanf(buf,"%[^ ] %[^ ] %[^ \r\n]",type,resource,protocol);
  printf("[%s] [%s] [%s]\n",type,resource,protocol);

  //判断是否为get请求
  if(strcasecmp(type,"GET")==0)
  {
    char* resfile=resource+1;
    //解码%E8%8B%A6%E7%93%9C时的情况
    strdecode(resfile,resfile);
    if(*resfile=='\0') //如果没有请求文件，默认请求当前目录
      resfile="./";
    //判断文件是否存在
    struct stat s;
    if(stat(resfile,&s)<0) //文件不存在发送404
    {
      //发送报头
      send_header(ev->data.fd,404,"Not Found",get_mime_type("*.html"),0);
      //发送文件
      send_file(ev->data.fd,"error.html",ev,epfd,1);
    }
    else 
    {
      if(S_ISREG(s.st_mode)) //普通文件
      {
        send_header(ev->data.fd,200,"OK",get_mime_type(resfile),s.st_size);
        send_file(ev->data.fd,resfile,ev,epfd,1);
      }
      else if(S_ISDIR(s.st_mode))//请求的是目录时，展示列表
      {
        send_header(ev->data.fd,200,"OK",get_mime_type("*.html"),0);
        //发送目录的头部
        send_file(ev->data.fd,"dir_header.html",ev,epfd,0);
       
        struct dirent **dirlist;
        char buf[1024];
        int len=0;
        int n=0;
        n=scandir(resfile,&dirlist,NULL,alphasort);//获取目录中的文件
        if(n<0)
          perror("scandir error");
        else 
        {
          int i=0;
          for(i=0;i<n;++i)
          {
            if(dirlist[i]->d_type==DT_DIR) //是目录时要在超链接中间加/ 浏览器请求目录时会自动在后面加/
              len=sprintf(buf,"<li><a href=%s/>%s</a></li>",dirlist[i]->d_name,dirlist[i]->d_name);
            else //是普通文件时
              len=sprintf(buf,"<li><a href=%s>%s</a></li>",dirlist[i]->d_name,dirlist[i]->d_name);

            sock_send(ev->data.fd,buf,len,0);
            free(dirlist[i]);
          }
          free(dirlist);

          send_file(ev->data.fd,"dir_tail.html",ev,epfd,1); //发送目录的尾部
        }

      }
    }
    //判断文件时普通文件还是目录
  }

}

void send_header(int cfd,int code,char* info,char* filetype,int size)
{
  char buf[1024];
  //发送状态行
  int n=sprintf(buf,"HTTP/1.1 %d %s\r\n",code,info);
  sock_send(cfd,buf,n,0);
  //发送消息报头
  n=sprintf(buf,"Content-Type:%s\r\n",filetype);
  sock_send(cfd,buf,n,0);
  if(size>0)
  {
    n=sprintf(buf,"Content-Length:%d\r\n",size);
    sock_send(cfd,buf,n,0);
  }
  //发送空行
  sock_send(cfd,"\r\n",2,0);
  //printf("send the head\n");
}

void send_file(int cfd,char* file,struct epoll_event*ev,int epfd,int flag)
{
  int fd=open(file,O_RDONLY);
  if(fd<0)
  {
    perror("open error");
    return;
  }
  char buf[4096];
  memset(buf,0,sizeof(buf));

  while(1)
  {
    int n=read(fd,buf,sizeof(buf));
    if(n<0)
    {
      perror("read error");
      break;
    }
    else if(n==0)
    {
      break;
    }
    else 
    {
     sock_send(cfd,buf,n,0);
    }
  }
  close(fd);
  //发送完文件后关闭链接
  if(flag==1)
  {
    close(cfd);
    epoll_ctl(epfd,EPOLL_CTL_DEL,cfd,ev);
  }
}
