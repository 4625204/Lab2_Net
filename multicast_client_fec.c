/* Receiver/client multicast Datagram example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <liquid/liquid.h>

struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
 
int main(int argc, char *argv[])
{
/* Create a datagram socket on which to receive. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
		perror("Opening datagram socket error");
		exit(1);
	}
	else
	printf("Opening datagram socket....OK.\n");
		 
	/* Enable SO_REUSEADDR to allow multiple instances of this */
	/* application to receive copies of the multicast datagrams. */
	{
		int reuse = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
	{
		perror("Setting SO_REUSEADDR error");
		close(sd);
		exit(1);
	}
	else
		printf("Setting SO_REUSEADDR...OK.\n");
	}
	 
	/* Bind to the proper port number with the IP address */
	/* specified as INADDR_ANY. */
	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(8888);
	localSock.sin_addr.s_addr = INADDR_ANY;
	if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock)))
	{
		perror("Binding datagram socket error");
		close(sd);
		exit(1);
	}
	else
		printf("Binding datagram socket...OK.\n");
	 
	/* Join the multicast group 226.1.1.1 on the local 203.106.93.94 */
	/* interface. Note that this IP_ADD_MEMBERSHIP option must be */
	/* called for each local interface over which the multicast */
	/* datagrams are to be received. */
	group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
	group.imr_interface.s_addr = inet_addr("127.0.0.1");
	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
	{
		perror("Adding multicast group error");
		close(sd);
		exit(1);
	}
	else
		printf("Adding multicast group...OK.\n");
	 
    
    unsigned int datalen=1024;
    // error-correcting scheme
    fec_scheme fs=LIQUID_FEC_HAMMING74;
    // compute size of encoded message
    unsigned int k=fec_get_enc_msg_length(fs,datalen);
    unsigned char databuf[1024];
    unsigned char data[585];
    // CREATE the fec object
    fec q=fec_create(fs,NULL);    
    
    //創建檔案
    FILE *fp;
    fp=fopen(argv[1],"wb");
    if(fp==NULL)
     { perror("Error File.");
       return 0;
     }
    int rev_len=k;//檔案大小
    //接收預計收到的封包數
    char p_num[256];
    bzero(p_num,sizeof(p_num));
    read(sd,p_num,sizeof(p_num));
    
    float p_total;
    //把p_num(封包數量)從string轉成float
    p_total=atof(p_num);
    float total=0.0;
    
    //清空buffer
    bzero(data,sizeof(data));
    bzero(databuf,sizeof(databuf));
    //開始接收資料
    while(1)
    {
        rev_len=read(sd,databuf,sizeof(databuf));
        total++;//計算收到幾個封包
        //decode message
        fec_decode(q,sizeof(data),databuf,data);
        //資料寫入
        fwrite(data,1,sizeof(data),fp);
        bzero(data,sizeof(data));
        bzero(databuf,sizeof(databuf));

        //當封包小於1024代表為最後一個封包所以跳出迴圈
        if(rev_len<1024)
        {break;}

     }
     // DESTROY the fec object
     fec_destroy(q);
     printf("封包遺失率：%f\n",(p_total-total)/p_total);

	return 0;
}
