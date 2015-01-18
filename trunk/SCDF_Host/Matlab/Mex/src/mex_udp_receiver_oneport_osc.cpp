#include "my_defines.h"
#if (_OS_==WIN_OS)
#include <winsock2.h>
#include <windows.h>
#include <WinDef.h>
static SOCKET SOCK_sd2;
#elif (_OS_==LINUX_OS)
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
static int SOCK_sd2;
#endif
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <stdint.h>
#include "mex.h"
#include "udp_receiver.h"
#include "my_typedefs.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"
#include "SCDF_OSCListener.h"




void double_convert_matlab (double *mat_buffer, float *inbuffer, int num_samples)
{

	unsigned int i = 0;
	
	for(i=0;i<num_samples;i++)
	
	{
	
		mat_buffer[i]=(double)inbuffer[i];
	}


}

void double_convert_matlab2 (double *mat_buffer, s_uint64 *inbuffer, int num_samples)
{

	unsigned int i = 0;
	
	for(i=0;i<num_samples;i++)
	
	{
	
		mat_buffer[i]=(double)inbuffer[i];
	}


}


void my_Sock_close()
{
mexPrintf("Closing socket with my close\n");
#if (_OS_==1)
closesocket(SOCK_sd2);
#else
close(SOCK_sd2);
#endif
}
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
#if (_OS_==1)
    static SOCKET SOCK_sd;	
#else
static int SOCK_sd;
#endif    
    /* socket descriptor for port 5005*/
	static fd_set fds;
    static struct sockaddr_in localport_info;		/* Information about the local part */
	double *x;
	char *local_IP, remote_IP[40];
	int port;
	mwSize mrows,ncols_audio,one_column,ncols_sensors,two_column;
	int receive_ret_val;
	//double out_samples[N_SAMPLES];
	//int buff_size;
	double *audio_matlab,*n_samples_audio,*accel_matlab,*n_samples_accel,*gyro_matlab,*n_samples_gyro,*magneto_matlab,*n_samples_magneto,*proxy_matlab,*n_samples_proxy,*light_matlab,*n_samples_light,*byte_rx_matlab,*init_val,*buff_data;
    double *audio_timestamps,*audio_timeid;
	static SensorData_T SCDF_rx_pkt;
    static char char_buff[MAX_CHAR_BUFF_LEN];
	static int init=0;
    u_long n_data_buffer=0;
    s_uint64 time_id=0;
    s_uint32  num_samples;
	SCDFPacketListener Test_Listener(MAX_SENSOR_FLOATS,MAX_AUDIO_FLOAT);
	IpEndpointName Test_endpoint;
