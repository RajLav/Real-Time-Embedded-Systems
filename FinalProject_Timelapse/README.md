**#Project : Time Lapse Video  
#Author : Raj Lavingia
#Term : Summer 18**

This project was made in order to make a time lapse video in order to save time from continuous evaluation of a subject.

**Time Lapse :** Time-lapse photography is a technique whereby the frequency at which film frames are captured is much more spread out than the frequency used to view the sequence.
When played at normal speed, time appears to be moving faster and thus lapsing. [Wiki]

In this project with the help of R-Pi micro controller and Camera C270, I implemented the capture rates of 1Hz, 5Hz and 10Hz.

The frames when captured at the above rates were stored real time into the memory of the R-pi. 1Hz frame capture rate means 1 frame per second.

This process had 0 jitter into the system which is explained by the video attached.

In 10Hz frequency there was jitter of nearly 1%. This was due to the limitation of the camera specification.

While the images were being captured by the camera and stored using OPENCV functions, they were also transmitted to the laptop with the help
of socket programming.

The images which were stored into the R-pi were also being converted to Video in real time.

**Concepts used :** multi threading, semaphores and mutex

**Software :** OpenCV installed on R-Pi

**Process :** Image capture - Store - Video Made - Sent to laptop

**Platform :** Linux
