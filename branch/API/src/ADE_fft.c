#include "headers/ADE_fft.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include "headers/ADE_Error_Handler.h"



/************** Private Methods Headers ******************/

static ADE_API_RET_T ADE_Fft_SetInBuff(ADE_FFT_T* p_fft,ADE_VOID_T *p_inbuff);
static ADE_API_RET_T ADE_Fft_SetOutBuff(ADE_FFT_T* p_fft,ADE_VOID_T *p_outbuff);
static ADE_API_RET_T ADE_Fft_SetPlan(ADE_FFT_T* p_fft);
static ADE_API_RET_T ADE_Fft_doSetPlan(ADE_FFT_TYPE_T type,ADE_INT32_T buff_len,ADE_VOID_T *p_in,ADE_VOID_T *p_out,ADE_FFTW_DIRECTION_T direction,ADE_INT32_T plan_flag,ADE_FFTW_PLAN_T* p_plan_o);
static ADE_API_RET_T ADE_Fft_SetDirection(ADE_FFT_T* p_fft,ADE_FFT_DIRECTION_T fft_dir);
static ADE_API_RET_T ADE_Fft_SetType(ADE_FFT_T* p_fft,ADE_FFT_TYPE_T fft_type);
static ADE_API_RET_T ADE_Fft_SetBufflength(ADE_FFT_T* p_fft,ADE_INT32_T buff_len);
static ADE_API_RET_T ADE_Fft_Configure_params(ADE_FFT_T* p_fft,ADE_FFT_TYPE_T fft_type, ADE_FFT_DIRECTION_T fft_dir,ADE_VOID_T *p_inbuff,ADE_VOID_T *p_outbuff,ADE_INT32_T buff_len);

/******** Init Methods ************/

ADE_API_RET_T ADE_Fft_Init(ADE_FFT_T** dp_this)
{
    ADE_FFT_T *p_this=NULL;
    ADE_INT32_T ret_fftw = 0;
    ADE_INT32_T max_buff_len = ADE_FFT_MAX_BUFF_LEN;

    p_this=calloc(1,sizeof(ADE_FFT_T));
     ADE_CHECK_MEMALLOC(ADE_CLASS_FFT,ADE_METHOD_Init,p_this);

        p_this->buff_len=0;
        p_this->max_buff_len=max_buff_len;

        #if (ADE_FFT_IMP==ADE_USE_FFTW)

            #if (ADE_FFTW_NTHREADS>0)
				#if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
                	ret_fftw=fftw_init_threads();
				#elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
 					ret_fftw=fftwf_init_threads();
				#else
					#error ADE_FP_PRECISION in ADE_FFft_Init
				#endif
                if (ret_fftw<0)
                {
                    ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_FFT,ADE_METHOD_Init,ret_fftw,"%d",(FILE*)ADE_STD_STREAM);
                    return ADE_RET_ERROR;
                }

            #endif
        #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)

    p_this->p_split_buff_in=calloc(max_buff_len,sizeof(ADE_FFTCPLX_T));//alloco sempre max dim FC2C se real We use half
    ADE_CHECK_MEMALLOC(ADE_CLASS_FFT,ADE_METHOD_Init,p_this->p_split_buff_in);
    p_this->p_split_buff_out=calloc(max_buff_len,sizeof(ADE_FFTCPLX_T));//alloco sempre max dim FC2C se real We use half
    ADE_CHECK_MEMALLOC(ADE_CLASS_FFT,ADE_METHOD_Init,p_this->p_split_buff_out);
        #endif

         p_this->direction=ADE_FFT_INVALID_DIR;
         p_this->type=ADE_FFT_INVALID_TYPE;

        *dp_this=p_this;


    return ADE_RET_SUCCESS;


}

