#include "headers/ADE_Blow.h"
#include "headers/ADE_fir.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_blas_level2.h"
#include "headers/ADE_polyfit.h"
#include "headers/ADE_downsampler.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include "headers/ADE_defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "headers/ADE_Error_Handler.h"
#include <string.h>
#ifdef ADE_MEX_PRINT
#include "mex.h"
#endif


//#include <string.h>
/****************** Configuration wrappers *********************/
static ADE_API_RET_T ADE_Blow_Fir_Configure_params(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Fir_Configure_inout(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Fir_Configure_bufflength(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Iir_Configure_params(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Iir_Configure_inout(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Iir2_Configure_params(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Iir2_Configure_inout(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Expander_Configure_params(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Expander_Configure_inout(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Downsampler_Configure(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Configuration(ADE_BLOW_T* p_blow);
/*************** Blow Set Methods ********************************/
static ADE_API_RET_T ADE_Blow_SetInBuff(ADE_BLOW_T* p_blow, ADE_FLOATING_T *p_buff);
static ADE_API_RET_T ADE_Blow_SetInBuffLength (ADE_BLOW_T* p_blow,ADE_INT32_T buff_len);
static ADE_API_RET_T ADE_Blow_SetOutBuffLength (ADE_BLOW_T* p_blow,ADE_INT32_T buff_len);
static ADE_API_RET_T ADE_Blow_SetFsIn (ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i);
static ADE_API_RET_T ADE_Blow_SetFsOut (ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_o);
static ADE_API_RET_T ADE_Blow_SetFirOrder (ADE_BLOW_T* p_blow,ADE_INT32_T order);
static ADE_API_RET_T ADE_Blow_Set_SatThresh(ADE_BLOW_T* p_blow,ADE_FLOATING_T nbit_i,ADE_FLOATING_T Margin_i);
static ADE_API_RET_T ADE_Blow_Set_EvalTimesample(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T eval_time_i);
static ADE_API_RET_T ADE_Blow_Set_NSatThresh(ADE_BLOW_T* p_blow,ADE_FLOATING_T n_sat_thres_i);
static ADE_API_RET_T ADE_Blow_Set_NPowThreshHigh(ADE_BLOW_T* p_blow,ADE_FLOATING_T pow_thresh_high_i);
static ADE_API_RET_T ADE_Blow_Set_NPowThreshLow(ADE_BLOW_T* p_blow,ADE_FLOATING_T pow_thresh_low_i);
static ADE_API_RET_T ADE_Blow_Set_RunPowWinFast(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T running_pow_win_time_fast_i);
static ADE_API_RET_T ADE_Blow_Set_RunPowWinSlow(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T running_pow_win_time_slow_i);
static ADE_API_RET_T ADE_Blow_Set_NPowThreshAttack(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T time_pow_thresh_attack_i);
static ADE_API_RET_T ADE_Blow_Set_NPowThreshRelease(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T time_pow_thresh_release_i);
static ADE_API_RET_T ADE_Blow_Set_Expander_static_params(ADE_BLOW_T* p_blow,ADE_INT32_T n_breaks_i,ADE_INT32_T poly_order_i);
/*********************************************************************/
static ADE_API_RET_T ADE_Blow_Static_Params(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Saturation_detector_sm(ADE_BLOW_T* p_blow,ADE_UINT32_T sample_idx);
static ADE_API_RET_T ADE_Blow_doSaturation_detector_sm(BLOW_SM_STATES_T state_int,BLOW_SM_STATES_T *p_state,ADE_UINT32_T eval_counter_int,
                                                        ADE_UINT32_T eval_pow_int,ADE_UINT32_T eval_timer_int,ADE_FLOATING_T abs_sample_i,ADE_FLOATING_T pow_thresh_high,ADE_FLOATING_T pow_thresh_low,
                                                        ADE_FLOATING_T sat_thresh,ADE_UINT32_T n_sat_thres,ADE_UINT32_T n_pow_thres_attack,ADE_UINT32_T n_pow_thres_release,ADE_UINT32_T eval_time_init,
                                                        ADE_UINT32_T *p_eval_pow,ADE_UINT32_T *p_eval_counter,ADE_INT32_T *p_eval_timer,
                                                        ADE_FLOATING_T running_pow_fast_i,ADE_FLOATING_T running_pow_slow_i);

/******************* Init Methods ************************/

ADE_API_RET_T ADE_Blow_Init(ADE_BLOW_T** dp_this)//,ADE_UINT32_T buff_len,ADE_FLOATING_T Fs_i,ADE_FLOATING_T Fs_o_i)
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
    ADE_API_RET_T ret_down=ADE_RET_ERROR;
    ADE_INT32_T max_internal_buff_len = ADE_GENERAL_MALLOC_BUFF_LEN;
    //ADE_INT32_T downsamp_fact=Fs_i/Fs_o_i;

    pthis=calloc(1,sizeof(ADE_BLOW_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis);

    //pthis->n_registered_funcs=n_functions;
    pthis->fir_order=0;
    // pthis->iir_order=iir_order;
    //pthis->iir2_order=iir2_order;
    pthis->Fs_i=0;//Fs_i;
    pthis->buff_len_i=0;
    pthis->max_internal_buff_len=max_internal_buff_len;
    pthis->Fs_o=0;//Fs_o_i;
    pthis->buff_len_o=0;//ceil(buff_len/downsamp_fact);
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

    ret_Iir1 = ADE_Iir_Init(&(pthis->p_iir));//, n_sos_sections_iir, buff_len,ADE_IIR_TRASP_II_B);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Init,ret_Iir1);

    /******************* IIR2 ALLOC **************************/

    ret_Iir2 = ADE_Iir_Init(&(pthis->p_iir2));//, n_sos_sections_iir2, pthis->buff_len_o,ADE_IIR_TRASP_II_B);
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

    pthis->p_in_squared=calloc(pthis->max_internal_buff_len,sizeof(ADE_FLOATING_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_in_squared);

    /********************* POW FAST ALLOC **********************************/

    pthis->p_pow_fast=calloc(pthis->max_internal_buff_len,sizeof(ADE_FLOATING_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_pow_fast);

    /********************* POW SLOW ALLOC **********************************/

    pthis->p_pow_slow=calloc(pthis->max_internal_buff_len,sizeof(ADE_FLOATING_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_pow_slow);

    /********************* POW SLOW DOWNSAMPLED ALLOC **********************************/

    pthis->p_pow_slow_downsampled=calloc(pthis->max_internal_buff_len,sizeof(ADE_FLOATING_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_pow_slow_downsampled);

    /********************* POW SLOW FILT ALLOC **********************************/

    pthis->p_pow_slow_filtered=calloc(pthis->max_internal_buff_len,sizeof(ADE_FLOATING_T));
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

    ret_b2 = ADE_Blas_level2_Init(&(pthis->p_blas_l2),ADE_MATH_REAL);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Init,ret_b2);

      /**************** FIR ALLOC (no more depends on static params) *****************************/

    ret_fir = ADE_Fir_Init(&(pthis->p_fir));//, pthis->running_pow_win_fast, buff_len,ADE_FIR_TRASP_II);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Init,ret_fir);

        /**************** POLYFIT ALLOC (depends on static params) *****************************/

    ret_poly=ADE_Polyfit_Init (&(pthis->p_poly));//,pthis->poly_order,pthis->n_breaks);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Init,ret_poly);

    /************** DOWNSAMPLER ALLOC ***********************/

    ret_down=ADE_Downsampler_Init(&(pthis->p_downsampler));//,pthis->buff_len_i,downsamp_fact);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Init,ret_down);

    /*********  CONFIGURATION *********************/

    //ADE_Blow_Static_Params(pthis);
   // pthis->fir_order=pthis->running_pow_win_fast-1;

    //ADE_Blow_Configuration(pthis);

    /******************* Output allocation *********************/
    pthis->p_out=calloc(pthis->max_internal_buff_len,sizeof(ADE_FLOATING_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_BLOW,ADE_METHOD_Init,pthis->p_out);


    *dp_this=pthis;

    return ADE_RET_SUCCESS;

}

ADE_VOID_T ADE_Blow_Release(ADE_BLOW_T* p_blow)
{

    ADE_Iir_Release(p_blow->p_iir);
    ADE_Iir_Release(p_blow->p_iir2);
    ADE_Fir_Release(p_blow->p_fir);
    ADE_Polyfit_Release(p_blow->p_poly);
    ADE_Downsampler_Release(p_blow->p_downsampler);
    ADE_Blas_level2_Release(p_blow->p_blas_l2);
    ADE_CHECKNFREE(p_blow->p_in_squared);
    ADE_CHECKNFREE(p_blow->p_pow_fast);
    ADE_CHECKNFREE(p_blow->p_pow_slow);
    ADE_CHECKNFREE(p_blow->p_pow_slow_downsampled);
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

/******************* Configure Methods ************************/

ADE_API_RET_T ADE_Blow_Configure_bufflength(ADE_BLOW_T* p_blow,ADE_INT32_T in_buff_len,ADE_INT32_T out_buff_len)
{
    ADE_API_RET_T ret_inbufflen=ADE_RET_ERROR;
    ADE_API_RET_T ret_outbufflen=ADE_RET_ERROR;
    ADE_API_RET_T ret_firbufflen=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir1bufflen=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir2bufflen=ADE_RET_ERROR;
    ADE_API_RET_T ret_poly=ADE_RET_ERROR;
    ADE_API_RET_T ret_ele=ADE_RET_ERROR;

    ret_inbufflen=ADE_Blow_SetInBuffLength ( p_blow, in_buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_bufflength,ret_inbufflen);

    ret_outbufflen=ADE_Blow_SetOutBuffLength (p_blow, out_buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_bufflength,ret_outbufflen);

    ret_firbufflen= ADE_Fir_Configure_bufflength(p_blow->p_fir,in_buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_bufflength,ret_outbufflen);

    ret_iir1bufflen= ADE_Iir_Configure_bufflength(p_blow->p_iir,in_buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_bufflength,ret_iir1bufflen);

    ret_iir2bufflen= ADE_Iir_Configure_bufflength(p_blow->p_iir2,in_buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_bufflength,ret_iir2bufflen);

    ret_poly = ADE_Polyfit_Configure_bufflength(p_blow->p_poly,in_buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_bufflength,ret_poly);

    ret_ele=ADE_Blas_level2_configure_elewise_bufflength(p_blow->p_blas_l2,in_buff_len);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_bufflength,ret_ele);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blow_Configure_params(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T Fs_o)
{

    /********** sets all internal parameters to the blow ******/
     ADE_API_RET_T ret_fir=ADE_RET_ERROR;
     ADE_API_RET_T ret_fir_len=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir2=ADE_RET_ERROR;
    ADE_API_RET_T ret_exp=ADE_RET_ERROR;
    ADE_API_RET_T ret_static_params=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir_params=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir_params=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir2_params=ADE_RET_ERROR;
    ADE_API_RET_T ret_exp_params=ADE_RET_ERROR;
    ADE_API_RET_T ret_ele=ADE_RET_ERROR;
    ADE_API_RET_T ret_down=ADE_RET_ERROR;

    ADE_API_RET_T ret_fsi=ADE_RET_ERROR;
    ADE_API_RET_T ret_fso=ADE_RET_ERROR;

    ret_fsi=ADE_Blow_SetFsIn (p_blow,Fs_i);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_fsi);

    ret_fso=ADE_Blow_SetFsOut ( p_blow, Fs_o);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_fso);


    /********* set all  static params of internal objects ****/

    ret_static_params=ADE_Blow_Static_Params(p_blow);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_static_params);

    ret_fir_params=ADE_Blow_Fir_Configure_params(p_blow);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_fir_params);

    ret_iir_params=ADE_Blow_Iir_Configure_params(p_blow);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_iir_params);

    ret_iir2_params=ADE_Blow_Iir2_Configure_params(p_blow);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_iir2_params);

    ret_exp_params=ADE_Blow_Expander_Configure_params(p_blow);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_exp_params);

    ret_ele=ADE_Blas_level2_configure_elewise_params(p_blow->p_blas_l2,1.0,0.0);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_ele);


     /********* set all  in out connection of internal objects ****/

     ret_fir=ADE_Blow_Fir_Configure_inout(p_blow);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_fir);

    ret_iir=ADE_Blow_Iir_Configure_inout(p_blow);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_iir);

     ret_down=ADE_Blow_Downsampler_Configure(p_blow);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_down);

    ret_iir2=ADE_Blow_Iir2_Configure_inout(p_blow);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_iir2);

    ret_exp=ADE_Blow_Expander_Configure_inout(p_blow);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_exp);

     /********* set all  buffer lengths of internal objects ****/

//    ret_fir_len=ADE_Blow_Fir_Configure_bufflength(p_blow);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_params,ret_fir_len);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blow_Configure_inout(ADE_BLOW_T* p_blow, ADE_FLOATING_T *p_inbuff)
{

    ADE_API_RET_T ret_in=ADE_RET_ERROR;
    ADE_API_RET_T ret_ele=ADE_RET_ERROR;


    ret_in=ADE_Blow_SetInBuff(p_blow, p_inbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_inout,ret_in);

     ret_ele= ADE_Blas_level2_configure_elewise_inout(p_blow->p_blas_l2,p_blow->p_in,p_blow->p_in ,p_blow->p_in_squared);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure_inout,ret_ele);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blow_Configure(ADE_BLOW_T* p_blow, ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T Fs_i,ADE_FLOATING_T Fs_o,ADE_INT32_T in_buff_len,ADE_INT32_T out_buff_len)
{

    ADE_API_RET_T ret_params=ADE_RET_ERROR;
    ADE_API_RET_T ret_inout=ADE_RET_ERROR;
    ADE_API_RET_T ret_bufflen=ADE_RET_ERROR;

    ret_bufflen=ADE_Blow_Configure_bufflength(p_blow,in_buff_len,out_buff_len);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure,ret_bufflen);

    ret_params=ADE_Blow_Configure_params(p_blow,Fs_i,Fs_o);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure,ret_params);

    ret_inout=ADE_Blow_Configure_inout(p_blow,p_inbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Configure,ret_inout);




     return ADE_RET_SUCCESS;

}

/********************** Processing Methods ****************************/

ADE_API_RET_T ADE_Blow_Step(ADE_BLOW_T* p_blow)
{

    ADE_UINT32_T i=0;
    ADE_IIR_T *p_iir2_int=NULL;
    ADE_API_RET_T fir_step_ret = ADE_RET_ERROR;
    ADE_API_RET_T iir_step_ret = ADE_RET_ERROR;
    ADE_API_RET_T iir_step_ret2 = ADE_RET_ERROR;
    ADE_API_RET_T down_ret = ADE_RET_ERROR;
    ADE_API_RET_T ret = ADE_RET_ERROR,ret_conf=ADE_RET_ERROR,ret_sat=ADE_RET_ERROR,ret_poly=ADE_RET_ERROR;
    //static ADE_UINT8_T flag;
    ADE_UINT8_T state_flag=0;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Step,p_blow);

//    ret_conf = ADE_Blas_level2_configure_elewise(p_blow->p_blas_l2, p_blow->p_in,p_blow->p_in,p_blow->p_in_squared,1.0,0.0,p_blow->buff_len_i);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,ret_conf);

    ret = ADE_Blas_level2_Elewise(p_blow->p_blas_l2);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,ret);

    fir_step_ret=ADE_Fir_Step(p_blow->p_fir);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,fir_step_ret);

    iir_step_ret = ADE_Iir_Step(p_blow->p_iir);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,iir_step_ret);

    state_flag=0;
    p_blow->state=ADE_FALSE;

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

    down_ret=ADE_Downsampler_Step(p_blow->p_downsampler);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,down_ret);

    p_iir2_int=p_blow->p_iir2;

    iir_step_ret2=ADE_Iir_Step(p_iir2_int);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,iir_step_ret2);

    ret_poly=ADE_Polyfit_Step(p_blow->p_poly);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Step,ret_poly);

    return ADE_RET_SUCCESS;
}


