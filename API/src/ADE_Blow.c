#include "headers/ADE_Blow.h"
#include "headers/ADE_fir.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_blas_level2.h"
#include "headers/ADE_polyfit.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include "headers/ADE_defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "headers/ADE_Error_Handler.h"





//#include <string.h>

static ADE_API_RET_T ADE_Blow_Fir_Config(ADE_BLOW_T* p_blow);
//static ADE_API_RET_T ADE_saturation_detector_sm(ADE_BLOW_T* p_blow,ADE_FLOATING_T sample_i,ADE_FLOATING_T running_pow_fast_i,ADE_FLOATING_T running_pow_slow_i);
static ADE_API_RET_T ADE_Blow_Iir_Config(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Iir2_Config(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Static_Params(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Configuration(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Set_SatThresh(ADE_BLOW_T* p_blow,ADE_FLOATING_T nbit_i,ADE_FLOATING_T Margin_i);
static ADE_API_RET_T ADE_Blow_Set_EvalTimesample(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T eval_time_i);
static ADE_API_RET_T ADE_Blow_Set_NSatThresh(ADE_BLOW_T* p_blow,ADE_FLOATING_T n_sat_thres_i);
static ADE_API_RET_T ADE_Blow_Set_NPowThreshHigh(ADE_BLOW_T* p_blow,ADE_FLOATING_T pow_thresh_high_i);
static ADE_API_RET_T ADE_Blow_Set_NPowThreshLow(ADE_BLOW_T* p_blow,ADE_FLOATING_T pow_thresh_low_i);
//static ADE_API_RET_T ADE_Blow_Set_Fs(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i);
static ADE_API_RET_T ADE_Blow_Set_RunPowWinFast(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T running_pow_win_time_fast_i);
static ADE_API_RET_T ADE_Blow_Set_RunPowWinSlow(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T running_pow_win_time_slow_i);
static ADE_API_RET_T ADE_Blow_Set_NPowThreshAttack(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T time_pow_thresh_attack_i);
static ADE_API_RET_T ADE_Blow_Set_NPowThreshRelease(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T time_pow_thresh_release_i);
//static ADE_API_RET_T ADE_square_vector(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Set_Expander_static_params(ADE_BLOW_T* p_blow,ADE_UINT32_T n_breaks_i,ADE_UINT32_T poly_order_i);
static ADE_API_RET_T ADE_Blow_Expander_Config(ADE_BLOW_T* p_blow);

static ADE_API_RET_T ADE_Blow_doSaturation_detector_sm(BLOW_SM_STATES_T state_int,BLOW_SM_STATES_T *p_state,ADE_UINT32_T eval_counter_int,
        ADE_UINT32_T eval_pow_int,ADE_UINT32_T eval_timer_int,ADE_FLOATING_T abs_sample_i,ADE_FLOATING_T pow_thresh_high,ADE_FLOATING_T pow_thresh_low,
        ADE_FLOATING_T sat_thresh,ADE_UINT32_T n_sat_thres,ADE_UINT32_T n_pow_thres_attack,ADE_UINT32_T n_pow_thres_release,ADE_UINT32_T eval_time_init,
        ADE_UINT32_T *p_eval_pow,ADE_UINT32_T *p_eval_counter,ADE_INT32_T *p_eval_timer,
        ADE_FLOATING_T running_pow_fast_i,ADE_FLOATING_T running_pow_slow_i);


ADE_API_RET_T ADE_Blow_Init(ADE_BLOW_T** dp_this,ADE_UINT32_T buff_len,ADE_FLOATING_T Fs_i,ADE_FLOATING_T Fs_o_i)
{
    ADE_BLOW_T* pthis = NULL;
    ADE_UINT32_T n_sos_sections_iir=IIR1_N_SECTIONS;//(ADE_UINT32_T)ceil((ADE_FLOATING_DP_T)iir_order/(ADE_FLOATING_DP_T)sos_order);
    ADE_UINT32_T n_sos_sections_iir2=IIR2_N_SECTIONS;//(ADE_UINT32_T)ceil((ADE_FLOATING_DP_T)iir2_order/(ADE_FLOATING_DP_T)sos_order);
    //ADE_UINT32_T poly_pieces=0;
#ifdef ADE_CONFIGURATION_INTERACTIVE
    char *p_matpath ="/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/matlab";
    char *p_scriptpath="/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/Main_scripts/blow_fine_control_swlike.m";
    char *p_matfilepath="./blow_config_ws.mat";
    ADE_API_RET_T mat_ret=0;
    // ADE_MATLAB_T* p_mat=NULL;
    Engine *p_mateng=NULL;
#endif
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;
    ADE_API_RET_T ret_Iir1=ADE_RET_ERROR;
    ADE_API_RET_T ret_Iir2=ADE_RET_ERROR;
    ADE_API_RET_T ret_b2=ADE_RET_ERROR;
    ADE_API_RET_T ret_poly=ADE_RET_ERROR;

    pthis=calloc(1,sizeof(ADE_BLOW_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis);

    //pthis->n_registered_funcs=n_functions;
    pthis->fir_order=0;
    // pthis->iir_order=iir_order;
    //pthis->iir2_order=iir2_order;
    pthis->Fs_i=Fs_i;
    pthis->buff_len_i=buff_len;
    pthis->Fs_o=Fs_o_i;
    pthis->buff_len_o=ceil(buff_len*Fs_o_i/Fs_i);
    pthis->pow_thresh_high=0.0;
    pthis->pow_thresh_low=0.0;
    pthis->sat_thresh=0.0;
    pthis->running_pow_win_fast=0.0;
    pthis->running_pow_win_slow=0.0;
    pthis->n_sat_thres=0;
    pthis->n_pow_thres_attack=0;
    pthis->n_pow_thres_release=0;
    pthis->eval_time_samples=0;
    pthis->state=ADE_FALSE;
    //pthis->Fs=Fs_i;
    (pthis->p_eval_counter)=calloc(1,sizeof(ADE_UINT32_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_eval_counter);

    (pthis->p_eval_pow)=calloc(1,sizeof(ADE_UINT32_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_eval_pow);

    (pthis->p_eval_timer)=calloc(1,sizeof(ADE_UINT32_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_eval_timer);

    (pthis->p_blow_state)=calloc(1,sizeof(BLOW_SM_STATES_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_blow_state);

    *(pthis->p_blow_state)=ADE_BLOW_WAITING;
    // pthis->n_breaks=n_breaks;
    //pthis->poly_order=poly_order;



    /******************* IIR ALLOC **************************/

    ret_Iir1 = ADE_Iir_Init(&(pthis->p_iir), n_sos_sections_iir, buff_len,ADE_IIR_TRASP_II_B);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Init,ret_Iir1);

    /******************* IIR2 ALLOC **************************/

    ret_Iir2 = ADE_Iir_Init(&(pthis->p_iir2), n_sos_sections_iir2, buff_len,ADE_IIR_TRASP_II_B);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Init,ret_Iir2);

    /****************** FUNCS ALLOC **************************/


//            pthis->p_blow_functions=calloc(n_functions,sizeof(ADE_BLOW_FUNCS_T));
////
//            if (pthis->p_blow_functions==NULL)
//            {
//                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_blow_functions,"%p",ADE_Blow_Init);
//                return ADE_E16;
//            }

    /********************* P_IN SQUARED ALLOC **********************************/

    pthis->p_in_squared=calloc(pthis->buff_len_i,sizeof(ADE_FLOATING_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_in_squared);

    /********************* POW FAST ALLOC **********************************/

    pthis->p_pow_fast=calloc(pthis->buff_len_i,sizeof(ADE_FLOATING_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_pow_fast);

    /********************* POW SLOW ALLOC **********************************/

    pthis->p_pow_slow=calloc(pthis->buff_len_i,sizeof(ADE_FLOATING_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_pow_slow);

    /********************* POW SLOW FILT ALLOC **********************************/

    pthis->p_pow_slow_filtered=calloc(pthis->buff_len_i,sizeof(ADE_FLOATING_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_pow_slow_filtered);

    /********************* STATE ALLOC **********************************/

//            pthis->p_blow_state=calloc(buff_len,sizeof(ADE_BLOW_FUNCS_T));
////
//            if (pthis->p_blow_state==NULL)
//            {
//                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_blow_state,"%p",ADE_Blow_Init);
//                return ADE_E16;
//            }

    /******** MATLAB ALLOC ********/

#ifdef ADE_CONFIGURATION_INTERACTIVE

    mat_ret = ADE_Matlab_Init(&(pthis->p_mat),p_mateng,p_scriptpath, p_matfilepath,p_matpath);

    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Init,mat_ret);

#endif

    /*********** BLAS L2 Alloc ************/

    ret_b2 = ADE_Blas_level2_Init(&(pthis->p_blas_l2),ADE_REAL);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Init,ret_b2);



    /*********  CONFIGURATION *********************/



    ADE_Blow_Static_Params(pthis);
    pthis->fir_order=pthis->running_pow_win_fast;

    /**************** FIR ALLOC (depends on static params) *****************************/

    ret_fir = ADE_Fir_Init(&(pthis->p_fir), pthis->running_pow_win_fast, buff_len,ADE_FIR_TRASP_II);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Init,ret_fir);

    /**************** POLYFIT ALLOC (depends on static params) *****************************/

    ret_poly=ADE_Polyfit_Init (&(pthis->p_poly),pthis->poly_order,pthis->n_breaks);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Init,ret_poly);

    ADE_Blow_Configuration(pthis);

    /******************* Output allocation *********************/
    pthis->p_out=calloc(pthis->buff_len_o,sizeof(ADE_FLOATING_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_out);


    *dp_this=pthis;

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Blow_Static_Params(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T nbit = 0;
    ADE_FLOATING_T Margin = 0;
    ADE_FLOATING_T eval_time = 0;
    ADE_FLOATING_T Fs = 0;
    ADE_FLOATING_T n_sat_thres = 0;
    ADE_FLOATING_T running_pow_win_time_fast =0;
    ADE_FLOATING_T running_pow_win_time_slow = 0;
    ADE_FLOATING_T time_pow_thresh_release = 0;
    ADE_FLOATING_T time_pow_thresh_attack = 0;
    ADE_FLOATING_T pow_thresh_low = 0;
    ADE_FLOATING_T pow_thresh_high = 0;
    ADE_UINT32_T n_breaks = 0;
    ADE_UINT32_T poly_order = 0;
    ADE_API_RET_T ret=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Static_Params,p_blow);

    Fs = p_blow->Fs_i;

#ifdef ADE_CONFIGURATION_INTERACTIVE
    nbit=ADE_Matlab_GetScalar(p_blow->p_mat,"nbit");
    Margin=ADE_Matlab_GetScalar(p_blow->p_mat,"Margin");
    eval_time=ADE_Matlab_GetScalar(p_blow->p_mat,"eval_time");
    n_sat_thres=ADE_Matlab_GetScalar(p_blow->p_mat,"n_sat_thres");
    running_pow_win_time_fast=ADE_Matlab_GetScalar(p_blow->p_mat,"running_pow_win_time_fast");
    running_pow_win_time_slow=ADE_Matlab_GetScalar(p_blow->p_mat,"running_pow_win_time_slow");
    time_pow_thresh_release=ADE_Matlab_GetScalar(p_blow->p_mat,"time_pow_thresh_release");
    time_pow_thresh_attack=ADE_Matlab_GetScalar(p_blow->p_mat,"time_pow_thresh_attack");
    pow_thresh_low=ADE_Matlab_GetScalar(p_blow->p_mat,"pow_thresh_low");
    pow_thresh_high=ADE_Matlab_GetScalar(p_blow->p_mat,"pow_thresh_high");
    n_breaks=ADE_Matlab_GetLength(p_blow->p_mat,"breaks");
    poly_order=ADE_Matlab_GetNCols(p_blow->p_mat,"coeffs")-1;


#else
    nbit=16;
    Margin = 0.3;
    eval_time = 100e-3;
    n_sat_thres = 20;
    running_pow_win_time_fast = 1.0e-3;
    running_pow_win_time_slow = 20.0e-3;
    time_pow_thresh_release = 2.0e-3;
    time_pow_thresh_attack = 1.0e-3;
    pow_thresh_low = 4e-3;
    pow_thresh_high = 0.25;
    n_breaks = POLY_N_BREAKS;
    poly_order = POLY_ORDER;
#endif

    ret=ADE_Blow_Set_SatThresh(p_blow,nbit,Margin);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Static_Params,ret);

    ret=ADE_Blow_Set_EvalTimesample(p_blow,Fs,eval_time);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Static_Params,ret);

    ret=ADE_Blow_Set_NSatThresh(p_blow,n_sat_thres);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Static_Params,ret);

    ret=ADE_Blow_Set_NPowThreshHigh(p_blow,pow_thresh_high);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Static_Params,ret);

    ret=ADE_Blow_Set_NPowThreshLow(p_blow,pow_thresh_low);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Static_Params,ret);

    ret=ADE_Blow_Set_RunPowWinFast(p_blow,Fs,running_pow_win_time_fast);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Static_Params,ret);

    ret=ADE_Blow_Set_RunPowWinSlow(p_blow,Fs,running_pow_win_time_slow);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Static_Params,ret);

    ret=ADE_Blow_Set_NPowThreshAttack(p_blow,Fs,time_pow_thresh_attack);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Static_Params,ret);

    ret=ADE_Blow_Set_NPowThreshRelease(p_blow,Fs,time_pow_thresh_release);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Static_Params,ret);

    ret=ADE_Blow_Set_Expander_static_params(p_blow,n_breaks,poly_order);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Static_Params,ret);

     return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Blow_Configuration(ADE_BLOW_T* p_blow)
{
    ADE_API_RET_T ret=ADE_RET_ERROR;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Configuration,p_blow);

    ret=ADE_Blow_Fir_Config(p_blow);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configuration,ret);

    ret=ADE_Blow_Iir_Config(p_blow);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configuration,ret);

    ret=ADE_Blow_Iir2_Config(p_blow);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configuration,ret);

    ret=ADE_Blow_Expander_Config(p_blow);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configuration,ret);

    return ADE_RET_SUCCESS;

}

