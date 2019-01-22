#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

typedef unsigned long ip_address;
typedef unsigned short uint16;

static const int BUFFER_SIZE = 16*1024;  //16kb data
static void UsageExit()
{
   printf("Usage:  xfer (send/receive) filename [host:Port]\n");
   exit(10);
}

static ip_address GetHostByName(const char * name)
{
   struct hostent * he = gethostbyname(name);
   return ntohl(he ? *((ip_address*)he->h_addr) : 0);
}

static void SendFile(ip_address ipAddress, uint16 port, const char * fileName)  
{
   FILE * fpIn = fopen(fileName, "r");
   if (fpIn)
   {
      int s = socket(AF_INET, SOCK_STREAM, 0);
      if (s >= 0)
      {
         struct sockaddr_in saAddr; memset(&saAddr, 0, sizeof(saAddr));
         saAddr.sin_family      = AF_INET;
         saAddr.sin_addr.s_addr = htonl(ipAddress);
         saAddr.sin_port        = htons(port);

         if (connect(s, (struct sockaddr *) &saAddr, sizeof(saAddr)) == 0)
         {
            printf("Connected to remote host, sending file [%s]\n", fileName);

            char buf[BUFFER_SIZE];
            while(1)
            {
               ssize_t bytesRead = fread(buf, 1, sizeof(buf), fpIn);
               if (bytesRead <= 0) break;
               printf("Read %i bytes from file, sending them to network...\n", (int)bytesRead);
               if (send(s, buf, bytesRead, 0) != bytesRead)
               {
                  perror("send");
                  break;
               }
            }
         }
         else perror("connect");

         close(s);
      }
      else perror("socket");

      fclose(fpIn);
   }
   else printf("Error, couldn't open file [%s] to send!\n", fileName);
}

static void ReceiveFile(uint16 port, const char * fileName)    //client code, put in client file 
{
   int s = socket(AF_INET, SOCK_STREAM, 0);
   if (s >= 0)
   {
#ifndef WIN32
      
      const int trueValue = 1;
      (void) setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &trueValue, sizeof(trueValue));
#endif

      struct sockaddr_in saAddr; memset(&saAddr, 0, sizeof(saAddr));
      saAddr.sin_family      = AF_INET;
      saAddr.sin_addr.s_addr = htonl(0);
      saAddr.sin_port        = htons(port);

      if ((bind(s, (struct sockaddr *) &saAddr, sizeof(saAddr)) == 0)&&(listen(s, 10) == 0))
      {
         memset(&saAddr, 0, sizeof(saAddr));
         socklen_t len = sizeof(saAddr);
         printf("Waiting for incoming TCP connection on port %u, to write file [%s]\n", port, fileName);
         int connectSocket = accept(s, (struct sockaddr *) &saAddr, &len);
         if (connectSocket >= 0)
         {
            FILE * fpIn = fopen(fileName, "w");
            if (fpIn)
            {
               char buf[BUFFER_SIZE];
               while(1)
               {
                  ssize_t bytesReceived = recv(connectSocket, buf, sizeof(buf), 0);
                  if (bytesReceived < 0) perror("recv");
                  if (bytesReceived == 0) break;

                  printf("Received %i bytes from network, writing them to file...\n", (int) bytesReceived);
                  if (fwrite(buf, 1, bytesReceived, fpIn) != (size_t) bytesReceived)
                  {
                     perror("fwrite");
                     break;
                  }
               }

               fclose(fpIn);
            }
            else printf("Error, couldn't open file [%s] to receive!\n", fileName);

            close(connectSocket);
         }
      }
      else perror("bind");

      close(s);
   }
   else perror("socket");
}

int main(int argc, char ** argv) 
{
   if (argc < 4) UsageExit();

#ifdef WIN32
   WORD versionWanted = MAKEWORD(1, 1);
   WSADATA wsaData;
   (void) WSAStartup(versionWanted, &wsaData);
#else 
   signal(SIGPIPE, SIG_IGN);
#endif

   bool isSend = false;
        if (strcmp(argv[1], "send")    == 0) isSend = true;    //argument code 
   else if (strcmp(argv[1], "receive") != 0) UsageExit();

   const char * fileName = argv[2];       // filename wala argument

   ip_address ip = isSend ? GetHostByName(argv[3]) : 0;
   uint16 port = 0;
   const char * portColon = strchr(argv[3], ':');
   port = portColon ? atoi(portColon+1) : 0;
   if (port == 0) port = 9999;

   if (isSend) SendFile(ip, port, fileName);
          else ReceiveFile(port, fileName);

   printf("Exiting, bye!\n");
   return 0;
}