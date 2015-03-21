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





//#include <string.h>

static ADE_API_RET_T ADE_Blow_Fir_Config(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_saturation_detector_sm(ADE_BLOW_T* p_blow,ADE_FLOATING_T sample_i,ADE_FLOATING_T running_pow_fast_i,ADE_FLOATING_T running_pow_slow_i);
static ADE_API_RET_T ADE_Blow_Iir_Config(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Iir2_Config(ADE_BLOW_T* p_blow);
static ADE_VOID_T ADE_Blow_Static_Params(ADE_BLOW_T* p_blow);
static ADE_VOID_T ADE_Blow_Configuration(ADE_BLOW_T* p_blow);
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


ADE_API_RET_T ADE_Blow_Init(ADE_BLOW_T** dp_this,ADE_UINT32_T buff_len,ADE_FLOATING_T Fs_i)
{
     ADE_BLOW_T* pthis = calloc(1,sizeof(ADE_BLOW_T));
     ADE_API_RET_T ret = ADE_DEFAULT_RET;
     //ADE_UINT32_T sos_order = 2;
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

     if (pthis!=NULL)
     {
            //pthis->n_registered_funcs=n_functions;
            pthis->fir_order=0;
           // pthis->iir_order=iir_order;
            //pthis->iir2_order=iir2_order;
            pthis->Fs=Fs_i;
            pthis->buff_len=buff_len;
            pthis->pow_thresh_high=0.0;
            pthis->pow_thresh_low=0.0;
            pthis->sat_thresh=0.0;
            pthis->running_pow_win_fast=0.0;
            pthis->running_pow_win_slow=0.0;
            pthis->n_sat_thres=0;
            pthis->n_pow_thres_attack=0;
            pthis->n_pow_thres_release=0;
            pthis->eval_time_samples=0;
            //pthis->Fs=Fs_i;
            (pthis->p_eval_counter)=calloc(1,sizeof(ADE_UINT32_T));
            if ((pthis->p_eval_counter)==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,(pthis->p_eval_counter),"%p",ADE_Blow_Init);
                return ADE_E16;
            }
            (pthis->p_eval_pow)=calloc(1,sizeof(ADE_UINT32_T));
            if ((pthis->p_eval_pow)==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,(pthis->p_eval_pow),"%p",ADE_Blow_Init);
                return ADE_E16;
            }
            (pthis->p_eval_timer)=calloc(1,sizeof(ADE_UINT32_T));
            if ((pthis->p_eval_timer)==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,(pthis->p_eval_timer),"%p",ADE_Blow_Init);
                return ADE_E16;
            }
            (pthis->p_blow_state)=calloc(1,sizeof(BLOW_SM_STATES_T));
            if ((pthis->p_blow_state)==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,(pthis->p_blow_state),"%p",ADE_Blow_Init);
                return ADE_E16;
            }
            *(pthis->p_blow_state)=ADE_BLOW_WAITING;
           // pthis->n_breaks=n_breaks;
            //pthis->poly_order=poly_order;



            /******************* IIR ALLOC **************************/

            ret = ADE_Iir_Init(&(pthis->p_iir), n_sos_sections_iir, buff_len,ADE_IIR_TRASP_II_B);

            if (ret<0)
            {
                ADE_PRINT_ERRORS(ADE_MEM,ret,"%d",ADE_Blow_Init);
                return ADE_E16;
            }

            /******************* IIR2 ALLOC **************************/

            ret = ADE_Iir_Init(&(pthis->p_iir2), n_sos_sections_iir2, buff_len,ADE_IIR_TRASP_II_B);

            if (ret<0)
            {
                ADE_PRINT_ERRORS(ADE_MEM,ret,"%d",ADE_Blow_Init);
                return ADE_E16;
            }

            /****************** FUNCS ALLOC **************************/


//            pthis->p_blow_functions=calloc(n_functions,sizeof(ADE_BLOW_FUNCS_T));
////
//            if (pthis->p_blow_functions==NULL)
//            {
//                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_blow_functions,"%p",ADE_Blow_Init);
//                return ADE_E16;
//            }

            /********************* P_IN SQUARED ALLOC **********************************/

            pthis->p_in_squared=calloc(pthis->buff_len,sizeof(ADE_FLOATING_T));
