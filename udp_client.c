#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
int main()
{
  struct sockaddr_in serverAddr;
  char buf[1500];
  memset(buf,0,sizeof(buf));
  int sockfd=socket(AF_INET,SOCK_DGRAM,0);
  if(sockfd==-1)\
  {
    perror("socket error");
    exit(-1);
  }
  serverAddr.sin_family=AF_INET;
  serverAddr.sin_port=htons(9527);
  inet_pton(AF_INET,"127.1",&serverAddr.sin_addr.s_addr);

  while(fgets(buf,sizeof(buf),stdin)!=NULL)
  {
    int n=sendto(sockfd,buf,strlen(buf),0,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    if(n==-1)
    {
      perror("sendto error");
      exit(-1);
    }
    n=recvfrom(sockfd,buf,sizeof(buf),0,NULL,NULL);
    if(n==-1)
    {
      perror("recvfrom error");
      exit(-1);
    }
    write(STDOUT_FILENO,buf,n);
  }
  close(sockfd);
  return 0;
}
