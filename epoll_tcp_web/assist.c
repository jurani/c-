#include "assist.h"
//通过文件名字获得文件类型
char *get_mime_type(char *name)
{
  char* dot;

  dot = strrchr(name, '.');	//自右向左查找‘.’字符;如不存在返回NULL
  /*
   *charset=iso-8859-1	西欧的编码，说明网站采用的编码是英文；
   *charset=gb2312		说明网站采用的编码是简体中文；
   *charset=utf-8			代表世界通用的语言编码；
   *						可以用到中文、韩文、日文等世界上所有语言编码上
   *charset=euc-kr		说明网站采用的编码是韩文；
   *charset=big5			说明网站采用的编码是繁体中文；
   *
   *以下是依据传递进来的文件名，使用后缀判断是何种文件类型
   *将对应的文件类型按照http定义的关键字发送回去
   */
  if (dot == (char*)0)
    return "text/plain; charset=utf-8";
  if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
    return "text/html; charset=utf-8";
  if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
    return "image/jpeg";
  if (strcmp(dot, ".gif") == 0)
    return "image/gif";
  if (strcmp(dot, ".png") == 0)
    return "image/png";
  if (strcmp(dot, ".css") == 0)
    return "text/css";
  if (strcmp(dot, ".au") == 0)
    return "audio/basic";
  if (strcmp( dot, ".wav") == 0)
    return "audio/wav";
  if (strcmp(dot, ".avi") == 0)
    return "video/x-msvideo";
  if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
    return "video/quicktime";
  if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
    return "video/mpeg";
  if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
    return "model/vrml";
  if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
    return "audio/midi";
  if (strcmp(dot, ".mp3") == 0)
    return "audio/mpeg";
  if (strcmp(dot, ".ogg") == 0)
    return "application/ogg";
  if (strcmp(dot, ".pac") == 0)
    return "application/x-ns-proxy-autoconfig";

  return "text/plain; charset=utf-8";
}


int get_line(int sock, char *buf, int size)
{
  int i = 0;
  char c = '\0';
  int n;

  while ((i < size - 1) && (c != '\n'))
  {
    n = recv(sock, &c, 1, 0);
    /* DEBUG printf("%02X\n", c); */
    if (n > 0)
    {
      if (c == '\r')
      {
        n = recv(sock, &c, 1, MSG_PEEK);//MSG_PEEK 从缓冲区读数据，但是数据不从缓冲区清除
        /* DEBUG printf("%02X\n", c); */
        if ((n > 0) && (c == '\n'))
          recv(sock, &c, 1, 0);
        else
          c = '\n';
      }
      buf[i] = c;
      i++;
    }
    else
      c = '\n';
  }
  buf[i] = '\0';

  return(i);
}

// %E8%8B%A6%E7%93%9C
void strdecode(char *to, char *from)
{
  for ( ; *from != '\0'; ++to, ++from) {

    if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2])) { //依次判断from中 %20 三个字符

      *to = hexit(from[1])*16 + hexit(from[2]);//字符串E8变成了真正的16进制的E8
      from += 2;                      //移过已经处理的两个字符(%21指针指向1),表达式3的++from还会再向后移一个字符
    } else
      *to = *from;
  }
  *to = '\0';
}

//16进制数转化为10进制, return 0不会出现
int hexit(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  return 0;
}
//封装send,使send发送错误时能重发
size_t sock_send(int cfd,const char* buf,size_t buflen,int option)
{
  int tmp=0;
  int total=buflen;
  const char* tmpbuf=buf;

  while(1)
  {
    tmp=send(cfd,tmpbuf,total,option);
    if(tmp<0)
    {
      if(errno==EINTR)//被中断时
        continue;
      else if(errno==EAGAIN) //缓冲队列已满时
        continue;
      else 
        return -1;
    }
    //printf("send=%d \n",tmp);
    if(tmp==total)
    {
      return buflen;
    }

    total-=tmp;
    tmpbuf+=tmp;
  }
}