ADE_VOID_T ADE_Blow_Release(ADE_BLOW_T* p_blow)
{

    ADE_Iir_Release(p_blow->p_iir);
    ADE_Iir_Release(p_blow->p_iir2);
    ADE_Fir_Release(p_blow->p_fir);
    ADE_Polyfit_Release(p_blow->p_poly);
    ADE_Blas_level2_Release(p_blow->p_blas_l2);
    ADE_CHECKNFREE(p_blow->p_in_squared);
    ADE_CHECKNFREE(p_blow->p_pow_fast);
    ADE_CHECKNFREE(p_blow->p_pow_slow);
    ADE_CHECKNFREE(p_blow->p_pow_slow_filtered);
    // ADE_CHECKNFREE(p_blow->p_blow_functions);
    // ADE_CHECKNFREE(p_blow->poly_coeffs);
    //ADE_CHECKNFREE(p_blow->breaks);

    ADE_CHECKNFREE(p_blow->p_eval_counter);
    ADE_CHECKNFREE(p_blow->p_eval_pow);
    ADE_CHECKNFREE(p_blow->p_eval_timer);
    ADE_CHECKNFREE(p_blow->p_blow_state);
    ADE_CHECKNFREE(p_blow->p_out);
#ifdef ADE_CONFIGURATION_INTERACTIVE
    ADE_Matlab_Release(p_blow->p_mat);
#endif
    ADE_CHECKNFREE(p_blow);


}

