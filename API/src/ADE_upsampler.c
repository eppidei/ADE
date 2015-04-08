#include "headers/ADE_upsampler.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_Error_Handler.h"
#include <stdlib.h>

static ADE_API_RET_T ADE_Upsampler_doStep_blas(ADE_blas_level1_T *p_b1);
static ADE_API_RET_T ADE_Upsampler_doStep_custom (ADE_UINT32_T in_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_UINT32_T upfact);
static ADE_API_RET_T ADE_Upsampler_doStep(ADE_UPSAMPLER_T *p_upsampler);
static ADE_API_RET_T ADE_Upsampler_Config_blas_memcpy(ADE_UPSAMPLER_T *p_upsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff);

/*************** ADE_METHOD_Init Methods ****************/

ADE_API_RET_T ADE_Upsampler_Init(ADE_UPSAMPLER_T **dp_upsampler,ADE_UINT32_T in_buff_len,ADE_UINT32_T upfact)
{
    ADE_UPSAMPLER_T *p_this;
    ADE_API_RET_T ret_blas=ADE_RET_ERROR;

    p_this=calloc(1,sizeof(ADE_UPSAMPLER_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_UPSAMPLER,ADE_METHOD_Init,p_this);


        p_this->up_fact=upfact;
        p_this->in_buff_len=in_buff_len;
        p_this->out_buff_len=upfact*in_buff_len;


        ret_blas=ADE_Blas_level1_Init(&p_this->p_blas_l1_memcpy,ADE_REAL);
        ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,ADE_METHOD_Init,ret_blas);

        *dp_upsampler=p_this;

    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_Upsampler_Release(ADE_UPSAMPLER_T *p_upsampler)
{
    ADE_Blas_level1_Release(p_upsampler->p_blas_l1_memcpy);
    ADE_CHECKNFREE(p_upsampler);
}
/*************** Set Methods ****************/
ADE_API_RET_T ADE_Upsampler_SetInBuff(ADE_UPSAMPLER_T *p_upsampler,ADE_FLOATING_T *p_buff)
{
    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;


   ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,ADE_METHOD_SetInBuff,p_upsampler);
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,ADE_METHOD_SetInBuff,p_buff);

    /* Do */

    p_upsampler->p_in=p_buff;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Upsampler_SetOutBuff(ADE_UPSAMPLER_T *p_upsampler,ADE_FLOATING_T *p_buff, ADE_SIZE_T buff_size)
{

    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,ADE_METHOD_SetOutBuff,p_upsampler);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,ADE_METHOD_SetOutBuff,p_buff);

    if (buff_size!=(p_upsampler->out_buff_len*sizeof(ADE_FLOATING_T)))
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_UPSAMPLER,ADE_METHOD_SetOutBuff,buff_size,"%u",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    p_upsampler->p_out=p_buff;


    return ADE_RET_SUCCESS;

}

/***************** Config Methods **************/
ADE_API_RET_T ADE_Upsampler_Configure(ADE_UPSAMPLER_T *p_upsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff, ADE_SIZE_T out_buff_size)
{
    ADE_API_RET_T ret_in=ADE_RET_ERROR;
    ADE_API_RET_T ret_out=ADE_RET_ERROR;
    ADE_API_RET_T ret_conf=ADE_RET_ERROR;

     ret_in=ADE_Upsampler_SetInBuff(p_upsampler,p_inbuff);
     ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,ADE_METHOD_Configure,ret_in);

     ret_out=ADE_Upsampler_SetOutBuff(p_upsampler,p_outbuff, out_buff_size);
     ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,ADE_METHOD_Configure,ret_out);

     ret_conf=ADE_Upsampler_Config_blas_memcpy(p_upsampler,p_inbuff,p_outbuff);
     ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,ADE_METHOD_Configure,ret_conf);

     return ADE_RET_SUCCESS;
}
/****************** Processing Methods ************************/
ADE_API_RET_T ADE_Upsampler_Step(ADE_UPSAMPLER_T *p_upsampler)
{

    ADE_API_RET_T ret_pure=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,ADE_METHOD_Step,p_upsampler);

    ret_pure = ADE_Upsampler_doStep(p_upsampler);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,ADE_METHOD_Step,ret_pure);


    return ADE_RET_SUCCESS;


}