//
            if (pthis->p_in_squared==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_in_squared,"%p",ADE_Blow_Init);
                return ADE_E16;
            }

            /********************* POW FAST ALLOC **********************************/

            pthis->p_pow_fast=calloc(pthis->buff_len,sizeof(ADE_FLOATING_T));
//
            if (pthis->p_pow_fast==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_pow_fast,"%p",ADE_Blow_Init);
                return ADE_E16;
            }

            /********************* POW SLOW ALLOC **********************************/

            pthis->p_pow_slow=calloc(pthis->buff_len,sizeof(ADE_FLOATING_T));
//
            if (pthis->p_pow_slow==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_pow_slow,"%p",ADE_Blow_Init);
                return ADE_E16;
            }

            /********************* POW SLOW FILT ALLOC **********************************/

            pthis->p_pow_slow_filtered=calloc(pthis->buff_len,sizeof(ADE_FLOATING_T));
//
            if (pthis->p_pow_slow_filtered==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_pow_slow_filtered,"%p",ADE_Blow_Init);
                return ADE_E16;
            }

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

            if (mat_ret<0)
            {
                ADE_PRINT_ERRORS(ADE_RETCHECKS,mat_ret,"%d",ADE_Blow_Init);
                return ADE_E25;
            }

            #endif

            /*********** BLAS L2 Alloc ************/

            ret = ADE_Blas_level2_Init(&(pthis->p_blas_l2),ADE_REAL);

            if (ret<0)
            {
                ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blow_Init);
                return ADE_E25;
            }


            /*********  CONFIGURATION *********************/



            ADE_Blow_Static_Params(pthis);
            pthis->fir_order=pthis->running_pow_win_fast;

             /**************** FIR ALLOC (depends on static params) *****************************/

            ret = ADE_Fir_Init(&(pthis->p_fir), pthis->running_pow_win_fast, buff_len,ADE_FIR_TRASP_II);
            if (ret<0)
            {
                ADE_PRINT_ERRORS(ADE_MEM,ret,"%d",ADE_Blow_Init);

                return ADE_E16;
            }

            /**************** POLYFIT ALLOC (depends on static params) *****************************/

            ADE_Polyfit_Init (&(pthis->p_poly),pthis->poly_order,pthis->n_breaks);

            ADE_Blow_Configuration(pthis);




              *dp_this=pthis;

     }
     else
     {
          ADE_PRINT_ERRORS(ADE_MEM,pthis,"%p",ADE_Blow_Init);
          return ADE_E16;
     }


     return ADE_DEFAULT_RET;

}

static ADE_VOID_T ADE_Blow_Static_Params(ADE_BLOW_T* p_blow)
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

    Fs = p_blow->Fs;

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

    ADE_Blow_Set_SatThresh(p_blow,nbit,Margin);
    //ADE_Blow_Set_Fs(p_blow->Fs,Fs);
    ADE_Blow_Set_EvalTimesample(p_blow,Fs,eval_time);
    ADE_Blow_Set_NSatThresh(p_blow,n_sat_thres);
    ADE_Blow_Set_NPowThreshHigh(p_blow,pow_thresh_high);
    ADE_Blow_Set_NPowThreshLow(p_blow,pow_thresh_low);
    ADE_Blow_Set_RunPowWinFast(p_blow,Fs,running_pow_win_time_fast);
    ADE_Blow_Set_RunPowWinSlow(p_blow,Fs,running_pow_win_time_slow);
    ADE_Blow_Set_NPowThreshAttack(p_blow,Fs,time_pow_thresh_attack);
    ADE_Blow_Set_NPowThreshRelease(p_blow,Fs,time_pow_thresh_release);
    ADE_Blow_Set_Expander_static_params(p_blow,n_breaks,poly_order);

}

