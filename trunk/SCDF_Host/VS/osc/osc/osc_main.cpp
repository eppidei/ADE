// udp_midi_streamer_v1.cpp : definisce il punto di ingresso dell'applicazione console.
//


//#include "stdafx.h"
#include "my_defines.h"
#include "my_typedefs.h"
#include <WinDef.h>
#include <winsock.h>
#include <WinBase.h>
#include <string.h>
#include "udp_receiver.h"
#include "dsp_utils.h"
#include "dsp_blow.h"
#include "midi_packager.h"
#include <math.h>
#include <mmsystem.h>
#include <stdio.h>
#include <iostream>
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"
#include "SCDF_OSCListener.h"
#include "stk/include/Stk.h"
#include "stk/include/Clarinet.h"
//#include <stdlib.h>

HANDLE ghSemaphore;
LARGE_INTEGER tic_wav_write,toc_wav_write,tic_buffer,toc_buffer,counter_freq;

void CALLBACK waveOutProc(
  HWAVEOUT hwo,
  UINT uMsg,
  DWORD_PTR dwInstance,
  DWORD_PTR dwParam1,
  DWORD_PTR dwParam2
)
{
	if (uMsg==WOM_DONE)
	{
#ifdef VERBOSE_THREAD
	QueryPerformanceCounter(&toc_wav_write);
	printf("playback ticks %f milliseconds\n", ((double)toc_wav_write.QuadPart-(double)tic_wav_write.QuadPart)/(double)counter_freq.QuadPart*1e3);
	#endif
	}

};

#ifdef _MIDI_
DWORD WINAPI midi_play_thread (LPVOID thread_params) 
{
	midiout_thread_params *midi_params_casted = NULL;
	//HMIDISTRM p_midi_handle= midi_params_casted->p_handle;
	//MIDIHDR *p_midi_header=midi_params_casted->p_midi_header;
	DWORD dwWaitResult;
	midimessage_T* shortmessage_int =NULL; 

	MMRESULT mmcap;

	while(1)
	{
		midi_params_casted=(midiout_thread_params*)thread_params;
		dwWaitResult = WaitForSingleObject( 
            ghSemaphore,   // handle to semaphore
            INFINITE);         

			if (dwWaitResult==WAIT_ABANDONED)
			{
#ifdef VERBOSE_THREAD
				printf("WAIT_ABANDONED\n");
#endif
			}
			else if (dwWaitResult==WAIT_OBJECT_0)
			{
				#ifdef VERBOSE_THREAD
				printf("WAIT_OBJECT_0\n");
			#endif
			}
			else if (dwWaitResult==WAIT_TIMEOUT)
			{
				#ifdef VERBOSE_THREAD
				printf("WAIT_TIMEOUT\n");
			#endif
			}
			else if (dwWaitResult==WAIT_FAILED)
			{
				#ifdef VERBOSE_THREAD
				printf("WAIT_FAILED\n");
			#endif
			}

			shortmessage_int=midi_params_casted->p_midi_message;
			mmcap=midiOutShortMsg(midi_params_casted->p_handle,shortmessage_int->word);

		if (mmcap==MMSYSERR_NOERROR)
		{
#ifdef VERBOSE_THREAD
			printf("Playing message %d\n",shortmessage_int->data[0]);
#endif
		}
		else if (mmcap==MMSYSERR_NOMEM)
		{
			printf("streamout error\n");
				return -1;
		}

		
	}

	return 0;
}
#else

#ifdef _STK_
DWORD WINAPI wav_play_thread (LPVOID thread_params) 
{
	wavout_thread_params_T *wav_params_casted = NULL;
	MMRESULT mmcap;
	DWORD dwWaitResult;
	while(1)
	{
		wav_params_casted=(wavout_thread_params_T*)thread_params;
			dwWaitResult = WaitForSingleObject( 
            ghSemaphore,   // handle to semaphore
            INFINITE);         

			if (dwWaitResult==WAIT_ABANDONED)
			{
#ifdef VERBOSE_THREAD
				printf("WAIT_ABANDONED\n");
#endif
			}
			else if (dwWaitResult==WAIT_OBJECT_0)
			{
				#ifdef VERBOSE_THREAD
				printf("WAIT_OBJECT_0\n");
			#endif
			}
			else if (dwWaitResult==WAIT_TIMEOUT)
			{
				#ifdef VERBOSE_THREAD
				printf("WAIT_TIMEOUT\n");
			#endif
			}
			else if (dwWaitResult==WAIT_FAILED)
			{
				#ifdef VERBOSE_THREAD
				printf("WAIT_FAILED\n");
			#endif
			}
			#ifdef VERBOSE_THREAD
			QueryPerformanceCounter(&tic_wav_write);
#endif
			mmcap =  waveOutWrite(wav_params_casted->p_handle,wav_params_casted->p_header,wav_params_casted->head_size);

		//	printf("PLAYING\n");

			if (mmcap!=MMSYSERR_NOERROR)
			{
				 if (mmcap==MMSYSERR_INVALHANDLE)
					{
					printf("MMSYSERR_INVALHANDLE\n");
					}
					else if(mmcap==MMSYSERR_NODRIVER)
					{
					printf("MMSYSERR_NODRIVER\n");
					}
					else if (mmcap==MMSYSERR_NOMEM)
					{
					printf("MMSYSERR_NOMEM\n");
					}
					else if (mmcap==WAVERR_UNPREPARED)
					{
					printf("WAVERR_UNPREPARED\n");
					}
					else if (mmcap==WAVERR_BADFORMAT)
					{
					printf("WAVERR_UBADFORMAT\n");
					}
					else if (mmcap==WAVERR_STILLPLAYING)
					{
					printf("WAVERR_STILLPLAYING\n");
					}
					else if (mmcap==WAVERR_UNPREPARED)
					{
					printf("WAVERR_UNPREPARED\n");
					}
					else if (mmcap==WAVERR_SYNC)
					{
					printf("WAVERR_SYNC\n");
					}
			
			}

			
 
	}

}
#endif
#endif

