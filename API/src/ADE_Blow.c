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

static ADE_API_RET_T ADE_Blow_Fir_Config(ADE_BLOW_T* p_blow);
static ADE_API_RET_T saturation_detector_sm(ADE_BLOW_T* p_blow,ADE_FLOATING_T sample_i,ADE_FLOATING_T running_pow_fast_i,ADE_FLOATING_T running_pow_slow_i);
static ADE_API_RET_T ADE_Blow_Iir_Config(ADE_BLOW_T* p_blow);
static ADE_API_RET_T ADE_Blow_Iir2_Config(ADE_BLOW_T* p_blow);

ADE_API_RET_T ADE_Blow_Init(ADE_BLOW_T** dp_this,ADE_UINT32_T n_functions,ADE_UINT32_T fir_order,ADE_UINT32_T iir_order,ADE_UINT32_T buff_size,ADE_UINT32_T n_breaks,ADE_UINT32_T poly_order)
{
     ADE_BLOW_T* pthis = calloc(1,sizeof(ADE_BLOW_T));
     ADE_API_RET_T ret = ADE_DEFAULT_RET;
     ADE_UINT32_T n_sos_sections=(ADE_UINT32_T)ceil((ADE_FLOATING_DP_T)iir_order);
     ADE_UINT32_T poly_pieces=0;

     if (pthis!=NULL)
     {
            pthis->n_registered_funcs=n_functions;
            pthis->fir_order=fir_order;
            pthis->iir_order=iir_order;
            pthis->buff_size=buff_size;
            pthis->pow_thresh_high=0.0;
            pthis->pow_thresh_low=0.0;
            pthis->sat_thresh=0.0;
            pthis->running_pow_win_fast=0.0;
            pthis->running_pow_win_slow=0.0;
            pthis->n_sat_thres=0;
            pthis->n_pow_thres_attack=0;
            pthis->n_pow_thres_release=0;
            pthis->eval_time_samples=0;
            *(pthis->p_eval_counter)=0;
            *(pthis->p_eval_pow)=0;
            *(pthis->p_eval_timer)=0;
            *(pthis->p_blow_state)=waiting;
            pthis->n_breaks=n_breaks;
            pthis->poly_order=poly_order;

            /**************** FIR ALLOC *****************************/

            ret = ADE_Fir_Init(&(pthis->p_fir), fir_order, buff_size);
            if (ret<0)
            {
                ADE_PRINT_ERRORS(ADE_MEM,ret,"%d",ADE_Blow_Init);

                return ADE_E16;
            }

            /******************* IIR ALLOC **************************/

            ret = ADE_Iir_Init(&(pthis->p_iir), n_sos_sections, buff_size);

            if (ret<0)
            {
                ADE_PRINT_ERRORS(ADE_MEM,ret,"%d",ADE_Blow_Init);
                return ADE_E16;
            }

            /******************* IIR2 ALLOC **************************/

            ret = ADE_Iir_Init(&(pthis->p_iir2), n_sos_sections, buff_size);

            if (ret<0)
            {
                ADE_PRINT_ERRORS(ADE_MEM,ret,"%d",ADE_Blow_Init);
                return ADE_E16;
            }

            /****************** FUNCS ALLOC **************************/


            pthis->p_blow_functions=calloc(n_functions,sizeof(ADE_BLOW_FUNCS_T));
//
            if (pthis->p_blow_functions==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_blow_functions,"%p",ADE_Blow_Init);
                return ADE_E16;
            }

            /********************* P_IN SQUARED ALLOC **********************************/

            pthis->p_in_squared=calloc(pthis->buff_size,sizeof(ADE_FLOATING_T));
//
            if (pthis->p_in_squared==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_in_squared,"%p",ADE_Blow_Init);
                return ADE_E16;
            }

            /********************* POW FAST ALLOC **********************************/

            pthis->p_pow_fast=calloc(pthis->buff_size,sizeof(ADE_FLOATING_T));
//
            if (pthis->p_pow_fast==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_pow_fast,"%p",ADE_Blow_Init);
                return ADE_E16;
            }

            /********************* POW SLOW ALLOC **********************************/

            pthis->p_pow_slow=calloc(pthis->buff_size,sizeof(ADE_FLOATING_T));