/****************************** Utils *******************************/

ADE_API_RET_T ADE_Blow_Print(ADE_BLOW_T* p_blow, ADE_FILE_T *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj)
{

     ADE_CHAR_T fixed_str[64];
    ADE_CHAR_T pri_str[128];
    ADE_SIZE_T len_str;
       ADE_CHAR_T temp_str[64];

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Print,p_blow);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Print,p_fid);

            memset(fixed_str,'\0',sizeof(fixed_str));
strcat(fixed_str,calling_obj);
strcat(fixed_str,"->");
strcat(fixed_str,obj_name);
strcat(fixed_str,"->");
len_str=strlen(fixed_str);
 memset(temp_str,'\0',sizeof(temp_str));

    if (p_fid!=NULL)
    {
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"Fs_i = %*.*f\n"),ADE_BLOW_PRINT_FLOAT_WIDTH,ADE_BLOW_PRINT_FLOAT_PRECISION,p_blow->Fs_i);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"buff_len_i = %u\n"),p_blow->buff_len_i);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"Fs_o = %*.*f\n"),ADE_BLOW_PRINT_FLOAT_WIDTH,ADE_BLOW_PRINT_FLOAT_PRECISION,p_blow->Fs_o);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"buff_len_o = %u\n"),p_blow->buff_len_o);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"fir_order = %u\n"),p_blow->fir_order);
        strncpy(temp_str,fixed_str,len_str-2);
        ADE_Fir_Print(p_blow->p_fir,p_fid,"p_fir",temp_str);
        ADE_Iir_Print(p_blow->p_iir,p_fid,"p_iir",temp_str);
        ADE_Downsampler_Print(p_blow->p_downsampler,p_fid,"p_downsampler",temp_str);
        ADE_Iir_Print(p_blow->p_iir2,p_fid,"p_iir2",temp_str);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_in = %p(%*.*f)\n"),p_blow->p_in,ADE_BLOW_PRINT_FLOAT_WIDTH,ADE_BLOW_PRINT_FLOAT_PRECISION,p_blow->p_in[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_in_squared = %p(%*.*f)\n"),p_blow->p_in_squared,ADE_BLOW_PRINT_FLOAT_WIDTH,ADE_BLOW_PRINT_FLOAT_PRECISION,p_blow->p_in_squared[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_pow_fast = %p(%*.*f)\n"),p_blow->p_pow_fast,ADE_BLOW_PRINT_FLOAT_WIDTH,ADE_BLOW_PRINT_FLOAT_PRECISION,p_blow->p_pow_fast[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_pow_slow = %p(%*.*f)\n"),p_blow->p_pow_slow,ADE_BLOW_PRINT_FLOAT_WIDTH,ADE_BLOW_PRINT_FLOAT_PRECISION,p_blow->p_pow_slow[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_pow_slow_downsampled = %p(%*.*f)\n"),p_blow->p_pow_slow,ADE_BLOW_PRINT_FLOAT_WIDTH,ADE_BLOW_PRINT_FLOAT_PRECISION,p_blow->p_pow_slow_downsampled[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_pow_slow_filtered = %p(%*.*f)\n"),p_blow->p_pow_slow_filtered,ADE_BLOW_PRINT_FLOAT_WIDTH,ADE_BLOW_PRINT_FLOAT_PRECISION,p_blow->p_pow_slow_filtered[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_out = %p(%*.*f)\n"),p_blow->p_out,ADE_BLOW_PRINT_FLOAT_WIDTH,ADE_BLOW_PRINT_FLOAT_PRECISION,p_blow->p_out[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"state = %u\n"),p_blow->state);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"pow_thresh_high = %*.*f\n"),ADE_BLOW_PRINT_FLOAT_WIDTH,ADE_BLOW_PRINT_FLOAT_PRECISION,p_blow->pow_thresh_high);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"pow_thresh_low = %*.*f\n"),ADE_BLOW_PRINT_FLOAT_WIDTH,ADE_BLOW_PRINT_FLOAT_PRECISION,p_blow->pow_thresh_low);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"sat_thresh = %*.*f\n"),ADE_BLOW_PRINT_FLOAT_WIDTH,ADE_BLOW_PRINT_FLOAT_PRECISION,p_blow->sat_thresh);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"running_pow_win_fast = %u\n"),p_blow->running_pow_win_fast);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"running_pow_win_slow = %u\n"),p_blow->running_pow_win_slow);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"n_sat_thres = %u\n"),p_blow->n_sat_thres);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"n_pow_thres_attack = %u\n"),p_blow->n_pow_thres_attack);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"eval_time_samples = %u\n"),p_blow->eval_time_samples);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_eval_counter = %p(%u)\n"),p_blow->p_eval_counter,p_blow->p_eval_counter[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_eval_pow = %p(%u)\n"),p_blow->p_eval_pow,p_blow->p_eval_pow[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_eval_timer = %p(%u)\n"),p_blow->p_eval_timer,p_blow->p_eval_timer[0]);
        ADE_Polyfit_Print(p_blow->p_poly,p_fid,"p_poly",temp_str);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_blow_state = %u\n"),p_blow->p_blow_state);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"n_breaks = %u\n"),p_blow->n_breaks);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"poly_order = %u\n"),p_blow->poly_order);
        strcpy(pri_str,fixed_str);
        ADE_Blas_level2_Print(p_blow->p_blas_l2,p_fid,"p_blas_l2",temp_str);

    }


 return ADE_RET_SUCCESS;
}

