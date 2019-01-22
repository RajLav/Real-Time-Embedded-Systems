
/* Name :- raj lavingia
 * Date :- 07/18/10018
 * credits :- Example by Sam Siewert 
 * Updated 10/29/16 for OpenCV 3.1
 */



//Standard Libraries Defined
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <sched.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>
#include <semaphore.h>
using namespace cv;
using namespace std;

//Defining pthreads
pthread_t pika,pika1,pika2;

//Structure defined for schedular
struct sched_param main_param;

//pthread attribute
pthread_attr_t main_attr;
pid_t mainpid;

CvCapture* capture;

//Structure Defined for frame 
struct timespec time1;
struct timespec time2;
struct timespec time3;
//defining horizontal and vertical resolution
#define HRES 500
#define VRES 375
 //500 375
//480 360
//80 60

// Transform display window
char timg_window_name[] = "Canny Edge Detector";
sem_t semC,semH,semE;

//Variables defined for Canny Threshold
int lowThreshold=0;
int const max_lowThreshold = 100;
int kernel_size = 3;
int edgeThresh = 1;
int ratio = 3;
int dev=0;


//Defining variables for jitter calculation , fps, frame time
int rt_max_prio, rt_min_prio;

double summation=0;
double summation1=0;
double summation2=0;
//Counter for Frames
double count1=0;
double count2=0;
double count3=0;

double p_t=0, past=0, fps2=0,result=0;
double p_t3=0, past1=0, fps3=0, result1=0;
double p_t6=0, past2=0,fps7=0,result2=0;

double fps = 0 ; double fps1;
double fps5 = 0; double fps6;
double fps8=0; double fps9;

double d_c=85;
double d_h = 800;
double d_e = 830;

double j_c;
double j_h;
double j_e;

double j_p=0, n_j=0;
double j_p1=0, n_j1=0;
double j_p2=0, n_j2=0;



//Convert image to grayscale from grid
Mat canny_frame, cdst, timg_gray, timg_grad;

//Image capture
IplImage* frame;

//Schedular function
void print_scheduler(void)
{
   int schedType;

   schedType = sched_getscheduler(getpid());

   switch(schedType)
   {
     case SCHED_FIFO:
           printf("Pthread Policy is SCHED_FIFO\n");
           break;
     case SCHED_OTHER:
           printf("Pthread Policy is SCHED_OTHER\n");
       break;
     case SCHED_RR:
           printf("Pthread Policy is SCHED_OTHER\n");
           break;
     default:
       printf("Pthread Policy is UNKNOWN\n");
   }
}


double p_t1,p_t2;
double p_t4,p_t5;
double p_t7,p_t8;

void *HoughCircle(void *c)
{
int rc2,scope2;

while(1)
{
	sem_wait(&semE);
        cvNamedWindow("Hough Circles", CV_WINDOW_AUTOSIZE);

	capture = (CvCapture *)cvCreateCameraCapture(dev);
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES);
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES);

        frame=cvQueryFrame(capture);
        if(!frame) break;
        else
        {
                //timestamp
                clock_gettime(CLOCK_REALTIME,&time3);
		p_t7=time3.tv_sec*1000;
                printf("time1 : %0.2f \n \t ",p_t7);
                p_t8=((double)time3.tv_nsec/1000000);
                p_t6 = ((double)p_t7 + (double)p_t8);
                printf(" time 2 : %0.2f \n \t",p_t7);
                count3++;
                printf(" Frame count is %d \n \t " ,(int)count3);
                //timestamp starting from 4th frame
		if(count3>2)
                {

                        fps8 = ((((int)count3-1)*fps8+(p_t6-past2))/(int)count3);
                        fps7 = 1 / (fps8/1000);
                }
        }
	Mat gray;
        vector<Vec3f> circles;
	Mat mat_frame(cvarrToMat(frame));
        // Does not work in OpenCV 3.1
        //cvtColor(mat_frame, gray, CV_BGR2GRAY);
        cvtColor(mat_frame, gray, COLOR_BGR2GRAY);

        GaussianBlur(gray, gray, Size(9,9), 2, 2);

        HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, gray.rows/8, 100, 50, 0, 0);

        printf("circles.size = %ld\n", circles.size());

        for( size_t i = 0; i < circles.size(); i++ )
        {
          Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
          int radius = cvRound(circles[i][2]);
          // circle center
          circle( mat_frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
          // circle outline
          circle( mat_frame, center, radius, Scalar(0,0,255), 3, 8, 0 );
        }

        // Does not work in OpenCV 3.1
        //cvShowImage("Capture Example", frame);

        imshow("Hough Circles", mat_frame);
	 result2=p_t6-past2;
        past2=p_t6;
        summation2=summation2+fps7;
	if(count3>2)
        {
                j_e=d_e-result2;
                if(j_e<0)
                {
  printf(" time in ms  =%d,  deadline missed at instant = %d \n",(int)result2,(int)count3);

                        n_j2=n_j2+j_e;

                }
                else
                {
                        printf("=deadline is not missed at instant %d\n",(int)j_e);
                        j_p2=j_p2+j_e;
                }
        }

                if((int)count3%10==0)
        {
                fps7=summation2/count3;

                printf("fps= %f\n",fps7);

                printf("final jitter calculated = %d\n",(((int)j_p2-(int)n_j2)/10));
                summation2=0;

        }

        char c = cvWaitKey(10);
        if( c == 'q' )
	{
	printf("Quit \n ");
	 break;
	}


    cvReleaseCapture(&capture);
   sem_post(&semC);
}

}


