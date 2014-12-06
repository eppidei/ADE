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

	char local_IP[40], remote_IP[40];
	int port=50000;

	int receive_ret_val;
	//double out_samples[N_SAMPLES];
	//int buff_size;


    static char char_buff[MAX_CHAR_BUFF_LEN];
	//static int init=0;

	SCDFPacketListener Test_Listener(MAX_SENSOR_FLOATS,MAX_AUDIO_FLOAT);
	IpEndpointName Test_endpoint;

unsigned int remote_ip1=192;
	unsigned int remote_ip2=168;
	unsigned int remote_ip3=1;
	unsigned int remote_ip4=30;
	unsigned int local_ip1=192;
	unsigned int local_ip2=168;
	unsigned int local_ip3=1;
	unsigned int local_ip4=52;
	unsigned long n_data_buffer;
	// if(nrhs!=3)
    // mexErrMsgTxt("Three inputs required.");
   // if(nlhs!=3)
     // mexErrMsgTxt("3 outputs required.");


memset(local_IP,'\0',sizeof(local_IP));

memset(remote_IP,'\0',sizeof(remote_IP));

	 sprintf(remote_IP,"%3.3d.%3.3d.%1.1d.%2.2d",remote_ip1,remote_ip2,remote_ip3,remote_ip4);
	 sprintf(local_IP,"%3.3d.%3.3d.%1.1d.%2.2d",local_ip1,local_ip2,local_ip3,local_ip4);
	//mexPrintf("local_IP %s\n",local_IP);
	//mexPrintf("remote_IP %s\n",remote_IP);



SCDF_Init(local_IP,remote_IP,port,&SOCK_sd,&localport_info);


	//mexPrintf("Port number %d\n",port);

	while(1)
	{
	     receive_ret_val=SCDF_receive(char_buff,sizeof(char_buff),local_IP,remote_IP,port,&SOCK_sd,&fds,&localport_info,&n_data_buffer);
	//mexPrintf("return_value %d , expected size of message %d, expected size of beamdata_pack %d\n",receive_ret_val,sizeof(t_usp_c1_data_msg),sizeof(t_usp_c1_beamforming_data));

        Test_endpoint.address=( (remote_ip1 & 0xFF) <<24) | ( (remote_ip2 & 0xFF) <<16) | ( (remote_ip3 & 0xFF) <<8) | ( (remote_ip4 & 0xFF) <<0) ;//(remote_IP);
		Test_endpoint.port=port;

		Test_Listener.ProcessPacket(char_buff,receive_ret_val,Test_endpoint);

//		double_convert_matlab (audio_matlab, Test_Listener.audio_data_buff_p,Test_Listener.n_audio_data);
//
//
//		double_convert_matlab (accel_matlab, Test_Listener.accel_data_buff_p,Test_Listener.n_accel_data);
//
//
//		double_convert_matlab (gyro_matlab, Test_Listener.gyro_data_buff_p,Test_Listener.n_gyro_data);
//
//
//		double_convert_matlab (magneto_matlab, Test_Listener.magneto_data_buff_p,Test_Listener.n_magneto_data);
//
//
//		double_convert_matlab (proxy_matlab, Test_Listener.proxy_data_buff_p,Test_Listener.n_proxy_data);
//
//
//		double_convert_matlab (light_matlab, Test_Listener.light_data_buff_p,Test_Listener.n_light_data);
//
//
//		double_convert_matlab2(audio_timestamps, Test_Listener.audio_timestamps_buff_p,2);

	}


	//receive_ret_val=SCDF_receive((char*)&SCDF_rx_pkt,sizeof(SCDF_rx_pkt),local_IP,remote_IP,port,&n_data_buffer);

	close(SOCK_sd);

	return 0;

}
