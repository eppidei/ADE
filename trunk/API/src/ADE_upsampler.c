#include "headers/ADE_upsampler.h"
#include "headers/ADE_fir.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_Error_Handler.h"
#include <stdlib.h>

static ADE_API_RET_T ADE_Upsampler_doPure_blas(ADE_blas_level1_T *p_b1);

ADE_API_RET_T ADE_Upsampler_Init(ADE_UPSAMPLER_T **dp_upsampler,ADE_UINT32_T in_buff_len,ADE_UINT32_T upfact,ADE_UINT32_T filter_order,
ADE_FIR_IMP_CHOICE_T fir_type, ADE_IIR_IMP_CHOICE_T iir_type,ADE_UINT32_T n_iir_sections,ADE_UPSAMP_CHOICE_T up_type)
{
    ADE_UPSAMPLER_T *p_this;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas=ADE_RET_ERROR;
     ADE_API_RET_T ret_blas_N=ADE_RET_ERROR;
      ADE_API_RET_T ret_blas_INCX=ADE_RET_ERROR;
       ADE_API_RET_T ret_blas_INCY=ADE_RET_ERROR;

    p_this=calloc(1,sizeof(ADE_UPSAMPLER_T));

    if (p_this!=NULL)
    {
        p_this->up_fact=upfact;
         p_this->up_samp_type=up_type;
         p_this->filter_order=0;
         p_this->in_buff_len=in_buff_len;
         p_this->n_iir_sections=0;

        if (up_type==ADE_UPSAMP_FIR)
        {
            p_this->filter_order=filter_order;
            ret_fir=ADE_Fir_Init(&p_this->p_fir,filter_order,in_buff_len,fir_type);
            if (ret_fir==ADE_RET_ERROR)
            {
                ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_UPSAMPLER,Init,ret_fir,"%p",(FILE*)ADE_STD_STREAM);
                return ADE_RET_ERROR;
            }
        }
        else if (up_type==ADE_UPSAMP_IIR)
        {
            p_this->filter_order=filter_order;
            p_this->n_iir_sections=n_iir_sections;
            ret_iir=ADE_Iir_Init(&p_this->p_iir,filter_order,in_buff_len,iir_type);
            if (ret_iir==ADE_RET_ERROR)
            {
                ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_UPSAMPLER,Init,ret_iir,"%p",(FILE*)ADE_STD_STREAM);
                return ADE_RET_ERROR;
            }
        }

        ret_blas=ADE_Blas_level1_Init(&p_this->p_blas_l1,ADE_REAL);
        if (ret_blas==ADE_RET_ERROR)
        {
                ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_UPSAMPLER,Init,ret_blas,"%p",(FILE*)ADE_STD_STREAM);
                return ADE_RET_ERROR;
        }
        ret_blas_INCY=ADE_Blas_level1_SetINCY(p_this->p_blas_l1,p_this->up_fact);
        if (ret_blas_INCY==ADE_RET_ERROR)
        {
                ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_UPSAMPLER,Init,ret_blas_INCY,"%p",(FILE*)ADE_STD_STREAM);
                return ADE_RET_ERROR;
        }
        ret_blas_INCX=ADE_Blas_level1_SetINCX(p_this->p_blas_l1,1);
        if (ret_blas_INCX==ADE_RET_ERROR)
        {
                ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_UPSAMPLER,Init,ret_blas_INCX,"%p",(FILE*)ADE_STD_STREAM);
                return ADE_RET_ERROR;
        }
        ret_blas_N=ADE_Blas_level1_SetN(p_this->p_blas_l1,p_this->in_buff_len);
        if (ret_blas_N==ADE_RET_ERROR)
        {
                ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_UPSAMPLER,Init,ret_blas_N,"%p",(FILE*)ADE_STD_STREAM);
                return ADE_RET_ERROR;
        }

        *dp_upsampler=p_this;
    }
    else
    {
       ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_UPSAMPLER,Init,p_this,"%p",(FILE*)ADE_STD_STREAM);
       return ADE_RET_ERROR;//47;
    }



    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_Upsampler_Release(ADE_UPSAMPLER_T *p_upsampler)
{
    if (p_upsampler->up_samp_type==ADE_UPSAMP_FIR)
    {
        ADE_Fir_Release(p_upsampler->p_fir);

    }
    else if (p_upsampler->up_samp_type==ADE_UPSAMP_IIR)
    {

        ADE_Iir_Release(p_upsampler->p_iir);
    }

    ADE_Blas_level1_Release(p_upsampler->p_blas_l1);
    ADE_CHECKNFREE(p_upsampler);
}

