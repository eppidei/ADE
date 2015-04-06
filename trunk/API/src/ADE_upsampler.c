#include "headers/ADE_upsampler.h"
#include "headers/ADE_fir.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_Error_Handler.h"
#include <stdlib.h>

static ADE_API_RET_T ADE_Upsampler_doPure_blas(ADE_blas_level1_T *p_b1);
static ADE_API_RET_T ADE_Upsampler_doPure_custom (ADE_UINT32_T in_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_UINT32_T upfact);
static ADE_API_RET_T ADE_Upsampler_Pure(ADE_UPSAMPLER_T *p_upsampler);
static ADE_API_RET_T ADE_Upsampler_Fir(ADE_UPSAMPLER_T *p_upsampler);

/*************** Init Methods ****************/

ADE_API_RET_T ADE_Upsampler_Init(ADE_UPSAMPLER_T **dp_upsampler,ADE_UINT32_T in_buff_len,ADE_UINT32_T upfact,ADE_UINT32_T filter_order,
ADE_FIR_IMP_CHOICE_T fir_type, ADE_IIR_IMP_CHOICE_T iir_type,ADE_UINT32_T n_iir_sections,ADE_UPSAMP_CHOICE_T up_type)
{
    ADE_UPSAMPLER_T *p_this;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas=ADE_RET_ERROR;



    p_this=calloc(1,sizeof(ADE_UPSAMPLER_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_UPSAMPLER,Init,p_this);


        p_this->up_fact=upfact;
         p_this->up_samp_type=up_type;
         p_this->filter_order=0;
         p_this->in_buff_len=in_buff_len;
         p_this->n_iir_sections=0;

        if (up_type==ADE_UPSAMP_FIR)
        {
            p_this->filter_order=filter_order;
            ret_fir=ADE_Fir_Init(&p_this->p_fir,filter_order,in_buff_len,fir_type);
            ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Init,ret_fir);

            p_this->p_temp_buff=calloc(in_buff_len*upfact,sizeof(ADE_FLOATING_T));
            ADE_CHECK_MEMALLOC(ADE_CLASS_UPSAMPLER,Init,p_this->p_temp_buff);
        }
        else if (up_type==ADE_UPSAMP_IIR)
        {
            p_this->filter_order=filter_order;
            p_this->n_iir_sections=n_iir_sections;
            ret_iir=ADE_Iir_Init(&p_this->p_iir,filter_order,in_buff_len,iir_type);
            ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Init,ret_iir);
        }

        ret_blas=ADE_Blas_level1_Init(&p_this->p_blas_l1_memcpy,ADE_REAL);
        ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Init,ret_blas);

        *dp_upsampler=p_this;

    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_Upsampler_Release(ADE_UPSAMPLER_T *p_upsampler)
{
    if (p_upsampler->up_samp_type==ADE_UPSAMP_FIR)
    {
        ADE_Fir_Release(p_upsampler->p_fir);
        ADE_CHECKNFREE(p_upsampler->p_temp_buff);
    }
    else if (p_upsampler->up_samp_type==ADE_UPSAMP_IIR)
    {

        ADE_Iir_Release(p_upsampler->p_iir);
    }

    ADE_Blas_level1_Release(p_upsampler->p_blas_l1_memcpy);
    ADE_CHECKNFREE(p_upsampler);
}
/*************** Set Methods ****************/
ADE_API_RET_T ADE_Upsampler_SetInBuff(ADE_UPSAMPLER_T *p_upsampler,ADE_FLOATING_T *p_buff)
{
    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,SetInBuff,p_upsampler);
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,SetInBuff,p_buff);

    /* Do */

    p_upsampler->p_in=p_buff;

    ret_blas1=ADE_Blas_level1_SetX(p_upsampler->p_blas_l1_memcpy,p_buff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,SetInBuff,ret_blas1);

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Upsampler_SetOutBuff(ADE_UPSAMPLER_T *p_upsampler,ADE_FLOATING_T *p_buff, ADE_SIZE_T buff_size)
{

    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,SetOutBuff,p_upsampler);
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,SetOutBuff,p_buff);

    if (buff_size!=(p_upsampler->in_buff_len*sizeof(ADE_FLOATING_T)))
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UPSAMPLER,SetOutBuff,buff_size,"%u",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;//49;
    }

    p_upsampler->p_out=p_buff;

     if (p_upsampler->up_samp_type==ADE_UPSAMP_PURE)
     {

        ret_blas1=ADE_Blas_level1_SetY(p_upsampler->p_blas_l1_memcpy,p_buff);
        ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,SetOutBuff,ret_blas1);

      } else if (p_upsampler->up_samp_type==ADE_UPSAMP_FIR)
      {

        ret_fir = ADE_Fir_SetOutBuff(p_upsampler->p_fir, p_buff);
        ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,SetOutBuff,ret_fir);
      }

    return ADE_RET_SUCCESS;

}

/***************** Config Methods ***************************/