static ADE_API_RET_T ADE_Blow_Set_SatThresh(ADE_BLOW_T* p_blow,ADE_FLOATING_T nbit_i,ADE_FLOATING_T Margin_i)//,ADE_FLOATING_T Fs_i,ADE_FLOATING_T eval_time_i,ADE_FLOATING_T running_pow_win_time_fast_i,ADE_FLOATING_T running_pow_win_time_slow_i,ADE_FLOATING_T time_pow_thresh_attack_i,ADE_FLOATING_T time_pow_thresh_release_i,ADE_UINT32_T n_sat_thres)
{
    ADE_FLOATING_T max_amp =0.0;

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_SatThresh,p_blow);

    max_amp = 1-pow(2.0,-(nbit_i-1));
    p_blow->sat_thresh = (1-Margin_i)*max_amp;

    return ADE_RET_SUCCESS;
}
static ADE_API_RET_T ADE_Blow_Set_EvalTimesample(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T eval_time_i)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_EvalTimesample,p_blow);

    p_blow->eval_time_samples = floor(eval_time_i*Fs_i+0.5)+1;

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Set_Expander_static_params(ADE_BLOW_T* p_blow,ADE_UINT32_T n_breaks_i,ADE_UINT32_T poly_order_i)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_Expander_static_params,p_blow);

    p_blow->poly_order = poly_order_i;
    p_blow->n_breaks=n_breaks_i;

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Blow_Set_NSatThresh(ADE_BLOW_T* p_blow,ADE_FLOATING_T n_sat_thres_i)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_NSatThresh,p_blow);

    p_blow->n_sat_thres=n_sat_thres_i;
    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Set_NPowThreshHigh(ADE_BLOW_T* p_blow,ADE_FLOATING_T pow_thresh_high_i)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshHigh,p_blow);

    p_blow->pow_thresh_high=pow_thresh_high_i;
    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Set_NPowThreshLow(ADE_BLOW_T* p_blow,ADE_FLOATING_T pow_thresh_low_i)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshLow,p_blow);

    p_blow->pow_thresh_low=pow_thresh_low_i;
    return ADE_RET_SUCCESS;
}

