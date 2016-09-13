#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>


#define MAX_IDLECONNCTIME 30//There is no communicatin during x seconds,the connect will be idle and disconnnected.
#define TIMEOUTSTEP 2
#define MAXCONNECTTIME (MAX_IDLECONNCTIME/(TIMEOUTSTEP*2))
#define MAXCLIENT 10

char g_debuglev = 2;
char g_ExitAllCommunicate = 0;

void *RecvDataForCurrentUser(void *fd);

void debug(int level, char *format, ...) {
    if (level > g_debuglev)
        return;
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void InitSocketServer(int portnum)
{
    int server_sockfd;
    int *client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int portreuse = 1;
    //int mNetTimeout = 1; //1S
    struct timeval NetTimeout = {TIMEOUTSTEP,0};//{s,us}    500ms {0,500000}
   // int tcp_connect = 0;

    /*create socket--IPv4 ,TCP*/
    if((server_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
    {
       perror("socket");
       return;
    }
    /* Enable address reuse */
    setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &portreuse, sizeof(portreuse) );//reuse the port
    setsockopt( server_sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&NetTimeout, sizeof(struct timeval) );//send timeout
    setsockopt( server_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&NetTimeout, sizeof(struct timeval) );//recv timeout

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr =  INADDR_ANY;
    server_address.sin_port = htons(portnum);
    server_len = sizeof(struct sockaddr);

    //tcp_connect=fcntl(server_sockfd,F_GETFL,0);  
    //fcntl(server_sockfd,F_SETFL,tcp_connect | O_NONBLOCK);//no-block

   
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);//bind socket
    //templen = sizeof(struct sockaddr);

    /*listen request-length 5*/
    listen(server_sockfd,5);

    debug(1,"server waiting for connect\n");

    while(1)
    {
        if(g_ExitAllCommunicate)
             break;
        usleep(500000);

        pthread_t thread;//creat threads for different clients
        client_sockfd = (int *)malloc(sizeof(int));
        client_len = sizeof(struct sockaddr_in);
        *client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);

        if(*client_sockfd <= 0)
        {
            //perror("accept");
            if(client_sockfd)
            {
                free(client_sockfd);
                client_sockfd = NULL;
            }
            continue;
        }
  

/*******************************
        if(1)
        { 
          char *tempstr = NULL;
          unsigned char tempactionid[1024] = {0};
          int aIdLenH = 0;//action id length
          int aIdLenL = 0;
          int aIdLen = 0;
          int templen = 0;
          unsigned char templenstr[4]={0};
          int recvn = 0;
          int recvlen = 0;
		 
          while(recvn < 4)
          {
            recvlen = recv(*client_sockfd,&templenstr[recvn],1,0);
            if(recvlen==0)// || (tryConnectTime > MAXCONNECTTIME))

            {
                 debug(1,"client is closed or reponse timeout! recvlen=%d line %d\n",recvlen,__LINE__);
                 break;
            }
            else if(recvlen != 1)
            {
                if(errno == EAGAIN || errno == EINTR)
                    continue;
                else
                {
                    printf("ERROR! errno=%d,EAGAIN=%d,exit!\n",errno,EAGAIN);
                    break;
                }
            }printf("templenstr[%d]=%d \n",recvn,templenstr[recvn]);
            recvn++;
          }
         close(*client_sockfd);
         shutdown(*client_sockfd,2);
         if(client_sockfd)
          {
                free(client_sockfd);
                client_sockfd = NULL;
          }
            continue;
        }*/
     if(pthread_create(&thread, NULL, RecvDataForCurrentUser, client_sockfd)!=0)//create thread
     {
          perror("pthread_create");
          break;
     }
     else
     {    
         debug(1,"%s:create thread success!\n",__FUNCTION__);  
         //SERVER_LOCK;
			
         //SERVER_UNLOCK;
     }
 }

 shutdown(server_sockfd,2);
 if(client_sockfd)
    free(client_sockfd);
}

/*****************************************
* name：RecvDataForCurrentUser
* function：receive the message from the client
* parm：fd--socket

* return：void
*****************************************/
void *RecvDataForCurrentUser(void *fd)
{
    printf("hahahaha!!!!!!!!!!\n");
    printf("ENTER RecvDataForCurrentUser\n");
    return NULL;
}


static int process_cmdline(int argc, char **argv)
{
    int count = argc;
    int i;

    if(count == 1)
        return 0;

    count--;

    for(i=1;i<=count;)
    {
        if(!strcmp("--log", argv[i]))
        {
            g_debuglev = 1;
            i+=1;
            continue;
        }
        else if(!strcmp("--log2", argv[i]))
        {
            g_debuglev = 2;
            i+=1;
            continue;
        }
        else if(!strcmp("--help", argv[i]))
        {
            //usage();
            exit(0);
        }
        else
        {
            return -1;
        }
    }
    return 0;
}


static void signalhandler(int sig)
{
    int i = 0;
    printf("The Client is closed or receiver exit signal: %d\n \
        0 AIRPLAY_OR_DLNA ON\n \
        2 SIGINT\n \
        7 SIGBUS\n \
        11 SIGSEGV\n \
        13 SIGPIPE\n \
        15 SIGTERM\n ",sig);
        g_ExitAllCommunicate = 1;
    sleep(1);
    exit(1);
}

int main(int argc,char *argv[])//test function
{
	
    signal(SIGPIPE, &signalhandler);//pipe broken
    signal(SIGINT,  &signalhandler);//ctrl+c
    signal(SIGTERM, &signalhandler);//kill
    signal(SIGSEGV, &signalhandler);//segmentfault
    signal(SIGBUS,  &signalhandler);//bus error/**/
	
	InitSocketServer(7879);
	return 1;
}