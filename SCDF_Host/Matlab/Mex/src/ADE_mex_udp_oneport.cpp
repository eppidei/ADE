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
#include "headers/ADE_SCDF_Interface.h"
#include "headers/ADE.h"
#include "headers/ADE_Blow.h"
#include "my_typedefs.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

// typedef struct SensorData {
//         SensorType_T type;
//         s_int32 rate;           /* time of the sensor reading as reported by the sensor*/
//         s_uint64 timeid;                /* will be the same for all data harvested in the same call*/
//         s_int32 num_frames;
//         s_int32 numChannels;
//         s_uint64 *ptimestamp; 
//         s_sample *pdata;
//         s_sample data[MAX_BUFFER_LEN]; /*uint32 per nthol*/
//         s_uint64 timestamp[MAX_BUFFER_TIMESTAMP];
//     } SensorData_T;
// 
// #pragma pack(pop)

static ADE_T *p_ADE=NULL;
static FILE* p_fid=NULL;
static int init=0;
void Get_SensorData(char *char_buff,ADE_SCDF_Input_Int_T* p_ADE_Input)
{

	unsigned int i=0;
	float temp_f = 0.0;
	s_int32 temp = 0;
	s_uint64 time_id_temp=0;
    static SensorData_T sens_data_parsed;
    unsigned int address=0,kk=0;

	s_int32 num_frames=0,num_chann=0;
    s_uint32 n_samples_int ;
    
    /************* parse udp packet ***********/
    
    address=0;
    memcpy(&(sens_data_parsed.type),&char_buff[address],sizeof(SensorType_T));
   // mexPrintf(" sens data type %d\n", sens_data_parsed.type);
	 address+=sizeof(SensorType_T);
//      mexPrintf(" address %d\n", address);
     
     memcpy(&(sens_data_parsed.rate),&char_buff[address],sizeof(s_int32));
    address+=sizeof(s_int32);
//      mexPrintf(" address %d\n", address);
    
    memcpy(&(sens_data_parsed.timeid),&char_buff[address],sizeof(s_uint64));
    address+=sizeof(s_uint64);
//      mexPrintf(" address %d\n", address);
    
    memcpy(&(sens_data_parsed.num_frames),&char_buff[address],sizeof(s_int32));
 //    mexPrintf(" num frames %d\n", sens_data_parsed.num_frames);
    address+=sizeof(s_int32);
//      mexPrintf(" address %d\n", address);
    
    memcpy(&(sens_data_parsed.numChannels),&char_buff[address],sizeof(s_int32));
    address+=sizeof(s_int32);
//      mexPrintf(" address %d\n", address);
    
//     memcpy(&(sens_data_parsed.ptimestamp),&char_buff[address],sizeof(s_uint64*));
//     address+=sizeof(s_uint64*);
//      mexPrintf(" address %d\n", address);
//     
     memcpy(&(sens_data_parsed.pdata),&char_buff[address],sizeof(s_sample*));
     //mexPrintf(" pdata %d\n", n_samples_int);
    address+=sizeof(s_sample*);
//      mexPrintf(" address %d\n", address);
    
    n_samples_int=sens_data_parsed.num_frames*sens_data_parsed.numChannels;
//      mexPrintf(" n samples %d\n", n_samples_int);
     
    
    memset(sens_data_parsed.data,0,sizeof(sens_data_parsed.data));
   // memcpy(sens_data_parsed.data,&char_buff[address],(n_samples_int)*sizeof(char));
    kk=address;
    for (i=0;i<n_samples_int;i++)
    {
           sens_data_parsed.data[i]=*((s_sample*)(&char_buff[kk]));
         //mexPrintf(" in data char buff %d=%f\n", i,*((s_sample*)(&char_buff[kk])));
         // mexPrintf(" in data sens_data_parsed %d=%f\n", i,sens_data_parsed.data[i]);
         kk+=sizeof(s_sample);
        
    }
    
//mexPrintf("n_samples_int %d, char size %d,sizedata %d\n",n_samples_int,sizeof(char), ((n_samples_int)*sizeof(char)));
     address+=((n_samples_int)*sizeof(s_sample));
//       mexPrintf(" address %d\n", address);
     
     
   memset(sens_data_parsed.timestamp,0,sizeof(sens_data_parsed.timestamp));
    memcpy(sens_data_parsed.timestamp,&char_buff[address],(2)*sizeof(s_uint64));
    
     address+=(2)*sizeof(s_uint64);
//       mexPrintf(" address %d\n", address);
    
      /********* replicate original sensor data struct *************/
    if (sens_data_parsed.type==AudioInput)
    {
       p_ADE_Input->type=ADE_AudioInput;
    }
       p_ADE_Input->rate=sens_data_parsed.rate;
       p_ADE_Input->timeid=sens_data_parsed.timeid;
       p_ADE_Input->num_frames=sens_data_parsed.num_frames;
       p_ADE_Input->num_channels=sens_data_parsed.numChannels;
       p_ADE_Input->data=sens_data_parsed.data;
       p_ADE_Input->timestamp=sens_data_parsed.timestamp;

}


