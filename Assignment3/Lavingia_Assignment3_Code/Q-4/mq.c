/*Name :- Raj lavingia
Date :- 07.08.2018
Credits :- Sam Siewert
*/
                                                                    
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<pthread.h>
#include<mqueue.h>
#include<sys/stat.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<mqueue.h>
#define ERROR -1
#define SNDRCV_MQ "/send_receive_mq_heap"

struct mq_attr mq_attr;				
static mqd_t mymq;
pthread_t r,s; 

pthread_attr_t attribute[10];	
struct sched_param parameter[10];
void *receiver(void *t)
{
  char buffer[sizeof(void *)+sizeof(int)];
  void *buffptr; 
  int prio;
  int nbytes;
  int count = 0;
  int id;
 
  while(1) {

    /* read oldest, highest priority msg from the message queue */

    printf("\nReading %ld bytes\n", sizeof(void *));
  
    if((nbytes = mq_receive(mymq, buffer, (size_t)(sizeof(void *)+sizeof(int)), &prio)) == ERROR)

    {
      perror("mq_receive");
    }
    else
    {
      memcpy(&buffptr, buffer, sizeof(void *));
      memcpy((void *)&id, &(buffer[sizeof(void *)]), sizeof(int));
      printf("\nReceive: ptr msg 0x%p received with priority = %d, length = %d, id = %d\n", buffptr, prio, nbytes, id);

      printf("\nContents of ptr = \n%s\n", (char *)buffptr);

      free(buffptr);

      printf("\nHeap space memory freed\n");

    }
    
  }

}


static char imagebuff[4096];

void *sender(void *t)
{
  char buffer[sizeof(void *)+sizeof(int)];
  void *buffptr;
  int prio;
  int nbytes;
  int id = 999;


  while(1) {

    /* send malloc'd message with priority=30 */

    buffptr = (void *)malloc(sizeof(imagebuff));
    strcpy(buffptr, imagebuff);
    printf("\nMessage to send = %s\n", (char *)buffptr);

    printf("\nSending %ld bytes\n", sizeof(buffptr));

    memcpy(buffer, &buffptr, sizeof(void *));
    memcpy(&(buffer[sizeof(void *)]), (void *)&id, sizeof(int));

    if((nbytes = mq_send(mymq, buffer, (size_t)(sizeof(void *)+sizeof(int)), 30)) == ERROR)
    {
      perror("mq_send");
    }
    else
    {
      printf("\nSend: message ptr 0x%p successfully sent\n", buffptr);
    }

    sleep(3);

  }
  
}


static int sid, rid;

//Main Function
void main()
{
  int i, j;
  char pixel = 'A';
  int rc;

  for(i=0;i<4096;i+=64) {
    pixel = 'A';
    for(j=i;j<i+64;j++) {
      imagebuff[j] = (char)pixel++;
    }
    imagebuff[j-1] = '\n';
  }
  imagebuff[4095] = '\0';
  imagebuff[63] = '\0';

  printf("buffer =\n%s", imagebuff);
 
  /* setup common message q attributes */
  mq_attr.mq_maxmsg = 100;
  mq_attr.mq_msgsize = sizeof(void *)+sizeof(int);

  mq_attr.mq_flags = 0;

  /* note that VxWorks does not deal with permissions? */
  mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, 0, &mq_attr);


 pthread_create(&r, NULL, receiver, NULL);
 pthread_create(&s, NULL, sender, NULL);

  pthread_join(r, NULL);
  pthread_join(s, NULL);


  
}

