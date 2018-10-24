#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h> 
#include <signal.h>

#define  SERV_PORT 5134

#define  bool	_Bool
#define  true	1
#define  false	0
bool	 r_SystemOff = true;
bool	 s_SystemOff = true;

void *malloc(size_t size);
void free(void *ptr);

void *recvfuction(void *recv1)
{	
	struct sockaddr_in s_addr;
	int    recvlength;
	char   recvstring[1024]={0x00};
	int    addr_len;
	int    sock;
	char   s[]="exit";
	struct timeval tv;
	int    ret;
	int    maxfd = -1;
	struct timeval
	{
		long tv_sec;
		long tv_usec;
	};

	sock = *(int *) recv1;
	recvlength = -1;
	fd_set rdfds;

	while(s_SystemOff)
	{
		FD_ZERO(&rdfds);
		FD_SET (sock, &rdfds);
		tv.tv_sec  = 5;
		tv.tv_usec = 0;
		ret = select(sock+1, &rdfds, NULL, NULL, &tv);
		switch(ret)
		{
			case -1:
			{
				perror("error : select function");
				break;
			}
			case  0:
			{
				printf("overtime\n");
				//FD_ZERO(&rdfds);
				break;
			}
			default:
			{
				printf("ret=%d\n", ret);
				if(FD_ISSET(sock, &rdfds))
				{
					recvlength=recvfrom(sock, recvstring, 1024, MSG_DONTWAIT,
	 						(struct sockaddr *) &s_addr, &addr_len);
					if(recvlength>=0)
					{
							printf("receive message %s\n",recvstring);
							memset( recvstring, 0, sizeof(recvstring));
							break;
					}
					else
					{
						printf("message fail\n");
						break;
					}
				}
			}	
		}
	}
	printf("thread is ready to shutdown\n");;
	return NULL;
	printf("error : thread still running!!\n");
}
int main(int argc, char **argv)
{
	struct sockaddr_in s_addr;
	int    sendlength;
	char   string[1024]={0x00};
	int    addr_len;
	char   s[]="exit";
	static int  sock1;
	int    ret;
	int    kill_rc;
	
	if ((sock1 = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	/*socket creat*/ 
	{	
		perror ("socket failed!");
		exit(errno);
	}
	else
	{
		printf("socket create\n");
	}
	//printf("%p",&sock1);
	s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(SERV_PORT);
	if (argv[1])
	{
		s_addr.sin_addr.s_addr = inet_addr(argv[1]);
	}
	else
	{
		printf("input server ip\n");
		exit(0);
	}
	addr_len = sizeof(s_addr);

	pthread_t thread;
	pthread_create(&thread, NULL, recvfuction, &sock1);
	
	while(s_SystemOff)
	{
		printf("keyin the message\n");
		scanf("%s",string);
		if(strcmp (string,s) != 0)			
		{
			sendlength=sendto(sock1, string, strlen(string), MSG_DONTWAIT,
				(struct sockaddr *) &s_addr, addr_len);
			printf("%d\n",sendlength);
			continue;
		}
		else
		{
			s_SystemOff = false;
			r_SystemOff = false;
			break;
		}
	}
	//sleep(3);
	printf("main is ready to shutdown\n");
	/*ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if(ret != 0)
	{
		printf("error : thread cancel state\n");
	}
	ret = pthread_cancel(thread);
	if(ret != 0)
	{
		printf("thread cancel fail\n");
	}*/
	pthread_join(thread,NULL);
	kill_rc = pthread_kill(thread,0);
	if(kill_rc == ESRCH)
	{
		printf("thread is released\n");
	}
	else if(kill_rc == EINVAL)
	{
		printf("signal is invalid\n");
	}
	else
	{
		printf("thread is alive\n");
	}
	printf("thank you\n");
	/*message send by UDP*/

	/*thread*/
	
	/*
	if(sendlength<0)
	{		
		printf("write to server error");
	}
	if(recvlength<0)
	{		
		printf("read from server error");
	}
	else
	{
		printf("congratulations\n");
	}
	*/

	return r_SystemOff;
}