//static ADE_API_RET_T ADE_Blow_Set_Fs(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i)
//{
//    p_blow->Fs=Fs_i;
//    return ADE_RET_SUCCESS;
//}

static ADE_API_RET_T ADE_Blow_Set_RunPowWinFast(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T running_pow_win_time_fast_i)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_RunPowWinFast,p_blow);

    p_blow->running_pow_win_fast = floor(running_pow_win_time_fast_i*Fs_i+0.5)+1;
    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Set_RunPowWinSlow(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T running_pow_win_time_slow_i)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_RunPowWinSlow,p_blow);

    p_blow->running_pow_win_slow = floor(running_pow_win_time_slow_i*Fs_i+0.5)+1;
    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Set_NPowThreshAttack(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T time_pow_thresh_attack_i)
{
    ADE_FLOATING_T n_pow_thres_sample_is_attack=0.0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshAttack,p_blow);

    n_pow_thres_sample_is_attack=floor(time_pow_thresh_attack_i*Fs_i+0.5)+1;
    p_blow->n_pow_thres_attack=floor(n_pow_thres_sample_is_attack+0.5);

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Set_NPowThreshRelease(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T time_pow_thresh_release_i)
{
    ADE_FLOATING_T n_pow_thres_sample_is_release=0.0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshRelease,p_blow);

    n_pow_thres_sample_is_release=floor(time_pow_thresh_release_i*Fs_i+0.5)+1;
    p_blow->n_pow_thres_release=floor(n_pow_thres_sample_is_release+0.5);

    return ADE_RET_SUCCESS;
}


