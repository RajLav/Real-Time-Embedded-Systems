/* Name :- Raj Lavingia
Date :- 07.06.2018
Credits :- Sam Siewert
*/

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <pthread.h> //Defining pthread
#include <string.h> //Defining string


#define SNDRCV_MQ "/send_receive_mq"
#define MAX_MSG_SIZE 128
#define ERROR (-1)

#include <mqueue.h> // for message queue
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//Naming the structure format
struct mq_attr mq_attr;

//Receive Function (Priority is higher than sender)
void receiver(void)
{
  mqd_t mymq;// Defining my message queue
  char buffer[MAX_MSG_SIZE]; //Defining Message Size
  int prio;
  int nbytes;

  mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, S_IRWXU, &mq_attr);//For creating new queue or updating the recent one

  if(mymq == (mqd_t)ERROR)
  {
    perror("mq_open"); 
    exit(-1);
  }

  /* read oldest, highest priority msg from the message queue */
  if((nbytes = mq_receive(mymq, buffer, MAX_MSG_SIZE, &prio)) == ERROR)
  {
    perror("mq_receive");
  }
  else
  {
    buffer[nbytes] = '\0';
    printf("receive: msg %s received with priority = %d, length = %d\n",
           buffer, prio, nbytes);
  }
    
}

//Defining your string 
static char canned_msg[] = "RTES Assignment 3- Raj Lavingia";

//Sender Function
void sender(void)
{
  mqd_t mymq;
  int prio;
  int nbytes;

  mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, S_IRWXU, &mq_attr);

  if(mymq == (mqd_t)ERROR)
  {
    perror("mq_open");
  }
  else
  {
    perror("opened mq\n");
  }

  /* send message with priority=30 */
  if((nbytes = mq_send(mymq, canned_msg, sizeof(canned_msg), 30)) == ERROR)
  {
    perror("mq_send");
  }
  else
  {
    printf("send: message successfully sent\n");
  }
  
}


void main(void)
{

  /* setup common message q attributes */
  mq_attr.mq_maxmsg = 100;
  mq_attr.mq_msgsize = MAX_MSG_SIZE;

  mq_attr.mq_flags = 0;

  sender(); // For sending data
  receiver(); //For receiving package
   
}