//
            if (pthis->p_pow_slow==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_pow_slow,"%p",ADE_Blow_Init);
                return ADE_E16;
            }

            /********************* POW SLOW FILT ALLOC **********************************/

            pthis->p_pow_slow_filtered=calloc(pthis->buff_size,sizeof(ADE_FLOATING_T));
//
            if (pthis->p_pow_slow_filtered==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_pow_slow_filtered,"%p",ADE_Blow_Init);
                return ADE_E16;
            }

            /********************* STATE ALLOC **********************************/

//            pthis->p_blow_state=calloc(buff_size,sizeof(ADE_BLOW_FUNCS_T));
////
//            if (pthis->p_blow_state==NULL)
//            {
//                ADE_PRINT_ERRORS(ADE_MEM,pthis->p_blow_state,"%p",ADE_Blow_Init);
//                return ADE_E16;
//            }

            /*************************** BREAKS AND POLY EXP COEFFS ALLOC **********************/
            pthis->breaks=calloc(n_breaks,sizeof(ADE_FLOATING_T));
            if (pthis->breaks==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->breaks,"%p",ADE_Blow_Init);
                return ADE_E16;
            }


            poly_pieces = pthis->n_breaks-1;
            if (poly_pieces<=0)
            {
                ADE_PRINT_ERRORS(ADE_INCHECKS,poly_pieces,"%d",ADE_Blow_Init);
                return ADE_E20;
            }
            pthis->poly_coeffs=calloc(poly_pieces*poly_order,sizeof(ADE_FLOATING_T));
            if (pthis->poly_coeffs==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,pthis->poly_coeffs,"%p",ADE_Blow_Init);
                return ADE_E16;
            }


            *dp_this=pthis;

            /********* FILTER CONFIGURATION *********************/

            ADE_Blow_Fir_Config(pthis);
            ADE_Blow_Iir_Config(pthis);
            ADE_Blow_Iir2_Config(pthis);
     }


     return ADE_DEFAULT_RET;

}

ADE_VOID_T ADE_Blow_Release(ADE_BLOW_T* p_blow)
{

    ADE_Iir_Release(p_blow->p_iir);
    ADE_Iir_Release(p_blow->p_iir2);
    ADE_Fir_Release(p_blow->p_fir);
    ADE_CHECKNFREE(p_blow->p_in_squared);
    ADE_CHECKNFREE(p_blow->p_pow_fast);
    ADE_CHECKNFREE(p_blow->p_pow_slow);
    ADE_CHECKNFREE(p_blow->p_pow_slow_filtered);
    ADE_CHECKNFREE(p_blow->p_blow_functions);
    ADE_CHECKNFREE(p_blow->poly_coeffs);
    ADE_CHECKNFREE(p_blow->breaks);
    ADE_CHECKNFREE(p_blow);

}

