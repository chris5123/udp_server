/* rs232.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h> 
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

//#define  BAUDRATE B9600
#define  dev    "/dev/ttyS0"
#define  bool	  _Bool
#define  true	  1
#define  false	0
bool	 w_SystemOff = true;
bool	 r_SystemOff = true;

/*Baudrate*/
int   speed_arr[]={B38400, B19200, B9600, B4800, B2400, B1200, B300};
int   name_arr[]={38400, 19200, 9600, 4800, 2400, 1200, 300};

void  set_speed(int fd, int speed)
{
  int   i;
  int   status;
  struct termios Opt;
  tcgetattr(fd, &Opt);
  for(i=0; i < sizeof(speed_arr)/sizeof(int); i++)
  {
    if(speed == name_arr[i])
    {
      tcflush(fd, TCIOFLUSH);
      cfsetispeed(&Opt, speed_arr[i]);
      cfsetospeed(&Opt, speed_arr[i]);
      status = tcsetattr(fd, TCSANOW, &Opt);
      if(status != 0)
      {
        printf("error : tcsetattr fd\n");
      }
    }
    tcflush(fd, TCIOFLUSH);
  }
}

/*Check bit*/
int set_Parity(int fd, int databits, int stopbits, int parity)
{
  struct termios options;
  if(tcgetattr(fd, &options) != 0)
  {
    printf("error : SteupSerial\n");
    return false;
  }
  options.c_cflag &= ~CSIZE;
  switch(databits)
  {
    case 7:
      options.c_cflag |= CS7;
      break;
    case 8:
      options.c_cflag |= CS8;
      break;
    default:
      fprintf(stderr, "Unsupported data size\n");
      return false;
  }
  switch(parity)
  {
    case 'n':
    case 'N':
      options.c_cflag &= ~PARENB;//initial odd&even check
      options.c_iflag &= ~INPCK;//initial odd&even check
      break;
    case 'o':
    case 'O':
      options.c_cflag |= (PARODD | PARENB);//setting odd&even check
      options.c_iflag |= INPCK;//stop odd&even check
      break;
    case 'e':
    case 'E':
      options.c_cflag |= PARENB;//odd&even check on
      options.c_cflag &= ~PARODD;//setting odd&even check
      options.c_iflag |= INPCK;//stop odd&even check
      break;
    case 'S':
    case 's'://no parity
      options.c_cflag &= ~PARENB;//odd&even check on
      options.c_cflag &= ~CSTOPB;//setting odd&even check
      break;  
    default:
      fprintf(stderr, "Unsupported parity\n");
      return false;
  }
  switch(stopbits)
  {
    case 1:
      options.c_cflag &= ~CSTOPB;
      printf("stop1\n");
      break;
    case 2:
      options.c_cflag |= CSTOPB;
      printf("stop2\n");
      break;
    default:
      fprintf(stderr, "Unsupported stop bits\n");
      return false;
  }
  if(parity != 'n')
  {
    options.c_iflag |= INPCK;
  }
  options.c_iflag &= ~ (INLCR | ICRNL | IGNCR);
  options.c_oflag &= ~(ONLCR | OCRNL);
  options.c_cc[VTIME] = 150;
  options.c_cc[VMIN] = 0;
  tcflush(fd, TCIFLUSH);//start
  if(tcsetattr(fd, TCSANOW, &options) != 0 )
  {
    printf("error : SetupSerial\n");
    return false;
  }
  return true;
}

/*thread for read*/
void  *serial_read_pthread(void *fd)
{
  ssize_t  res;
  int   fdr;
  int   i;
  unsigned char  recvstring[28]={0x00};

  fdr = *(int *) fd;
  //fcntl(fdr,F_SETFL,FNDELAY);
  while(r_SystemOff)
  {
    //memset(recvstring, 0, 28);
 
    if(res = read(fdr, recvstring, 36) > 0)
		{
 //     printf("fdr = %d\n",fdr);
      
      printf("res = %ld\n",res);
			for(i=0; i<6; i++)
      {
        printf("receive message %X\n",recvstring[i]);
      }
      //memset(recvstring, 0, 28);
		}
	  else
		{
			printf("message fail\n");
		}
	}
}

/*choose mode*/
int main()
{
  int   fd;
  int   mes;
  char  string[32]={0x00};
  char  r[]="read";
  char  w[]="write";
  char  c[]="close";
  int   kill_rc;
  int   mode;
  unsigned char tmp1[32] = {0x00};

  tmp1[0] = 0x01;
  tmp1[1] = 0x04;
  tmp1[2] = 0x08;
  tmp1[3] = 0x2E;
  tmp1[4] = 0x00;
  tmp1[5] = 0x01;
  tmp1[6] = 0x53;
  tmp1[7] = 0xA3;

  if((fd = open(dev, O_RDWR)) == -1)
  {
    printf("error : open\n");
    exit(EXIT_FAILURE);
  }
  set_speed(fd, B9600);
  if((set_Parity(fd, 8, 2,'N')) == false)
  {
    printf("error : Set Parity \n");
    exit(EXIT_FAILURE);
  }
  pthread_t thread;
	pthread_create(&thread, NULL, serial_read_pthread, &fd);
  printf("fd = %d\n", fd);
  while(w_SystemOff)
  {
    printf("choose your mode\n");
    scanf("%s",string);
    if(strcmp (string,w) != 0)
    {
      if(strcmp (string,r) != 0)
      {
        if(strcmp (string,c) != 0)
        {
          mode = 0;
        }
        else
        {
          mode = 3;
        }
      }
      else
      {
        mode = 2;
      }
    }
    else
    {
      mode = 1;
    }
    switch(mode)
    {
      case 1:
      {
        printf("write mode\n");
        //printf("keyin the message\n");
        if((mes = write(fd, tmp1, 8)) == -1)
        {
          printf("error : write\n");
          exit(EXIT_FAILURE);
        }
        for(int i=0; i<7; i++)
        {
          printf("send message %X\n",tmp1[i]);
        }
        printf("mes = %d\n",mes);
        break;
      }
      case 2:
      {
        printf("read mode\n");
        //printf("keyin the message\n");
        if((mes = write(fd, tmp1, 8)) == -1)
        {
          printf("error : write\n");
          exit(EXIT_FAILURE);
        }
        for(int i=0; i<7; i++)
        {
          printf("send message %X\n",tmp1[i]);
        }
        printf("mes = %d\n",mes);
        break;
      }
      case 3:
      {
        w_SystemOff = 0;
        r_SystemOff = 0;
        break;
      }
      default:
      {
        printf("error : wrong mode\n");
        break;
      }
    }
  }
  printf("main is ready to shutdown\n");
  close(fd);
	pthread_join(thread, NULL);
	kill_rc = pthread_kill(thread, 0);
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
  return r_SystemOff;
  printf("error : main is still alive\n");
}