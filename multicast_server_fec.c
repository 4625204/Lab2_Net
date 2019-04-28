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
    //error-correcting scheme
    fec_scheme fs=LIQUID_FEC_HAMMING74;
    int size=585;//每個封包讀取的資料大小
    // compute size of encoded message
    unsigned int k=fec_get_enc_msg_length(fs,size);
    num=file_size/585;//計算要切成幾個資料量為585封包
    n=file_size%585;//計算最後一個不足585的封包的大小
    unsigned char data[size];
    unsigned char databuf[k];

    // CREATE the fec object
    fec q=fec_create(fs,NULL);
    
    //告訴client會送幾個封包過去
    char p_num[256];
    memset(p_num,0,sizeof(p_num));
    sprintf(p_num,"%d",num+1);
    sendto(sd, p_num,sizeof(p_num), 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
    
    fseek(fp,0,SEEK_SET);
    int i;

    //傳送封包
    for(i=0;i<num;i++)
    {
       memset(databuf,0,sizeof(databuf));
       memset(data,0,sizeof(data));
       fread(data,sizeof(data),1,fp);
       // encode message
       fec_encode(q,sizeof(data),data,databuf);    
       sendto(sd, databuf,sizeof(databuf), 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
    }
    //傳送最後一個封包
    // compute size of encoded message
      unsigned int b=fec_get_enc_msg_length(fs,n);
      memset(databuf,0,sizeof(databuf));
      memset(data,0,sizeof(data));
      fread(data,n,1,fp);
      // encode message
      fec_encode(q,n,data,databuf);    
      sendto(sd, databuf,b, 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
   
     fec_destroy(q);//DESTROY the fec object
    /* Send a message to the multicast group specified by the*/
       
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
