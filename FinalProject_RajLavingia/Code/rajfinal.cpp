/*Name :- Raj Lavingia
Credits :- Sam Siewert
*/

//Defining Libraries
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <semaphore.h>
#include <pthread.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc.hpp>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sstream>
#include <sched.h>
#include <time.h>
#include <syslog.h>
#include <sys/time.h>
#include <errno.h>

using namespace cv;
using namespace std;

//Defining time
#define USEC_PER_MSEC (1000)
#define NANOSEC_PER_SEC (1000000000)
#define NUM_CPU_CORES (1)
#define TRUE (1)
#define FALSE (0)

//Defining number of threads
#define NUM_THREADS (7+1)

//Resolution of an image
#define HRES 640
#define VRES 480

//Abort test
int abortTest=FALSE;
int abortS1=FALSE, abortS2=FALSE, abortS3=FALSE, abortS4=FALSE, abortS5=FALSE, abortS6=FALSE, abortS7=FALSE;

//Defining number of frames
int framecount=0;

//Opencv command for frame
IplImage* frame;

//opencv Command for capture of an image
CvCapture* capture;

//Timeval function for second and microsecond
struct timeval start_time_val,current_time_val;

//Structure defined for thread identification
typedef struct
{
    int threadIdx; //Defining thread ID
    unsigned long long sequencePeriods; //Numbe of cycle it will run
} threadParams_t;

//Define semaphores
sem_t pika1,pika2,pika3,pika4;

//For defining sequence of the services
void *Sequencer(void *threadp);
	ofstream plot;
char jitteringraph[]="graph.csv";
//to run syslog = less /var/log/syslog
//to run mp4 = ffmpeg -i input.avi output.mp4
// to make video = ffmpeg -f image2 -i image%d.jpg video.avi
 
//For defining Services
void *Service_1(void *threadp);
void *Service_2(void *threadp);

double getTimeMsec(void);
void print_scheduler(void);

//Defining Timespec
struct timespec start3,end3,start2,end2,start4,end4,start5,end5;
double gap3;
char forppm[35];
char forjpg[35];