static ADE_API_RET_T ADE_Blow_Saturation_detector_sm(ADE_BLOW_T* p_blow,ADE_UINT32_T sample_idx)
{


    BLOW_SM_STATES_T state_int = *(p_blow->p_blow_state);
    BLOW_SM_STATES_T *p_state=p_blow->p_blow_state;

    ADE_UINT32_T eval_counter_int=0;
    ADE_UINT32_T eval_pow_int=0;
    ADE_UINT32_T eval_timer_int=0;
    ADE_FLOATING_T sample_i=0;
    ADE_FLOATING_T abs_sample_i = 0;
    ADE_FLOATING_T pow_thresh_high=0;
    ADE_FLOATING_T pow_thresh_low=0;
    ADE_FLOATING_T sat_thresh=0;
    ADE_UINT32_T n_sat_thres=0;
    ADE_UINT32_T n_pow_thres_attack=0;
    ADE_UINT32_T n_pow_thres_release=0;
    ADE_UINT32_T eval_time_init=0;
    ADE_UINT32_T *p_eval_pow=NULL;
    ADE_UINT32_T *p_eval_counter=NULL;
    ADE_INT32_T *p_eval_timer=NULL;
    ADE_FLOATING_T running_pow_fast_i=0;
    ADE_FLOATING_T running_pow_slow_i=0;
    ADE_API_RET_T ret=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Saturation_detector_sm,p_blow);

    eval_counter_int=*(p_blow->p_eval_counter);
    eval_pow_int=*(p_blow->p_eval_pow);
    eval_timer_int=*(p_blow->p_eval_timer);
    sample_i=(p_blow->p_in)[sample_idx];
    abs_sample_i = fabs(sample_i);
    pow_thresh_high=p_blow->pow_thresh_high;
    pow_thresh_low=p_blow->pow_thresh_low;
    sat_thresh=p_blow->sat_thresh;
    n_sat_thres=p_blow->n_sat_thres;
    n_pow_thres_attack=p_blow->n_pow_thres_attack;
    n_pow_thres_release=p_blow->n_pow_thres_release;
    eval_time_init=p_blow->eval_time_samples;
    p_eval_pow=p_blow->p_eval_pow;
    p_eval_counter=p_blow->p_eval_counter;
    p_eval_timer=p_blow->p_eval_timer;
    running_pow_fast_i=((p_blow->p_fir)->p_out)[sample_idx];
    running_pow_slow_i=((p_blow->p_iir)->p_out)[sample_idx];

    ret=ADE_Blow_doSaturation_detector_sm( state_int, p_state, eval_counter_int,eval_pow_int, eval_timer_int, abs_sample_i, pow_thresh_high, pow_thresh_low,
                                           sat_thresh, n_sat_thres, n_pow_thres_attack,n_pow_thres_release,eval_time_init,p_eval_pow,p_eval_counter,p_eval_timer, running_pow_fast_i, running_pow_slow_i);


    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Saturation_detector_sm,ret);

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Blow_doSaturation_detector_sm(BLOW_SM_STATES_T state_int,BLOW_SM_STATES_T *p_state,ADE_UINT32_T eval_counter_int,
        ADE_UINT32_T eval_pow_int,ADE_UINT32_T eval_timer_int,ADE_FLOATING_T abs_sample_i,ADE_FLOATING_T pow_thresh_high,ADE_FLOATING_T pow_thresh_low,
        ADE_FLOATING_T sat_thresh,ADE_UINT32_T n_sat_thres,ADE_UINT32_T n_pow_thres_attack,ADE_UINT32_T n_pow_thres_release,ADE_UINT32_T eval_time_init,
        ADE_UINT32_T *p_eval_pow,ADE_UINT32_T *p_eval_counter,ADE_INT32_T *p_eval_timer,
        ADE_FLOATING_T running_pow_fast_i,ADE_FLOATING_T running_pow_slow_i)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_doSaturation_detector_sm,p_state);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_doSaturation_detector_sm,p_eval_pow);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_doSaturation_detector_sm,p_eval_counter);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_doSaturation_detector_sm,p_eval_timer);

    if (state_int==ADE_BLOW_TRACKING)
    {

        if (running_pow_slow_i<=pow_thresh_low)
        {
            *p_eval_pow=eval_pow_int+1;
        }
        else
        {
            *p_eval_pow=eval_pow_int;
        }

        if (*p_eval_pow>n_pow_thres_release)
        {
            *p_state=ADE_BLOW_WAITING;
        }
        else
        {
            *p_state=state_int;
        }

        *p_eval_counter=0;
        *p_eval_timer=0;

    }
    else if (state_int==ADE_BLOW_EVALUATION)
    {
        *p_eval_timer=eval_timer_int-1;

        if (abs_sample_i>sat_thresh)
        {
            *p_eval_counter=eval_counter_int+1;
        }
        else
        {
            *p_eval_counter=eval_counter_int;
        }
        if (running_pow_fast_i>=pow_thresh_high)
        {
            *p_eval_pow=eval_pow_int+1;
        }
        else
        {
            *p_eval_pow=eval_pow_int;
        }


        if (*p_eval_counter>n_sat_thres && *p_eval_timer>=0 && *p_eval_pow>=n_pow_thres_attack)
        {
            *p_state=ADE_BLOW_TRACKING;
            *p_eval_pow=0;
        }
        else if (*p_eval_timer==0)
        {
            *p_state=ADE_BLOW_WAITING;
        }
        else
        {
            *p_state=ADE_BLOW_EVALUATION;
        }

    }

    else if (state_int==ADE_BLOW_WAITING)


        if (abs_sample_i>sat_thresh)
        {
            *p_eval_timer=eval_time_init;
            *p_eval_counter=0;
            *p_eval_pow= 0;

            *p_state=ADE_BLOW_EVALUATION;
        }
        else
        {
            *p_eval_timer=eval_timer_int;
            *p_eval_counter= eval_counter_int;
            *p_eval_pow=0;
            *p_state=ADE_BLOW_WAITING;

        }

    else
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLOW,ADE_METHOD_doSaturation_detector_sm,state_int,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;

    }

    return ADE_RET_SUCCESS;

}