/****************************************************************************/
/**************** PRIVATE METHODS ******************************************/
/****************************************************************************/


/******************* Configure  Wrappers ************************/
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

   // Fs = p_blow->Fs_i;

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


      p_blow->fir_order=p_blow->running_pow_win_fast-1;

     return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Blow_Fir_Configure_params(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T moving_average_coeff = 0.0;
    ADE_UINT32_T i = 0;
    ADE_UINT32_T order = 0;
    ADE_API_RET_T ret_buff=ADE_RET_ERROR;
    ADE_FLOATING_T *p_temp_num=NULL;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Fir_Configure_params,p_blow);



    order = p_blow->fir_order;

     p_temp_num=calloc(order+1,sizeof(ADE_FLOATING_T));

    if ((p_blow->running_pow_win_fast)==0)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLOW,ADE_METHOD_Fir_Configure_params,p_blow->running_pow_win_fast,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    moving_average_coeff = (ADE_FLOATING_T)(1)/(p_blow->running_pow_win_fast);

    /**FILL THE TAPS**/

    for(i=0; i<(order+1); i++)
    {
        p_temp_num[i]=moving_average_coeff;
    }

    ADE_Fir_Configure_params(p_blow->p_fir,p_temp_num,(order+1),ADE_FIR_TRASP_II);


    ADE_CHECKNFREE(p_temp_num);

    return ADE_RET_SUCCESS;
}