void my_Sock_close()
{
mexPrintf("Closing socket with my close\n");
#if (_OS_==1)
closesocket(SOCK_sd2);
#else
close(SOCK_sd2);
#endif
ADE_Release(p_ADE,BLOW_FLAG);
ADE_Release(p_ADE,SNAP_FLAG);
init=0;
fclose(p_fid);

}
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{

   #if (_OS_==1)
    static SOCKET SOCK_sd;	
#else
static int SOCK_sd;
#endif 					/* socket descriptor for port 5005*/
	static fd_set fds;
    static struct sockaddr_in localport_info;		/* Information about the local part */
	double *x;
	char *local_IP, *remote_IP;
	int port;
	mwSize mrows,ncols,ncols2;
	int receive_ret_val;
	//double out_samples[N_SAMPLES];
	//int buff_size;
 	double *blow_state,*blow_data,*snap_state,*snap_data;
    static SensorData_T SCDF_rx_pkt;
    static char char_buff[MAX_CHAR_BUFF_LEN];
    static ADE_SCDF_Input_Int_T ADE_in_struct;
    u_long n_data_buffer=0;
    s_uint64 time_id=0;
    s_int32  num_samples;
    unsigned int i=0;
    unsigned int audio_buff_len=0;
    float audio_fs=44100;
      double *remote_ip1_from_mat,*remote_ip2_from_mat,*remote_ip3_from_mat,*remote_ip4_from_mat;
    unsigned int remote_ip1;
	unsigned int remote_ip2;
	unsigned int remote_ip3;
	unsigned int remote_ip4;
	int ret_init=-1;
    
    ADE_SCDF_Output_Int_T *p_out_blow=NULL;
    ADE_SCDF_Output_Int_T *p_out_snap=NULL;


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
	//mexPrintf("local_IP %s\n",local_IP);
	//mexPrintf("remote_IP %s\n",remote_IP);

	x = mxGetPr(prhs[5]);


	port =(int)(*x);
	//mexPrintf("Port number %d\n",port);

	 if (port>65536)
	 mexWarnMsgTxt("port number not valid");

	 mrows = 1;
	 ncols = MAX_BUFFER_LEN;
	 ncols2 = 1;
 	 plhs[0] = mxCreateDoubleMatrix(mrows,ncols2, mxREAL);
 	 plhs[1] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
 	 plhs[2] = mxCreateDoubleMatrix(mrows,ncols2, mxREAL);
//  	  plhs[3] = mxCreateDoubleMatrix(mrows,ncols2, mxREAL);
//  plhs[4] = mxCreateDoubleMatrix(mrows,ncols2, mxREAL);
//  
	blow_state= mxGetPr(plhs[0]);
	blow_data= mxGetPr(plhs[1]);
	snap_state= mxGetPr(plhs[2]);
// 	snap_data= mxGetPr(plhs[3]);
//     num_samples_val=mxGetPr(plhs[4]);


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
		ret_init=SCDF_Init(local_IP,remote_IP,port,&SOCK_sd,&localport_info);
		if (ret_init<0)
		{
		    mexPrintf("Socket Init failed\n");
            mexAtExit(my_Sock_close);
		  return ;
		}
		audio_buff_len = 256;
		ADE_Init(&p_ADE,BLOW_FLAG,audio_buff_len,audio_fs);
        ADE_Configure_params(p_ADE,BLOW_FLAG);
        ADE_Init(&p_ADE,SNAP_FLAG,audio_buff_len,audio_fs);
        mexPrintf("******** Remember to check firewall rules on port %d ************\n",port);
		memset(&SCDF_rx_pkt,0,sizeof(SCDF_rx_pkt));
		init=1;
        p_fid=fopen("./Debug_blow.txt","w");
	}


	//receive_ret_val=SCDF_receive((char*)&SCDF_rx_pkt,sizeof(SCDF_rx_pkt),local_IP,remote_IP,port,&n_data_buffer);
    receive_ret_val=SCDF_receive(char_buff,sizeof(char_buff),local_IP,remote_IP,port,&SOCK_sd,&fds,&localport_info,&n_data_buffer);
	//mexPrintf("return_value %d , expected size of message %d, expected size of beamdata_pack %d\n",receive_ret_val,sizeof(t_usp_c1_data_msg),sizeof(t_usp_c1_beamforming_data));
