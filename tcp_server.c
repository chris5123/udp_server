#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <resolv.h>


#define  SERV_PORT 5134

int main(int argc, char **argv)
{
	struct sockaddr_in s_addr;
	struct sockaddr_in c_addr;
    int  ret;
    char string[1024];
	int  addr_len;
	int  sock;
	int  i;
		
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	/*socket creat*/ 
	{	
		perror ("socket failed!");
		exit(EXIT_FAILURE);
	}
	bzero(&c_addr,sizeof(c_addr));
	c_addr.sin_family = AF_INET;
	c_addr.sin_port   =htons(SERV_PORT);
	if(inet_aton(argv[1], (struct in_addr*)&s_addr.sin_addr.s_addr)==0)
	{
		perror(argv[1]);
		exit(1);
	}
	bzero(&s_addr,sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port   =htons(SERV_PORT);
	//s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(inet_aton(argv[2],(struct in_addr*)&s_addr.sin_addr.s_addr)==0);
	{
		perror(argv[2]);
		exit(1);
	}
	if (bind(sock, (struct sockaddr *)&s_addr, sizeof(s_addr)) ==-1) 
	{
        perror(argv[3]);
		exit(1);
    }
	if(connect(sock,(struct sockaddr *)&s_addr, sizeof(s_addr)!=0))
	{
		perror ("Connect");
        exit(1);
	}
	if(fcntl(sock,F_SETFL,O_NONBLOCK)==-1)
	{
		perror ("fcnt1\n");
        exit(1);
	}
	addr_len = sizeof(c_addr);
	
	while(1)
	{
		
		ret = recv(sock, string, 1024, 0);
		if (ret > 0)
		{	
			printf("get %d message:%s",ret,string);
			ret=0;
		}
		else if(ret<0)
		{		
			errno=0;
			continue;	
		}
		else
		{
			perror("recv");
			exit(1);
		}
		memset(string,'\0',1024);
		printf("message send:");
		fgets(string,1024,stdin);
		if (ret=send(sock, &string, strlen(string),0) == -1) 
		{
            perror("Could not send datagram!!\n");
            exit(1);
		}
	}
	close(sock);
	return 0;
}