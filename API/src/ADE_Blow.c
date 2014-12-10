#include "headers/ADE_Blow.h"
#include "headers/ADE_fir.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <string.h>

ADE_API_RET_T ADE_Blow_Init(ADE_BLOW_T** dp_this,ADE_UINT32_T n_functions,ADE_UINT32_T fir_order,ADE_UINT32_T iir_order,ADE_UINT32_T buff_size)
{
     ADE_BLOW_T* pthis = calloc(1,sizeof(ADE_BLOW_T));
     ADE_API_RET_T ret = ADE_DEFAULT_RET;
     ADE_UINT32_T n_sos_sections=(ADE_UINT32_T)ceil((ADE_FLOATING_DP_T)iir_order);

     if (pthis!=NULL)
     {
            pthis->n_registered_funcs=n_functions;
            pthis->fir_order=fir_order;
            pthis->iir_order=iir_order;
            pthis->buff_size=buff_size;

            ret = ADE_Fir_Init(&(pthis->p_fir), fir_order, buff_size);
            if (ret<0)
            {
                ADE_PRINT_ERRORS(ADE_MEM,ret,"%d",ADE_Blow_Init);

                return ADE_E16;
            }

            ret = ADE_Iir_Init(&(pthis->p_iir), n_sos_sections, buff_size);

            if (ret<0)
            {
                ADE_PRINT_ERRORS(ADE_MEM,ret,"%d",ADE_Blow_Init);
                return ADE_E16;
            }


            pthis->p_blow_functions=calloc(n_functions,sizeof(ADE_BLOW_FUNCS_T));
//
            if (pthis->p_blow_functions==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_blow_functions,"%d",ADE_Blow_Init);
                return ADE_E16;
            }

            pthis->p_blow_state=calloc(buff_size,sizeof(ADE_BLOW_FUNCS_T));
//
            if (pthis->p_blow_state==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_blow_state,"%d",ADE_Blow_Init);
                return ADE_E16;
            }

            *dp_this=pthis;

     }


     return ADE_DEFAULT_RET;

}

ADE_VOID_T ADE_Blow_Release(ADE_BLOW_T* p_blow)
{

    ADE_Iir_Release(p_blow->p_iir);
    ADE_Fir_Release(p_blow->p_fir);
    ADE_CHECKNFREE(p_blow->p_blow_functions);
    ADE_CHECKNFREE(p_blow->p_blow_state);
    ADE_CHECKNFREE(p_blow);

}

ADE_API_RET_T ADE_Blow_Params_Config(ADE_BLOW_T* p_blow,ADE_FLOATING_T nbit_i,ADE_FLOATING_T Margin_i,ADE_FLOATING_T Fs_i,ADE_FLOATING_T eval_time_i,ADE_FLOATING_T running_pow_win_time_fast_i,ADE_FLOATING_T running_pow_win_time_slow_i,ADE_FLOATING_T time_pow_thresh_attack_i,ADE_FLOATING_T time_pow_thresh_release_i,ADE_UINT32_T n_sat_thres)
{
    ADE_FLOATING_T max_amp =0.0;
	ADE_FLOATING_T n_pow_thres_samples_attack=0.0;
	ADE_FLOATING_T n_pow_thres_samples_release=0.0;

  max_amp = 1-pow(2.0,-(nbit_i-1));
  p_blow->sat_thresh = (1-Margin_i)*max_amp;

  p_blow->eval_time_samples = floor(eval_time_i*Fs_i+0.5)+1;

  p_blow->n_sat_thres=n_sat_thres;

  p_blow->running_pow_win_fast = floor(running_pow_win_time_fast_i*Fs_i+0.5)+1;

  p_blow->running_pow_win_slow = floor(running_pow_win_time_slow_i*Fs_i+0.5)+1;



  n_pow_thres_samples_attack=floor(time_pow_thresh_attack_i*Fs_i+0.5)+1;
  p_blow->n_pow_thres_attack=floor(n_pow_thres_samples_attack+0.5);

  n_pow_thres_samples_release=floor(time_pow_thresh_release_i*Fs_i+0.5)+1;
  p_blow->n_pow_thres_release=floor(n_pow_thres_samples_release+0.5);

return ADE_DEFAULT_RET;
}

//ADE_API_RET_T ADE_Blow_Fir_Config(ADE_BLOW_T* p_blow
