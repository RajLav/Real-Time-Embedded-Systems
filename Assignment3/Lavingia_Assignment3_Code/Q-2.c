/* Name :- Raj Lavingia
Date :- 07.05.2018
Q-2 
RTES Assignment -3
*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h> //creating p thread
#include<time.h> //Timestamp function
#include<malloc.h> //Space
#include<unistd.h>
int i;


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


//define attribute name
attribute_a navigation;

//define mutex for lock & unlock
pthread_mutex_t acceleration;

struct timespec t;

//function for reading all the attributes
 
void *read1(void *threadid)
{
//	attribute_a *pika = (attribute_a *) malloc(sizeof(attribute_a));

	while(1){
		printf("Reading is started \n \t");

//mutex locking
 pthread_mutex_lock(&acceleration);
	 clock_gettime(CLOCK_REALTIME, &t);

//reading all attributes
   printf("X coordinate is %lf : \n \t",navigation.x);
   printf("Y coordinate is %lf : \n \t",navigation.y);
   printf("Z coordinate is %lf : \n \t",navigation.z);
   printf("pitch is %lf : \n \t",navigation.p);
   printf("roll is %lf : \n \t",navigation.r);
   printf("acceleration is %lf : \n \t",navigation.a);
   printf("yaw is %lf : \n \t",navigation.y1);

//timespec reading in seconds
   printf("time stamp reading is : %ld \n \t ", t.tv_sec);

//timespec reading in nanoseconds
   printf("time stamp reading is : %ld \n \t ", t.tv_nsec);

printf(" \n \n \n \n ");
  
   pthread_mutex_unlock(&acceleration);
sleep(1);
}

}

//Function for writing all the attributes
void *write1(void *threadid)
{
while(1){

printf("Writing is started \n \t");


//mutex locking
 pthread_mutex_lock(&acceleration);
 
printf("Writing is started2 \n \t");
 for(i=0;i<=2;i++)
{
	navigation.x++;
} 
for(i=0;i<=4;i++)
{
	navigation.y++;
}
for(i=0;i<=1;i++)
{ 
	navigation.z++;
}
for(i=0;i<=5;i++)
{ 
	navigation.p++;
}
for(i=0;i<=2;i++)
{ 
	navigation.r++;
}
for(i=0;i<=3;i++)
{ 
	navigation.a++;
}
	navigation.y1++;

 printf("X coordinate is %lf : \n \t",navigation.x);
   printf("Y coordinate is %lf : \n \t",navigation.y);
   printf("Z coordinate is %lf : \n \t",navigation.z);
   printf("pitch is %lf : \n \t",navigation.p);
   printf("roll is %lf : \n \t",navigation.r);
   printf("acceleration is %lf : \n \t",navigation.a);
   printf("yaw is %lf : \n \t",navigation.y1);

//timespec reading in seconds
   printf("time stamp reading is : %ld \n \t ", t.tv_sec);

//timespec reading in nanoseconds
   printf("time stamp reading is : %ld \n \t ", t.tv_nsec);

printf(" \n \n \n \n ");


 pthread_mutex_unlock(&acceleration);
sleep(1); //Delay Given for seperating pthreads
}
}


int main() 
{
 //writing & Defining all attributes
 navigation.x = 5;
 navigation.y = 10;
 navigation.z = 15;
 navigation.p = 2;
 navigation.r = 3;
 navigation.a = 4;
 navigation.y1 =3;
   
//pthread Creation 
pthread_create(&thread2, NULL, write1, NULL);
pthread_create(&thread1, NULL, read1, NULL);

//Pthread Join
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

   
    exit(0);
}
