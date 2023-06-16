#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <event.h>
#include <ctype.h>
#include <event2/listener.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

void cmd_input_cb(evutil_socket_t fd, short events, void *arg);
void server_msg_cb(struct bufferevent* bev,void* arg);
void event_cb(struct bufferevent* bev,short event,void* arg);

char buf[1500];

int main(int argc,char* argv[])
{
  //检查是否输入ip和端口
  if(argc<3)
  {
    printf("please input ip and port\n");
    exit(-1);
  }
  struct sockaddr_in server_addr;
  struct event_base* base;
  struct bufferevent* bev;

  base=event_base_new();  //建立根节点
  if(!base)
  {
    fprintf(stderr,"event_base_new error\n");
    exit(-1);
  }
  //初始化buffer缓冲区
  bev=bufferevent_socket_new(base,-1,BEV_OPT_CLOSE_ON_FREE);
  //监听终端输入事件
  struct event* ev=event_new(base,STDIN_FILENO,EV_READ|EV_PERSIST,
                            cmd_input_cb,(void*)bev);
  event_add(ev,NULL); //上树监听
  
  //初始化服务器地址
  memset(&server_addr,0,sizeof(server_addr));
  server_addr.sin_family=AF_INET;
  server_addr.sin_port=htons(atoi(argv[2]));
  inet_pton(AF_INET,argv[1],&server_addr.sin_addr.s_addr);
  bufferevent_socket_connect(bev,(struct sockaddr*)&server_addr,sizeof(server_addr));
  bufferevent_setcb(bev,server_msg_cb,NULL,event_cb,(void*)ev);//设置buffer的回调
  bufferevent_enable(bev,EV_READ | EV_PERSIST);//使能读回调
  
  event_base_dispatch(base); //开始循环监听
  //清理工作
  event_free(ev);
  bufferevent_free(bev);
  event_base_free(base);
  
  return 0;

}
//终端输入回调
void cmd_input_cb(evutil_socket_t fd, short events, void *arg)
{
  struct bufferevent* bev=(struct bufferevent*)arg;
  memset(buf,0,sizeof(buf));
  int n=read(fd,buf,sizeof(buf));
  if(n>0)
  {
    bufferevent_write(bev,buf,n);
  }

}

void server_msg_cb(struct bufferevent* bev,void* arg)
{
  //接受服务器数据
  memset(buf,0,sizeof(buf));
  int n=bufferevent_read(bev,buf,sizeof(buf));
  if(n>0)
  {
    write(STDOUT_FILENO,buf,n);
  }
}

void event_cb(struct bufferevent* bev,short event,void* arg)
{
  if(event & BEV_EVENT_EOF)
    printf("server close\n");
  else if(event & BEV_EVENT_ERROR)
    printf("some other error\n");
  else if(event & BEV_EVENT_CONNECTED)
  {
    printf("connect to server\n");
    return;
  }
  //异常清除工作
  bufferevent_free(bev);
  struct event* ev=(struct event*)arg;
  event_free(ev);
  exit(-1);
}