static ADE_VOID_T ADE_Blow_Configuration(ADE_BLOW_T* p_blow)
{

    ADE_Blow_Fir_Config(p_blow);
    ADE_Blow_Iir_Config(p_blow);
    ADE_Blow_Iir2_Config(p_blow);
    ADE_Blow_Expander_Config(p_blow);
    ADE_Blas_level2_Elewise_Config(p_blow->p_blas_l2, p_blow->p_in,p_blow->p_in,p_blow->p_in_squared,1.0,0.0,p_blow->buff_len);

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
    #ifdef ADE_CONFIGURATION_INTERACTIVE
     ADE_Matlab_Release(p_blow->p_mat);
     #endif
    ADE_CHECKNFREE(p_blow);


}

static ADE_API_RET_T ADE_Blow_Set_SatThresh(ADE_BLOW_T* p_blow,ADE_FLOATING_T nbit_i,ADE_FLOATING_T Margin_i)//,ADE_FLOATING_T Fs_i,ADE_FLOATING_T eval_time_i,ADE_FLOATING_T running_pow_win_time_fast_i,ADE_FLOATING_T running_pow_win_time_slow_i,ADE_FLOATING_T time_pow_thresh_attack_i,ADE_FLOATING_T time_pow_thresh_release_i,ADE_UINT32_T n_sat_thres)
{
    ADE_FLOATING_T max_amp =0.0;

	//ADE_FLOATING_T n_pow_thres_sample_is_release=0.0;

  max_amp = 1-pow(2.0,-(nbit_i-1));
  p_blow->sat_thresh = (1-Margin_i)*max_amp;

  return ADE_DEFAULT_RET;
}
static ADE_API_RET_T ADE_Blow_Set_EvalTimesample(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T eval_time_i)
{
    p_blow->eval_time_samples = floor(eval_time_i*Fs_i+0.5)+1;

    return ADE_DEFAULT_RET;
}

static ADE_API_RET_T ADE_Blow_Set_Expander_static_params(ADE_BLOW_T* p_blow,ADE_UINT32_T n_breaks_i,ADE_UINT32_T poly_order_i)
{

    p_blow->poly_order = poly_order_i;
    p_blow->n_breaks=n_breaks_i;

    return ADE_DEFAULT_RET;

}

static ADE_API_RET_T ADE_Blow_Set_NSatThresh(ADE_BLOW_T* p_blow,ADE_FLOATING_T n_sat_thres_i)
{
    p_blow->n_sat_thres=n_sat_thres_i;
    return ADE_DEFAULT_RET;
}

static ADE_API_RET_T ADE_Blow_Set_NPowThreshHigh(ADE_BLOW_T* p_blow,ADE_FLOATING_T pow_thresh_high_i)
{
    p_blow->pow_thresh_high=pow_thresh_high_i;
    return ADE_DEFAULT_RET;
}

static ADE_API_RET_T ADE_Blow_Set_NPowThreshLow(ADE_BLOW_T* p_blow,ADE_FLOATING_T pow_thresh_low_i)
{
    p_blow->pow_thresh_low=pow_thresh_low_i;
    return ADE_DEFAULT_RET;
}

//static ADE_API_RET_T ADE_Blow_Set_Fs(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i)
//{
//    p_blow->Fs=Fs_i;
//    return ADE_DEFAULT_RET;
//}

static ADE_API_RET_T ADE_Blow_Set_RunPowWinFast(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T running_pow_win_time_fast_i)
{
     p_blow->running_pow_win_fast = floor(running_pow_win_time_fast_i*Fs_i+0.5)+1;
    return ADE_DEFAULT_RET;
}

static ADE_API_RET_T ADE_Blow_Set_RunPowWinSlow(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T running_pow_win_time_slow_i)
{
     p_blow->running_pow_win_slow = floor(running_pow_win_time_slow_i*Fs_i+0.5)+1;
    return ADE_DEFAULT_RET;
}

static ADE_API_RET_T ADE_Blow_Set_NPowThreshAttack(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T time_pow_thresh_attack_i)
{
    ADE_FLOATING_T n_pow_thres_sample_is_attack=0.0;

     n_pow_thres_sample_is_attack=floor(time_pow_thresh_attack_i*Fs_i+0.5)+1;
  p_blow->n_pow_thres_attack=floor(n_pow_thres_sample_is_attack+0.5);

  return ADE_DEFAULT_RET;
}