ADE_VOID_T ADE_Fft_Release(ADE_FFT_T* p_fft)
{
    #if (ADE_FFT_IMP==ADE_USE_FFTW)
        //fftw_free(p_fft->p_in);
        //fftw_free(p_fft->p_out);

       #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
			#if (ADE_FFTW_NTHREADS>0)
            	fftw_cleanup_threads();
            #endif
        fftw_destroy_plan(p_fft->plan);
		 fftw_cleanup();
        #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
			#if (ADE_FFTW_NTHREADS>0)
            	fftwf_cleanup_threads();
            #endif
            fftwf_destroy_plan(p_fft->plan);
            fftwf_cleanup();
        #else
         #error (ADE_FP_PRECISION in ADE_Fft_Release)
        #endif


    #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
        #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
            vDSP_destroy_fftsetup (p_fft->p_setup );
        #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
            vDSP_destroy_fftsetupD (p_fft->p_setup );
        #else
             #error(ADE_FP_PRECISION);
        #endif
    ADE_CHECKNFREE(p_fft->p_split_buff_in);
    ADE_CHECKNFREE(p_fft->p_split_buff_out);

    #else
         #error(ADE_FFT_IMP);
    #endif
    free(p_fft);
}

/************* Configure Methods ****************/
/** In fftw configure cannot be separated because plan needs everithing **/
ADE_API_RET_T ADE_Fft_Configure(ADE_FFT_T* p_fft,ADE_FFT_TYPE_T fft_type, ADE_FFT_DIRECTION_T fft_dir,ADE_VOID_T *p_inbuff,ADE_VOID_T *p_outbuff,ADE_INT32_T buff_len)
{
    ADE_API_RET_T ret_params = ADE_RET_ERROR;
   // ADE_API_RET_T ret_inout = ADE_RET_ERROR;
    ADE_API_RET_T ret_bufflen = ADE_RET_ERROR;

    ret_params =ADE_Fft_Configure_params(p_fft, fft_type,  fft_dir,p_inbuff,p_outbuff,buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_Configure,ret_params);

//    ret_inout =ADE_Fft_Configure_bufflength(p_fft, buff_len);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_Configure,ret_inout);

//    ret_bufflen =ADE_Fft_Configure_inout(p_fft, p_inbuff, p_outbuff);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_Configure,ret_bufflen);

    return ADE_RET_SUCCESS;

}
//
//ADE_API_RET_T ADE_Fft_Configure_bufflength(ADE_FFT_T* p_fft,ADE_INT32_T buff_len)
//{
//    ADE_API_RET_T ret_bufflen= ADE_RET_ERROR;
//
//    ret_bufflen= ADE_Fft_SetBufflength(p_fft,buff_len);
//     ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_Configure_bufflength,ret_bufflen);
//
//    return ADE_RET_SUCCESS;
//}

//
//ADE_API_RET_T ADE_Fft_Configure_inout(ADE_FFT_T* p_fft,ADE_VOID_T *p_inbuff,ADE_VOID_T *p_outbuff)
//{
//
//    ADE_API_RET_T ret_in= ADE_RET_ERROR;
//    ADE_API_RET_T ret_out= ADE_RET_ERROR;
//
//     ret_in=ADE_Fft_SetInBuff(p_fft,p_inbuff);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_Configure_inout,ret_in);
//    ret_out=ADE_Fft_SetOutBuff(p_fft,p_outbuff);
//    ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_Configure_inout,ret_out);
//
//    return ADE_RET_SUCCESS;
//}

static ADE_API_RET_T ADE_Fft_Configure_params(ADE_FFT_T* p_fft,ADE_FFT_TYPE_T fft_type, ADE_FFT_DIRECTION_T fft_dir,ADE_VOID_T *p_inbuff,ADE_VOID_T *p_outbuff,ADE_INT32_T buff_len)
{

    ADE_API_RET_T plan_ret= ADE_RET_ERROR;
    ADE_API_RET_T ret_set= ADE_RET_ERROR;
     ADE_API_RET_T ret_bufflen= ADE_RET_ERROR;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_Configure_params,p_fft);

    ret_set=ADE_Fft_SetType(p_fft,fft_type);
    ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_Configure,ret_set);

     ret_bufflen= ADE_Fft_SetBufflength(p_fft,buff_len);
     ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_Configure_params,ret_bufflen);

     ret_set=ADE_Fft_SetInBuff(p_fft,p_inbuff);
     ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_Configure,ret_set);
     ret_set=ADE_Fft_SetOutBuff(p_fft,p_outbuff);
     ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_Configure,ret_set);

    #if (ADE_FFT_IMP==ADE_USE_FFTW)
          #if (ADE_FFTW_NTHREADS>0)
				#if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
                fftw_plan_with_nthreads(ADE_FFTW_NTHREADS);
				#elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
				fftwf_plan_with_nthreads(ADE_FFTW_NTHREADS);
				#else
         			#error (ADE_FP_PRECISION in ADE_Fft_Configure)
        		#endif
          #endif
      #endif

    ret_set=ADE_Fft_SetDirection(p_fft,fft_dir);
    ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_Configure_params,ret_set);


    plan_ret=ADE_Fft_SetPlan(p_fft);
    ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_Configure_params,plan_ret);


     return ADE_RET_SUCCESS;


}