static ADE_API_RET_T ADE_Blow_Fir_Configure_inout(ADE_BLOW_T* p_blow)
{
 ADE_API_RET_T ret_fir=ADE_RET_ERROR;

 ret_fir=ADE_Fir_Configure_inout(p_blow->p_fir,p_blow->p_in_squared,p_blow->p_pow_fast);
 ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Fir_Configure_inout,ret_fir);

 return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Fir_Configure_bufflength(ADE_BLOW_T* p_blow)
{
 ADE_API_RET_T ret_fir=ADE_RET_ERROR;

 ret_fir=ADE_Fir_Configure_bufflength(p_blow->p_fir,p_blow->buff_len_i);
 ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Fir_Configure_bufflength,ret_fir);

 return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Iir_Configure_params(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T iir_gains[IIR1_N_SECTIONS];
    ADE_FLOATING_T bbslow[3],bbslow2[3],bbslow3[3],bbslow4[3];
    ADE_FLOATING_T aaslow[3],aaslow2[3],aaslow3[3],aaslow4[3];
    ADE_FLOATING_T *nums[IIR1_N_SECTIONS]= {bbslow,bbslow2,bbslow3,bbslow4};
    ADE_FLOATING_T *denoms[IIR1_N_SECTIONS]= {aaslow,aaslow2,aaslow3,aaslow3};
    ADE_API_RET_T ret_set=ADE_RET_ERROR;
    ADE_API_RET_T ret_conf=ADE_RET_ERROR;
    ADE_IIR_IMP_CHOICE_T imp_type = ADE_IIR_TRASP_II_B;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Iir_Configure_params,p_blow);