static ADE_API_RET_T ADE_Blow_Fir_Config(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T moving_average_coeff = 0.0;
    ADE_UINT32_T i = 0;
    ADE_UINT32_T order = 0;
    ADE_API_RET_T ret_buff=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Fir_Config,p_blow);

    order = (p_blow->p_fir)->filter_order;

    if ((p_blow->running_pow_win_fast)==0.0)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLOW,ADE_METHOD_Fir_Config,p_blow->running_pow_win_fast,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    moving_average_coeff = (ADE_FLOATING_T)(1)/(p_blow->running_pow_win_fast);

    /**FILL THE TAPS**/

    for(i=0; i<order; i++)
    {
        ((p_blow->p_fir)->p_num)[i]=moving_average_coeff;
    }

    /** POINT INOUT BUFFS ***/

    ret_buff=ADE_Fir_SetInBuff(p_blow->p_fir,p_blow->p_in_squared);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Fir_Config,ret_buff);
    ret_buff=ADE_Fir_SetOutBuff(p_blow->p_fir,p_blow->p_pow_fast);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Fir_Config,ret_buff);

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Iir_Config(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T iir_gains[IIR1_N_SECTIONS];
    ADE_FLOATING_T bbslow[IIR1_N_SECTIONS],bbslow2[IIR1_N_SECTIONS],bbslow3[IIR1_N_SECTIONS];
    ADE_FLOATING_T aaslow[IIR1_N_SECTIONS],aaslow2[IIR1_N_SECTIONS],aaslow3[IIR1_N_SECTIONS];
    ADE_FLOATING_T *nums[IIR1_N_SECTIONS]= {bbslow,bbslow2,bbslow3};
    ADE_FLOATING_T *denoms[IIR1_N_SECTIONS]= {aaslow,aaslow2,aaslow3};
    ADE_API_RET_T ret_set=ADE_RET_ERROR;
    ADE_API_RET_T ret_conf=ADE_RET_ERROR;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Iir_Config,p_blow);