/********** Processing Methods *****************/
ADE_API_RET_T ADE_Fft_Step(ADE_FFT_T* p_fft)
{

    ADE_FLOATING_T scale=0.0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_Step,p_fft);

    #if (ADE_FFT_IMP==ADE_USE_FFTW)

        #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
        fftw_execute(p_fft->plan);
        #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
       fftwf_execute(p_fft->plan);
        #else
         #error (ADE_FP_PRECISION in ADE_Fft_Step)
        #endif



    #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)


        if (p_fft->type==ADE_FFT_C2C)
        {
            #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
                 vDSP_fft_zopD ( p_fft->p_setup, &(p_fft->split_in), 1, &(p_fft->split_out), 1, log2(p_fft->buff_len), p_fft->direction );
                 vDSP_ztocD (&p_fft->split_out,1,(ADE_DSPDoubleComplex*) p_fft->p_out,2,p_fft->buff_len);
            #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
                vDSP_fft_zop ( p_fft->p_setup, &(p_fft->split_in), 1, &(p_fft->split_out), 1, log2(p_fft->buff_len), p_fft->direction );
            vDSP_ztoc (&p_fft->split_out,1,(ADE_DSPComplex*) p_fft->p_out,2,p_fft->buff_len);
                #else
                    #error(ADE_FP_PRECISION in ADE_Fft_Step)
                #endif
        }
        else if (p_fft->type==ADE_FFT_R2C)
        {
             scale = (ADE_FLOATING_T) 1.0 / (2);
        #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
            vDSP_fft_zropD ( p_fft->p_setup, &(p_fft->split_in), 1, &(p_fft->split_out), 1, log2(p_fft->buff_len), p_fft->direction );
            vDSP_vsmulD(p_fft->split_out.realp, 1, &scale, p_fft->split_out.realp, 1, p_fft->buff_len/2);
            vDSP_vsmulD(p_fft->split_out.imagp, 1, &scale, p_fft->split_out.imagp, 1, p_fft->buff_len/2);
            vDSP_ztocD (&p_fft->split_out,1,(ADE_DSPDoubleComplex*) p_fft->p_out,2,p_fft->buff_len/2);
        #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
            vDSP_fft_zrop ( p_fft->p_setup, &(p_fft->split_in), 1, &(p_fft->split_out), 1, log2(p_fft->buff_len), p_fft->direction );
            vDSP_vsmul(p_fft->split_out.realp, 1, &scale, p_fft->split_out.realp, 1, p_fft->buff_len/2);
            vDSP_vsmul(p_fft->split_out.imagp, 1, &scale, p_fft->split_out.imagp, 1, p_fft->buff_len/2);
            vDSP_ztoc (&p_fft->split_out,1,(ADE_DSPComplex*)p_fft->p_out,2,p_fft->buff_len/2);
        #else
            #error(ADE_FP_PRECISION in ADE_Fft_Step)
        #endif
        }




    #else
         #error(ADE_FFT_IMP in ADE_Fft_Step);

    #endif

    return ADE_RET_SUCCESS;

}

