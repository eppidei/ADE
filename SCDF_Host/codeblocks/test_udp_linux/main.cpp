#include "my_defines.h"
#if (_OS_==1)
#include <winsock2.h>
#include <windows.h>
#include <WinDef.h>
static SOCKET SOCK_sd2;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <pthread.h>
static int SOCK_sd2;
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "udp_receiver.h"
#include "my_typedefs.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"
#include "SCDF_OSCListener.h"
#include "UDPSendersManager.h"
#include "UDPSender.h"
#include "ThreadUtils.h"

/******************RECEIVER DATA STRUCT******************/
typedef struct RX_S
{
  unsigned int remote_ip1;
	unsigned int remote_ip2;
	unsigned int remote_ip3;
	unsigned int remote_ip4;
	unsigned int local_ip1;
	unsigned int local_ip2;
	unsigned int local_ip3;
	unsigned int local_ip4;
    int port;
    fd_set *p_fds;
    sockaddr_in *p_localport_info;
    int *p_sock;
    char *p_buff;
    unsigned int buff_len;
    int start;
}RX_T;


void* rx_thread(void *p_params)
{
    RX_T *p_rxdata=(RX_T*)p_params;
    unsigned long n_data_buffer;
    char local_IP[40], remote_IP[40];
    SCDFPacketListener Test_Listener(MAX_SENSOR_FLOATS,MAX_AUDIO_FLOAT);
	IpEndpointName Test_endpoint;
	int receive_ret_val;

    memset(local_IP,'\0',sizeof(local_IP));

    memset(remote_IP,'\0',sizeof(remote_IP));

    sprintf(remote_IP,"%3.3d.%3.3d.%1.1d.%2.2d",p_rxdata->remote_ip1,p_rxdata->remote_ip2,p_rxdata->remote_ip3,p_rxdata->remote_ip4);
    sprintf(local_IP,"%3.3d.%3.3d.%1.1d.%2.2d",p_rxdata->local_ip1,p_rxdata->local_ip2,p_rxdata->local_ip3,p_rxdata->local_ip4);


    SCDF_Init(local_IP,remote_IP,p_rxdata->port,p_rxdata->p_sock,p_rxdata->p_localport_info);

	while(p_rxdata->start)
	{
	     receive_ret_val=SCDF_receive(p_rxdata->p_buff,p_rxdata->buff_len*sizeof(char),local_IP,remote_IP,p_rxdata->port,p_rxdata->p_sock,p_rxdata->p_fds,p_rxdata->p_localport_info,&n_data_buffer);

        if (receive_ret_val>0)
        {


        Test_endpoint.address=( (p_rxdata->remote_ip1 & 0xFF) <<24) | ( (p_rxdata->remote_ip2 & 0xFF) <<16) | ( (p_rxdata->remote_ip3 & 0xFF) <<8) | ( (p_rxdata->remote_ip4 & 0xFF) <<0) ;//(remote_IP);
		Test_endpoint.port=p_rxdata->port;

		Test_Listener.ProcessPacket(p_rxdata->p_buff,receive_ret_val,Test_endpoint);
		  }




	}

	close(*(p_rxdata->p_sock));
	pthread_exit(NULL);
}


//void* tx_thread(void *p_params)
//{
//    scdf::UDPSendersManager *p_sendmanager;
//
//
//     p_sendmanager->InitSender(55000, "127.0.0.1");
//     p_sendmanager->SetOutputPort(55000);
//    p_sendmanager->SetOutputAddress("127.0.0.1");
//    p_sendmanager->SetMultiOutput(false);
//    p_sendmanager->SetUseOSCPackaging(true);
//
//    while (1)
//    {
//        p_sendmanager->senderHelper::SendData(tx_buffer);
//    }
//
//
//
//}



int main ()
{
#if (_OS_==1)
    static SOCKET SOCK_sd;
#else
static int SOCK_sd;
#endif
    /* socket descriptor for port 5005*/
	static fd_set fds;
    static struct sockaddr_in localport_info;		/* Information about the local part */


pthread_t thread1;
int tret;

    static char char_buff[MAX_CHAR_BUFF_LEN];
     scdf::UDPSendersManager *p_sendmanager= scdf::UDPSendersManager::Instance();
     scdf::UDPSenderHelperBase *p_help;

	RX_T rx_data;

    rx_data.port=55000;
    rx_data.p_fds=&fds;
    rx_data.p_localport_info=&localport_info;
    rx_data.p_sock=&SOCK_sd;
    rx_data.p_buff=&(char_buff[0]);
    rx_data.buff_len=MAX_CHAR_BUFF_LEN;
    rx_data.remote_ip1=127;
	rx_data.remote_ip2=0;
	rx_data.remote_ip3=0;
	rx_data.remote_ip4=1;
	rx_data.local_ip1=127;
	rx_data.local_ip2=0;
	rx_data.local_ip3=0;
	rx_data.local_ip4=1;
	rx_data.start=1;

	tret = pthread_create(&thread1, NULL, rx_thread, (void *)&rx_data);

	if(tret!=0)
	{
	    printf("error creating threads\n");
	}


   p_sendmanager->SetOutputPort(55000);
    p_sendmanager->SetOutputAddress("127.0.0.1");
    p_sendmanager->SetMultiOutput(false);
    p_sendmanager->SetUseOSCPackaging(true);


 p_help=p_sendmanager->GetSender();
 p_help->Activate(true);
 scdf::ThreadUtils::JoinThread(p_help->handle);
pthread_join(thread1, NULL);

int vip=0;





	return 0;

}