#ifdef ADE_CONFIGURATION_INTERACTIVE
    ret_conf=ADE_Matlab_Configure_Iir_sos(p_blow->p_mat,p_blow->p_iir, "iir_sosmatrix", "iir_scaleval");
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir_Config,ret_conf);

#else
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


    ret_set=ADE_Iir_setGains( p_blow->p_iir, iir_gains);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir_Config,ret_set);

    ret_set=ADE_Iir_setNums( p_blow->p_iir, &(*nums));
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir_Config,ret_set);

    ret_set=ADE_Iir_setDenoms( p_blow->p_iir, &(*denoms));
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir_Config,ret_set);

#endif

    ret_set=ADE_Iir_SetInBuff(p_blow->p_iir,p_blow->p_in_squared);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir_Config,ret_set);

    ret_set=ADE_Iir_SetOutBuff(p_blow->p_iir,p_blow->p_pow_slow);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir_Config,ret_set);



    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Iir2_Config(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T iir_gains[IIR2_N_SECTIONS];
    ADE_FLOATING_T bbslow[IIR2_N_SECTIONS],bbslow2[IIR2_N_SECTIONS],bbslow3[IIR2_N_SECTIONS];
    ADE_FLOATING_T aaslow[IIR2_N_SECTIONS],aaslow2[IIR2_N_SECTIONS],aaslow3[IIR2_N_SECTIONS];
    ADE_FLOATING_T *nums[IIR2_N_SECTIONS]= {bbslow,bbslow2,bbslow3};
    ADE_FLOATING_T *denoms[IIR2_N_SECTIONS]= {aaslow,aaslow2,aaslow3};
    ADE_API_RET_T ret_conf=ADE_RET_ERROR;
    ADE_API_RET_T ret_set=ADE_RET_ERROR;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Iir2_Config,p_blow);

#ifdef ADE_CONFIGURATION_INTERACTIVE
    ret_conf=ADE_Matlab_Configure_Iir_sos(p_blow->p_mat,p_blow->p_iir2, "iir2_sosmatrix", "iir2_scaleval");
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir2_Config,ret_conf);
#else

    iir_gains[0]=2.73316597838323e-08;
    iir_gains[1]=2.73276093372558e-08;
    iir_gains[2]=2.73252713574353e-08;
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
    aaslow[1]=-1.999828743143229;
    aaslow[2]=  0.999828852469868;
    aaslow2[0]=1.0;
    aaslow2[1]=-1.999532376235760;
    aaslow2[2]=0.999532485546198;
    aaslow3[0]=1.0;
    aaslow3[1]= -1.999361308716150;
    aaslow3[2]= 0.999361418017236;


    ret_set=ADE_Iir_setGains( p_blow->p_iir2, iir_gains);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir2_Config,ret_set);

    ret_set=ADE_Iir_setNums( p_blow->p_iir2, &(*nums));
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir2_Config,ret_set);

    ret_set=ADE_Iir_setDenoms( p_blow->p_iir2, &(*denoms));
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir2_Config,ret_set);


#endif
    ret_set=ADE_Iir_SetInBuff(p_blow->p_iir2,p_blow->p_pow_slow);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir2_Config,ret_set);

    ret_set=ADE_Iir_SetOutBuff(p_blow->p_iir2,p_blow->p_pow_slow_filtered);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir2_Config,ret_set);

    return ADE_RET_SUCCESS;
}


static ADE_API_RET_T ADE_Blow_Expander_Config(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T breaks[POLY_N_BREAKS]= {0,0.05,0.1,0.3,0.5,1};
    ADE_FLOATING_T coeffs[(POLY_N_BREAKS-1)*(POLY_ORDER+1)]= {0.000000000000089,-0.000000000000009,1.000000000000000,0,
                                                              0,0,1.000000000000000,0.050000000000000,
                                                              8.333333333333334,-1.666666666666667,1.000000000000000,0.100000000000000,
                                                              -21.505376344086020,7.634408602150536,1.333333333333333,0.300000000000000
                                                              -0.317050691244241,-0.254377880184331,1.806451612903226,0.700000000000000
                                                             };
    ADE_API_RET_T ret_set=ADE_RET_ERROR;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Expander_Config,p_blow);