void *HoughLines(void *b)
{

int rc1,scope1;
       

while(1)
{
	sem_wait(&semH);

	cvNamedWindow("Hough Lines", CV_WINDOW_AUTOSIZE);
	capture = (CvCapture *)cvCreateCameraCapture(dev);
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES);
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES);

	frame=cvQueryFrame(capture);
        if(!frame) break;
        else
        {
		//timestamp
                clock_gettime(CLOCK_REALTIME,&time2);
		p_t4=time2.tv_sec*1000;
		printf("time1 : %0.2f \n \t ",p_t4);
		p_t5=((double)time2.tv_nsec/1000000);
		p_t3 = ((double)p_t4 + (double)p_t5);
		printf(" time 2 : %0.2f \n \t",p_t5);
		count2++;
		printf(" Frame count is %d \n \t " ,(int)count2);
		//timestamp starting from 4th frame
                if(count2>2)
                {

			fps5 = ((((int)count2-1.0)*fps5+(p_t3-past1))/(int)count2);
                        fps3 = 1 / (fps5/1000);
                }
        }



            Mat gray, canny_frame, cdst;
            vector<Vec4i> lines;
         Mat mat_frame(cvarrToMat(frame));
        Canny(mat_frame, canny_frame, 50, 200, 3);

        cvtColor(canny_frame, cdst, CV_GRAY2BGR);
        cvtColor(mat_frame, gray, CV_BGR2GRAY);

        HoughLinesP(canny_frame, lines, 1, CV_PI/180, 50, 50, 10);

	for( size_t i = 0; i < lines.size(); i++ )
        {
          Vec4i l = lines[i];
          line(mat_frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
        }

        imshow("Hough Lines",mat_frame);

	result1=p_t3-past1;
        past1=p_t3;
        summation1=summation1+fps3;

	if(count2>2)
        {
                j_h=d_h-result1;
                if(j_h<0)
                {
                        printf(" time in ms  =%d,  deadline missed at instant = %d \n",(int)result1,(int)count2);
                        n_j1=n_j1+j_h;

                }
                else
                {
                        printf("deadline is not missed at instant =%d\n",(int)j_h);
                        j_p1=j_p1+j_h;
                }
        }

                if((int)count2%10==0)
        {
                fps3=summation1/count2;
		printf(" \n \t \n \t \n \t \n \t ");
                printf("fps= %f\n",fps3);
                
                printf("final  jitter calculated = %d\n",(((int)j_p1-(int)n_j1)/10));
                summation1=0;

        }

        char c = cvWaitKey(10);
        if( c == 'q' )
	{
	printf("got quit \n ");
	 break;
         }

    cvReleaseCapture(&capture);
   sem_post(&semE);

}

}


