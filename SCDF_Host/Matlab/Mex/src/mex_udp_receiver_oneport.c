#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <WinDef.h>
#include <stdint.h>
#include "mex.h"
#include "udp_receiver.h"
#include "my_defines.h"
#include "my_typedefs.h"

static SOCKET SOCK_sd2;

void my_Sock_close()
{
mexPrintf("Closing socket with my close\n");
closesocket(SOCK_sd2);
}
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{

    static SOCKET SOCK_sd;						/* socket descriptor for port 5005*/
	static fd_set fds;
    static struct sockaddr_in localport_info;		/* Information about the local part */
	double *x;
	char *local_IP, *remote_IP;
	int port;
	mwSize mrows,ncols,ncols2;
	int receive_ret_val;
	//double out_samples[N_SAMPLES];
	//int buff_size;
	double *audio_matlab,*byte_rx_matlab,*init_val,*buff_data,*num_samples_val;
    static SensorData_T SCDF_rx_pkt;
    static char char_buff[18000];
	static int init=0;
    u_long n_data_buffer=0;
    s_uint64 time_id=0;
    s_int32  num_samples;


	// if(nrhs!=3)
    // mexErrMsgTxt("Three inputs required.");
   // if(nlhs!=3)
     // mexErrMsgTxt("3 outputs required.");



	/* copy the string data from prhs[0] into a C string input_ buf.    */
    local_IP = mxArrayToString(prhs[0]);
	remote_IP = mxArrayToString(prhs[1]);
	//mexPrintf("local_IP %s\n",local_IP);
	//mexPrintf("remote_IP %s\n",remote_IP);

	x = mxGetPr(prhs[2]);


	port =(int)(*x);
	//mexPrintf("Port number %d\n",port);

	 if (port>65536)
	 mexWarnMsgTxt("port number not valid");

	 mrows = 1;
	 ncols = MAX_BUFFER_LEN;
	 ncols2 = 1;
	 plhs[0] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
	 plhs[1] = mxCreateDoubleMatrix(mrows,ncols2, mxREAL);
	 plhs[2] = mxCreateDoubleMatrix(mrows,ncols2, mxREAL);
	  plhs[3] = mxCreateDoubleMatrix(mrows,ncols2, mxREAL);
 plhs[4] = mxCreateDoubleMatrix(mrows,ncols2, mxREAL);
 
	audio_matlab= mxGetPr(plhs[0]);
	byte_rx_matlab= mxGetPr(plhs[1]);
	buff_data= mxGetPr(plhs[2]);
	init_val= mxGetPr(plhs[3]);
    num_samples_val=mxGetPr(plhs[4]);


	 if (init==0)
	 {
		if( FD_ISSET(SOCK_sd, &fds) )
		{
			closesocket(SOCK_sd);
			mexPrintf("Closing socket at init\n");
		}
		SCDF_Init(local_IP,remote_IP,port,&SOCK_sd,&localport_info);
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

	if (receive_ret_val>=0)
	{
	//mexPrintf("0");
       // memcpy(audio_matlab,&(SCDF_rx_pkt.data[0]),MAX_BUFFER_LEN*4);
		Parse_audio_data_double(&SCDF_rx_pkt,char_buff, audio_matlab,&time_id,&num_samples);
       
	}
    *num_samples_val=num_samples;
//mexPrintf("1");
     ioctlsocket(SOCK_sd,FIONREAD,&n_data_buffer);
      *buff_data = (double)n_data_buffer;
SOCK_sd2=SOCK_sd;
mexAtExit(my_Sock_close);
	return;

}