#ifdef ADE_CONFIGURATION_INTERACTIVE
    ret_set=ADE_Polyfit_SetBreaks(p_blow->p_poly,ADE_Matlab_GetDataPointer(p_blow->p_mat,"breaks"));
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Expander_Config,ret_set) ;

    ret_set=ADE_Polyfit_SetCoeffs(p_blow->p_poly,ADE_Matlab_GetDataPointer(p_blow->p_mat,"coeffs"));
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Expander_Config,ret_set) ;
#else
    if (p_blow->poly_order!=POLY_ORDER)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLOW,ADE_METHOD_Expander_Config,p_blow->poly_order,"%u",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    if (p_blow->n_breaks!=POLY_N_BREAKS)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLOW,ADE_METHOD_Expander_Config,p_blow->n_breaks,"%u",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    ret_set=ADE_Polyfit_SetBreaks(p_blow->p_poly,breaks);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Expander_Config,ret_set) ;

    ret_set=ADE_Polyfit_SetCoeffs(p_blow->p_poly,coeffs);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Expander_Config,ret_set) ;

#endif

    return ADE_RET_SUCCESS;

}



//static ADE_API_RET_T ADE_Blow_square_vector(ADE_BLOW_T* p_blow)
//{
//    ADE_UINT32_T i=0;
//    ADE_API_RET_T ret = ADE_RET_SUCCESS;
//





//    return ADE_RET_SUCCESS;
//}

//*y=coeffs[coeffs_row_idx+3]+coeffs[coeffs_row_idx+2]*(data-x_breaks[zz])+coeffs[coeffs_row_idx+1]*(data-x_breaks[zz])^2+coeffs[coeffs_row_idx+0]*(data-x_breaks[zz])^3;
ADE_API_RET_T ADE_Blow_Step(ADE_BLOW_T* p_blow)
{

    ADE_UINT32_T i=0;
    ADE_IIR_T *p_iir2_int=NULL;
    ADE_API_RET_T fir_step_ret = ADE_RET_ERROR;
    ADE_API_RET_T iir_step_ret = ADE_RET_ERROR;
    ADE_API_RET_T iir_step_ret2 = ADE_RET_ERROR;
    ADE_API_RET_T ret = ADE_RET_ERROR,ret_conf=ADE_RET_ERROR,ret_sat=ADE_RET_ERROR,ret_poly=ADE_RET_ERROR;
    //static ADE_UINT8_T flag;
    ADE_UINT8_T state_flag=0;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Step,p_blow);

    ret_conf = ADE_Blas_level2_Elewise_Config(p_blow->p_blas_l2, p_blow->p_in,p_blow->p_in,p_blow->p_in_squared,1.0,0.0,p_blow->buff_len_i);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,ret_conf);

    ret = ADE_Blas_level2_Elewise(p_blow->p_blas_l2);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,ret);

    fir_step_ret=ADE_Fir_Step(p_blow->p_fir);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,fir_step_ret);

    iir_step_ret = ADE_Iir_Step(p_blow->p_iir);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,iir_step_ret);

    state_flag=0;

    for (i=0; i<p_blow->buff_len_i; i++)
    {
        ret_sat=ADE_Blow_Saturation_detector_sm(p_blow,i);
        ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,ret_sat);

        if (state_flag==0 && *(p_blow->p_blow_state)==ADE_BLOW_TRACKING)
        {
            p_blow->state=ADE_TRUE;
            state_flag=1;
        }
    }

    p_iir2_int=p_blow->p_iir2;

    iir_step_ret2=ADE_Iir_Step(p_iir2_int);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,iir_step_ret2);

    ret_poly=ADE_Polyfit_Step(p_blow->p_poly,p_iir2_int->p_out,p_blow->p_out,p_blow->buff_len_i);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,ret_poly);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blow_SetInBuff(ADE_BLOW_T* p_blow, ADE_FLOATING_T *p_buff)
{

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_SetInBuff,p_blow);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_SetInBuff,p_buff);

    p_blow->p_in = p_buff;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blow_GetOutBuff(ADE_BLOW_T* p_blow, ADE_FLOATING_T **dp_buff)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_GetOutBuff,p_blow);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_GetOutBuff,dp_buff);

    *dp_buff=p_blow->p_out;

    return ADE_RET_SUCCESS;

}

