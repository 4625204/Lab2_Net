/* Send Multicast Datagram code example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <liquid/liquid.h>

struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;
char databuf[1024];
int datalen = sizeof(databuf);
 
int main (int argc, char *argv[ ])
{
/* Create a datagram socket on which to send. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
	  perror("Opening datagram socket error");
	  exit(1);
	}
	else
	  printf("Opening the datagram socket...OK.\n");
	 
	/* Initialize the group sockaddr structure with a */
	/* group address of 225.1.1.1 and port 8888. */
	memset((char *) &groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
	groupSock.sin_port = htons(8888);
	 
	/* Disable loopback so you do not receive your own datagrams.
	{
	char loopch = 0;
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
	{
	perror("Setting IP_MULTICAST_LOOP error");
	close(sd);
	exit(1);
	}
	else
	printf("Disabling the loopback...OK.\n");
	}
	*/
	 
	/* Set local interface for outbound multicast datagrams. */
	/* The IP address specified must be associated with a local, */
	/* multicast capable interface. */
	localInterface.s_addr = inet_addr("127.0.0.1");
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
	{
	  perror("Setting local interface error");
	  exit(1);
	}
	else
	  printf("Setting the local interface...OK\n");

    /* Send a message to the multicast group specified by the*/
    
	FILE *fp;
    int num;//封包數量-1
    int n;//最後一個封包的大小
    int file_size;//檔案大小
    fp=fopen(argv[1],"rb");
    if(fp==NULL){
        perror("Error File");
         return 0;
       }
    //計算file大小
    fseek(fp,0,SEEK_END);
    file_size=ftell(fp)-1;

    num=file_size/1024;//計算要切成幾個大小1024封包
    n=file_size%1024;//計算最後一個不足1024的封包的大小
    memset(databuf,0,sizeof(databuf));
    sprintf(databuf,"%d",num+1);
    //告訴client會送幾個封包過去
    sendto(sd, databuf,sizeof(databuf), 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
    memset(databuf,0,sizeof(databuf));
    fseek(fp,0,SEEK_SET);
    int i;
    //傳送封包
    for(i=0;i<num;i++)
    {
       memset(databuf,0,sizeof(databuf));
       fread(databuf,sizeof(databuf),1,fp);
       sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
    }
    //傳送最後一個封包
      memset(databuf,0,sizeof(databuf));
      fread(databuf,n,1,fp);
      sendto(sd, databuf,n, 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
    
    /* Send a message to the multicast group specified by the*/
	/* 
       unsign int n=1024;
       fec_scheme fs=LIQUID_FEC_HAMMING74;
       unsign int k=fec_get_enc_msg_length;
       fec q=fec_create(fs,NULL);
       fec_encode(q,n,data,databuf);
       
    */
    /* groupSock sockaddr structure. */
	/*int datalen = 1024;*/
	/*if(sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
	{
		perror("Sending datagram message error");
	}
	else
	  printf("Sending datagram message...OK\n");
	*/
/*	 Try the re-read from the socket if the loopback is not disable
	if(read(sd, databuf, datalen) < 0)
	{
	perror("Reading datagram message error\n");
	close(sd);
	exit(1);
	}
	else
	{
	printf("Reading datagram message from client...OK\n");
	printf("The message is: %s\n", databuf);
	}
*/	
	return 0;
}