// 	*byte_rx_matlab=receive_ret_val;
// 	*init_val = init;
	if (receive_ret_val==-1)
	mexErrMsgTxt("recvfrom error\n");
//  mexPrintf(" received %d bytes \n",receive_ret_val); 
	if (receive_ret_val>=0)
	{
        Get_SensorData(char_buff,&ADE_in_struct);  
       // mexPrintf("****** 1 blow buff len = %d *******\n",audio_buff_len);
//          for (i=0;i<p_ADE->p_blow->buff_len_i;i++)
//          {
//                 //mexPrintf("****** 2 *******\n");
//              mexPrintf(" in data %d=%f\n", i,ADE_in_struct.data[i]);
//             
//          }
      //  mexPrintf("****** 1 *******\n");
         if (ADE_in_struct.type==ADE_AudioInput)
         {
            // mexPrintf("****** 1 *******\n");
         ADE_Step(p_ADE,BLOW_FLAG,&ADE_in_struct);
          ADE_Step(p_ADE,SNAP_FLAG,&ADE_in_struct);
        // ADE_Blow_Print(p_ADE->p_blow,p_fid,"p_blow","p_ade_handle");
         }
      //  mexPrintf("****** 2 *******\n");
     //   ADE_Step(p_ADE,SNAP_FLAG,&ADE_in_struct);
        p_out_blow=ADE_GetOutBuff(p_ADE,BLOW_FLAG);
        p_out_snap=ADE_GetOutBuff(p_ADE,SNAP_FLAG);
      //  p_out_snap=ADE_GetOutBuff(p_ADE,SNAP_FLAG);
    //     mexPrintf("****** 3 *******\n");
        *blow_state=p_out_blow->state;
         *snap_state=p_out_snap->state;
     //    mexPrintf("****** 3.5 *******\n");
       // memset(blow_data,0,ncols*sizeof(double));
         //mexPrintf("****** 4 *******\n");
        for (i=0;i<p_out_blow->n_data;i++)
        {
            blow_data[i]=p_out_blow->p_data[i];
          //   mexPrintf(" blow  data %d= %f\n",i,  blow_data[i]);
        }
        //  mexPrintf("****** 5 *******\n");
//         *snap_state=p_out_snap->state;
      //   mexPrintf("****** 6 *******\n");
	}
//     *num_samples_val=num_samples;
//mexPrintf("1");
//      ioctlsocket(SOCK_sd,FIONREAD,&n_data_buffer);
//       *buff_data = (double)n_data_buffer;
SOCK_sd2=SOCK_sd;
mexAtExit(my_Sock_close);
	return;

}