ADE_API_RET_T ADE_Upsampler_Config_blas_memcpy(ADE_UPSAMPLER_T *p_upsampler)
{

    ADE_API_RET_T ret_blas_N=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas_INCX=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas_INCY=ADE_RET_ERROR;
     ADE_API_RET_T ret_blas_Y=ADE_RET_ERROR;

      /* Input Check */

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,Config_blas_memcpy,p_upsampler);

    /* Do */

    ret_blas_INCY=ADE_Blas_level1_SetINCY(p_upsampler->p_blas_l1_memcpy,p_upsampler->up_fact);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Config_blas_memcpy,ret_blas_INCY);

    ret_blas_INCX=ADE_Blas_level1_SetINCX(p_upsampler->p_blas_l1_memcpy,1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Config_blas_memcpy,ret_blas_INCX);

    ret_blas_N=ADE_Blas_level1_SetN(p_upsampler->p_blas_l1_memcpy,p_upsampler->in_buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Config_blas_memcpy,ret_blas_N);

     if (p_upsampler->up_samp_type==ADE_UPSAMP_FIR)
     {
        ret_blas_Y=ADE_Blas_level1_SetY(p_upsampler->p_blas_l1_memcpy,p_upsampler->p_temp_buff);
        ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Config_blas_memcpy,ret_blas_Y);
     }

        return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Upsampler_Config_fir(ADE_UPSAMPLER_T *p_upsampler,ADE_FLOATING_T *p_num,ADE_FLOATING_T *p_out,ADE_SIZE_T out_size)
{
    ADE_API_RET_T ret_firnum=ADE_RET_ERROR;
    ADE_API_RET_T ret_firbuf=ADE_RET_ERROR;

    /* Input Check */
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,Config_fir,p_upsampler);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,Config_fir,p_num);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,Config_fir,p_out);


    if (p_upsampler->up_samp_type!=ADE_UPSAMP_FIR)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UPSAMPLER,Config_fir,p_upsampler->up_samp_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    /* Do */

    ret_firnum = ADE_Fir_setNum(p_upsampler->p_fir, p_num);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Config_fir,ret_firnum);

    ret_firbuf = ADE_Fir_SetInBuff(p_upsampler->p_fir, p_upsampler->p_temp_buff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Config_fir,ret_firbuf);

    return ADE_RET_SUCCESS;
}
/****************** Processing Methods ************************/
ADE_API_RET_T ADE_Upsampler_Step(ADE_UPSAMPLER_T *p_upsampler)
{

    ADE_API_RET_T ret_pure=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,Step,p_upsampler);

    if (p_upsampler->up_samp_type==ADE_UPSAMP_PURE)
    {
         ret_pure = ADE_Upsampler_Pure(p_upsampler);
         ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Step,ret_pure);
    }
    else if (p_upsampler->up_samp_type==ADE_UPSAMP_FIR)
    {
        ret_fir = ADE_Upsampler_Fir(p_upsampler);
        ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Step,ret_fir);

    }

    return ADE_RET_SUCCESS;


}

/********************* Static Methods *******************************/

static ADE_API_RET_T ADE_Upsampler_Pure(ADE_UPSAMPLER_T *p_upsampler)
{
    ADE_API_RET_T ret_up=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,Pure,p_upsampler);

    #if (ADE_UPSAMPLER_IMP==ADE_UPSAMPLER_USE_BLAS)

         ret_up=ADE_Upsampler_doPure_blas(p_upsampler->p_blas_l1_memcpy);

    #elif (ADE_UPSAMPLER_IMP==ADE_UPSAMPLER_USE_CUSTOM)

        ret_up= ADE_Upsampler_doPure_custom (p_upsampler->in_buff_len,p_upsampler->p_out,p_upsampler->p_in,p_upsampler->up_fact);

    #else

        #error ADE_UPSAMPLER_IMP in ADE_Upsampler_Pure
    #endif

    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Pure,ret_up);

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Upsampler_doPure_blas(ADE_blas_level1_T *p_b1)
{

ADE_API_RET_T ret=ADE_RET_ERROR;
ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,doPure_blas,p_b1);

     ret=ADE_Blas_level1_copy(p_b1);
     ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,doPure_blas,ret);

     return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Upsampler_doPure_custom (ADE_UINT32_T in_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_UINT32_T upfact)

{

    ADE_UINT32_T i=0;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,doPure_custom,p_out);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,doPure_custom,p_in);

        for (i=0;i<in_buff_len;i++)
        {
            p_out[i*upfact]=p_in[i];

        }

    return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Upsampler_Fir(ADE_UPSAMPLER_T *p_upsampler)
{

    ADE_API_RET_T ret_up=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,Fir,p_upsampler);

    ret_up=ADE_Upsampler_Pure(p_upsampler);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Fir,ret_up);

    ret_fir=ADE_Fir_Step(p_upsampler->p_fir);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,Fir,ret_fir);

    return ADE_RET_SUCCESS;
}