#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
struct ListNode
{
  int num;
  struct ListNode* next;
};
struct ListNode* head=NULL;
void SysError(char* str,int err)
{
  fprintf(stderr,"%s error:%s\n",str,strerror(err));
  exit(-1);
}

pthread_mutex_t lock;
pthread_cond_t cond;
void* Product(void* arg)
{
  int i=50;
  struct ListNode* tmp;
  while(i--)
  {
    //生产资源
  tmp=(struct ListNode*)malloc(sizeof(struct ListNode));
  tmp->num=rand()%1000+1;
  //放置资源
  pthread_mutex_lock(&lock);
  tmp->next=head;
  head=tmp;
  pthread_mutex_unlock(&lock);
  printf("生产了%d\n",tmp->num);
  pthread_cond_signal(&cond);
  sleep(rand()%4);
  }
  return NULL;
}
void* Comsumer(void* arg)
{
  struct ListNode* tmp=NULL;
  int i=50;
  while(i--)
  {
  while(head==NULL)
  {
    //消费资源
    pthread_cond_wait(&cond,&lock);
  }
  tmp=head;
  head=head->next;
  pthread_mutex_unlock(&lock);
  printf("消费了%d\n",tmp->num);
  free(tmp);
  sleep(rand()%4);
  }
  return NULL;
}
int main()
{
  int ret;
  srand(time(NULL));
  pthread_t pid,cid;
  ret=pthread_mutex_init(&lock,NULL);   //初始化锁和条件
  if(ret!=0)
  {
    SysError("mutex_init",ret);
  }
  ret=pthread_cond_init(&cond,NULL);
  if(ret!=0)
  {
    SysError("cond_init",ret);
  }
  ret=pthread_create(&pid,NULL,Product,NULL);  //创建线程
  if(ret!=0)
  {
    SysError("pthread_create",ret);
  }
  ret=pthread_create(&cid,NULL,Comsumer,NULL);
  if(ret!=0)
  {
    SysError("pthread_create",ret);
  }
  
  pthread_join(pid,NULL);   //回收资源
  pthread_join(cid,NULL);
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&cond);
  return 0;
}
