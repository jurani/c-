#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <event.h>
#include <ctype.h>
#include <event2/listener.h>
#define PORT 9527


void listener_cb(struct evconnlistener *evl, evutil_socket_t fd,
                 struct sockaddr *cliaddr, int socklen, void *ptr);
void readcb(struct bufferevent *bev, void *ctx);
void eofcb(struct bufferevent *bev, short event, void *ctx);

char buf[1500];

int main(int args,char* argv[])
{
  struct sockaddr_in sin;
  struct event_base* base;
  struct evconnlistener* listener;

  base=event_base_new(); //建立根节点
  if(!base)
  {
    fprintf(stderr,"event_base_new error\n");
    exit(-1);
  }
  //struct evconnlistener *evconnlistener_new_bind(struct event_base *base,
  // evconnlistener_cb cb, void *ptr, unsigned flags, int backlog,
  //const struct sockaddr *sa, int socklen);
  memset(&sin,0,sizeof(sin));
  //服务器地址结构
  sin.sin_family=AF_INET;
  sin.sin_port=htons(PORT);
  sin.sin_addr.s_addr=htonl(INADDR_ANY);
  //建立链接监听器
  listener=evconnlistener_new_bind(base,listener_cb,(void*)base,
      LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,-1,(struct sockaddr*)&sin,sizeof(sin)); 
  if(!listener)
  {
    fprintf(stderr,"evconnlistener_new_bind error\n");
    exit(-1);
  }
  //清除工作
  event_base_dispatch(base);
  evconnlistener_free(listener);
  event_base_free(base);

  return 0;
}

void listener_cb(struct evconnlistener *evl, evutil_socket_t fd, struct sockaddr *cliaddr, int socklen, void *ptr)
{
  struct sockaddr_in* clientaddr=(struct sockaddr_in*)cliaddr;
  struct event_base* base=(struct event_base*)ptr;
  struct bufferevent* bev=bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);
  if(!bev)
  {
    fprintf(stderr,"bufferevent_socket_new error\n");
    event_base_loopbreak(base);
    return;
  }
  bufferevent_setcb(bev,readcb, NULL,eofcb, NULL); //设置回调
  bufferevent_enable(bev,EV_READ); //使能读回调
  char ip[16];
  printf("connect ip is%s,port is %d\n",inet_ntop(AF_INET,&clientaddr->sin_addr.s_addr,ip,16)
      ,ntohs(clientaddr->sin_port)); //显示客户端地址
}

void readcb(struct bufferevent *bev, void *ctx)
{
  memset(buf,0,sizeof(buf));
  int n=bufferevent_read(bev,buf,sizeof(buf));
  if(n>0)
  {
    for(int i=0;i<n;i++)
    {
      buf[i]=toupper(buf[i]);
    }
  }
  bufferevent_write(bev,buf,n);
}

void eofcb(struct bufferevent *bev, short event, void *ctx)
{
  //客户端关闭
  if(event & BEV_EVENT_EOF)
  {
    printf("client close\n");
    bufferevent_free(bev);
  }
  //出错时
  if(event & BEV_EVENT_ERROR)
  {
    fprintf(stderr,"read error\n");
    return;
  }
}