/********************* Static Methods *******************************/


static ADE_API_RET_T ADE_Upsampler_Config_blas_memcpy(ADE_UPSAMPLER_T *p_upsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff)
{

    ADE_API_RET_T ret_blas_N=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas_INCX=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas_INCY=ADE_RET_ERROR;
     ADE_API_RET_T ret_blas_Y=ADE_RET_ERROR;
     ADE_API_RET_T ret_blas_X=ADE_RET_ERROR;

      /* Input Check */

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,ADE_METHOD_Config_blas_memcpy,p_upsampler);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,ADE_METHOD_Config_blas_memcpy,p_inbuff);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,ADE_METHOD_Config_blas_memcpy,p_outbuff);

    /* Do */
    ret_blas_X=ADE_Blas_level1_SetX(p_upsampler->p_blas_l1_memcpy,p_inbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,ADE_METHOD_SetInBuff,ret_blas_X);

      ret_blas_Y=ADE_Blas_level1_SetY(p_upsampler->p_blas_l1_memcpy,p_outbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,ADE_METHOD_SetOutBuff,ret_blas_Y);


    ret_blas_INCY=ADE_Blas_level1_SetINCY(p_upsampler->p_blas_l1_memcpy,p_upsampler->up_fact);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,ADE_METHOD_Config_blas_memcpy,ret_blas_INCY);

    ret_blas_INCX=ADE_Blas_level1_SetINCX(p_upsampler->p_blas_l1_memcpy,1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,ADE_METHOD_Config_blas_memcpy,ret_blas_INCX);

    ret_blas_N=ADE_Blas_level1_SetN(p_upsampler->p_blas_l1_memcpy,p_upsampler->in_buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,ADE_METHOD_Config_blas_memcpy,ret_blas_N);

        return ADE_RET_SUCCESS;
}


static ADE_API_RET_T ADE_Upsampler_doStep(ADE_UPSAMPLER_T *p_upsampler)
{
    ADE_API_RET_T ret_up=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,ADE_METHOD_Pure,p_upsampler);

    #if (ADE_UPSAMPLER_IMP==ADE_UPSAMPLER_USE_BLAS)

         ret_up=ADE_Upsampler_doStep_blas(p_upsampler->p_blas_l1_memcpy);

    #elif (ADE_UPSAMPLER_IMP==ADE_UPSAMPLER_USE_CUSTOM)

        ret_up= ADE_Upsampler_doStep_custom (p_upsampler->in_buff_len,p_upsampler->p_out,p_upsampler->p_in,p_upsampler->up_fact);

    #else

        #error ADE_UPSAMPLER_IMP in ADE_Upsampler_Pure
    #endif

    ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,ADE_METHOD_doStep,ret_up);

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Upsampler_doStep_blas(ADE_blas_level1_T *p_b1)
{

ADE_API_RET_T ret=ADE_RET_ERROR;
ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,ADE_METHOD_doStep_blas,p_b1);

     ret=ADE_Blas_level1_copy(p_b1);
     ADE_CHECK_ADERETVAL(ADE_CLASS_UPSAMPLER,ADE_METHOD_doStep_blas,ret);

     return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Upsampler_doStep_custom (ADE_UINT32_T in_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_UINT32_T upfact)

{

    ADE_UINT32_T i=0;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,ADE_METHOD_doStep_custom,p_out);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_UPSAMPLER,ADE_METHOD_doStep_custom,p_in);

        for (i=0;i<in_buff_len;i++)
        {
            p_out[i*upfact]=p_in[i];

        }

    return ADE_RET_SUCCESS;
}