//#if (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
//ADE_API_RET_T ADE_Fft_FillSplitIn(ADE_FFT_T* p_fft,ADE_FLOATING_T real,ADE_FLOATING_T imag,ADE_UINT32_T idx)
//{
//
//    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_FillSplitIn,p_fft);
//
//    if (p_fft->type==ADE_FFT_C2C || p_fft->type==ADE_FFT_C2R)
//    {
//        ADE_Utils_FillSplitCplx(real,imag,idx,&(p_fft->split_in));
//    }
//    else if (p_fft->type==ADE_FFT_R2C || p_fft->type==ADE_FFT_R2R)
//
//    {
//        //ADE_Utils_FillSplitReal(real,idx,&(p_fft->split_in));
//
//        /////it seems that for real fft accel_framework still want a fake complex type made by real value ///*splitted
//
//        if (idx%2==0)
//        {
//         ADE_Utils_FillSplitReal(real,idx/2,&(p_fft->split_in));
//        }
//        else
//        {
//
//            ADE_Utils_FillSplitImag(real,idx/2,&(p_fft->split_in));
//        }
//    }
//
//    return ADE_RET_SUCCESS;
//}
//
//ADE_API_RET_T ADE_Fft_FillSplitOut(ADE_FFT_T* p_fft,ADE_FLOATING_T real,ADE_FLOATING_T imag,ADE_UINT32_T idx)
//{
//
//    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_FillSplitOut,p_fft);
//
//    if (p_fft->type==ADE_FFT_C2C || p_fft->type==ADE_FFT_R2C)
//    {
//        ADE_Utils_FillSplitCplx(real,imag,idx,&(p_fft->split_out));
//    }
//    else if (p_fft->type==ADE_FFT_C2R || p_fft->type==ADE_FFT_R2R)
//
//    {
//        ADE_Utils_FillSplitReal(real,idx,&(p_fft->split_out));
//    }
//
//    return ADE_RET_SUCCESS;
//}
//
//#endif



/******************** Private Methods *****************************/

static ADE_API_RET_T ADE_Fft_SetBufflength(ADE_FFT_T* p_fft,ADE_INT32_T buff_len)
{
    ADE_INT32_T val0=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_SetBufflength,p_fft);
    ADE_CHECK_INTERVAL_G_MIN_LE_MAX(ADE_CLASS_FFT,ADE_METHOD_SetBufflength,buff_len,"%d",val0,p_fft->max_buff_len);


    p_fft->buff_len=buff_len;

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Fft_SetInBuff(ADE_FFT_T* p_fft,ADE_VOID_T *p_inbuff)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_SetInBuff,p_fft);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_SetInBuff,p_inbuff);

    #if (ADE_FFT_IMP==ADE_USE_FFTW)
    p_fft->p_in=p_inbuff;
    #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
    p_fft->p_in=p_inbuff;

     #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
    if (p_fft->type==ADE_FFT_C2C)
    {
        ADE_Utils_SetSplit(p_fft->p_split_buff_in,p_fft->buff_len,&(p_fft->split_in));
        vDSP_ctozD((ADE_DSPDoubleComplex *) p_inbuff, 2, &p_fft->split_in, 1, p_fft->buff_len);
    }
    else if (p_fft->type==ADE_FFT_R2C)
    {
        ADE_Utils_SetSplit(p_fft->p_split_buff_in,p_fft->buff_len/2,&(p_fft->split_in));
        vDSP_ctozD((ADE_DSPDoubleComplex *) p_inbuff, 2, &p_fft->split_in, 1, p_fft->buff_len/2);
    }
    else
    {
        fprintf(ADE_STDERR_STREAM,"Value p_fft->type = %d not handled in ADE_Fft_SetInBuff\n",p_fft->type) ;
    }


    #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
    if (p_fft->type==ADE_FFT_C2C)
    {
         ADE_Utils_SetSplit(p_fft->p_split_buff_in,p_fft->buff_len,&(p_fft->split_in));
        vDSP_ctoz((ADE_DSPComplex *) p_inbuff, 2, &p_fft->split_in, 1, p_fft->buff_len);
    }
    else if (p_fft->type==ADE_FFT_R2C)
    {
        ADE_Utils_SetSplit(p_fft->p_split_buff_in,p_fft->buff_len/2,&(p_fft->split_in));
       vDSP_ctoz((ADE_DSPComplex *) p_inbuff, 2, &p_fft->split_in, 1, p_fft->buff_len/2);
    }
    else
    {
        fprintf(ADE_STDERR_STREAM,"Value p_fft->type = %d not handled in ADE_Fft_SetInBuff\n",p_fft->type) ;
    }
#endif

    #endif

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Fft_SetOutBuff(ADE_FFT_T* p_fft,ADE_VOID_T *p_outbuff)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_SetOutBuff,p_fft);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_SetOutBuff,p_outbuff);

#if (ADE_FFT_IMP==ADE_USE_FFTW)
    p_fft->p_out=p_outbuff;
#elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
    p_fft->p_out=p_outbuff;

