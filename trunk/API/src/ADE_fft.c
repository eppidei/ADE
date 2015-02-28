#include "headers/ADE_fft.h"
#include "headers/ADE_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>





static ADE_VOID_T ADE_Fft_SetInBuff(ADE_FFT_T* p_fft,ADE_VOID_T *p_inbuff);
static ADE_VOID_T ADE_Fft_SetOutBuff(ADE_FFT_T* p_fft,ADE_VOID_T *p_outbuff);
static ADE_API_RET_T ADE_Fft_SetPlan(ADE_FFT_T* p_fft,ADE_FFT_TYPE_T fft_type, ADE_FFT_DIRECTION_T fft_dir);


ADE_API_RET_T ADE_Fft_Init(ADE_FFT_T** dp_this,ADE_UINT32_T buff_len)
{
    ADE_FFT_T *p_this=NULL;
    ADE_INT32_T ret_fftw = ADE_DEFAULT_RET;

    p_this=calloc(1,sizeof(ADE_FFT_T));

    if (p_this!=NULL)
    {
        p_this->buff_len=buff_len;

        #if (ADE_FFT_IMP==ADE_USE_FFTW)

            #if (ADE_FFTW_NTHREADS>0)

                ret_fftw=fftw_init_threads();

                if (ret_fftw<0)
                {
                    ADE_PRINT_ERRORS(ADE_INCHECKS,ret_fftw,"%d",ADE_Fft_Init);
                    return ADE_E33;
                }

            #endif
        #endif



        *dp_this=p_this;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_MEM,p_this,"%p",ADE_Fft_Init);
        return ADE_E31;
    }

    return ADE_DEFAULT_RET;


}

ADE_API_RET_T ADE_Fft_Configure(ADE_FFT_T* p_fft,ADE_FFT_TYPE_T fft_type, ADE_FFT_DIRECTION_T fft_dir,ADE_VOID_T *p_inbuff,ADE_VOID_T *p_outbuff)
{

    ADE_API_RET_T plan_ret= ADE_DEFAULT_RET;

     ADE_Fft_SetInBuff(p_fft,p_inbuff);
     ADE_Fft_SetOutBuff(p_fft,p_outbuff);

    #if (ADE_FFT_IMP==ADE_USE_FFTW)
          #if (ADE_FFTW_NTHREADS>0)

                fftw_plan_with_nthreads(ADE_FFTW_NTHREADS);
          #endif
      #endif


     plan_ret=ADE_Fft_SetPlan(p_fft,fft_type,fft_dir);

     if (plan_ret<0)
     {
         ADE_PRINT_ERRORS(ADE_RETCHECKS,plan_ret,"%d",ADE_Fft_Configure);
         return ADE_E33;
     }

     return ADE_DEFAULT_RET;


}

static ADE_VOID_T ADE_Fft_SetInBuff(ADE_FFT_T* p_fft,ADE_VOID_T *p_inbuff)
{
    p_fft->p_in=p_inbuff;

}

static ADE_VOID_T ADE_Fft_SetOutBuff(ADE_FFT_T* p_fft,ADE_VOID_T *p_outbuff)
{
    p_fft->p_out=p_outbuff;

}


