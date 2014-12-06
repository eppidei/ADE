#ifndef _TYPEDEFS_H
#define _TYPEDEFS_H

typedef float dsp_float_t;

typedef struct Blowargs {

dsp_float_t* in_buff;
unsigned int n_sos_sections;
dsp_float_t** section_num;
dsp_float_t** section_denom;
dsp_float_t* section_gain;
dsp_float_t** section_iir_states;
dsp_float_t** section_buffers;
unsigned int n_fir;
dsp_float_t* n_fir_coeffs;
dsp_float_t** fir_coeffs;
dsp_float_t* fir_gains;
dsp_float_t** fir_states;
dsp_float_t* blow_state_array;
dsp_float_t* run_pow_fast;
dsp_float_t* run_pow_slow;
dsp_float_t* eval_counter;
dsp_float_t* eval_pow;
dsp_float_t* eval_timer;
unsigned short *pstate;
unsigned int num_samples;
unsigned int running_pow_win_fast;
unsigned int running_pow_win_slow;
dsp_float_t pow_thresh_high;
dsp_float_t pow_thresh_low;
dsp_float_t sat_thresh;
unsigned int n_sat_thres;
unsigned int eval_time_samples;
unsigned int  n_pow_thres_attack;
unsigned int  n_pow_thres_release;
}Blowargs_T;

#endif