#if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
    if (p_fft->type==ADE_FFT_C2C)
    {
        ADE_Utils_SetSplit(p_fft->p_split_buff_out,p_fft->buff_len,&(p_fft->split_out));
        vDSP_ctozD((ADE_DSPDoubleComplex *) p_outbuff, 2, &p_fft->split_out, 1, p_fft->buff_len);
    }
    else if (p_fft->type==ADE_FFT_R2C)
    {
        ADE_Utils_SetSplit(p_fft->p_split_buff_out,p_fft->buff_len/2,&(p_fft->split_out));
        vDSP_ctozD((ADE_DSPDoubleComplex *) p_outbuff, 2, &p_fft->split_out, 1, p_fft->buff_len/2);
    }
    else
    {
        fprintf(ADE_STDERR_STREAM,"Value p_fft->type = %d not handled in ADE_Fft_SetInBuff\n",p_fft->type) ;
    }


#elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
    if (p_fft->type==ADE_FFT_C2C)
    {
        ADE_Utils_SetSplit(p_fft->p_split_buff_out,p_fft->buff_len,&(p_fft->split_out));
       vDSP_ctoz((ADE_DSPComplex *) p_outbuff, 2, &p_fft->split_out, 1, p_fft->buff_len);
    }
    else if (p_fft->type==ADE_FFT_R2C)
    {
        ADE_Utils_SetSplit(p_fft->p_split_buff_out,p_fft->buff_len/2,&(p_fft->split_out));
        vDSP_ctoz((ADE_DSPComplex *) p_outbuff, 2, &p_fft->split_out, 1, p_fft->buff_len/2);
    }
    else
    {
        fprintf(ADE_STDERR_STREAM,"Value p_fft->type = %d not handled in ADE_Fft_SetOutBuff\n",p_fft->type) ;
    }
#endif

#endif

 return ADE_RET_SUCCESS;
}



static ADE_API_RET_T ADE_Fft_SetDirection(ADE_FFT_T* p_fft,ADE_FFT_DIRECTION_T fft_dir)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_SetDirection,p_fft);

    if (fft_dir!=ADE_FFT_FORWARD && fft_dir!=ADE_FFT_BACKWARD)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FFT,ADE_METHOD_SetDirection,fft_dir,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    else if (p_fft->type==ADE_FFT_R2C && fft_dir==ADE_FFT_BACKWARD)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FFT,ADE_METHOD_SetDirection,fft_dir,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    else if (p_fft->type==ADE_FFT_C2R && fft_dir==ADE_FFT_FORWARD)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FFT,ADE_METHOD_SetDirection,fft_dir,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    #if (ADE_FFT_IMP==ADE_USE_FFTW)
    if (fft_dir==ADE_FFT_FORWARD)
    {
        p_fft->direction=ADE_FFTW_FORWARD;
    }
    else if (fft_dir==ADE_FFT_BACKWARD)
        {
            p_fft->direction=ADE_FFTW_BACKWARD;
        }
    #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
        if (fft_dir==ADE_FFT_FORWARD)
        {
             p_fft->direction=ADE_vDSP_DFT_FORWARD;
        }
        else if (fft_dir==ADE_FFT_BACKWARD)
        {
            p_fft->direction=ADE_vDSP_DFT_INVERSE;
        }


    #else

    #endif
return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Fft_SetType(ADE_FFT_T* p_fft,ADE_FFT_TYPE_T fft_type)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_SetType,p_fft);
    if (fft_type!=ADE_FFT_C2C && fft_type!=ADE_FFT_C2R && fft_type!=ADE_FFT_R2C && fft_type!=ADE_FFT_R2R)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FFT,ADE_METHOD_SetType,fft_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    p_fft->type=fft_type;