ADE_API_RET_T ADE_Blow_Set_SatThresh(ADE_BLOW_T* p_blow,ADE_FLOATING_T nbit_i,ADE_FLOATING_T Margin_i)//,ADE_FLOATING_T Fs_i,ADE_FLOATING_T eval_time_i,ADE_FLOATING_T running_pow_win_time_fast_i,ADE_FLOATING_T running_pow_win_time_slow_i,ADE_FLOATING_T time_pow_thresh_attack_i,ADE_FLOATING_T time_pow_thresh_release_i,ADE_UINT32_T n_sat_thres)
{
    ADE_FLOATING_T max_amp =0.0;

	//ADE_FLOATING_T n_pow_thres_sample_is_release=0.0;

  max_amp = 1-pow(2.0,-(nbit_i-1));
  p_blow->sat_thresh = (1-Margin_i)*max_amp;

  return ADE_DEFAULT_RET;
}
ADE_API_RET_T ADE_Blow_Set_EvalTimesample_is(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T eval_time_i)
{
    p_blow->eval_time_samples = floor(eval_time_i*Fs_i+0.5)+1;

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Blow_Set_NSatThresh(ADE_BLOW_T* p_blow,ADE_FLOATING_T n_sat_thres_i)
{
    p_blow->n_sat_thres=n_sat_thres_i;
    return ADE_DEFAULT_RET;
}
ADE_API_RET_T ADE_Blow_Set_RunPowWinFast(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T running_pow_win_time_fast_i)
{
     p_blow->running_pow_win_fast = floor(running_pow_win_time_fast_i*Fs_i+0.5)+1;
    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Blow_Set_RunPowWinSlow(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T running_pow_win_time_slow_i)
{
     p_blow->running_pow_win_slow = floor(running_pow_win_time_slow_i*Fs_i+0.5)+1;
    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Blow_Set_NPowThreshAttack(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T time_pow_thresh_attack_i)
{
    ADE_FLOATING_T n_pow_thres_sample_is_attack=0.0;

     n_pow_thres_sample_is_attack=floor(time_pow_thresh_attack_i*Fs_i+0.5)+1;
  p_blow->n_pow_thres_attack=floor(n_pow_thres_sample_is_attack+0.5);

  return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Blow_Set_NPowThreshRelease(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T time_pow_thresh_release_i)
{
    ADE_FLOATING_T n_pow_thres_sample_is_release=0.0;

    n_pow_thres_sample_is_release=floor(time_pow_thresh_release_i*Fs_i+0.5)+1;
  p_blow->n_pow_thres_release=floor(n_pow_thres_sample_is_release+0.5);

  return ADE_DEFAULT_RET;
}


static ADE_API_RET_T saturation_detector_sm(ADE_BLOW_T* p_blow,ADE_FLOATING_T sample_i,ADE_FLOATING_T running_pow_fast_i,ADE_FLOATING_T running_pow_slow_i)
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
ADE_UINT32_T *p_eval_timer=p_blow->p_eval_timer;


    if (state_int==tracking)
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
            *p_state=waiting;
	   }
       else
	   {
             *p_state=state_int;
	   }

       *p_eval_counter=0;
       *p_eval_timer=0;

	}
    else if (state_int==evaluation)
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
            *p_state=tracking;
            *p_eval_pow=0;
		}
        else if (*p_eval_timer==0)
		{
            *p_state=waiting;
		}
        else
		{
            *p_state=evaluation;
		}

	}

    else if (state_int==waiting)


        if (abs_sample_i>sat_thresh)
		{
            *p_eval_timer=eval_time_init;
            *p_eval_counter=0;
            *p_eval_pow= 0;

            *p_state=evaluation;
		}
        else
		{
            *p_eval_timer=eval_timer_int;
             *p_eval_counter= eval_counter_int;
             *p_eval_pow=0;
            *p_state=waiting;

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
        ADE_PRINT_ERRORS(ADE_INCHECKS,(p_blow->running_pow_win_fast),"%f",ADE_Blow_Fir_Config);

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


     for(i=0;i<order;i++)
     {
         ((p_blow->p_fir)->p_num)[i]=moving_average_coeff;
     }

     ADE_Fir_setInbuff(p_blow->p_fir,p_blow->p_in_squared);
     ADE_Fir_setOutbuff(p_blow->p_fir,p_blow->p_pow_fast);


    return ADE_DEFAULT_RET;
}

static ADE_API_RET_T ADE_Blow_Iir_Config(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T iir_gains[IIR1_SECTIONS];
    ADE_FLOATING_T bbslow[IIR1_SECTIONS],bbslow2[IIR1_SECTIONS],bbslow3[IIR1_SECTIONS];
    ADE_FLOATING_T aaslow[IIR1_SECTIONS],aaslow2[IIR1_SECTIONS],aaslow3[IIR1_SECTIONS];
    ADE_FLOATING_T *nums[IIR1_SECTIONS]={bbslow,bbslow2,bbslow3};
    ADE_FLOATING_T *denoms[IIR1_SECTIONS]={aaslow,aaslow2,aaslow3};

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
ADE_Iir_setInbuff(p_blow->p_iir,p_blow->p_in_squared);
ADE_Iir_setOutbuff(p_blow->p_iir,p_blow->p_pow_slow);
    return ADE_DEFAULT_RET;
}

static ADE_API_RET_T ADE_Blow_Iir2_Config(ADE_BLOW_T* p_blow)
{

    ADE_FLOATING_T iir_gains[IIR1_SECTIONS];
    ADE_FLOATING_T bbslow[IIR1_SECTIONS],bbslow2[IIR1_SECTIONS],bbslow3[IIR1_SECTIONS];
    ADE_FLOATING_T aaslow[IIR1_SECTIONS],aaslow2[IIR1_SECTIONS],aaslow3[IIR1_SECTIONS];
    ADE_FLOATING_T *nums[IIR1_SECTIONS]={bbslow,bbslow2,bbslow3};
    ADE_FLOATING_T *denoms[IIR1_SECTIONS]={aaslow,aaslow2,aaslow3};

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
ADE_Iir_setInbuff(p_blow->p_iir2,p_blow->p_pow_slow);
ADE_Iir_setOutbuff(p_blow->p_iir2,p_blow->p_pow_slow_filtered);
    return ADE_DEFAULT_RET;
}