using namespace stk;

int main(int argc,char* argv[])// _TCHAR* argv[])
{
	 char ip_local[40];
    char ip_remote[40];
    int port_id = 50000;

	
static SensorData_T SCDF_rx_pkt;
static s_float audio_buffer[MAX_BUFFER_LEN];
static short wav_buffer[MAX_WAV_BUFFER_LEN];
static unsigned short blow_array[MAX_BUFFER_LEN];
static char char_buff[MAX_CHAR_BUFF_LEN];


static SOCKET SOCK_sd;						/* socket descriptor for port 5005*/
static fd_set fds;
static struct sockaddr_in localport_info;		/* Information about the local part */
static dsp_float_t bbfast[MAX_FILT_LEN];
static dsp_float_t bbslow[MAX_FILT_LEN];
static dsp_float_t bbslow2[MAX_FILT_LEN];
static dsp_float_t bbslow3[MAX_FILT_LEN];
static dsp_float_t aafast[MAX_FILT_LEN];
static dsp_float_t aaslow[MAX_FILT_LEN];
static dsp_float_t aaslow2[MAX_FILT_LEN];
static dsp_float_t aaslow3[MAX_FILT_LEN];
static dsp_float_t statefast[MAX_FILT_LEN];
static dsp_float_t stateslow[MAX_FILT_LEN];
static dsp_float_t stateslow2[MAX_FILT_LEN];
static dsp_float_t stateslow3[MAX_FILT_LEN];
static dsp_float_t pow_filt_a[MAX_FILT_LEN];
static dsp_float_t pow_filt_b[MAX_FILT_LEN];
static dsp_float_t pow_filt_a2[MAX_FILT_LEN];
static dsp_float_t pow_filt_b2[MAX_FILT_LEN];
static dsp_float_t pow_filt_a3[MAX_FILT_LEN];
static dsp_float_t pow_filt_b3[MAX_FILT_LEN];
static dsp_float_t statepow[MAX_FILT_LEN];
static dsp_float_t statepow2[MAX_FILT_LEN];
static dsp_float_t statepow3[MAX_FILT_LEN];
dsp_float_t gain1=0,gain2=0,gain3=0;

//unsigned int dec_factor = AUDIO_RATE_H/MIDI_RATE_H; // aggiungere controllo intero
static unsigned long midi_buffer[MAX_MIDI_BUFFER_SIZE_ULONG];
unsigned int midi_long_wrote = 0;
unsigned int n_audio_buff_count_o=0;
unsigned int max_frames_per_wv_buff = MAX_WAV_BUFFER_LEN/DB_BUFFER_MULT_CONST/N_UDP_SAMPLES;//MAX_FRAMES_PER_SINGLE_WAV_BUFF;
unsigned long sock_buff_count = 0;
unsigned long while_count=0;
	DOUBLE tick_time_len_sec = 1/AUDIO_RATE_H;
	DOUBLE aa=AUDIO_RATE_H;
	//DOUBLE bb=MIDI_RATE_H;

//	DOUBLE Midi_len_ticks = aa/bb;
//	DWORD dWnoteon_ticks=0;
	
		static dsp_float_t run_pow_slow[MAX_BUFFER_LEN];
	static dsp_float_t run_pow_fast[MAX_BUFFER_LEN];
	static dsp_float_t run_pow_slow_temp[MAX_BUFFER_LEN];
static dsp_float_t run_pow_slow_temp2[MAX_BUFFER_LEN];
static dsp_float_t run_pow_slow_filtered[MAX_BUFFER_LEN];
static dsp_float_t run_pow_slow_filtered_expanded[MAX_BUFFER_LEN];

    int n_byte_rec;
	s_int32 out_val = 0;

	
	MIDIOUTCAPS midicap;
	WAVEOUTCAPS pwoc;
	HWAVEOUT phwo;
	WAVEFORMATEX wvformatex;
	WAVEHDR wvheader;
    MMRESULT mmcap;
	UINT num_wav_devs = 0;
    //HMIDISTRM midistream_handle;
	HMIDIOUT midiout_handle;
    DWORD dwCallbackInstance=0;
    UINT midi_device_id = MIDI_DEVICE_ID;
	UINT wav_device_id = WAVE_MAPPER;
	MIDIHDR midi_header;
	MIDIPROPTIMEDIV proptd;
	MIDIPROPTEMPO   prop;
	//DWORD bpm = MIDI_BPM;
	//
	UINT i=0,j=0,k=0,idx=0,base_idx=0;
    unsigned long	err;
	HANDLE hThread=NULL;
	DWORD ThreadID ;
	DOUBLE udp_MAX_BUFFER_LEN = UINT(APPROX_UDP_MAX_BUFFER_LEN_SAMPLES);
	DOUBLE audio_buf_time_ms = udp_MAX_BUFFER_LEN/AUDIO_RATE_H*1e3;
	//DOUBLE midi_single_buff_len_mult_factor=MIDI_SINGLE_BUFF_LEN_MULT_FACTOR;
	//DOUBLE midi_single_buf_time_ms = audio_buf_time_ms*midi_single_buff_len_mult_factor;
	//DOUBLE n_midi_messages_per_single_buffer = midi_single_buf_time_ms*1e-3*MIDI_RATE_H;
	UINT n_midi_messages_per_single_buffer_uint = 0;
	//UINT midi_single_buffer_uint_len = STANDARD_MIDI_INFO_PER_MESSAGE*DWORD(n_midi_messages_per_single_buffer);
	unsigned long* midi_buffer_start_addres = &midi_buffer[0];
	unsigned long* midi_buffer_half_addres = &midi_buffer[STANDARD_MIDI_INFO_PER_MESSAGE*MAX_MIDI_MESSAGES_PER_SINGLE_BUF];
	short* wav_buffer_start_addres = &wav_buffer[0];
	short* wav_buffer_half_addres = &wav_buffer[N_UDP_SAMPLES*max_frames_per_wv_buff];
	/*midimessage_T message_note1,message_note2;
	midimessage_T message_note_off1,message_note_off2;
	midimessage_T message_note_play1,message_note_play2;
	midimessage_T message_note_off_play1,message_note_off_play2;*/
	INT ret_o=0;
	unsigned long *midi_producer_address_io,*midi_consumer_address_io;
	short *wav_producer_address_io,*wav_consumer_address_io;
	unsigned long midi_note,midi_noteoff;
	midiout_thread_params_T midi_thread_params;
	wavout_thread_params_T wav_thread_params;
	BOOL semout_o=FALSE,double_buff_region=FALSE;
	DWORD sleep_time = 0;
	//DOUBLE tick_time_len_sec = 1/AUDIO_RATE_H;
	DOUBLE bpm_timediv_prod = 60e6/(tick_time_len_sec*1e6);
	DOUBLE bpm =120;
	DOUBLE time_div = bpm_timediv_prod/bpm; 
	DWORD last_err_o = 0;
	midimessage_T prog_change,note_on, note_off,aftertouch,control_change;
	 s_uint64 time_id=0;
    s_uint32  num_samples=0;

	unsigned short pstate=1;
	dsp_float_t eval_counter ;
	dsp_float_t eval_pow ;
	 dsp_float_t eval_timer ;
	 SCDFPacketListener Test_Listener(MAX_SENSOR_FLOATS,MAX_AUDIO_FLOAT);
	IpEndpointName Test_endpoint;

	unsigned int remote_ip1=192;
	unsigned int remote_ip2=168;
	unsigned int remote_ip3=1;
	unsigned int remote_ip4=5;

	unsigned int local_ip1=192;
	unsigned int local_ip2=168;
	unsigned int local_ip3=1;
	unsigned int local_ip4=51;

	BOOLEAN blow_state=FALSE;
	s_float run_pow_fast_max = 0.6;
	s_float run_pow_slow_max = 0.45;
	unsigned int blow_frame_cnt=0;
	unsigned int n_frames_into_wav=0;
	unsigned int receive_cnt = 0;
	unsigned int kk=0;
	DWORD dwRate;
	//LARGE_INTEGER counter_freq ;
	BOOL out_query;
	s_float temp=0.0;
	unsigned int running_pow_win_fast=0U;
	unsigned int running_pow_win_slow=0U;
	s_float sat_thresh=0.0F;
	unsigned int n_sat_thres=0U;
	unsigned int eval_time_samples=0U;
	unsigned int  n_pow_thres_attack=0U;
	unsigned int  n_pow_thres_release=0U;
	unsigned int sample_count=0;
	s_float cg_val=0.0F;
	unsigned int temp_idx = 0;
	s_float breaks[6]={0,0.05,0.1,0.3,0.5,1};
	s_float fit_coeffs[(6-1)*4]={ 0.000000000000089,-0.000000000000009,1.000000000000000 ,0, 0,0,1.000000000000000,0.050000000000000,
										8.333333333333334 , -1.666666666666667 ,  1.000000000000000 ,  0.100000000000000,
									-21.505376344086020 ,  7.634408602150536  , 1.333333333333333 ,  0.300000000000000,
									-0.317050691244241 , -0.254377880184331  , 1.806451612903226  , 0.700000000000000};
	
                  
   
 
  
	

	/************STKINIT********/
	Stk::setSampleRate( AUDIO_RATE_H );
	//Stk::showWarnings( true );
	static Clarinet Flute(8.0);
	//StkFrames synth_frame(0,N_UDP_SAMPLES,1);
	static StkFloat synth_frame[N_UDP_SAMPLES];
	prog_change.data[0] = PROG_CHANGE; 
   prog_change.data[1] = INSTRUMENT_CODE;   
   prog_change.data[2] = 0;   
   prog_change.data[3] = 0;

   note_on.data[0] = NOTEON; 
   note_on.data[1] = 0x3d;   
   note_on.data[2] = 0x64;   
   note_on.data[3] = 0;

    note_off.data[0] = NOTEOFF; 
   note_off.data[1] = note_on.data[1];   
   note_off.data[2] = note_on.data[2];   
   note_off.data[3] = 0;
   #ifdef USE_AFTERTOUCH
   aftertouch.data[0]=POLYPHONIC_AFTERTOUCH;
#else
    aftertouch.data[0]=CHANNEL_PRESSURE;
#endif
   aftertouch.data[1]=0;
   aftertouch.data[2]=0;
   aftertouch.data[3]=0;

   control_change.data[0] = CONTROL_CHANGE; 
   control_change.data[1] = EXPRESSION;   
   control_change.data[2] = 0;   
   control_change.data[3] = 0;
  


    sprintf(ip_local,"%2.2d.%2.2d.%2.2d.%2.2d",local_ip1,local_ip2,local_ip3,local_ip4);
    sprintf(ip_remote,"%3.3d.%3.3d.%2.2d.%2.2d",remote_ip1,remote_ip2,remote_ip3,remote_ip4);

    printf("ip local %s\n",ip_local);
    printf("ip local %s\n",ip_remote);

    SCDF_Init(ip_local,ip_remote,port_id,&SOCK_sd,&localport_info);
    memset(&SCDF_rx_pkt,0,sizeof(SCDF_rx_pkt));
    memset(audio_buffer,0,sizeof(audio_buffer));

	
	blow_config2(N_BIT,MARGIN,FS,EVAL_TIME,RUNNING_POW_WIN_TIME_FAST,RUNNING_POW_WIN_TIME_SLOW,TIME_POW_THRESH_ATTACK,TIME_POW_THRESH_RELEASE,
	&sat_thresh,&n_sat_thres,&eval_time_samples,&n_pow_thres_attack,&n_pow_thres_release,&running_pow_win_fast,&running_pow_win_slow);


	running_pow_win_slow = 3;

	/***FILTER INIT****/

	for (i=0;i<running_pow_win_fast;i++)
	{
		bbfast[i]=1/((s_float)running_pow_win_fast);
		aafast[i]=0;
	}

	//for (i=0;i<running_pow_win_slow;i++)
	//{
	//	bbslow[i]=1/((s_float)running_pow_win_slow);
	//	aaslow[i]=0;
	//}
	gain1=0.000003515544430;
	gain2=0.000003508969011;
	gain3=0.001872555449701;
	bbslow[0]=1.0;
	bbslow[1]=2.0;
	bbslow[2]=1.0;
	bbslow2[0]=1.0;
	bbslow2[1]=2.0;
	bbslow2[2]=1.0;
	bbslow3[0]=1.0;
	bbslow3[1]=1.0;
	bbslow3[2]=0.0;
	aaslow[0]=1.0;
	aaslow[1]=-1.997669683425078;  
	aaslow[2]= 0.997683745602800;
	aaslow2[0]=1.0;
	aaslow2[1]=-1.993933273456987;
	aaslow2[2]=0.993947309333033;
	aaslow3[0]=1.0;
	aaslow3[1]= -0.996254889100598;
	aaslow3[2]=0.0;

	pow_filt_a[0]=1.0;
	pow_filt_a[1]=-1.997887293982930 ;   
	pow_filt_a[2]=0.997898864193988;

	pow_filt_b[0]=1.0*2.73316597838323e-08;
	pow_filt_b[1]=2.0*2.73316597838323e-08;
	pow_filt_b[2]=1.0*2.73316597838323e-08;

	pow_filt_a2[0]=1.0;
	pow_filt_a2[1]=-1.994496939236835;   
	pow_filt_a2[2]= 0.994508489813592;

	pow_filt_b2[0]=1.0*2.73276093372558e-08;
	pow_filt_b2[1]=2.0*2.73276093372558e-08;
	pow_filt_b2[2]=1.0*2.73276093372558e-08;

	pow_filt_a3[0]=1.0;
	pow_filt_a3[1]= -1.999361308716150;
	pow_filt_a3[2]= 0.999361418017236;

	pow_filt_b3[0]=1.0*2.73252713574353e-08;
	pow_filt_b3[1]=2.0*2.73252713574353e-08;
	pow_filt_b3[2]=1.0*2.73252713574353e-08;

		//dWnoteon_ticks =(DWORD)Midi_len_ticks;

	//if (midi_single_buf_time_ms<(1e3/MIDI_RATE_H))
	//{
	//	printf("buffer too small or midi rate too low\n");
	//	//return -27;
	//}

	/*if (midi_single_buffer_uint_len*DB_BUFFER_MULT_CONST>MAX_MIDI_BUFFER_SIZE_ULONG)
	{
		printf("BUFFER MIDI EXCEED MAXIMUM!\n");
		return -1;

	}*/

	/*if (udp_MAX_BUFFER_LEN>MAX_MIDI_BUFFER_SIZE_ULONG)
	{
		printf("BUFFER MIDI EXCEED MAXIMUM!\n");
		return -1;

	}*/

	//if ( fmod(AUDIO_RATE_H,MIDI_RATE_H) != 0 )
	//{
	//	printf("for easier handling midi rate must be integer multiple od audio rate\n");
	//	//return -1;
	//}

	/*********************MIDI INITIALIZATION**************************/

#ifdef _MIDI_

	mmcap= midiOutGetDevCaps (midi_device_id,&midicap,sizeof(midicap));

    if (mmcap!=MMSYSERR_NOERROR)
    {
		printf("ERROR in midiOutGetDevCaps\n");
        return -1;
    }

#else

#ifdef _STK_

	num_wav_devs= waveOutGetNumDevs();

	mmcap =  waveOutGetDevCaps((UINT_PTR)wav_device_id,&pwoc,sizeof(pwoc));

	if (mmcap!=MMSYSERR_NOERROR)
    {
		 if (mmcap==MMSYSERR_BADDEVICEID)
		{
        printf("MMSYSERR_BADDEVICEID\n");
		}
        else if(mmcap==MMSYSERR_NODRIVER)
		{
        printf("MMSYSERR_NODRIVER\n");
		}
        else if (mmcap==MMSYSERR_NOMEM)
		{
        printf("MMSYSERR_NOMEM\n");
		}
      //  return -1;
    }

	if ((pwoc.dwSupport & WAVECAPS_PLAYBACKRATE)==1)
	{
		printf("device supports pb rate change\n");
	}
	else
	{
		printf("device DO NOT supports pb rate change\n");
	}


#endif


#endif


	#ifdef _MIDI_

    mmcap = midiOutOpen(&midiout_handle,midi_device_id,0,0,  NULL);

    if (mmcap!=MMSYSERR_NOERROR)
    {
        if (mmcap==MMSYSERR_BADDEVICEID)
		{
        printf("MMSYSERR_BADDEVICEID\n");
		}
        else if(mmcap==MMSYSERR_INVALPARAM)
		{
        printf("MMSYSERR_INVALPARAM\n");
		}
        else if (mmcap==MMSYSERR_NOMEM)
		{
        printf("MMSYSERR_NOMEM\n");
		}
        return -2;
    }

#else 

#ifdef _STK_

	wvformatex.wFormatTag=WAVE_FORMAT_PCM;
	wvformatex.nChannels=1;
	wvformatex.wBitsPerSample=16;
	wvformatex.nSamplesPerSec=AUDIO_RATE_H;
    wvformatex.nBlockAlign=wvformatex.nChannels*(wvformatex.wBitsPerSample/8);
    wvformatex.nAvgBytesPerSec=wvformatex.nSamplesPerSec*wvformatex.nBlockAlign;
	wvformatex.cbSize=0;

	
	out_query=QueryPerformanceFrequency(&counter_freq);

//	mmcap =  waveOutOpen(&phwo,(UINT_PTR)wav_device_id,&wvformatex,NULL,0,CALLBACK_NULL | WAVE_FORMAT_DIRECT);

    mmcap =  waveOutOpen(&phwo,(UINT_PTR)wav_device_id,&wvformatex,(DWORD_PTR)waveOutProc,(DWORD_PTR)dwCallbackInstance,CALLBACK_FUNCTION | WAVE_FORMAT_DIRECT);

	if (mmcap!=MMSYSERR_NOERROR)
    {
        if (mmcap==MMSYSERR_BADDEVICEID)
		{
        printf("MMSYSERR_BADDEVICEID\n");
		}
        else if(mmcap==MMSYSERR_NODRIVER)
		{
        printf("MMSYSERR_NODRIVER\n");
		}
		else if(mmcap==MMSYSERR_INVALPARAM)
		{
        printf("MMSYSERR_INVALPARAM\n");
		}
        else if (mmcap==MMSYSERR_NOMEM)
		{
        printf("MMSYSERR_NOMEM\n");
		}
		 else if (mmcap==MMSYSERR_ALLOCATED)
		{
        printf("MMSYSERR_ALLOCATED\n");
		}
		 else if (mmcap==WAVERR_BADFORMAT)
		{
        printf("WAVERR_BADFORMAT\n");
		}
		  else if (mmcap==WAVERR_SYNC)
		{
        printf("WAVERR_SYNC\n");
		}
		
        return -2;
    }

	/*mmcap = waveOutGetPlaybackRate(phwo,&dwRate);

	if (dwRate!=0x00010000)
	{
		printf("pb rate = %x\n",dwRate);
	}
*/
	wav_thread_params.p_handle = phwo;
	wvheader.dwBufferLength=MAX_WAV_BUFFER_LEN/2*sizeof(short);
	wvheader.dwLoops=0;
	wav_thread_params.p_header = &wvheader;
	wav_thread_params.head_size=sizeof(wvheader);

	QueryPerformanceCounter(&toc_wav_write);
#endif

#endif

	

	//memset(&midi_header,0,sizeof(midi_header));
#ifdef _MIDI_ 
    memset(midi_buffer,0,sizeof(midi_buffer));
		midi_thread_params.p_handle=midiout_handle;
		midi_thread_params.p_midi_message=&prog_change;//&midi_header;
#else
#ifdef _STK_

	 memset(wav_buffer,0,sizeof(wav_buffer));

#endif

#endif
		  ghSemaphore = CreateSemaphore( 
						NULL,           // default security attributes
						SEMAPHORE_INIT_VALUE,  // initial count
						MAX_SEM_COUNT,  // maximum count
						NULL); 

		  if (ghSemaphore==NULL)
		  {
			  GetLastError();
			  printf("error in creating Semaphore\n");
			  goto gently_closure;
		  }
#ifdef _MIDI_ 
		  hThread = CreateThread(
                                NULL,                   // default security attributes
                                0,                      // use default stack size
                                midi_play_thread,       // thread function name
                                (LPVOID)&midi_thread_params,          // argument to thread function
                                0,                      // use default creation flags
                                &ThreadID);   // returns the thread identifier
#else
#ifdef _STK_
		  hThread = CreateThread(
                                NULL,                   // default security attributes
                                0,                      // use default stack size
                                wav_play_thread,       // thread function name
                                (LPVOID)&wav_thread_params,          // argument to thread function
                                0,                      // use default creation flags
                                &ThreadID);   // returns the thread identifier
#endif

#endif
		  if (hThread==NULL)
		  {
			  GetLastError();
			  printf("error in creating Thread\n");
			  goto gently_closure;
		  }

		

		

#ifdef _MIDI_

		  midi_producer_address_io=midi_buffer_half_addres;
		  midi_consumer_address_io=midi_buffer_start_addres;
		  /********** PROG CHANGE INIT **********/

		  midi_producer_address_io[0]=prog_change.word;

		  ret_o =  swap_addresses(midi_buffer_start_addres, midi_buffer_half_addres, &midi_producer_address_io, &midi_consumer_address_io);

			semout_o=ReleaseSemaphore( 
                        ghSemaphore,  // handle to semaphore
                        1,            // increase count by one
                        NULL);
				if (semout_o==FALSE)
				{
					last_err_o=GetLastError();
					printf("semaphore not released error code %d\n",last_err_o);
				}
				else
				{
					//printf("semaphore  released\n");
				}
#else
#ifdef _STK_

		  wav_producer_address_io=wav_buffer_half_addres;
		  wav_consumer_address_io=wav_buffer_start_addres;
		  mmcap =  waveOutRestart(phwo);
#endif
#endif

   /************************************************************************/
		Test_endpoint.address=( (remote_ip1 & 0xFF) <<24) | ( (remote_ip2 & 0xFF) <<16) | ( (remote_ip3 & 0xFF) <<8) | ( (remote_ip4 & 0xFF) <<0) ;//(remote_IP);
		Test_endpoint.port=port_id;
		//Flute.startBlowing(2,2);
			//	Flute.noteOn(440,0.8);
for (temp_idx=0;temp_idx<2048*N_FRAMES_FOR_SINGLE_BUFF;temp_idx++)//   while(1)//
    {
		if (temp_idx%N_FRAMES_FOR_SINGLE_BUFF==0)
		{
		QueryPerformanceCounter(&tic_buffer);
		}
        n_byte_rec=SCDF_receive(char_buff,sizeof(char_buff),ip_local,ip_remote,port_id,&SOCK_sd,&fds,&localport_info,&sock_buff_count);
		
		//memcpy(char_buff_cpy,char_buff,n_byte_rec);
	
		if (n_byte_rec>0)
		{
			
			receive_cnt++;
		Test_Listener.ProcessPacket(char_buff,n_byte_rec,Test_endpoint);
       //Parse_audio_data(&SCDF_rx_pkt,char_buff, audio_buffer,&time_id,&num_samples);
      //  printf("Time id is %lu \n",SCDF_rx_pkt.timeid);
		//num_samples=32;
		
	//	memset(run_pow_slow,0,sizeof(run_pow_slow));
	//	memset(run_pow_fast,0,sizeof(run_pow_fast));
		
		num_samples=Test_Listener.n_audio_data;
		memset(blow_array,0,num_samples*sizeof(unsigned short));
		out_val= blow_v2(Test_Listener.audio_data_buff_p,aaslow,bbslow,aaslow2,bbslow2,aaslow3,bbslow3,stateslow,stateslow2,stateslow3,aafast,bbfast,statefast,blow_array,run_pow_slow,run_pow_fast,&eval_counter,&eval_pow,&eval_timer,&pstate,Test_Listener.n_audio_data,
			running_pow_win_fast,running_pow_win_slow,sat_thresh, n_sat_thres,eval_time_samples,n_pow_thres_attack,n_pow_thres_release);

#ifdef _MIDI_
		
		for(i=0;i<num_samples-1;i++)
		{
			if (blow_state)
			{
				blow_frame_cnt++;
				if (blow_frame_cnt%1000==0)
				{
#ifdef VERBOSE_OSC
					printf("frame counter %u\n",blow_frame_cnt); 
#endif
				}
			}
			
			if (blow_array[i]==0 && blow_array[i+1]==3)
			{
#ifdef VERBOSE_OSC
				printf("BLOW STARTED at index %u\n",i);
#endif
				blow_state=TRUE;
				

				note_on.data[2]=(char)(run_pow_fast[i]/run_pow_fast_max*128);
				printf("BLOW Velocity %d \n",note_on.data[2]);
				midi_producer_address_io[0]=note_on.word;
				ret_o =  swap_addresses(midi_buffer_start_addres, midi_buffer_half_addres, &midi_producer_address_io, &midi_consumer_address_io);
				if (ret_o<0)
				{
					printf("error in swap\n");
					goto gently_closure;
				}
			  
				midi_thread_params.p_midi_message=(midimessage_T*)(midi_consumer_address_io);

			semout_o=ReleaseSemaphore( 
                        ghSemaphore,  // handle to semaphore
                        1,            // increase count by one
                        NULL);
				if (semout_o==FALSE)
				{
					last_err_o=GetLastError();
					printf("semaphore not released error code %d\n",last_err_o);
				}
				else
				{
					//printf("semaphore  released\n");
				}
			}
			else if (blow_array[i]==3 && blow_array[i+1]==0)
			{
				#ifdef VERBOSE_OSC
				printf("BLOW RELEASED\n");
#endif
				blow_frame_cnt=0;
				blow_state=FALSE;
				note_off.data[2]=(char)(run_pow_slow[i]/run_pow_fast_max*128);
				midi_producer_address_io[0]=note_off.word;

				
				ret_o =  swap_addresses(midi_buffer_start_addres, midi_buffer_half_addres, &midi_producer_address_io, &midi_consumer_address_io);
				if (ret_o<0)
				{
					printf("error in swap\n");
					goto gently_closure;
				}
			  
				midi_thread_params.p_midi_message=(midimessage_T*)(midi_consumer_address_io);

			semout_o=ReleaseSemaphore( 
                        ghSemaphore,  // handle to semaphore
                        1,            // increase count by one
                        NULL);
				if (semout_o==FALSE)
				{
					last_err_o=GetLastError();
					printf("semaphore not released error code %d\n",last_err_o);
				}
				else
				{
					//printf("semaphore  released\n");
				}
			}
			else if (blow_state && ( (blow_frame_cnt%10000==9999 ) ) )
			{
//#ifdef USE_AFTERTOUCH
//				aftertouch.data[1]=note_on.data[1];
//				 aftertouch.data[2]=(char)(run_pow_slow[i]/run_pow_slow_max*128);//-note_on.data[2
//#ifdef VERBOSE_OSC
//				printf("After touch value  %d at index %u\n", aftertouch.data[2],i);
//#endif
//				 midi_producer_address_io[0]=aftertouch.word;
//
//#else
//
//					//aftertouch.data[1]=note_on.data[1];
//				 aftertouch.data[1]=(char)(run_pow_slow[i]/run_pow_slow_max*128);//-note_on.data[2
//				 aftertouch.data[2]=0;
//#ifdef VERBOSE_OSC
//				printf("After touch value  %d at index %u\n", aftertouch.data[1],i);
//#endif
//				 midi_producer_address_io[0]=aftertouch.word;
//
//
//#endif
				 control_change.data[2]=(char)(run_pow_slow[i]/run_pow_slow_max*128);//-note_on.data[2
#ifdef VERBOSE_OSC
				printf("After touch value  %d at index %u\n", aftertouch.data[2],i);
#endif
				 midi_producer_address_io[0]=control_change.word;
				
				ret_o =  swap_addresses(midi_buffer_start_addres, midi_buffer_half_addres, &midi_producer_address_io, &midi_consumer_address_io);
				if (ret_o<0)
				{
					printf("error in swap\n");
					goto gently_closure;
				}
			  
				midi_thread_params.p_midi_message=(midimessage_T*)(midi_consumer_address_io);

			semout_o=ReleaseSemaphore( 
                        ghSemaphore,  // handle to semaphore
                        1,            // increase count by one
                        NULL);
				if (semout_o==FALSE)
				{
					last_err_o=GetLastError();
					printf("semaphore not released error code %d\n",last_err_o);
				}
				else
				{
					//printf("semaphore  released\n");
				}
			}
		//	

		//	
		//
		}

#else

#ifdef _STK_


//Flute.setFrequency(1200.0);

if (N_UDP_SAMPLES!=num_samples)
{
	return -55;
}

/************************************************ INSERT POINT ***********************/

for (i=0;i<num_samples-1;i++)
{
	run_pow_slow[i]=run_pow_slow[i]/0.45;
	/*if (run_pow_slow[i]>1) 
	{
		printf("pow not normalized correctly\n");
	}*/
}

out_val = filter_DII_T (run_pow_slow, run_pow_slow_temp, pow_filt_a, 2, pow_filt_b, statepow,num_samples);
out_val = filter_DII_T (run_pow_slow_temp, run_pow_slow_temp2, pow_filt_a2, 2, pow_filt_b2, statepow2,num_samples);
out_val = filter_DII_T (run_pow_slow_temp2, run_pow_slow_filtered, pow_filt_a3, 2, pow_filt_b3, statepow3,num_samples);
out_val = static_nonlinearity(run_pow_slow_filtered_expanded,run_pow_slow_filtered,num_samples,breaks,6,fit_coeffs,3 );
//for (i=0;i<num_samples-1;i++)
//{
//	
//	if (run_pow_slow_filtered_expanded[i]>1) 
//	{
//		printf("pow expanded not norm\n");
//	}
//}

for(i=0;i<num_samples-1;i++)
		{
			if (blow_state)
			{
				blow_frame_cnt++;
				if (blow_frame_cnt%1000==0)
				{
#ifdef VERBOSE_OSC
					printf("frame counter %u\n",blow_frame_cnt); 
#endif
				}
			}
			
			if (blow_array[i]==0 && blow_array[i+1]==3)
			{
#ifdef VERBOSE_OSC
				printf("BLOW STARTED at index %u\n",i);
#endif
				blow_state=TRUE;
				//Flute.startBlowing(0.4,0.1);
				Flute.noteOn(400,0.9);

			
			}
			else if (blow_array[i]==3 && blow_array[i+1]==0)
			{
				#ifdef VERBOSE_OSC
				printf("BLOW RELEASED\n");
#endif
				blow_frame_cnt=0;
				blow_state=FALSE;
				Flute.noteOff(0.9);
				//Flute.stopBlowing(0.4);
				
			}
		//	else if (blow_state )
		//	{
		//		//cg_val=40*(sin(2*PI*fmod(2/FS*sample_count,1)))+80;
		//		temp=(run_pow_slow_filtered_expanded[i]*(128-60)+60);
		//		/*if (temp>128.0 || temp<0)
		//		{
		//			printf("wrong cg value \n");
		//		}*/
		//		printf("Blowinf pressure %f\n",run_pow_slow[i]);
		//		Flute.controlChange(128,(StkFloat)temp);
		//		sample_count++;
		//	}
		////	

			synth_frame[i]=Flute.tick(0);
			//printf("wave %f\n",synth_frame[i]);
			/*if (synth_frame[i]>1.0 || synth_frame[i]<-1.0)
			{
				printf("wave overflow\n");
			}*/
		//
		}




/*********************************** END INSERT POINT **************************************/

n_frames_into_wav=(receive_cnt-1)%max_frames_per_wv_buff;

for (kk=0;kk<num_samples;kk++)
{
	//wav_producer_address_io[n_frames_into_wav*num_samples+kk]=(short)(Test_Listener.audio_data_buff_p[kk]*pow(2.0,15));
	wav_producer_address_io[n_frames_into_wav*num_samples+kk]=(short)(synth_frame[kk]*pow(2.0,15));
}
//memcpy(&wav_producer_address_io[n_frames_into_wav*num_samples],Test_Listener.audio_data_buff_p,num_samples*sizeof(s_float));

if (n_frames_into_wav==(max_frames_per_wv_buff-1))
{
	ret_o =  wav_swap_addresses(wav_buffer_start_addres, wav_buffer_half_addres, &wav_producer_address_io, &wav_consumer_address_io);
				if (ret_o<0)
				{
					printf("error in swap\n");
					goto gently_closure;
				}

				wvheader.dwBytesRecorded=max_frames_per_wv_buff*num_samples*sizeof(short);
			wvheader.lpData =(LPSTR) wav_consumer_address_io;
			wvheader.dwFlags=0;

			mmcap =  waveOutPrepareHeader(phwo,&wvheader,sizeof(wvheader));
			if (mmcap!=MMSYSERR_NOERROR)
			{
				 if (mmcap==MMSYSERR_INVALHANDLE)
					{
					printf("MMSYSERR_INVALHANDLE\n");
					}
					else if(mmcap==MMSYSERR_NODRIVER)
					{
					printf("MMSYSERR_NODRIVER\n");
					}
					else if (mmcap==MMSYSERR_NOMEM)
					{
					printf("MMSYSERR_NOMEM\n");
					}
					else if(mmcap==MMSYSERR_INVALPARAM)
					{
					printf("MMSYSERR_INVALPARAM\n");
					}
					
				return -98;
			}

					//wav_thread_params.p_midi_message=(midimessage_T*)(wav_consumer_address_io);

			
				QueryPerformanceCounter(&toc_buffer);
	//printf("buffer ticks %f ms %d bytes\n", ((double)toc_buffer.QuadPart-(double)tic_buffer.QuadPart)/(double)counter_freq.QuadPart*1e3,n_byte_rec);
			semout_o=ReleaseSemaphore( 
                        ghSemaphore,  // handle to semaphore
                        1,            // increase count by one
                        NULL);
				if (semout_o==FALSE)
				{
					last_err_o=GetLastError();
					printf("semaphore not released error code %d\n",last_err_o);
				}
				else
				{
					//printf("semaphore  released\n");
				}

}

#endif
#endif
		

	//	out_val= midi_packer(blow_array,dec_factor, midi_producer_address_io,run_pow_slow,(unsigned int) MIDI_SINGLE_BUFF_LEN_MULT_FACTOR,&n_audio_buff_count_o,&midi_long_wrote,dWnoteon_ticks);
		
		}
		else
		{

			printf("0 byte received !\n");
		}

		while_count++;
    }

	gently_closure : CloseHandle(ghSemaphore);
					CloseHandle(hThread);
					

				
#ifdef _MIDI_
				midiOutReset(midiout_handle);

   // Remove any data in MIDI device and close the MIDI Output port
   midiOutClose(midiout_handle);

#endif
	return 0;
}

