#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define  SERV_PORT 5134

int main(int argc, char **argv)
{
	struct sockaddr_in s_addr;
	struct sockaddr_in c_addr;
        int  length;
        char string[1024];
	int  addr_len;
	int  sock;
		
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	/*socket creat*/ 
	{	
		perror ("socket failed!");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("socket create\n");
	}
	memset(&s_addr, 0, sizeof(struct sockaddr_in));
	
	s_addr.sin_family = AF_INET;
	s_addr.sin_port   =htons(SERV_PORT);
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (struct sockaddr *)&s_addr, sizeof(s_addr)) <0) 
	{
                perror ("bind failed\n");
                exit(1);
        }
	else
	{
		printf("bind address to socket\n");
	}
	addr_len = sizeof(c_addr);
	
	while(1)
	{
		
		length = recvfrom(sock, string, 1024, 0,
			 (struct sockaddr *) &c_addr, &addr_len);
		if (length < 0)
		{	
			perror("recvfrom");
			continue;
		}
		string[length]='\0';
		printf("success %s:%d message:%s\r\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port), string);
		if (sendto(sock, &string, length, MSG_DONTWAIT, (struct sockaddr*)&c_addr, addr_len) < 0) 
		{
                        perror("Could not send datagram!!\n");
                        continue;
		}
	}
	return 0;
}