static ADE_API_RET_T ADE_Blow_Set_NPowThreshRelease(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T time_pow_thresh_release_i)
{
    ADE_FLOATING_T n_pow_thres_sample_is_release=0.0;

    n_pow_thres_sample_is_release=floor(time_pow_thresh_release_i*Fs_i+0.5)+1;
  p_blow->n_pow_thres_release=floor(n_pow_thres_sample_is_release+0.5);

  return ADE_DEFAULT_RET;
}


static ADE_API_RET_T ADE_saturation_detector_sm(ADE_BLOW_T* p_blow,ADE_FLOATING_T sample_i,ADE_FLOATING_T running_pow_fast_i,ADE_FLOATING_T running_pow_slow_i)
{


BLOW_SM_STATES_T state_int = *(p_blow->p_blow_state);
BLOW_SM_STATES_T *p_state=p_blow->p_blow_state;

ADE_UINT32_T eval_counter_int=*(p_blow->p_eval_counter);
ADE_UINT32_T eval_pow_int=*(p_blow->p_eval_pow);
ADE_UINT32_T eval_timer_int=*(p_blow->p_eval_timer);
ADE_FLOATING_T abs_sample_i = fabs(sample_i);
ADE_FLOATING_T pow_thresh_high=p_blow->pow_thresh_high;
ADE_FLOATING_T pow_thresh_low=p_blow->pow_thresh_low;
ADE_FLOATING_T sat_thresh=p_blow->sat_thresh;
ADE_UINT32_T n_sat_thres=p_blow->n_sat_thres;
ADE_UINT32_T n_pow_thres_attack=p_blow->n_pow_thres_attack;
ADE_UINT32_T n_pow_thres_release=p_blow->n_pow_thres_release;
ADE_UINT32_T eval_time_init=p_blow->eval_time_samples;
ADE_UINT32_T *p_eval_pow=p_blow->p_eval_pow;
ADE_UINT32_T *p_eval_counter=p_blow->p_eval_counter;
ADE_INT32_T *p_eval_timer=p_blow->p_eval_timer;


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

        ADE_PRINT_ERRORS(ADE_GENERAL,state_int,"%d",ADE_Blow_Fir_Config);
		return ADE_E19;

	}

	return ADE_DEFAULT_RET;

}






static ADE_API_RET_T ADE_Blow_Fir_Config(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T moving_average_coeff = 0.0;
    ADE_UINT32_T i = 0;
    ADE_UINT32_T order = (p_blow->p_fir)->filter_order;


    if ((p_blow->running_pow_win_fast)==0.0)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,(p_blow->running_pow_win_fast),"%u",ADE_Blow_Fir_Config);

        return ADE_E18;
    }

    moving_average_coeff = 1/(p_blow->running_pow_win_fast);

    if (p_blow->p_fir==NULL)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,(p_blow->p_fir),"%p",ADE_Blow_Fir_Config);
        return ADE_E16;

    }

    if ( (p_blow->p_fir)->p_num==NULL)
     {
         ADE_PRINT_ERRORS(ADE_INCHECKS,(p_blow->p_fir),"%p",ADE_Blow_Fir_Config);
        return ADE_E16;
     }

    /**FILL THE TAPS**/

     for(i=0;i<order;i++)
     {
         ((p_blow->p_fir)->p_num)[i]=moving_average_coeff;
     }

     /** POINT INOUT BUFFS ***/

     ADE_Fir_setInbuff(p_blow->p_fir,p_blow->p_in_squared);
     ADE_Fir_setOutbuff(p_blow->p_fir,p_blow->p_pow_fast);

//     /*** SET FILTER IMPLEMENTATION ***/
//
//     ADE_Fir_setFilt_Implementation(p_blow->p_fir,ADE_FIR_TRASP_II);


    return ADE_DEFAULT_RET;
}

