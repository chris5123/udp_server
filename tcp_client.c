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

/*#define  SERV_PORT 5134

#define  bool	_Bool
#define  true	1
#define  false	0
bool	 r_SystemOff = true;
bool	 s_SystemOff = true;*/


int main(int argc, char **argv)
{
	struct sockaddr_in s_addr;
	struct sockaddr_in c_addr;
	char   string[1024]={0x00};
	int    server_len;
	int    client_len;
	int    s_sock;
	int    c_sock;
	int    i;
	int    byte;
	s_sock = socket(AF_INET, SOCK_STREAM, 0);
	s_addr.sin_family = AF_INET;
	if (inet_aton(argv[1],(struct in_addr *) &s_addr.sin_addr.s_addr) == 0) 
	/*socket creat*/ 
	{	
		perror (argv[1]);
		exit(1);
	}
	s_addr.sin_port = htons(SERV_PORT);
	server_len = sizeof(s_addr);
	//s_addr.sin_family = AF_INET;
	bind(s_sock, (struct sockaddr *) &s_addr, server_len);
	listen(s_sock,5);
	printf("waiting connect\n");
	client_len=sizeof(c_addr);
	c_sock=accept(s_sock, (struct sockaddr *) &c_addr, (socklen_t *)&client_len);
	for(i=0;i<5;i++)
	{
		memset(string,'\0',1024);
		printf("message send\n");
		fgets(string,1024,stdin);
		if(byte=send(c_sock,string,strlen(string),0)==-1)
		{
			perror("send");
			exit(1);
		}
		memset(string,'\0',1024);
		byte = recv(c_sock,string,1024,MSG_DONTWAIT);
		if(byte>0)			
		{
			printf("get %d message:%s\n",byte,string);
			byte=0;
		}
		else if(byte<0)
		{
			if(errno==EAGAIN)
			{
				errno=0;
				continue;
			}
			else
			{
				perror("recv");
				exit(1);
			}
		}
	}
	shutdown(c_sock,2);
	shutdown(s_sock,2);
}
	
	//printf("main fuction end\n");
	//printf("thank you\n");




