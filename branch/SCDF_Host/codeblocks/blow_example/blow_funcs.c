#include "typedef.h"
#include "defines.h"
#include "blow_funcs.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

int blow_config2(dsp_float_t nbit_i,dsp_float_t Margin_i,dsp_float_t Fs_i,dsp_float_t eval_time_i,dsp_float_t running_pow_win_time_fast_i,dsp_float_t running_pow_win_time_slow_i,dsp_float_t time_pow_thresh_attack_i,dsp_float_t time_pow_thresh_release_i,
	dsp_float_t *sat_thresh,unsigned int *n_sat_thres,unsigned int *eval_time_samples,unsigned int  *n_pow_thres_attack,unsigned int  *n_pow_thres_release,unsigned int *running_pow_win_fast,unsigned int *running_pow_win_slow)

{


	dsp_float_t max_amp =0.0F;
	dsp_float_t n_pow_thres_samples_attack=0.0F;
	dsp_float_t n_pow_thres_samples_release=0.0F;

  max_amp = 1-pow(2.0F,-(nbit_i-1));

  *sat_thresh = (1-Margin_i)*max_amp;


  *eval_time_samples = floor(eval_time_i*Fs_i+0.5)+1;
 *n_sat_thres=20;

*running_pow_win_fast = floor(running_pow_win_time_fast_i*Fs_i+0.5)+1;

*running_pow_win_slow = floor(running_pow_win_time_slow_i*Fs_i+0.5)+1;



n_pow_thres_samples_attack=floor(time_pow_thresh_attack_i*Fs_i+0.5)+1;
*n_pow_thres_attack=floor(n_pow_thres_samples_attack+0.5);

n_pow_thres_samples_release=floor(time_pow_thresh_release_i*Fs_i+0.5)+1;
*n_pow_thres_release=floor(n_pow_thres_samples_release+0.5);


 /**bb_fast=1/running_pow_win_fast*ones(1,running_pow_win_fast);

 *bb_slow=1/running_pow_win_slow*ones(1,running_pow_win_slow);*/


 return SAFE_RET_VAL;

}

int saturation_detector_compact(Blowargs_T* p_blow_struct,dsp_float_t sample,dsp_float_t running_pow_fast,dsp_float_t running_pow_slow)
{


static unsigned short  State_waiting   = 1;
static unsigned short  State_evaluation= 2;
static unsigned short  State_trackin   = 3;
unsigned short pstate_int = *(p_blow_struct->pstate);
dsp_float_t eval_counter_int=*(p_blow_struct->eval_counter);
dsp_float_t eval_pow_int=*(p_blow_struct->eval_pow);
dsp_float_t eval_timer_int=*(p_blow_struct->eval_timer);
dsp_float_t abs_sample = fabs(sample);
dsp_float_t pow_thresh_high=p_blow_struct->pow_thresh_high;
dsp_float_t pow_thresh_low=p_blow_struct->pow_thresh_low;
dsp_float_t sat_thresh=p_blow_struct->sat_thresh;
dsp_float_t n_sat_thres=p_blow_struct->n_sat_thres;
dsp_float_t n_pow_thres_attack=p_blow_struct->n_pow_thres_attack;
dsp_float_t n_pow_thres_release=p_blow_struct->n_pow_thres_release;
dsp_float_t eval_time_init=p_blow_struct->eval_time_samples;
dsp_float_t *eval_pow=p_blow_struct->eval_pow;
dsp_float_t *eval_counter=p_blow_struct->eval_counter;
dsp_float_t *eval_timer=p_blow_struct->eval_timer;
unsigned short *pstate=p_blow_struct->pstate;

    if (pstate_int==State_trackin)
	{

        if (running_pow_slow<=pow_thresh_low)
		{
            *eval_pow=eval_pow_int+1;
		}
        else
		{
            *eval_pow=eval_pow_int;
		}

       if (*eval_pow>n_pow_thres_release)
	   {
            *pstate=State_waiting;
	   }
       else
	   {
             *pstate=pstate_int;
	   }

       *eval_counter=0;
       *eval_timer=0;

	}
    else if (pstate_int==State_evaluation)
	{
        *eval_timer=eval_timer_int-1;

		if (abs_sample>sat_thresh)
		{
            *eval_counter=eval_counter_int+1;
		}
        else
		{
             *eval_counter=eval_counter_int;
		}
        if (running_pow_fast>=pow_thresh_high)
		{
            *eval_pow=eval_pow_int+1;
		}
        else
		{
            *eval_pow=eval_pow_int;
		}


        if (*eval_counter>n_sat_thres && *eval_timer>=0 && *eval_pow>=n_pow_thres_attack)
		{
            *pstate=State_trackin;
            *eval_pow=0;
		}
        else if (*eval_timer==0)
		{
            *pstate=State_waiting;
		}
        else
		{
            *pstate=State_evaluation;
		}

	}

    else if (pstate_int==State_waiting)


        if (abs_sample>sat_thresh)
		{
            *eval_timer=eval_time_init;
            *eval_counter=0;
            *eval_pow= 0;

            *pstate=State_evaluation;
		}
        else
		{
            *eval_timer=eval_timer_int;
             *eval_counter= eval_counter_int;
             *eval_pow=0;
            *pstate=State_waiting;

		}

    else
	{

        printf("error('State Unknown');\n");
		return -1;

	}

	return SAFE_RET_VAL;

}