static ADE_API_RET_T ADE_Blow_Iir_Config(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T iir_gains[IIR1_N_SECTIONS];
    ADE_FLOATING_T bbslow[IIR1_N_SECTIONS],bbslow2[IIR1_N_SECTIONS],bbslow3[IIR1_N_SECTIONS];
    ADE_FLOATING_T aaslow[IIR1_N_SECTIONS],aaslow2[IIR1_N_SECTIONS],aaslow3[IIR1_N_SECTIONS];
    ADE_FLOATING_T *nums[IIR1_N_SECTIONS]={bbslow,bbslow2,bbslow3};
    ADE_FLOATING_T *denoms[IIR1_N_SECTIONS]={aaslow,aaslow2,aaslow3};


#ifdef ADE_CONFIGURATION_INTERACTIVE
ADE_Matlab_Configure_Iir_sos(p_blow->p_mat,p_blow->p_iir, "iir_sosmatrix", "iir_scaleval");

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


ADE_Iir_setGains( p_blow->p_iir, iir_gains);
ADE_Iir_setNums( p_blow->p_iir, &(*nums));
ADE_Iir_setDenoms( p_blow->p_iir, &(*denoms));

#endif

ADE_Iir_setInbuff(p_blow->p_iir,p_blow->p_in_squared);
ADE_Iir_setOutbuff(p_blow->p_iir,p_blow->p_pow_slow);
//ADE_Iir_setFilt_Implementation(p_blow->p_iir,ADE_FIR_TRASP_II);


    return ADE_DEFAULT_RET;
}

static ADE_API_RET_T ADE_Blow_Iir2_Config(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T iir_gains[IIR2_N_SECTIONS];
    ADE_FLOATING_T bbslow[IIR2_N_SECTIONS],bbslow2[IIR2_N_SECTIONS],bbslow3[IIR2_N_SECTIONS];
    ADE_FLOATING_T aaslow[IIR2_N_SECTIONS],aaslow2[IIR2_N_SECTIONS],aaslow3[IIR2_N_SECTIONS];
    ADE_FLOATING_T *nums[IIR2_N_SECTIONS]={bbslow,bbslow2,bbslow3};
    ADE_FLOATING_T *denoms[IIR2_N_SECTIONS]={aaslow,aaslow2,aaslow3};

  #ifdef ADE_CONFIGURATION_INTERACTIVE
    ADE_Matlab_Configure_Iir_sos(p_blow->p_mat,p_blow->p_iir2, "iir2_sosmatrix", "iir2_scaleval");
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


ADE_Iir_setGains( p_blow->p_iir2, iir_gains);
ADE_Iir_setNums( p_blow->p_iir2, &(*nums));
ADE_Iir_setDenoms( p_blow->p_iir2, &(*denoms));


#endif
ADE_Iir_setInbuff(p_blow->p_iir2,p_blow->p_pow_slow);
ADE_Iir_setOutbuff(p_blow->p_iir2,p_blow->p_pow_slow_filtered);
//ADE_Iir_setFilt_Implementation(p_blow->p_iir2,ADE_FIR_TRASP_II);
    return ADE_DEFAULT_RET;
}


static ADE_API_RET_T ADE_Blow_Expander_Config(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T breaks[POLY_N_BREAKS]={0,0.05,0.1,0.3,0.5,1};
    ADE_FLOATING_T coeffs[(POLY_N_BREAKS-1)*(POLY_ORDER+1)]={0.000000000000089,-0.000000000000009,1.000000000000000,0,
                                                           0,0,1.000000000000000,0.050000000000000,
                                                           8.333333333333334,-1.666666666666667,1.000000000000000,0.100000000000000,
                                                           -21.505376344086020,7.634408602150536,1.333333333333333,0.300000000000000
                                                           -0.317050691244241,-0.254377880184331,1.806451612903226,0.700000000000000};

   #ifdef ADE_CONFIGURATION_INTERACTIVE
ADE_Polyfit_SetBreaks(p_blow->p_poly,ADE_Matlab_GetDataPointer(p_blow->p_mat,"breaks"));
ADE_Polyfit_SetCoeffs(p_blow->p_poly,ADE_Matlab_GetDataPointer(p_blow->p_mat,"coeffs"));
#else
    if (p_blow->poly_order!=POLY_ORDER)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blow->poly_order,"%u",ADE_Blow_Expander_Config);
        return ADE_E20;
    }
    if (p_blow->n_breaks!=POLY_N_BREAKS)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blow->n_breaks,"%u",ADE_Blow_Expander_Config);
        return ADE_E20;
    }

    ADE_Polyfit_SetBreaks(p_blow->p_poly,breaks);
    ADE_Polyfit_SetCoeffs(p_blow->p_poly,coeffs);
    #endif

    return ADE_DEFAULT_RET;

}



