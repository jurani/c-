#include <event.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "wrap.h"
#include <ctype.h>
#define PORT 9527

typedef struct Event
{
  int fd;
  struct event* ev;
}Event;
Event arr[100]; //用数组保存所创建的cfd事件节点的信息，方便之后释放event;
void add_event(int fd,struct event* ev)
{
  int i=0;
  while(i<100)
  {
    if(arr[i].fd==0)
    {
      arr[i].fd=fd;
      arr[i].ev=ev;
      return; 
    }
    else
      i++;
  }
}
struct event* get_event(size_t fd)
{
  int i=0;
  while(i<100)
  {
    if(arr[i].fd==fd)
    {
      arr[i].fd=0;
      return arr[i].ev;
    }
    else
      i++;
  }
  return NULL;
}
void cfd_callback(evutil_socket_t cfd,short event,void*arg)
{
  char buf[1500];
  memset(buf,0,sizeof(buf));
  int n=read(cfd,buf,sizeof(buf));
  if(n<=0)
  {
    perror("read error");
    struct event* ev=get_event(cfd); //获取cfd对应的event
    event_del(ev); //释放event;
    close(cfd);
    event_free(ev);
  }
  else
  {
    for(int i=0;i<n;i++)
    {
      buf[i]=toupper(buf[i]);
    }
    write(cfd,buf,n);
  }
}
void lfd_callback(evutil_socket_t lfd,short event,void* arg)
{
  struct event_base* base=(struct event_base*)arg;
  struct sockaddr_in clientAddr;
  char ip[16];;
  socklen_t len=sizeof(clientAddr);
  int cfd=Accept(lfd,(struct sockaddr*)&clientAddr,&len);
  printf("connect ip is%s,port is%d\n",inet_ntop(AF_INET,&clientAddr.sin_addr.s_addr,ip,
        sizeof(ip)),ntohs(clientAddr.sin_port));
  //创建cfd的节点
  struct event* cfdev=event_new(base,cfd,EV_READ|EV_PERSIST,cfd_callback,NULL);
  event_add(cfdev,NULL);
  add_event(cfd,cfdev);//添加cfd和对应事件到数组
}
int main(int argc,char*argv[])
{
  memset(arr,0,sizeof(arr));
  int lfd;
  struct sockaddr_in serverAddr;
  lfd=Socket(AF_INET,SOCK_STREAM,0);
  serverAddr.sin_family=AF_INET;
  serverAddr.sin_port=htons(PORT);
  serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
  Bind(lfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
  Listen(lfd,128);
  //创建event_base根节点
  struct event_base* base;
  base=event_base_new();

  //初始化上树节点
  //struct event *event_new(struct event_base *base, evutil_socket_t fd, short events, event_callback_fn cb, void *arg);
  struct event* lfdev=event_new(base,lfd,EV_READ|EV_PERSIST,lfd_callback,(void*)base);
  //节点上树int event_add(struct event *ev, const struct timeval *timeout);
  event_add(lfdev,NULL);

  event_base_dispatch(base);
  close(lfd);
  event_base_free(base);
  return 0;
}