int filter_DII_T_b (dsp_float_t *in, dsp_float_t *out, dsp_float_t *a, unsigned int order, dsp_float_t *b,dsp_float_t gain, dsp_float_t *state,unsigned int len_frame)
{
    unsigned int i=0,k=0;

    for (k=0;k<len_frame;k++)
    {


    out[k] = gain*b[0]*(in[k])+state[0];

        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        {
            state[i]=b[i+1]*(in[k])+state[i+1]-a[i+1]*(out[k]);
        }

    }

    return SAFE_RET_VAL;

}

int filter_fir (dsp_float_t *in, dsp_float_t *out, unsigned int order, dsp_float_t *b,dsp_float_t gain, dsp_float_t *state,unsigned int len_frame)
{
    unsigned int i=0,k=0;

    for (k=0;k<len_frame;k++)
    {


    out[k] = gain*b[0]*(in[k])+state[0];

        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        {
            state[i]=b[i+1]*(in[k])+state[i+1];
        }

    }

    return SAFE_RET_VAL;

}

int blow_compact(Blowargs_T* p_blow_struct)
{
	unsigned int out_val;

	unsigned int i=0;
	int ret = 0;
	/*static unsigned short pstate=1;
	static dsp_float_t eval_counter ;
	static dsp_float_t eval_pow ;
	static dsp_float_t eval_timer ;
*/
	static dsp_float_t audio_pow[MAX_BUFFER_LEN];
	//static dsp_float_t slow_pow_temp[MAX_BUFFER_LEN];
	//static dsp_float_t slow_pow_temp2[MAX_BUFFER_LEN];

	unsigned int num_samples=p_blow_struct->num_samples;
	unsigned int sos_section_order = 2;
	dsp_float_t* audio_data=p_blow_struct->in_buff;
	unsigned int n_sos_sections = p_blow_struct->n_sos_sections;
	dsp_float_t* a=NULL;
	dsp_float_t* b=NULL;
	dsp_float_t* state=NULL;
	dsp_float_t* in_buff=NULL;
	dsp_float_t* out_buff=NULL;
	dsp_float_t gain=0;
	dsp_float_t run_pow_fast_sample=0;
	dsp_float_t run_pow_slow_sample=0;

	for (i=0;i<num_samples;i++)
	{
		audio_pow[i]=audio_data[i]*audio_data[i];
	}

	memcpy(audio_pow,p_blow_struct->section_buffers[0],sizeof(audio_pow));

	for (i=0;i<n_sos_sections;i++)
	{
	  a=p_blow_struct->section_denom[i];
	  b=p_blow_struct->section_num[i];
	  state=p_blow_struct->section_iir_states[i];
	  gain=p_blow_struct->section_gain[i];
	  if(i==0)
	  {
	    in_buff=audio_pow;
	  }
	  else
	  {
	    in_buff=p_blow_struct->section_buffers[i-1];
	  }
	  out_buff=p_blow_struct->section_buffers[i];

	out_val = filter_DII_T_b (in_buff, out_buff, a, sos_section_order, b, gain,state,num_samples);

	}

	memcpy(out_buff,p_blow_struct->run_pow_slow,sizeof(audio_pow));

	out_val = filter_fir (audio_pow,p_blow_struct-> run_pow_fast,p_blow_struct-> running_pow_win_fast, p_blow_struct->fir_coeffs[0], p_blow_struct->fir_gains[0],p_blow_struct->fir_states[0],num_samples);

	for (i=0;i<num_samples;i++)
	{
		run_pow_fast_sample=p_blow_struct-> run_pow_fast[i];
		run_pow_slow_sample=p_blow_struct-> run_pow_slow[i];
		ret= saturation_detector_compact(p_blow_struct,audio_data[i],run_pow_fast_sample,run_pow_slow_sample);

		if (*(p_blow_struct->pstate)==3)
		{
			p_blow_struct->blow_state_array[i]=*(p_blow_struct->pstate);
		//	printf("BLOWWWWWWWWWW\n");
		}
	}

	return 0;
}
