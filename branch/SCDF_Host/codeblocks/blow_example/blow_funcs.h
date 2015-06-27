#ifndef _BLOW_FUNCS_H
#define _BLOW_FUNCS_H
#include "typedef.h"

int blow_config2(dsp_float_t nbit_i,dsp_float_t Margin_i,dsp_float_t Fs_i,dsp_float_t eval_time_i,dsp_float_t running_pow_win_time_fast_i,dsp_float_t running_pow_win_time_slow_i,dsp_float_t time_pow_thresh_attack_i,dsp_float_t time_pow_thresh_release_i,
	dsp_float_t *sat_thresh,unsigned int *n_sat_thres,unsigned int *eval_time_samples,unsigned int  *n_pow_thres_attack,unsigned int  *n_pow_thres_release,unsigned int *running_pow_win_fast,unsigned int *running_pow_win_slow);


int blow_compact(Blowargs_T* p_blow_struct);
int filter_DII_T_b (dsp_float_t *in, dsp_float_t *out, dsp_float_t *a, unsigned int order, dsp_float_t *b,dsp_float_t gain, dsp_float_t *state,unsigned int len_frame);
int filter_fir (dsp_float_t *in, dsp_float_t *out, unsigned int order, dsp_float_t *b,dsp_float_t gain, dsp_float_t *state,unsigned int len_frame);

	#endif