static ADE_API_RET_T memoryless_blow_expander(ADE_BLOW_T* p_blow,ADE_FLOATING_T* frame_i,ADE_FLOATING_T* y_o)

{

  //  ADE_UINT32_T frame_size = p_blow->buff_size;
    ADE_UINT32_T i=0, k=0,zz=0;
    ADE_FLOATING_T data=0.0;
    ADE_FLOATING_T* coeffs=p_blow->poly_coeffs;
    ADE_UINT32_T n_pieces = p_blow->n_breaks-1;
    ADE_FLOATING_T *x_breaks = p_blow->breaks;
    ADE_UINT32_T coeffs_row_idx=0, coeffs_col_idx=0;
    ADE_UINT32_T n_coeff_per_piece = p_blow->poly_order+1;

    #if (ADE_CHECK_INPUTS==1)

        printf("INPUT CHECKS STILL NOT IMPLEMENTED in memoryless_blow_expander\n");

    #endif

//for (i=0;i<frame_size;i++)
//{
    *y_o=0;
    data = *frame_i;

    if ( data>=0 )
    {
        for (k=0;k<n_pieces;k++)
        {
             if (k==0)
             {
                  if ( (data>=x_breaks[k]) && (data<x_breaks[k+1]) )
                  {
                      zz=k;
                    break;
                  }

             }

        else if (k==(n_pieces-1))
        {
             if ( (data>=x_breaks[k]) && (data<=x_breaks[k+1]) )
             {
                 zz=k;
                 break;
             }
        }
        else
        {
             if ( (data>=x_breaks[k]) && (data<x_breaks[k+1]) )
             {
                   zz=k;
                    break;
             }

        }



        }


    }


    else
    {
        zz=0;
        data = 0;
    }

    coeffs_row_idx=zz*n_coeff_per_piece;

    for(i=0;i<n_coeff_per_piece;i++)
    {
        coeffs_col_idx = n_coeff_per_piece-1;
       *y_o +=coeffs[coeffs_row_idx+coeffs_col_idx]*pow((data-x_breaks[zz]),i);
    }

    return ADE_DEFAULT_RET;

}

static ADE_API_RET_T ADE_square_vector(ADE_FLOATING_T* in ,ADE_FLOATING_T* out ,ADE_UINT32_T len)
{
    ADE_UINT32_T i=0;

    for (i=0;i<len;i++)
    {
        out[i]=in[i]*in[i];
    }

    return ADE_DEFAULT_RET;
}

//*y=coeffs[coeffs_row_idx+3]+coeffs[coeffs_row_idx+2]*(data-x_breaks[zz])+coeffs[coeffs_row_idx+1]*(data-x_breaks[zz])^2+coeffs[coeffs_row_idx+0]*(data-x_breaks[zz])^3;
ADE_API_RET_T ADE_Blow_Step(ADE_BLOW_T* p_blow)
{

    ADE_UINT32_T i=0;
    ADE_IIR_T *p_iir2_int=NULL;

    #if (ADE_CHECK_INPUTS==1)

        printf("CHECKs INPUTS STILL NEED TO BE IMPLEMENTED in ADE_Blow_Step\n");

    #endif

    ADE_square_vector(p_blow->p_in,p_blow->p_in_squared,p_blow->buff_size);
    ADE_Fir_Step(p_blow->p_fir);
    ADE_Iir_Step(p_blow->p_iir);

    for (i=0;i<p_blow->buff_size;i++)
    {
        saturation_detector_sm(p_blow,(p_blow->p_in)[i],((p_blow->p_fir)->p_out)[i],((p_blow->p_iir)->p_out)[i]);
    }

    p_iir2_int=p_blow->p_iir2;

    ADE_Iir_Step(p_iir2_int);

    for (i=0;i<p_blow->buff_size;i++)
    {
        memoryless_blow_expander(p_blow,&((p_iir2_int->p_out)[i]),&((p_blow->p_out)[i]));
    }


    return ADE_DEFAULT_RET;
}

