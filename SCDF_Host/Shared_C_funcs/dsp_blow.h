#ifndef _DSP_BLOW_H
#define _DSP_BLOW_H

#include "my_typedefs.h"



#ifdef __cplusplus
extern "C"
{
#endif

int blow_v1(float* audio_data,s_float* a_slow,s_float *b_slow,s_float *state_slow,s_float* a_fast,s_float *b_fast,s_float *state_fast,
	unsigned short *blow_array,dsp_float_t* run_pow_slow,dsp_float_t* run_pow_fast,dsp_float_t* eval_counter,dsp_float_t* eval_pow,dsp_float_t* eval_timer,unsigned short *pstate,s_int32 num_samples);

int blow_v2(float* audio_data,s_float* a_slow,s_float *b_slow,s_float* a_slow2,s_float *b_slow2,s_float* a_slow3,s_float *b_slow3,s_float *state_slow,s_float *state_slow2,s_float *state_slow3,s_float* a_fast,s_float *b_fast,
	s_float *state_fast,unsigned short *blow_array,dsp_float_t* run_pow_slow,dsp_float_t* run_pow_fast,dsp_float_t* eval_counter,dsp_float_t* eval_pow,dsp_float_t* eval_timer,unsigned short *pstate,s_int32 num_samples,
	unsigned int running_pow_win_fast,unsigned int running_pow_win_slow,s_float sat_thresh,unsigned int n_sat_thres,unsigned int eval_time_samples,unsigned int  n_pow_thres_attack,unsigned int  n_pow_thres_release);

int saturation_detector_sample_based(unsigned short *pstate, dsp_float_t sample,dsp_float_t running_pow_fast, dsp_float_t running_pow_slow,dsp_float_t pow_thresh_high,
	dsp_float_t pow_thresh_low,dsp_float_t sat_thresh,dsp_float_t n_sat_thres,dsp_float_t n_pow_thres,dsp_float_t eval_time_init,dsp_float_t *eval_counter, dsp_float_t *eval_pow,dsp_float_t *eval_timer);

int saturation_detector_sample_based2(unsigned short *pstate, dsp_float_t sample,dsp_float_t running_pow_fast, dsp_float_t running_pow_slow,dsp_float_t pow_thresh_high,
	dsp_float_t pow_thresh_low,dsp_float_t sat_thresh,dsp_float_t n_sat_thres,dsp_float_t n_pow_thres_attack,dsp_float_t n_pow_thres_release,dsp_float_t eval_time_init,dsp_float_t *eval_counter, dsp_float_t *eval_pow,dsp_float_t *eval_timer);

int blow_config2(s_float nbit_i,s_float Margin_i,s_float Fs_i,s_float eval_time_i,s_float running_pow_win_time_fast_i,s_float running_pow_win_time_slow_i,s_float time_pow_thresh_attack_i,s_float time_pow_thresh_release_i,
	s_float *sat_thresh,unsigned int *n_sat_thres,unsigned int *eval_time_samples,unsigned int  *n_pow_thres_attack,unsigned int  *n_pow_thres_release,unsigned int *running_pow_win_fast,unsigned int *running_pow_win_slow);

int static_nonlinearity(s_float *data_out,s_float *data_frame,unsigned int len_frame,s_float *breaks,unsigned int n_breaks,s_float *coeffs,unsigned int order );

#ifdef __cplusplus
}
#endif


#endif