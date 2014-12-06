#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <WinDef.h>
#include <stdint.h>
#include "mex.h"
#include "udp_receiver.h"
#include "my_defines.h"
#include "my_typedefs.h"
#include "stk/include/Stk.h"
#include "stk/include/Clarinet.h"

#define MAX_FRAME_LEN (1048576)

using namespace stk;

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{

	//double *x;

	mwSize mrows,ncols_audio,one_column,ncols_sensors,two_column;
	int receive_ret_val;
	//double out_samples[N_SAMPLES];
	//int buff_size;

	static int init=0;
    u_long n_data_buffer=0;
    s_uint64 time_id=0;
    s_uint32  num_samples;
	
	double *blow_amplitude,*blow_rate,*note_on_frequency,*note_on_amplitude,*frame_len,*blowing_state,*note_on_state,*control_changestate,*control_change_num,*control_change_val;
	double *samples_o;

	Stk::setSampleRate( AUDIO_RATE_H );
	//Stk::showWarnings( true );
	static Clarinet Flute(8.0);
	// if(nrhs!=3)
    // mexErrMsgTxt("Three inputs required.");
   // if(nlhs!=3)
     // mexErrMsgTxt("3 outputs required.");
	 unsigned int i=0;
	 BOOL blow_state=false;



	/* copy the string data from prhs[0] into a C string input_ buf.    */
    // local_IP = mxArrayToString(prhs[0]);
	blowing_state= mxGetPr(prhs[0]);
	blow_amplitude = mxGetPr(prhs[1]);
	blow_rate = mxGetPr(prhs[2]);
	note_on_state= mxGetPr(prhs[3]);
	note_on_frequency = mxGetPr(prhs[4]);
	note_on_amplitude = mxGetPr(prhs[5]);
	control_changestate= mxGetPr(prhs[6]);
	control_change_num= mxGetPr(prhs[7]);
	control_change_val= mxGetPr(prhs[8]);
	frame_len = mxGetPr(prhs[9]);
	
	
	// sprintf(remote_IP,"%3.3d.%3.3d.%2.2d.%2.2d",remote_ip1,remote_ip2,remote_ip3,remote_ip4);
	//mexPrintf("local_IP %s\n",local_IP);
	//mexPrintf("remote_IP %s\n",remote_IP);

	//mexPrintf("Port number %d\n",port);

	
	// mexWarnMsgTxt("port number not valid");

	 mrows = 1;
	 ncols_audio = MAX_FRAME_LEN;
	 // ncols_sensors =MAX_SENSOR_FLOATS;
	 // one_column = 1;
	 // two_column = 2;
	 plhs[0] = mxCreateDoubleMatrix(mrows,ncols_audio, mxREAL);
	 samples_o=mxGetPr(plhs[0]);
	 
	 for (i=1;i<*frame_len;i++)
	 {
	 
	
	 
	 
	 // if (blowing_state[i-1]==0 && blowing_state[i]==1)
	 // {
		
	 // }
	 // else if (blowing_state[i-1]==1 && blowing_state[i]==0)
	 // {
		
	 // }
	 if (note_on_state[i-1]==0 && note_on_state[i]==1)
	 {
	 blow_state=true;
	 Flute.startBlowing(*blow_amplitude,*blow_rate);
		Flute.noteOn(*note_on_frequency,*note_on_amplitude);
		}
     else if (note_on_state[i-1]==1 && note_on_state[i]==0)
	 {
	 blow_state=false;
	 Flute.noteOff(*note_on_amplitude);
	 Flute.stopBlowing(*blow_amplitude);
	 }
	
	 else if (blow_state)//(control_changestate[i]==1)
	 {
	   Flute.controlChange(*control_change_num,(StkFloat)control_change_val[i]);
	 }
	 
	 samples_o[i]=Flute.tick(0);
	
	 }
 
	
  //  num_samples_val=mxGetPr(plhs[13]);



	return;

}