#ifdef ADE_CONFIGURATION_INTERACTIVE
    ret_conf=ADE_Matlab_Configure_Iir_sos(p_blow->p_mat,p_blow->p_iir, "iir_sosmatrix", "iir_scaleval",p_blow->buff_len_i,imp_type);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir_Configure_params,ret_conf);

#else
    iir_gains[0]= 0.000009602156988;
    iir_gains[1]= 0.000009580765989;
    iir_gains[2]=0.000009564458305;
    iir_gains[3]=0.000009555655780;

    bbslow[0]=1.0;
    bbslow[1]=2.0;
    bbslow[2]=1.0;
    bbslow2[0]=1.0;
    bbslow2[1]=2.0;
    bbslow2[2]=1.0;
    bbslow3[0]=1.0;
    bbslow3[1]=2.0;
    bbslow3[2]=1.0;
     bbslow4[0]=1.0;
    bbslow4[1]=2.0;
    bbslow4[2]=1.0;

    aaslow[0]=1.0;
    aaslow[1]= -1.997544931748277;
    aaslow[2]= 0.997583340376228;
    aaslow2[0]=1.0;
    aaslow2[1]=-1.993094943956327 ;
    aaslow2[2]= 0.993133267020283;
    aaslow3[0]=1.0;
    aaslow3[1]= -1.989702442428220;
    aaslow3[2]=0.989740700261438;
    aaslow4[0]=1.0;
    aaslow4[1]= -1.987871245822342 ;
    aaslow4[2]=0.987909468445464;


    ret_conf=ADE_Iir_Configure_params( p_blow->p_iir,iir_gains,&(*nums),&(*denoms),IIR1_N_SECTIONS,imp_type);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir_Configure_params,ret_conf);