ADE_API_RET_T ADE_Upsampler_SetInBuff(ADE_UPSAMPLER_T *p_upsampler,ADE_FLOATING_T *p_buff)
{
    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;

    if (p_upsampler==NULL)
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UPSAMPLER,SetInBuff,p_upsampler,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }
    if (p_buff==NULL)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UPSAMPLER,SetInBuff,p_buff,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }

    p_upsampler->p_in=p_buff;

    ret_blas1=ADE_Blas_level1_SetX(p_upsampler->p_blas_l1,p_buff);

    if (ret_blas1==ADE_RET_ERROR)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_UPSAMPLER,SetInBuff,ret_blas1,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }


    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Upsampler_SetOutBuff(ADE_UPSAMPLER_T *p_upsampler,ADE_FLOATING_T *p_buff, ADE_SIZE_T buff_size)
{

    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;

    if (p_upsampler==NULL)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UPSAMPLER,SetOutBuff,p_upsampler,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }
    if (p_buff==NULL)
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UPSAMPLER,SetOutBuff,p_buff,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }
    if (buff_size!=(p_upsampler->in_buff_len*sizeof(ADE_FLOATING_T)))
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UPSAMPLER,SetOutBuff,buff_size,"%u",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }

    p_upsampler->p_out=p_buff;

    ret_blas1=ADE_Blas_level1_SetY(p_upsampler->p_blas_l1,p_buff);


    if (ret_blas1==ADE_RET_ERROR)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_UPSAMPLER,SetOutBuff,ret_blas1,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }


    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Upsampler_Pure(ADE_UPSAMPLER_T *p_upsampler)
{
    ADE_API_RET_T ret_up=ADE_RET_ERROR;

    #if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
        if (p_upsampler==NULL)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UPSAMPLER,Upsampler_Pure,p_upsampler,"%p",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }
    #endif

    #if (ADE_UPSAMPLER_IMP==ADE_UPSAMPLER_USE_BLAS)

         ret_up=ADE_Upsampler_doPure_blas(p_upsampler->p_blas_l1);

    #elif (ADE_UPSAMPLER_IMP==ADE_UPSAMPLER_USE_CUSTOM)

        ret_up= ADE_Upsampler_doPure_custom (p_upsampler->in_buff_len,p_upsampler->p_out,p_upsampler->p_in,p_upsampler->up_fact);

    #else

        #error ADE_UPSAMPLER_IMP in ADE_Upsampler_Pure
    #endif

 #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
         if (ret_up=ADE_RET_ERROR)
         {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_UPSAMPLER,Upsampler_Pure,ret_up,"%d",(FILE*)ADE_STD_STREAM);
             return ADE_RET_ERROR;
         }
        #endif




}

static ADE_API_RET_T ADE_Upsampler_doPure_blas(ADE_blas_level1_T *p_b1)
{

ADE_API_RET_T ret=ADE_RET_ERROR;
ADE_UINT32_T i=0;

     #if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
    if (p_b1==NULL)
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UPSAMPLER,Upsampler_doPure_blas,p_b1,"%p",(FILE*)ADE_STD_STREAM);
         return ADE_RET_ERROR;
    }
     #endif
     ret=ADE_Blas_level1_copy(p_b1);
     #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
     if (ret=ADE_RET_ERROR)
     {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_RETCHECKS,ADE_CLASS_UPSAMPLER,Upsampler_doPure_blas,p_b1,"%p",(FILE*)ADE_STD_STREAM);
         return ADE_RET_ERROR;
     }
    #endif
}

static ADE_API_RET_T ADE_Upsampler_doPure_custom (ADE_UINT32_T in_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_UINT32_T upfact)

{

    ADE_UINT32_T i=0;

#if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
    if (p_out==NULL)
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UPSAMPLER,Upsampler_doPure_custom,p_out,"%p",(FILE*)ADE_STD_STREAM);
         return ADE_RET_ERROR;
    }
     if (p_in==NULL)
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UPSAMPLER,Upsampler_doPure_custom,p_in,"%p",(FILE*)ADE_STD_STREAM);
         return ADE_RET_ERROR;
    }
     #endif


        for (i=0;i<in_buff_len;i++)
        {
            p_out[i*upfact]=p_in[i];

        }

    return ADE_RET_SUCCESS;
}

//static ADE_Upsampler_doFir(ADE_blas_level1_T *p_b1,ADE_FIR_T *p_fir)
//{
//
//}