static ADE_API_RET_T ADE_Fft_SetPlan(ADE_FFT_T* p_fft,ADE_FFT_TYPE_T fft_type, ADE_FFT_DIRECTION_T fft_dir)
{

ADE_kFFTRadix_T Ios_radix=kFFTRadix2;

/*** In-Place or not in-place is controlled just with pointers i.e for in-place set p_in=p_out externally**/

      #if (ADE_FFT_IMP==ADE_USE_FFTW)
        if (fft_type==ADE_FFT_C2C)
        {
            if (fft_dir==ADE_FFT_FORWARD)
            {
                #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
                    p_fft->plan = fftw_plan_dft_1d(p_fft->buff_len,(ADE_FFTCPLX_T*)p_fft->p_in,(ADE_FFTCPLX_T*)p_fft->p_out,FFTW_FORWARD,ADE_FFTW_PLAN_FLAGS);
                #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
                    p_fft->plan = fftwf_plan_dft_1d(p_fft->buff_len,(ADE_FFTCPLX_T*)p_fft->p_in,(ADE_FFTCPLX_T*)p_fft->p_out,FFTW_FORWARD,ADE_FFTW_PLAN_FLAGS);
                #else

                    ADE_DEFINE_ERROR(ADE_FP_PRECISION)

                #endif

            }
            else if (fft_dir==ADE_FFT_BACKWARD)
            {
                 #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
                     p_fft->plan = fftw_plan_dft_1d(p_fft->buff_len,(ADE_FFTCPLX_T*)p_fft->p_in,(ADE_FFTCPLX_T*)p_fft->p_out,FFTW_BACKWARD,ADE_FFTW_PLAN_FLAGS);
                 #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
                      p_fft->plan = fftwf_plan_dft_1d(p_fft->buff_len,(ADE_FFTCPLX_T*)p_fft->p_in,(ADE_FFTCPLX_T*)p_fft->p_out,FFTW_BACKWARD,ADE_FFTW_PLAN_FLAGS);
                 #else

                    ADE_DEFINE_ERROR(ADE_FP_PRECISION)

                #endif
            }
            else
            {
                ADE_PRINT_ERRORS(ADE_INCHECKS,fft_dir,"%d",ADE_Fft_SetPlan);
                return ADE_E32;
            }

        }
        else if (fft_type==ADE_FFT_R2C)
        {
            if (fft_dir==ADE_FFT_FORWARD)
            {
                 #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
                    p_fft->plan = fftw_plan_dft_r2c_1d(p_fft->buff_len,(ADE_FLOATING_T*)p_fft->p_in,(ADE_FFTCPLX_T*)p_fft->p_out,ADE_FFTW_PLAN_FLAGS);
                 #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
                     p_fft->plan = fftwf_plan_dft_r2c_1d(p_fft->buff_len,(ADE_FLOATING_T*)p_fft->p_in,(ADE_FFTCPLX_T*)p_fft->p_out,ADE_FFTW_PLAN_FLAGS);
                  #else

                    ADE_DEFINE_ERROR(ADE_FP_PRECISION)

                #endif
            }
            else if (fft_dir==ADE_FFT_BACKWARD)
            {
                 ADE_PRINT_ERRORS(ADE_INCHECKS,fft_dir,"%d",ADE_Fft_SetPlan);
                return ADE_E32;
            }
            else
            {
                ADE_PRINT_ERRORS(ADE_INCHECKS,fft_dir,"%d",ADE_Fft_SetPlan);
                return ADE_E32;
            }

        }
        else if (fft_type==ADE_FFT_C2R)
        {
            if (fft_dir==ADE_FFT_FORWARD)
            {

                   ADE_PRINT_ERRORS(ADE_INCHECKS,fft_dir,"%d",ADE_Fft_SetPlan);
                return ADE_E32;

            }
            else if (fft_dir==ADE_FFT_BACKWARD)
            {
                #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
                    p_fft->plan = fftw_plan_dft_c2r_1d(p_fft->buff_len,(ADE_FFTCPLX_T*)p_fft->p_in,(ADE_FLOATING_T*)p_fft->p_out,ADE_FFTW_PLAN_FLAGS);
                #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
                    p_fft->plan = fftwf_plan_dft_c2r_1d(p_fft->buff_len,(ADE_FFTCPLX_T*)p_fft->p_in,(ADE_FLOATING_T*)p_fft->p_out,ADE_FFTW_PLAN_FLAGS);
                #else
                    ADE_DEFINE_ERROR(ADE_FP_PRECISION)

                #endif
            }
            else
            {
                ADE_PRINT_ERRORS(ADE_INCHECKS,fft_dir,"%d",ADE_Fft_SetPlan);
                return ADE_E32;
            }

        }
        else
        {
            ADE_PRINT_ERRORS(ADE_INCHECKS,fft_type,"%d",ADE_Fft_SetPlan);
            return ADE_E32;
        }

        if (p_fft->plan==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_fft->plan,"%p",ADE_Fft_SetPlan);
            return ADE_E31;
        }

        #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)

            #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

                p_fft->p_setup=vDSP_create_fftsetup ( (ADE_vDSP_Length) ceil(log2(p_fft->buff_len)), Ios_radix );

            #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

                p_fft->p_setup=vDSP_create_fftsetupD ( (ADE_vDSP_Length) ceil(log2(p_fft->buff_len)), Ios_radix );

            #else
              ADE_DEFINE_ERROR(ADE_FP_PRECISION);

            #endif

        if (p_fft->p_setup==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_fft->p_setup,"%p",ADE_Fft_SetPlan);
            return ADE_E31;
        }

        #else

        ADE_DEFINE_ERROR(ADE_FFT_IMP);

        #endif



        return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Fft_Step(ADE_FFT_T* p_fft)
{
    #if (ADE_FFT_IMP==ADE_USE_FFTW)

        #if (ADE_CHECK_INPUTS==1)
            if (p_fft->p_in==NULL)
            {
                 ADE_PRINT_ERRORS(ADE_INCHECKS,p_fft->p_in,"%p",ADE_Fft_Step);
                return ADE_E32;
            }
            if (p_fft->p_out==NULL)
            {
                ADE_PRINT_ERRORS(ADE_INCHECKS,p_fft->p_out,"%p",ADE_Fft_Step);
                return ADE_E32;

            }
            if (p_fft->plan==NULL)
            {
                ADE_PRINT_ERRORS(ADE_INCHECKS,p_fft->plan,"%p",ADE_Fft_Step);
                return ADE_E32;
            }




        #endif

        fftw_execute(p_fft->plan);


    #else
         ADE_DEFINE_ERROR(ADE_FFT_IMP);

    #endif

    return ADE_DEFAULT_RET;

}

ADE_VOID_T ADE_Fft_Release(ADE_FFT_T* p_fft)
{
    #if (ADE_FFT_IMP==ADE_USE_FFTW)
        //fftw_free(p_fft->p_in);
        //fftw_free(p_fft->p_out);
           #if (ADE_FFTW_NTHREADS>0)
            fftw_cleanup_threads();
           #endif
        fftw_destroy_plan(p_fft->plan);
        fftw_cleanup();
    #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
        #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
            vDSP_destroy_fftsetup (p_fft->p_setup );
        #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
            vDSP_destroy_fftsetupD (p_fft->p_setup );
        #else
             ADE_DEFINE_ERROR(ADE_FP_PRECISION);
        #endif

    #else
         ADE_DEFINE_ERROR(ADE_FFT_IMP);
    #endif
    free(p_fft);
}