#endif

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Iir_Configure_inout(ADE_BLOW_T* p_blow)
{

    ADE_API_RET_T ret_set=ADE_RET_ERROR;


    ret_set= ADE_Iir_Configure_inout(p_blow->p_iir,p_blow->p_in_squared,p_blow->p_pow_slow);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir_Configure_inout,ret_set);

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Iir2_Configure_params(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T iir_gains[IIR2_N_SECTIONS];
    ADE_FLOATING_T bbslow[3];
    ADE_FLOATING_T aaslow[3];
    ADE_FLOATING_T *nums[IIR2_N_SECTIONS]= {bbslow};
    ADE_FLOATING_T *denoms[IIR2_N_SECTIONS]= {aaslow};
    #if (ADE_TARGET_TYPE==ADE_ANDROID)
    ADE_FLOATING_T max_pow=0.7;
    #elif  (ADE_TARGET_TYPE==ADE_IOS)
    ADE_FLOATING_T max_pow=0.25;
    #else
        ADE_FLOATING_T max_pow=0.7;
    #endif
    ADE_API_RET_T ret_conf=ADE_RET_ERROR;
    ADE_IIR_IMP_CHOICE_T imp_type=ADE_IIR_TRASP_II_B;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Iir2_Configure_params,p_blow);

#ifdef ADE_CONFIGURATION_INTERACTIVE
    ret_conf=ADE_Matlab_Configure_Iir_sos(p_blow->p_mat,p_blow->p_iir2, "iir2_sosmatrix", "iir2_scaleval",p_blow->buff_len_i,imp_type);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir2_Configure_params,ret_conf);
#else

    iir_gains[0]=  3.571171247268579e-03;
   // iir_gains[1]=2.844811749336196e-08;

    bbslow[0]=  1.0/max_pow;
    bbslow[1]=1.0/max_pow;
    bbslow[2]=  0.0/max_pow;
//    bbslow2[0]=1.0;
//    bbslow2[1]=2.0;
//    bbslow2[2]=1.0;

    aaslow[0]=1.0;
    aaslow[1]=  -9.928576575054627e-01;
    aaslow[2]=0.0;
//    aaslow2[0]=1.0;
//    aaslow2[1]=-1.999376676193966;
//    aaslow2[2]= 9.993767899864360e-01;





    ADE_Iir_Configure_params( p_blow->p_iir2,iir_gains,&(*nums),&(*denoms),IIR2_N_SECTIONS,imp_type);




#endif
//    ret_set=ADE_Iir_SetInBuff(p_blow->p_iir2,p_blow->p_pow_slow);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir2_Config,ret_set);
//
//    ret_set=ADE_Iir_SetOutBuff(p_blow->p_iir2,p_blow->p_pow_slow_filtered);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir2_Config,ret_set);

    return ADE_RET_SUCCESS;
}


