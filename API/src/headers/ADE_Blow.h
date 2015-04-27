#ifndef _ADE_BLOW_H
#define _ADE_BLOW_H
#include "headers/ADE_typedefs.h"
#ifdef ADE_CONFIGURATION_INTERACTIVE
#include "headers/ADE_Matlab.h"
#include "matrix.h"
#include "engine.h"
#endif

struct ADE_BLOW_S
{
    ADE_FLOATING_T Fs_i;
    ADE_UINT32_T buff_len_i;
    ADE_FLOATING_T Fs_o;
    ADE_UINT32_T buff_len_o;
    ADE_UINT32_T fir_order;
    ADE_FIR_T *p_fir;
    ADE_IIR_T *p_iir;
    ADE_IIR_T *p_iir2;
    ADE_FLOATING_T *p_in;//allocato fuori
    ADE_FLOATING_T *p_in_squared;//allocato dentro
    ADE_FLOATING_T *p_pow_fast;//allocato dentro
    ADE_FLOATING_T *p_pow_slow;//allocato dentro
    ADE_FLOATING_T *p_pow_slow_filtered;//allocato dentro
    ADE_FLOATING_T *p_out;//allocato dentro
    ADE_BOOL_T state;
    ADE_FLOATING_T pow_thresh_high;
    ADE_FLOATING_T pow_thresh_low;
    ADE_FLOATING_T sat_thresh;
    ADE_UINT32_T running_pow_win_fast;
    ADE_UINT32_T running_pow_win_slow;
    ADE_UINT32_T n_sat_thres;
    ADE_UINT32_T n_pow_thres_attack;
    ADE_UINT32_T n_pow_thres_release;
    ADE_UINT32_T eval_time_samples;
    ADE_UINT32_T *p_eval_counter;
    ADE_UINT32_T *p_eval_pow;
    ADE_INT32_T *p_eval_timer;
    BLOW_SM_STATES_T *p_blow_state;
    ADE_POLYFIT_T *p_poly;
    //ADE_BLOW_FUNCS_T *p_blow_functions;
    ADE_UINT32_T n_breaks;
    //ADE_FLOATING_T *breaks;
    ADE_UINT32_T poly_order;
    //ADE_FLOATING_T *poly_coeffs;
    ADE_blas_level2_T *p_blas_l2;
    #ifdef ADE_CONFIGURATION_INTERACTIVE
    ADE_MATLAB_T *p_mat;
    #endif
};

#ifdef __cplusplus
    extern "C" {
#endif

/***************** Init Methods *************************/
ADE_API_RET_T ADE_Blow_Init(ADE_BLOW_T** dp_this,ADE_UINT32_T buff_size,ADE_FLOATING_T Fs_i,ADE_FLOATING_T Fs_o_i);
ADE_VOID_T ADE_Blow_Release(ADE_BLOW_T* p_blow);
/************* Set Methods *************************/
/************* Get Methods *************************/
ADE_API_RET_T ADE_Blow_GetOutBuff(ADE_BLOW_T* p_blow, ADE_FLOATING_T **dp_buff);
/******************* Configure Methods ************************/
ADE_API_RET_T ADE_Blow_Configure_params(ADE_BLOW_T* p_blow);
ADE_API_RET_T ADE_Blow_Configure_inout(ADE_BLOW_T* p_blow,ADE_FLOATING_T *p_inbuff);
ADE_API_RET_T ADE_Blow_Configure(ADE_BLOW_T* p_blow,ADE_FLOATING_T *p_inbuff);
/************ Processing methods ***********************/
ADE_API_RET_T ADE_Blow_Step(ADE_BLOW_T* p_blow);

#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_BLOW_H