return ADE_RET_SUCCESS;
}
static ADE_API_RET_T ADE_Fft_SetPlan(ADE_FFT_T* p_fft)
{

    ADE_API_RET_T ret=ADE_RET_ERROR;
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_SetPlan,p_fft);
     ADE_FFTW_PLAN_T plan;

   ret=ADE_Fft_doSetPlan(p_fft->type,p_fft->buff_len,p_fft->p_in,p_fft->p_out,p_fft->direction,ADE_FFTW_PLAN_FLAGS,&plan);
   p_fft->plan=plan;
   ADE_CHECK_ADERETVAL(ADE_CLASS_FFT,ADE_METHOD_SetPlan,ret);


        return ADE_RET_SUCCESS;
}
static ADE_API_RET_T ADE_Fft_doSetPlan(ADE_FFT_TYPE_T type,ADE_INT32_T buff_len,ADE_VOID_T *p_in,ADE_VOID_T *p_out,ADE_FFTW_DIRECTION_T direction,ADE_INT32_T plan_flag,ADE_FFTW_PLAN_T* p_plan_o)
{
ADE_INT32_T val0 = 0,max_buff_len=ADE_FFT_MAX_BUFF_LEN;
 ADE_kFFTRadix_T Ios_radix=ADE_kFFTRadix2;
 ADE_FFT_TYPE_T type_list[3]={ADE_FFT_C2C,ADE_FFT_R2C,ADE_FFT_C2R};
 ADE_FFTW_DIRECTION_T direction_list[2]={ADE_FFTW_FORWARD,ADE_FFTW_BACKWARD};

ADE_CHECK_INTERVAL_G_MIN_LE_MAX(ADE_CLASS_FFT,ADE_METHOD_doSetPlan,buff_len,"%d",val0,max_buff_len);
ADE_CHECK_VALUE_LIST(ADE_CLASS_FFT,ADE_METHOD_doSetPlan,type,"%d",type_list,3);
ADE_CHECK_VALUE_LIST(ADE_CLASS_FFT,ADE_METHOD_doSetPlan,direction,"%d",direction_list,2);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_doSetPlan,p_in);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_doSetPlan,p_out);

/*** In-Place or not in-place is controlled just with pointers i.e for in-place set p_in=p_out externally**/

      #if (ADE_FFT_IMP==ADE_USE_FFTW)
        if (type==ADE_FFT_C2C)
        {

                #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
                    *p_plan_o = fftw_plan_dft_1d(buff_len,(ADE_FFTCPLX_T*)p_in,(ADE_FFTCPLX_T*)p_out,direction,ADE_FFTW_PLAN_FLAGS);
                #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
                    *p_plan_o = fftwf_plan_dft_1d(buff_len,(ADE_FFTCPLX_T*)p_in,(ADE_FFTCPLX_T*)p_out,direction,ADE_FFTW_PLAN_FLAGS);
                #else

                    #error(ADE_FP_PRECISION)

                #endif

        }
        else if (type==ADE_FFT_R2C)
        {

             #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
                *p_plan_o = fftw_plan_dft_r2c_1d(buff_len,(ADE_FLOATING_T*)p_in,(ADE_FFTCPLX_T*)p_out,ADE_FFTW_PLAN_FLAGS);
             #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
                 *p_plan_o = fftwf_plan_dft_r2c_1d(buff_len,(ADE_FLOATING_T*)p_in,(ADE_FFTCPLX_T*)p_out,ADE_FFTW_PLAN_FLAGS);
              #else

                #error(ADE_FP_PRECISION)

            #endif


        }
        else if (type==ADE_FFT_C2R)
        {

                #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
                    *p_plan_o = fftw_plan_dft_c2r_1d(buff_len,(ADE_FFTCPLX_T*)p_in,(ADE_FLOATING_T*)p_out,ADE_FFTW_PLAN_FLAGS);
                #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
                    *p_plan_o = fftwf_plan_dft_c2r_1d(buff_len,(ADE_FFTCPLX_T*)p_in,(ADE_FLOATING_T*)p_out,ADE_FFTW_PLAN_FLAGS);
                #else
                    #error(ADE_FP_PRECISION)

                #endif
        }
        else
        {
           ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_FFT,ADE_METHOD_SetPlan,type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }


        #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)

            #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

                p_setup=vDSP_create_fftsetup ( (ADE_vDSP_Length) ceil(log2(buff_len)), Ios_radix );

            #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

                p_setup=vDSP_create_fftsetupD ( (ADE_vDSP_Length) ceil(log2(buff_len)), Ios_radix );

            #else
              #error(ADE_FP_PRECISION)

            #endif

        ADE_CHECK_INPUTPOINTER(ADE_CLASS_FFT,ADE_METHOD_doSetPlan, p_setup);

        #else

        #error(ADE_FFT_IMP)

        #endif

 return ADE_RET_SUCCESS;

}
