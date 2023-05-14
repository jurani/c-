#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define N 5
int Queue[N];  //创建循环队列

void SysError(char* str)
{
  perror(str);
  exit(-1);
}

sem_t blank,star;
pthread_t pid,cid;
void* Comsumer(void* arg)
{
  int j=0;
  int i=50;
  while(i--)
  {  // 消费数据
    sem_wait(&star);
    printf("消费了%d\n",Queue[j]);
    Queue[j]=0;
    sem_post(&blank);
    j=(j+1)%N;
    sleep(rand()%4);
  }
  return NULL;
}
void* Product(void* arg)
{

  int j=0;
  int i=50;
  while(i--)
  {
    //生产数据
  sem_post(&star);
  Queue[j]=rand()%1000+1;
  printf("生产了%d\n",Queue[j]);
  sem_wait(&blank);
  j=(j+1)%N;
  sleep(rand()%4);
  }
  return NULL;
}
int main()
{
  int ret;

  srand(time(NULL));
  ret=sem_init(&blank,0,5);  //初始化信号量
  if(ret==-1)
  {
    SysError("sem_init error");
  }
  ret=sem_init(&star,0,0);
  if(ret==-1)
  {
    SysError("sem_init error");
  }
  ret=pthread_create(&pid,NULL,Product,NULL);  //创建线程
  if(ret!=0)
  {
    SysError("pthread_create");
  }
  ret=pthread_create(&cid,NULL,Comsumer,NULL);
  if(ret!=0)
  {
    SysError("pthread_create");
  }
  
  pthread_join(pid,NULL);   //回收资源
  pthread_join(cid,NULL);
  sem_destroy(&blank);
  sem_destroy(&star);
  return 0;
}