//static ADE_API_RET_T ADE_Blow_square_vector(ADE_BLOW_T* p_blow)
//{
//    ADE_UINT32_T i=0;
//    ADE_API_RET_T ret = ADE_DEFAULT_RET;
//





//    return ADE_DEFAULT_RET;
//}

//*y=coeffs[coeffs_row_idx+3]+coeffs[coeffs_row_idx+2]*(data-x_breaks[zz])+coeffs[coeffs_row_idx+1]*(data-x_breaks[zz])^2+coeffs[coeffs_row_idx+0]*(data-x_breaks[zz])^3;
ADE_API_RET_T ADE_Blow_Step(ADE_BLOW_T* p_blow)
{

    ADE_UINT32_T i=0;
    ADE_IIR_T *p_iir2_int=NULL;
    ADE_API_RET_T fir_step_ret = ADE_DEFAULT_RET;
    ADE_API_RET_T iir_step_ret = ADE_DEFAULT_RET;
    ADE_API_RET_T ret = ADE_DEFAULT_RET;
    static ADE_UINT8_T flag;

    #if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)

    if (p_blow->p_in==NULL)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blow->p_in,"%p",ADE_Blow_Step);
        return ADE_E20;
    }

    if (p_blow->p_out==NULL)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blow->p_out,"%p",ADE_Blow_Step);
        return ADE_E20;
    }

    if (p_blow->p_in_squared==NULL)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blow->p_in_squared,"%p",ADE_Blow_Step);
        return ADE_E20;
    }

      //  printf("CHECKs INPUTS STILL NEED TO BE IMPLEMENTED in ADE_Blow_Step\n");

    #endif
 //ret = ADE_Blas_level2_Elewise_Config(p_blow->p_blas_l2, p_blow->p_in,p_blow->p_in,p_blow->p_in_squared,1.0,0.0,p_blow->buff_len);

 #if ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE

 if (ret<0)
 {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blow_Step);
        return ADE_E25;
    }

 #endif
      ret = ADE_Blas_level2_Elewise(p_blow->p_blas_l2);

      #if ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE

 if (ret<0)
 {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blow_Step);
        return ADE_E25;
    }

 #endif
   // ADE_Blow_square_vector(p_blow);
    fir_step_ret=ADE_Fir_Step(p_blow->p_fir);

    #if ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE

    if (fir_step_ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,fir_step_ret,"%d",ADE_Blow_Step);
        return ADE_E25;
    }

    #endif
    iir_step_ret = ADE_Iir_Step(p_blow->p_iir);

     #if ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE

    if (iir_step_ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,iir_step_ret,"%d",ADE_Blow_Step);
        return ADE_E25;
    }

    #endif

    for (i=0;i<p_blow->buff_len;i++)
    {
        ADE_saturation_detector_sm(p_blow,(p_blow->p_in)[i],((p_blow->p_fir)->p_out)[i],((p_blow->p_iir)->p_out)[i]);
    }

    p_iir2_int=p_blow->p_iir2;

    iir_step_ret=ADE_Iir_Step(p_iir2_int);

    #if ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE

    if (iir_step_ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,iir_step_ret,"%d",ADE_Blow_Step);
        return ADE_E25;
    }

    #endif

   // for (i=0;i<p_blow->buff_len;i++)
   // {
        ADE_Polyfit_Step(p_blow->p_poly,p_iir2_int->p_out,p_blow->p_out,p_blow->buff_len);
        //memoryless_blow_expander(p_blow,&((p_iir2_int->p_out)[i]),&((p_blow->p_out)[i]));
    //}


    return ADE_DEFAULT_RET;
}

ADE_VOID_T ADE_Blow_SetInBuff(ADE_BLOW_T* p_blow, ADE_FLOATING_T *p_buff)
{

    p_blow->p_in = p_buff;


}

ADE_VOID_T ADE_Blow_SetOutBuff(ADE_BLOW_T* p_blow, ADE_FLOATING_T *p_buff)
{

    p_blow->p_out = p_buff;


}