static ADE_API_RET_T ADE_Blow_Iir2_Configure_inout(ADE_BLOW_T* p_blow)
{

    ADE_API_RET_T ret_set=ADE_RET_ERROR;

    ret_set= ADE_Iir_Configure_inout(p_blow->p_iir2,p_blow->p_pow_slow_downsampled,p_blow->p_pow_slow_filtered);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Iir2_Configure_inout,ret_set);

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Blow_Expander_Configure_params(ADE_BLOW_T* p_blow)
{





    ADE_FLOATING_T breaks[POLY_N_BREAKS]= {0,0.05,0.1,0.3,0.6,1};
    ADE_FLOATING_T coeffs[(POLY_N_BREAKS-1)*(POLY_ORDER+1)]={0.0,0.0,1.0,0.0,// {8.881784197001252e-14,-8.881784197001252e-15,1.0,0,
                                                             0.0,0.0,1.0,0.05, // 6.153846153846158e+01, -3.076923076923079e+00,1.0, 5.0e-02,
                                                            0.0,0.0,1.0,0.1, //9.665726943213772e-01, 1.537454691904957 ,1.153846153846154,1.0e-01,
                                                            0.0,0.0,1.0,0.3,  // -1.555362917306849e+00 ,3.072775250991937e+00,1.884816753926702e+00,  4.0e-01,
                                                             0.0,0.0,1.0,0.6 // -8.990439720746057e-01, 3.338282796707305e+00, 3.308533916849015e+00, 1.200e+00
                                                             };
    ADE_API_RET_T ret_set=ADE_RET_ERROR;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Expander_Config,p_blow);

#ifdef ADE_CONFIGURATION_INTERACTIVE

ret_set= ADE_Polyfit_Configure_params(p_blow->p_poly,ADE_Matlab_GetDataPointer(p_blow->p_mat,"breaks"),ADE_Matlab_GetNCols(p_blow->p_mat,"breaks"),ADE_Matlab_GetDataPointer(p_blow->p_mat,"coeffs"),ADE_Matlab_GetNCols(p_blow->p_mat,"coeffs"));

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

    ret_set=ADE_Polyfit_Configure_params(p_blow->p_poly,breaks,p_blow->n_breaks,coeffs,p_blow->poly_order+1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Expander_Config,ret_set) ;

#endif

    return ADE_RET_SUCCESS;

}


static ADE_API_RET_T ADE_Blow_Downsampler_Configure(ADE_BLOW_T* p_blow)
{

    ADE_API_RET_T  ret=ADE_RET_ERROR;
    ADE_INT32_T downfact = p_blow->Fs_i/p_blow->Fs_o;

    ret=ADE_Downsampler_Configure(p_blow->p_downsampler, p_blow->buff_len_o,downfact,p_blow->p_pow_slow,p_blow->p_pow_slow_downsampled);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Downsampler_Configure,ret) ;

     return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Blow_Expander_Configure_inout(ADE_BLOW_T* p_blow)
{
    ADE_API_RET_T  ret_inout=ADE_RET_ERROR;

    ret_inout=ADE_Polyfit_Configure_inout(p_blow->p_poly,p_blow->p_iir2->p_out,p_blow->p_out);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLOW,ADE_METHOD_Expander_Configure_inout,ret_inout) ;

     return ADE_RET_SUCCESS;

}

/****************** SET Methods *********************************/


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
    ADE_FLOATING_T val0 =0.0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_EvalTimesample,p_blow);
     ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_EvalTimesample,Fs_i,"%f",val0);
     ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_EvalTimesample,eval_time_i,"%f",val0);

    p_blow->eval_time_samples = floor(eval_time_i*Fs_i+0.5)+1;

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Set_Expander_static_params(ADE_BLOW_T* p_blow,ADE_INT32_T n_breaks_i,ADE_INT32_T poly_order_i)
{
    ADE_INT32_T val0 = 0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_Expander_static_params,p_blow);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_Expander_static_params,n_breaks_i,"%d",val0);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_Expander_static_params,poly_order_i,"%d",val0);

    p_blow->poly_order = poly_order_i;
    p_blow->n_breaks=n_breaks_i;

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Blow_Set_NSatThresh(ADE_BLOW_T* p_blow,ADE_FLOATING_T n_sat_thres_i)
{
    ADE_FLOATING_T val0 =0.0;


    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_NSatThresh,p_blow);
     ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_NSatThresh,n_sat_thres_i,"%f",val0);

    p_blow->n_sat_thres=n_sat_thres_i;
    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Set_NPowThreshHigh(ADE_BLOW_T* p_blow,ADE_FLOATING_T pow_thresh_high_i)
{
    ADE_FLOATING_T val0 =0.0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshHigh,p_blow);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshHigh,pow_thresh_high_i,"%f",val0);

    p_blow->pow_thresh_high=pow_thresh_high_i;
    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Set_NPowThreshLow(ADE_BLOW_T* p_blow,ADE_FLOATING_T pow_thresh_low_i)
{
     ADE_FLOATING_T val0 =0.0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshLow,p_blow);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshLow,pow_thresh_low_i,"%f",val0);

    p_blow->pow_thresh_low=pow_thresh_low_i;
    return ADE_RET_SUCCESS;
}


