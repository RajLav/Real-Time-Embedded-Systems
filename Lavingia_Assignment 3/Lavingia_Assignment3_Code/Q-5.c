/*Name :- Raj lavingia
Assignment 3 Q-5 
*/

//Including header files
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<malloc.h>
#include<unistd.h>

//define pthread
pthread_t thread1, thread2; //thread 1 = read, thread2 =  write 

//define attribute structure of 
typedef struct attribute {
   double x; //x co ordinate
   double y; // y coordinate
   double z; //z coordinate
   double p; // pitch
   double r; // roll
   double a; //acceleration
   double y1; // yaw
   
 
}attribute_a;

//Structure defined for timespec
struct timespec t;
struct timespec t1;
time_t tv_spec;


//define attribute name
attribute_a navigation;

//define mutex for lock & unlock
pthread_mutex_t acceleration;

//function for reading all the attributes

void *read1(void *threadid)
{
while(1)
{
attribute_a *pika = (attribute_a *) malloc(sizeof(attribute_a));

  int a = -1;

    // delay to let the other thread grab the resource
    sleep(1);

    while(a !=0) 
{

        t1.tv_sec = time(0) + 10;
       a=  pthread_mutex_timedlock(&acceleration, &t1.tv_sec);
        if (a != 0) {
            t1.tv_sec = time(0);
            printf("No new data available at time %ld\n",t1.tv_sec);
        }
    }
 printf("Reading is started \n \t");

//mutex locking
// pthread_mutex_lock(&acceleration);
//pthread_mutex_timedlock(&acceleration,t1.0);
//reading all attributes 
   printf("X coordinate is %lf : \n \t",navigation.x);
   printf("Y coordinate is %lf : \n \t",navigation.y);
   printf("Z coordinate is %lf : \n \t",navigation.z);
   printf("pitch is %lf : \n \t",navigation.p);
   printf("roll is %lf : \n \t",navigation.r);
   printf("acceleration is %lf : \n \t",navigation.a);
   printf("yaw is %lf : \n \t",navigation.y1);

//timespec reading in seconds
   printf("time stamp reading in sec is : %ld \n \t ",t.tv_sec);

//timespec reading in nanoseconds
   printf("time stamp reading in nanosec is : %ld \n \t ",t.tv_nsec);

printf(" \n \n \n \n ");
//sleep(11);   
   pthread_mutex_unlock(&acceleration);
usleep(10);

}
}


//Function for writing all the attributes
void *write1(void *threadid)
{
while(1)
{
//mutex locking
 pthread_mutex_lock(&acceleration);

printf("Writing is started \n \t");
 

 navigation.x++;
 navigation.y++;
 navigation.z++;
 navigation.p++;
 navigation.r++;
 navigation.a++;
 navigation.y1++;
printf("X coordinate is %lf : \n \t",navigation.x);
   printf("Y coordinate is %lf : \n \t",navigation.y);
   printf("Z coordinate is %lf : \n \t",navigation.z);
   printf("pitch is %lf : \n \t",navigation.p);
   printf("roll is %lf : \n \t",navigation.r);
   printf("acceleration is %lf : \n \t",navigation.a);
   printf("yaw is %lf : \n \t",navigation.y1);
   printf("time stamp reading in sec is %ld \n \t ",t.tv_sec);

	printf("time stamp reading nanosec %ld \n \t ",t.tv_sec);
 clock_gettime(CLOCK_REALTIME, &(t));
sleep(11);
 pthread_mutex_unlock(&acceleration);
printf(" \n \n \n \n ");
usleep(100000);
}
}
int main() {
navigation.x = 5;
 navigation.y = 10;
 navigation.z = 15;
 navigation.p = 2;
 navigation.r = 3;
 navigation.a = 4;
 navigation.y1 =3;
pthread_mutex_init(&acceleration,NULL);
    //writing all attributes 


	pthread_create(&thread2, NULL, write1, NULL);
	pthread_create(&thread1, NULL, read1, NULL);


        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);

    
    exit(0);
//}
}