double *remote_ip1_from_mat,*remote_ip2_from_mat,*remote_ip3_from_mat,*remote_ip4_from_mat;
unsigned int remote_ip1;
	unsigned int remote_ip2;
	unsigned int remote_ip3;
	unsigned int remote_ip4;
	// if(nrhs!=3)
    // mexErrMsgTxt("Three inputs required.");
   // if(nlhs!=3)
     // mexErrMsgTxt("3 outputs required.");



	/* copy the string data from prhs[0] into a C string input_ buf.    */
    local_IP = mxArrayToString(prhs[0]);
	remote_ip1_from_mat = mxGetPr(prhs[1]);
	remote_ip2_from_mat = mxGetPr(prhs[2]);
	remote_ip3_from_mat = mxGetPr(prhs[3]);
	remote_ip4_from_mat = mxGetPr(prhs[4]);
	remote_ip1=(unsigned int)(*remote_ip1_from_mat);
	remote_ip2=(unsigned int)(*remote_ip2_from_mat);
	remote_ip3=(unsigned int)(*remote_ip3_from_mat);
	remote_ip4=(unsigned int)(*remote_ip4_from_mat);
	
	 sprintf(remote_IP,"%3.3d.%3.3d.%3.3d.%3.3d",remote_ip1,remote_ip2,remote_ip3,remote_ip4);
	//mexPrintf("local_IP %s\n",local_IP);
	//mexPrintf("remote_IP %s\n",remote_IP);

	x = mxGetPr(prhs[5]);


	port =(int)(*x);
	//mexPrintf("Port number %d\n",port);

	 if (port>65536)
	 //mexWarnMsgTxt("port number not valid");

	 mrows = 1;
	 ncols_audio = MAX_AUDIO_FLOAT;
	 ncols_sensors =MAX_SENSOR_FLOATS;
	 one_column = 1;
	 two_column = 2;
	 plhs[0] = mxCreateDoubleMatrix(mrows,ncols_audio, mxREAL);
	 plhs[1] = mxCreateDoubleMatrix(mrows,one_column, mxREAL);
	 
	 plhs[2] = mxCreateDoubleMatrix(mrows,ncols_sensors, mxREAL);
	 plhs[3] = mxCreateDoubleMatrix(mrows,one_column, mxREAL);
	 
	  plhs[4] = mxCreateDoubleMatrix(mrows,ncols_sensors, mxREAL);
	 plhs[5] = mxCreateDoubleMatrix(mrows,one_column, mxREAL);
	 
	  plhs[6] = mxCreateDoubleMatrix(mrows,ncols_sensors, mxREAL);
	 plhs[7] = mxCreateDoubleMatrix(mrows,one_column, mxREAL);
	 
	  plhs[8] = mxCreateDoubleMatrix(mrows,ncols_sensors, mxREAL);
	 plhs[9] = mxCreateDoubleMatrix(mrows,one_column, mxREAL); 
	 
	 plhs[10] = mxCreateDoubleMatrix(mrows,ncols_sensors, mxREAL);
	 plhs[11] = mxCreateDoubleMatrix(mrows,one_column, mxREAL); 
	 
	  plhs[12] = mxCreateDoubleMatrix(mrows,one_column, mxREAL);
 plhs[13] = mxCreateDoubleMatrix(mrows,one_column, mxREAL);
  plhs[14] = mxCreateDoubleMatrix(mrows,one_column, mxREAL);
 plhs[15] = mxCreateDoubleMatrix(mrows,two_column, mxREAL);
  plhs[16] = mxCreateDoubleMatrix(mrows,one_column, mxREAL);
 
 
	audio_matlab= mxGetPr(plhs[0]);
	n_samples_audio= mxGetPr(plhs[1]);
	
	accel_matlab= mxGetPr(plhs[2]);
	n_samples_accel= mxGetPr(plhs[3]);
	
	gyro_matlab= mxGetPr(plhs[4]);
	n_samples_gyro= mxGetPr(plhs[5]);
	
	magneto_matlab= mxGetPr(plhs[6]);
	n_samples_magneto= mxGetPr(plhs[7]);
	
	proxy_matlab= mxGetPr(plhs[8]);
	n_samples_proxy= mxGetPr(plhs[9]);
	
	light_matlab= mxGetPr(plhs[10]);
	n_samples_light= mxGetPr(plhs[11]);
	
	byte_rx_matlab= mxGetPr(plhs[12]);
	buff_data= mxGetPr(plhs[13]);
	init_val= mxGetPr(plhs[14]);
	audio_timestamps= mxGetPr(plhs[15]);
    audio_timeid= mxGetPr(plhs[16]);
  //  num_samples_val=mxGetPr(plhs[13]);


	 if (init==0)
	 {
		if( FD_ISSET(SOCK_sd, &fds) )
		{
		  #if (_OS_==1)
			closesocket(SOCK_sd);
		  #else
			close(SOCK_sd);
		  #endif
			mexPrintf("Closing socket at init\n");
		}
		SCDF_Init(local_IP,remote_IP,port,&SOCK_sd,&localport_info);
        mexPrintf("******** Remeber to check firewall rules on port %d ************\n",port);
		memset(&SCDF_rx_pkt,0,sizeof(SCDF_rx_pkt));
		init=1;
	}


	//receive_ret_val=SCDF_receive((char*)&SCDF_rx_pkt,sizeof(SCDF_rx_pkt),local_IP,remote_IP,port,&n_data_buffer);
    receive_ret_val=SCDF_receive(char_buff,sizeof(char_buff),local_IP,remote_IP,port,&SOCK_sd,&fds,&localport_info,&n_data_buffer);
	//mexPrintf("return_value %d , expected size of message %d, expected size of beamdata_pack %d\n",receive_ret_val,sizeof(t_usp_c1_data_msg),sizeof(t_usp_c1_beamforming_data));
	*byte_rx_matlab=receive_ret_val;
	*init_val = init;
	if (receive_ret_val==-1)
	mexErrMsgTxt("recvfrom error\n");
	
	Test_endpoint.address=( (remote_ip1 & 0xFF) <<24) | ( (remote_ip2 & 0xFF) <<16) | ( (remote_ip3 & 0xFF) <<8) | ( (remote_ip4 & 0xFF) <<0) ;//(remote_IP);
		Test_endpoint.port=port;
		
		//mexPrintf("receive_ret_val %d\n",receive_ret_val);
	if (	receive_ret_val>0)
	{
		Test_Listener.ProcessPacket(char_buff,receive_ret_val,Test_endpoint);
		
		double_convert_matlab (audio_matlab, Test_Listener.audio_data_buff_p,Test_Listener.n_audio_data);
		*n_samples_audio=Test_Listener.n_audio_data;
		
		double_convert_matlab (accel_matlab, Test_Listener.accel_data_buff_p,Test_Listener.n_accel_data);
		*n_samples_accel=Test_Listener.n_accel_data;
		
		double_convert_matlab (gyro_matlab, Test_Listener.gyro_data_buff_p,Test_Listener.n_gyro_data);
		*n_samples_gyro=Test_Listener.n_gyro_data;
		
		double_convert_matlab (magneto_matlab, Test_Listener.magneto_data_buff_p,Test_Listener.n_magneto_data);
		*n_samples_magneto=Test_Listener.n_magneto_data;
		
		double_convert_matlab (proxy_matlab, Test_Listener.proxy_data_buff_p,Test_Listener.n_proxy_data);
		*n_samples_proxy=Test_Listener.n_proxy_data;
		
		double_convert_matlab (light_matlab, Test_Listener.light_data_buff_p,Test_Listener.n_light_data);
		*n_samples_light=Test_Listener.n_light_data;
		
		double_convert_matlab2(audio_timestamps, Test_Listener.audio_timestamps_buff_p,2);
        
        *audio_timeid=Test_Listener.audio_timeid;
	}
	
	
      *buff_data = (double)n_data_buffer;
	  
SOCK_sd2=SOCK_sd;
mexAtExit(my_Sock_close);
	return;

}