static ADE_API_RET_T ADE_Blow_Set_RunPowWinFast(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T running_pow_win_time_fast_i)
{
    ADE_FLOATING_T val0 =0.0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_RunPowWinFast,p_blow);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_RunPowWinFast,Fs_i,"%f",val0);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_RunPowWinFast,running_pow_win_time_fast_i,"%f",val0);

    p_blow->running_pow_win_fast = floor(running_pow_win_time_fast_i*Fs_i+0.5)+1;
    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Set_RunPowWinSlow(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T running_pow_win_time_slow_i)
{
    ADE_FLOATING_T val0 =0.0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_RunPowWinSlow,p_blow);
     ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_RunPowWinSlow,Fs_i,"%f",val0);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_RunPowWinSlow,running_pow_win_time_slow_i,"%f",val0);

    p_blow->running_pow_win_slow = floor(running_pow_win_time_slow_i*Fs_i+0.5)+1;
    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Set_NPowThreshAttack(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T time_pow_thresh_attack_i)
{
    ADE_FLOATING_T n_pow_thres_sample_is_attack=0.0;
    ADE_FLOATING_T val0 =0.0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshAttack,p_blow);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshAttack,Fs_i,"%f",val0);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshAttack,time_pow_thresh_attack_i,"%f",val0);


    n_pow_thres_sample_is_attack=floor(time_pow_thresh_attack_i*Fs_i+0.5)+1;
    p_blow->n_pow_thres_attack=floor(n_pow_thres_sample_is_attack+0.5);

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_Set_NPowThreshRelease(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T time_pow_thresh_release_i)
{
    ADE_FLOATING_T n_pow_thres_sample_is_release=0.0;
     ADE_FLOATING_T val0 =0.0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshRelease,p_blow);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshRelease,Fs_i,"%f",val0);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_Set_NPowThreshRelease,time_pow_thresh_release_i,"%f",val0);

    n_pow_thres_sample_is_release=floor(time_pow_thresh_release_i*Fs_i+0.5)+1;
    p_blow->n_pow_thres_release=floor(n_pow_thres_sample_is_release+0.5);

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_SetInBuffLength (ADE_BLOW_T* p_blow,ADE_INT32_T buff_len)
{
    ADE_INT32_T val0 = 0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_SetInBuffLength,p_blow);
     ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_SetInBuffLength,buff_len,"%d",val0);

     p_blow->buff_len_i=buff_len;

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_SetOutBuffLength (ADE_BLOW_T* p_blow,ADE_INT32_T buff_len)
{
    ADE_INT32_T val0 = 0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_SetOutBuffLength,p_blow);
     ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_SetOutBuffLength,buff_len,"%d",val0);

     p_blow->buff_len_o=buff_len;

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_SetFsIn (ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i)
{
    ADE_FLOATING_T val0 = 0.0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_SetFsIn,p_blow);
     ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_SetFsIn,Fs_i,"%f",val0);

     p_blow->Fs_i=Fs_i;

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_SetFsOut (ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_o)
{
    ADE_FLOATING_T val0 = 0.0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_SetFsOut,p_blow);
     ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_SetFsOut,Fs_o,"%f",val0);

     p_blow->Fs_o=Fs_o;

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_SetFirOrder (ADE_BLOW_T* p_blow,ADE_INT32_T order)
{
    ADE_INT32_T val0 = 0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_SetFirOrder,p_blow);
     ADE_CHECK_VALUE_MAJOR(ADE_CLASS_BLOW,ADE_METHOD_SetFirOrder,order,"%d",val0);

     p_blow->fir_order=order;

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blow_SetInBuff(ADE_BLOW_T* p_blow, ADE_FLOATING_T *p_buff)
{

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_SetInBuff,p_blow);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_SetInBuff,p_buff);

    p_blow->p_in = p_buff;

    return ADE_RET_SUCCESS;

}

/***************************************************************************/

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



//*y=coeffs[coeffs_row_idx+3]+coeffs[coeffs_row_idx+2]*(data-x_breaks[zz])+coeffs[coeffs_row_idx+1]*(data-x_breaks[zz])^2+coeffs[coeffs_row_idx+0]*(data-x_breaks[zz])^3;


ADE_API_RET_T ADE_Blow_GetOutBuff(ADE_BLOW_T* p_blow, ADE_FLOATING_T **dp_buff)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_GetOutBuff,p_blow);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLOW,ADE_METHOD_GetOutBuff,dp_buff);

    *dp_buff=p_blow->p_out;

    return ADE_RET_SUCCESS;

}