void *Cannyfunction(void *a)
{
 int rc, scope;

while(1)
{
 	sem_wait(&semC);

//        int rc, scope;

    namedWindow( timg_window_name, CV_WINDOW_AUTOSIZE );

    // Create a Trackbar for user to enter threshold
    createTrackbar( "Min Threshold:", timg_window_name, &lowThreshold, max_lowThreshold, 0,0 );

    capture = (CvCapture *)cvCreateCameraCapture(dev);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES);

    
//	sem_wait(&semC);

        frame=cvQueryFrame(capture);

	//frame break used in case of missing frame
        if(!frame) break;

	else
	{
		//timestamp
		clock_gettime(CLOCK_REALTIME,&time1);
		p_t1 =time1.tv_sec*1000;
		printf("time1 : %0.2f \n \t ",p_t1);
		p_t2 = ((double)time1.tv_nsec/1000000);
		p_t=((double)p_t1+(double)p_t2);
		printf("time2 : %0.2f \n \t",p_t2);

		count1++;

		printf(" Frame count is %d \n \t " ,(int)count1);

		//timestamp starting from 4th frame
		if(count1>2)
		{
			fps=((((int)count1-1.0)*fps+(p_t-past))/(int)count1);
        		fps2=1/(fps/1000);
		}
	}


    //Mat mat_frame(frame);
    Mat mat_frame(cvarrToMat(frame));

    cvtColor(mat_frame, timg_gray, CV_RGB2GRAY);

    /// Reduce noise with a kernel 3x3
    blur( timg_gray, canny_frame, Size(3,3) );

    /// Canny detector
    Canny( canny_frame, canny_frame, lowThreshold, lowThreshold, kernel_size );

    /// Using Canny's output as a mask, we display our result
    timg_grad = Scalar::all(0);

    mat_frame.copyTo( timg_grad, canny_frame);

    imshow( timg_window_name, timg_grad );


	//result frame
	result=p_t-past;
        
	past=p_t;
	
	summation=summation+fps2;
	 if(count1>2)
        {
                j_c=d_c-result;
                if(j_c<0)
                {
                        printf("time in ms =%d,  deadline missed at instant  = %d\n",(int)result,(int)count1);
			n_j=n_j+j_c;

                }
                else
                {
                        printf("deadline not missed at instant =%d\n",(int)j_c);
                	j_p=j_p+j_c;
		}
        }

	        if((int)count1%10==0)
        {
                fps2=summation/count1;
		printf(" \n \t \n \t  ");
		 printf(" \n \t \n \t  ");
		 printf(" \n \t \n \t  ");
		 printf(" \n \t \n \t  ");

		 printf("fps= %d\n \t",(int)fps2);
                printf("final jitter calculated = %d \n \t",(((int)j_p-(int)n_j)/10));
		summation=0;
        }

//stop key
 char q = cvWaitKey(33);
        if( q == 'q' )
        {
            printf("got quit\n"); 
        }

    

	cvReleaseCapture(&capture);
	sem_post(&semH);
}

}


int main( int argc, char** argv )

{

sem_init(&semC, 0 ,1);
sem_init(&semH, 0 ,0);
sem_init(&semE, 0 ,0);

pthread_attr_t rt_sched_attr;

   int rc;
   int i, scope;
   cpu_set_t cpuset;

   mainpid=getpid();

	 if(argc > 1)
    {
        sscanf(argv[1], "%d", &dev);
        printf("using %s\n", argv[1]);
    }
    else if(argc == 1)
        printf("using default\n");

    else
    {
        printf("usage: capture [dev]\n");
        exit(-1);
    }

struct sched_param rt_param;

	// All threads are set with same priority of 98
	rt_param.sched_priority=98;

	//Pthread Attributes and Parameters Init
	pthread_attr_init(&rt_sched_attr);
	pthread_attr_setinheritsched(&rt_sched_attr,PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&rt_sched_attr,SCHED_FIFO);
	pthread_attr_setschedparam(&rt_sched_attr,&rt_param);
   print_scheduler();
   rc=sched_getparam(mainpid, &main_param);
   main_param.sched_priority=rt_max_prio;
   rc=sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
//   if(rc < 0) perror("main_param");
 //  print_scheduler();

 rt_max_prio = sched_get_priority_max(SCHED_FIFO);
   rt_min_prio = sched_get_priority_min(SCHED_FIFO);
while(1)
{


//pthread create and join

	pthread_create(&pika,NULL,Cannyfunction,NULL );
	pthread_create(&pika1,NULL,HoughLines,NULL);
	pthread_create(&pika2,NULL,HoughCircle,NULL);

        pthread_join(pika, NULL);
	pthread_join(pika1,NULL);
	pthread_join(pika2,NULL);

}


};



