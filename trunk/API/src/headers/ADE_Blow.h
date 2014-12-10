#ifndef _ADE_BLOW_H
#define _ADE_BLOW_H
#include "headers/ADE_typedefs.h"

struct ADE_BLOW_S
{
    ADE_UINT32_T n_registered_funcs;
    ADE_UINT32_T fir_order;
    ADE_UINT32_T iir_order;
    ADE_UINT32_T buff_size;
    ADE_FIR_T *p_fir;
    ADE_IIR_T *p_iir;
    ADE_FLOATING_T *p_in;
    ADE_FLOATING_T *p_out;
    ADE_FLOATING_T pow_thresh_high;
    ADE_FLOATING_T pow_thresh_low;
    ADE_FLOATING_T sat_thresh;
    ADE_FLOATING_T running_pow_win_fast;
    ADE_FLOATING_T running_pow_win_slow;
    ADE_UINT32_T n_sat_thres;
    ADE_UINT32_T n_pow_thres_attack;
    ADE_UINT32_T n_pow_thres_release;
    ADE_UINT32_T eval_time_samples;
    ADE_UINT32_T *p_eval_counter;
    ADE_UINT32_T *p_eval_pow;
    ADE_UINT32_T *p_eval_timer;
    BLOW_SM_STATES_T *p_blow_state;
    ADE_BLOW_FUNCS_T *p_blow_functions;
};




#endif //_ADE_BLOW_H
