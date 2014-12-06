#include <stdio.h>
#include <stdlib.h>
#include "defines.h"
#include "typedef.h"
#include "blow_funcs.h"

int main()
{

static dsp_float_t Input_audio_buffer[MAX_BUFFER_LEN];
    /************Allocazione statica memoria*********/
static dsp_float_t bbfast[MAX_FIR_FILT_LEN];
static dsp_float_t bbslow[MAX_IIR_FILT_LEN];
static dsp_float_t bbslow2[MAX_IIR_FILT_LEN];
static dsp_float_t bbslow3[MAX_IIR_FILT_LEN];
static dsp_float_t aaslow[MAX_IIR_FILT_LEN];
static dsp_float_t aaslow2[MAX_IIR_FILT_LEN];
static dsp_float_t aaslow3[MAX_IIR_FILT_LEN];
static dsp_float_t fir_statefast[MAX_IIR_FILT_LEN];
static dsp_float_t iir_stateslow[MAX_IIR_FILT_LEN];
static dsp_float_t iir_stateslow2[MAX_IIR_FILT_LEN];
static dsp_float_t iir_stateslow3[MAX_IIR_FILT_LEN];

static dsp_float_t run_pow_slow[MAX_BUFFER_LEN];
static dsp_float_t run_pow_fast[MAX_BUFFER_LEN];

static dsp_float_t filter_buff1[MAX_BUFFER_LEN];
static dsp_float_t filter_buff2[MAX_BUFFER_LEN];
static dsp_float_t filter_buff3[MAX_BUFFER_LEN];
static dsp_float_t filter_buff4[MAX_BUFFER_LEN];

static dsp_float_t iir_gains[N_SOS_SECTIONS];
static dsp_float_t fir_gains[N_FIR];
static dsp_float_t len_fir_coeffs[N_FIR];

static dsp_float_t blow_array[MAX_BUFFER_LEN];

Blowargs_T blow_params;

dsp_float_t eval_counter ;
dsp_float_t eval_pow ;
dsp_float_t eval_timer ;
unsigned short pstate=1;
unsigned num_frames_per_buffer=8;
unsigned num_samples_per_frame=MAX_BUFFER_LEN/num_frames_per_buffer;


/*************Allocazione statica e inizializzazione puntatori ai buffer*************/

static dsp_float_t* section_num[N_SOS_SECTIONS]={bbslow,bbslow2,bbslow3};
static dsp_float_t* section_denom[N_SOS_SECTIONS]={aaslow,aaslow2,aaslow3};
static dsp_float_t* iir_states[N_SOS_SECTIONS]={iir_stateslow,iir_stateslow2,iir_stateslow3};
static dsp_float_t* fir_states[N_FIR]={fir_statefast};
static dsp_float_t* buffers[N_SOS_SECTIONS+1]={filter_buff1,filter_buff2,filter_buff3,filter_buff4};
static dsp_float_t* fir_coeffs[N_FIR]={bbfast};

/**********Allocazione parametri blow************/

unsigned int running_pow_win_fast=0U;
unsigned int running_pow_win_slow=0U;
dsp_float_t sat_thresh=0.0F;
unsigned int n_sat_thres=0U;
unsigned int eval_time_samples=0U;
unsigned int  n_pow_thres_attack=0U;
unsigned int  n_pow_thres_release=0U;
unsigned int i=0,k=0;





blow_params.n_sos_sections=N_SOS_SECTIONS;
blow_params.section_num=section_num;
blow_params.section_denom=section_denom;
blow_params.section_gain=iir_gains;
blow_params.section_iir_states=iir_states;
blow_params.section_buffers=buffers;
blow_params.n_fir=N_FIR;
blow_params.n_fir_coeffs=len_fir_coeffs;
blow_params.fir_coeffs=fir_coeffs;
blow_params.fir_gains=fir_gains;
blow_params.fir_states=fir_states;
blow_params.blow_state_array=blow_array;
blow_params.run_pow_fast=run_pow_fast;
blow_params.run_pow_slow=run_pow_slow;
blow_params.eval_counter=&eval_counter;
blow_params.eval_pow=&eval_pow;
blow_params.eval_timer=&eval_timer;
blow_params.pstate=&pstate;
blow_params.num_samples=num_samples_per_frame;

blow_params.pow_thresh_high=POW_THRESH_HIGH;
blow_params.pow_thresh_low=POW_THRESH_LOW;



blow_config2(N_BIT,MARGIN,FS,EVAL_TIME,RUNNING_POW_WIN_TIME_FAST,RUNNING_POW_WIN_TIME_SLOW,TIME_POW_THRESH_ATTACK,TIME_POW_THRESH_RELEASE,
	&sat_thresh,&n_sat_thres,&eval_time_samples,&n_pow_thres_attack,&n_pow_thres_release,&running_pow_win_fast,&running_pow_win_slow);

blow_params.running_pow_win_fast=running_pow_win_fast;
blow_params.running_pow_win_slow=0;
blow_params.sat_thresh=sat_thresh;
blow_params.n_sat_thres=n_sat_thres;
blow_params.eval_time_samples=eval_time_samples;
blow_params.n_pow_thres_attack=n_pow_thres_attack;
blow_params.n_pow_thres_release=n_pow_thres_release;

fir_gains[0]=1.0;
iir_gains[0]=0.000003515544430;
iir_gains[1]=0.000003508969011;
iir_gains[2]=0.001872555449701;
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

for (i=0;i<running_pow_win_fast;i++)
	{
		bbfast[i]=1/((dsp_float_t)running_pow_win_fast);
	}
len_fir_coeffs[0]=running_pow_win_fast;


for (i=0;i<num_frames_per_buffer;i++)
{
    blow_params.in_buff=&Input_audio_buffer[i*num_samples_per_frame];
    blow_compact(&blow_params);


    for(k=0;k<num_samples_per_frame;k++)
    {
        if (*(blow_params.pstate)==3)
        {
            printf("Blowing with power %f\n",blow_params.run_pow_slow[k]);
        }
    }

}



    return SAFE_RET_VAL;
}