pthread_mutex_t mutexA;
int main(int argc, char** argv)
{
	
        pthread_mutex_init(&mutexA,NULL); 
    int i, rc, scope;
	
    cpu_set_t threadcpu;
    pthread_t threads[NUM_THREADS];
    threadParams_t threadParams[NUM_THREADS];
    pthread_attr_t rt_sched_attr[NUM_THREADS];
    int rt_max_prio, rt_min_prio;
    struct sched_param rt_param[NUM_THREADS];
    struct sched_param main_param;
    pthread_attr_t main_attr;
    pid_t mainpid;
    cpu_set_t allcpuset;
    printf("Starting Sequencer Demo\n");
    gettimeofday(&start_time_val, (struct timezone *)0);
    gettimeofday(&current_time_val, (struct timezone *)0);
    syslog(LOG_CRIT, "Sequencer @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);

  // printf("System has %d processors configured and %d available.\n", get_nprocs_conf(), get_nprocs());

   CPU_ZERO(&allcpuset);

   for(i=0; i < NUM_CPU_CORES; i++)
       CPU_SET(i, &allcpuset);
//run on 1 core only
   printf("Using CPUS=%d from total available.\n", CPU_COUNT(&allcpuset));


    // initialize the sequencer semaphores
    //
    if (sem_init (&pika1, 0, 0)) { printf ("Failed to initialize S1 semaphore\n"); exit (-1); }
    if (sem_init (&pika2, 0, 0)) { printf ("Failed to initialize S2 semaphore\n"); exit (-1); }

//get the id of the processor
    mainpid=getpid();

//to set the priority 99 max min 1
    rt_max_prio = sched_get_priority_max(SCHED_FIFO);
    rt_min_prio = sched_get_priority_min(SCHED_FIFO);

//main param  = policy set, priorty
    rc=sched_getparam(mainpid, &main_param);
    main_param.sched_priority=rt_max_prio;
    rc=sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
    if(rc < 0) perror("main_param");
 


    pthread_attr_getscope(&main_attr, &scope);

    if(scope == PTHREAD_SCOPE_SYSTEM)
      printf("PTHREAD SCOPE SYSTEM\n");
    else if (scope == PTHREAD_SCOPE_PROCESS)
      printf("PTHREAD SCOPE PROCESS\n");
    else
      printf("PTHREAD SCOPE UNKNOWN\n");

    printf("rt_max_prio=%d\n", rt_max_prio);
    printf("rt_min_prio=%d\n", rt_min_prio);

    for(i=0; i < NUM_THREADS; i++)
    {
//core set
      CPU_ZERO(&threadcpu);
      CPU_SET(3, &threadcpu);

	//Attributes given
      rc=pthread_attr_init(&rt_sched_attr[i]);
      rc=pthread_attr_setinheritsched(&rt_sched_attr[i], PTHREAD_EXPLICIT_SCHED);
      rc=pthread_attr_setschedpolicy(&rt_sched_attr[i], SCHED_FIFO);
      //rc=pthread_attr_setaffinity_np(&rt_sched_attr[i], sizeof(cpu_set_t), &threadcpu);

      rt_param[i].sched_priority=rt_max_prio-i;
      pthread_attr_setschedparam(&rt_sched_attr[i], &rt_param[i]);

      threadParams[i].threadIdx=i;
    }
   
    printf("Service threads will run on %d CPU cores\n", CPU_COUNT(&threadcpu));

    // Create Service threads which will block awaiting release for:
    //

    // Servcie_1 = RT_MAX-1	@ 1 Hz
    //
    rt_param[1].sched_priority=rt_max_prio-1;
    pthread_attr_setschedparam(&rt_sched_attr[1], &rt_param[1]);
    rc=pthread_create(&threads[1],               // pointer to thread descriptor
                      &rt_sched_attr[1],         // use specific attributes
                      //(void *)0,               // default attributes
                      Service_1,                 // thread function entry point
                      (void *)&(threadParams[1]) // parameters to pass in
                     );
    if(rc < 0)
        perror("pthread_create for service 1");
    else
        printf("pthread_create successful for service 1\n");


    // Service_2 = RT_MAX-2	@ 1 Hz
    //
    rt_param[2].sched_priority=rt_max_prio-2;
    pthread_attr_setschedparam(&rt_sched_attr[2], &rt_param[2]);
    rc=pthread_create(&threads[2], &rt_sched_attr[2], Service_2, (void *)&(threadParams[2]));
    if(rc < 0)
        perror("pthread_create for service 2");
    else
        printf("pthread_create successful for service 2\n");


 
    // Create Sequencer thread, which like a cyclic executive, is highest prio
    printf("Start sequencer\n");
    threadParams[0].sequencePeriods=80000;

    // Sequencer = RT_MAX	@ 30 Hz
    //
	//thread create checking     
	rt_param[0].sched_priority=rt_max_prio;
    pthread_attr_setschedparam(&rt_sched_attr[0], &rt_param[0]);
    rc=pthread_create(&threads[0], &rt_sched_attr[0], Sequencer, (void *)&(threadParams[0]));
    if(rc < 0)
        perror("pthread_create for sequencer service 0");
    else
        printf("pthread_create successful for sequeencer service 0\n");

	//Thread joining
   for(i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL);

	 printf("\nTEST COMPLETE\n");
}

void *Sequencer(void *threadp)
{ 
	//for my while loop
	long long int loop=1;
	//structure for current time in sec and msec
    struct timeval current_time_val;
    struct timespec delay_time = {0,33000000};
	//timimng defined
    struct timespec remaining_time;

	//calling  structure from the above defined for thread ID
     threadParams_t *threadParams = (threadParams_t *)threadp;
	//timestamp parameters
	struct timespec start1, end1;
	//for difference of timestamp
	double gap1;
	
	//Define my sleep time for delay generation 
	while(1)
	{
		//for timestamp
		clock_gettime(CLOCK_REALTIME,&start1);	

		nanosleep(&delay_time,&remaining_time);//sleep time (may be awaken early
		loop++; //increment loop number infinite times
		if(loop%3==0) //3 for 10hz, 30 for 1Hz
		{
			pthread_mutex_lock(&mutexA);
			sem_post(&pika1);
			pthread_mutex_unlock(&mutexA);	
		}
		clock_gettime(CLOCK_REALTIME,&end1);
		gap1 = (end1.tv_sec-start1.tv_sec);
		syslog(LOG_CRIT,"loop timing",gap1);
	}

}

void *Service_1(void *threadp)
{
	//Defined for putting text on the screen
	CvFont font;
	char c1[35];
	//Timestamp Variables
	double gap2,gap3;
	//Capture images from camera
	capture=(CvCapture*)cvCreateCameraCapture(0);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,HRES);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,VRES);
	//Open window on the teminal
	cvNamedWindow("Window Opening",CV_WINDOW_AUTOSIZE);

	//Graph f jitter plot
	plot.open(jitteringraph);	
	plot << "Frame Number , Difference\r \n ";

	std::ofstream plot1;
	
	while(framecount<6000)
	{
		sem_wait(&pika1);
		clock_gettime(CLOCK_REALTIME,&start2);
	//for time 
		gettimeofday(&current_time_val, (struct timezone *)0);
    	//for seeing log on the kernel
		syslog(LOG_CRIT, "Frame Sampler thread @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);
    	//	printf("Frame Sampler thread @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);
	
	//For capturing the frame
	frame=cvQueryFrame(capture);
	//For converting frame into mat frame
	//Convert farme into Mat frame
	Mat mat_frame(cvarrToMat(frame));
		
	//if frame >6000 then break out of the loop
	if(!frame) break;
	//Excel file open
	plot1.open(jitteringraph,std::ios_base::app);
	plot1 << framecount;
	plot1 << ",";
	plot1 << gap3;
	plot1 << "\r\n";
	framecount++;	
	//excel file close
	plot1.close();
	//generate timestamp on the image
	sprintf(c1,"		time right now is %d",start2);	
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX,0.4,0.4);
	cvPutText(frame,c1,cvPoint(30,30),&font,cvScalar(255,255,255));
	
	clock_gettime(CLOCK_REALTIME,&start3);
	gettimeofday(&current_time_val, (struct timezone *)0);
       
//	syslog(LOG_CRIT, "Frame Sampler release %llu @ sec=%d, msec=%d\n", S1Cnt, (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);
      	gettimeofday(&current_time_val, (struct timezone *)0);
	syslog(LOG_CRIT, "Time-stamp with Image Analysis thread @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);
	printf("Frame Number is %d,sec= %d \n ",framecount,(int)(current_time_val.tv_sec-start_time_val.tv_sec));		
	clock_gettime(CLOCK_REALTIME,&end3);
	gap2 = (end3.tv_sec-start3.tv_sec);
	syslog(LOG_CRIT,"Time Difference is : ",gap2);
	clock_gettime(CLOCK_REALTIME,&end2);
	gap3 = (end2.tv_sec-start2.tv_sec);
	
	syslog(LOG_CRIT,"Time Difference 2 is:",gap3);

	sem_post(&pika2);
	}
	

}

void *Service_2(void *threadp)
{
	while(1)
	{
	sem_wait(&pika2);
	clock_gettime(CLOCK_REALTIME,&start4);
	Mat mat_frame(cvarrToMat(frame));
	//Convert Image data from one file format into another
	sprintf(forppm,"IMG%d.ppm",framecount);
	sprintf(forjpg,"IMG%d.jpg",framecount);
	clock_gettime(CLOCK_REALTIME,&end4);
	gap3 = (end4.tv_sec - start4.tv_sec);
	syslog(LOG_CRIT,"Timne difference 3 is :",gap3);
	clock_gettime(CLOCK_REALTIME,&start5);
	imwrite(forppm,mat_frame);
	imwrite(forjpg,mat_frame);
	clock_gettime(CLOCK_REALTIME,&end5);
	gettimeofday(&current_time_val, (struct timezone *)0);
	syslog(LOG_CRIT, "Time-stamp with Image Analysis thread @ sec=%d \n", (int)(current_time_val.tv_sec-start_time_val.tv_sec));	
//	framecount++;
//	plot.close();
	}
}
