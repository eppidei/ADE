#include "headers/ADE_downsampler.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_Error_Handler.h"
#include <stdlib.h>

static ADE_API_RET_T ADE_Downsampler_doStep_blas(ADE_blas_level1_T *p_b1);
static ADE_API_RET_T ADE_Downsampler_doStep_custom (ADE_UINT32_T in_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_UINT32_T upfact);
static ADE_API_RET_T ADE_Downsampler_doStep(ADE_DOWNSAMPLER_T *p_downsampler);
static ADE_API_RET_T ADE_Downsampler_Config_blas_memcpy(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff);
static ADE_API_RET_T ADE_Downsampler_SetInBuff(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_buff);
static ADE_API_RET_T ADE_Downsampler_SetOutBuff(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_buff, ADE_SIZE_T buff_size);

/*************** ADE_METHOD_Init Methods ****************/

ADE_API_RET_T ADE_Downsampler_Init(ADE_DOWNSAMPLER_T **dp_downsampler,ADE_UINT32_T in_buff_len,ADE_UINT32_T downfact)
{
    ADE_DOWNSAMPLER_T *p_this;
    ADE_API_RET_T ret_blas=ADE_RET_ERROR;

    p_this=calloc(1,sizeof(ADE_DOWNSAMPLER_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Init,p_this);


        p_this->down_fact=downfact;
        p_this->in_buff_len=in_buff_len;
        if ( (in_buff_len%downfact)!=0)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Init,downfact,"%d",(FILE*)ADE_STD_STREAM);
            fprintf((FILE*)ADE_STD_STREAM,"ERROR : downfact %u is not an integer multiple of in buff len %u \n",downfact, in_buff_len);
            return ADE_RET_ERROR;
        }
        p_this->out_buff_len=in_buff_len/downfact;


        ret_blas=ADE_Blas_level1_Init(&p_this->p_blas_l1_memcpy,ADE_MATH_REAL);
        ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Init,ret_blas);

        *dp_downsampler=p_this;

    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_Downsampler_Release(ADE_DOWNSAMPLER_T *p_downsampler)
{
    ADE_Blas_level1_Release(p_downsampler->p_blas_l1_memcpy);
    ADE_CHECKNFREE(p_downsampler);
}
/*************** Set Methods ****************/

/***************** Config Methods **************/
ADE_API_RET_T ADE_Downsampler_Configure(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff, ADE_SIZE_T out_buff_size)
{
    ADE_API_RET_T ret_in=ADE_RET_ERROR;
    ADE_API_RET_T ret_out=ADE_RET_ERROR;
    ADE_API_RET_T ret_conf=ADE_RET_ERROR;

     ret_in=ADE_Downsampler_SetInBuff(p_downsampler,p_inbuff);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure,ret_in);

     ret_out=ADE_Downsampler_SetOutBuff(p_downsampler,p_outbuff, out_buff_size);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure,ret_out);

     ret_conf=ADE_Downsampler_Config_blas_memcpy(p_downsampler,p_inbuff,p_outbuff);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Configure,ret_conf);

     return ADE_RET_SUCCESS;
}
/****************** Processing Methods ************************/
ADE_API_RET_T ADE_Downsampler_Step(ADE_DOWNSAMPLER_T *p_downsampler)
{

    ADE_API_RET_T ret_pure=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Step,p_downsampler);

    ret_pure = ADE_Downsampler_doStep(p_downsampler);
    ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Step,ret_pure);


    return ADE_RET_SUCCESS;


}

/********************* Static Methods *******************************/
ADE_API_RET_T ADE_Downsampler_SetInBuff(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_buff)
{
    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas_conf=ADE_RET_ERROR;

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetInBuff,p_downsampler);
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetInBuff,p_buff);

    /* Do */

    p_downsampler->p_in=p_buff;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Downsampler_SetOutBuff(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_buff, ADE_SIZE_T buff_size)
{

    ADE_API_RET_T ret_blas1=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetOutBuff,p_downsampler);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetOutBuff,p_buff);

    if (buff_size!=(p_downsampler->out_buff_len*sizeof(ADE_FLOATING_T)))
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetOutBuff,buff_size,"%u",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    p_downsampler->p_out=p_buff;



    return ADE_RET_SUCCESS;

}


static ADE_API_RET_T ADE_Downsampler_Config_blas_memcpy(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff)
{

    ADE_API_RET_T ret_blas_N=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas_INCX=ADE_RET_ERROR;
    ADE_API_RET_T ret_blas_INCY=ADE_RET_ERROR;
     ADE_API_RET_T ret_blas_Y=ADE_RET_ERROR;
     ADE_API_RET_T ret_blas_X=ADE_RET_ERROR;

      /* Input Check */

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Config_blas_memcpy,p_downsampler);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Config_blas_memcpy,p_outbuff);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Config_blas_memcpy,p_inbuff);

    /* Do */

     ret_blas_X=ADE_Blas_level1_SetX(p_downsampler->p_blas_l1_memcpy,p_inbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetInBuff,ret_blas_X);

    ret_blas_Y=ADE_Blas_level1_SetY(p_downsampler->p_blas_l1_memcpy,p_outbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_SetOutBuff,ret_blas_Y);

    ret_blas_INCY=ADE_Blas_level1_SetINCY(p_downsampler->p_blas_l1_memcpy,1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Config_blas_memcpy,ret_blas_INCY);

    ret_blas_INCX=ADE_Blas_level1_SetINCX(p_downsampler->p_blas_l1_memcpy,p_downsampler->down_fact);
    ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Config_blas_memcpy,ret_blas_INCX);

    ret_blas_N=ADE_Blas_level1_SetN(p_downsampler->p_blas_l1_memcpy,p_downsampler->out_buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Config_blas_memcpy,ret_blas_N);

        return ADE_RET_SUCCESS;
}


static ADE_API_RET_T ADE_Downsampler_doStep(ADE_DOWNSAMPLER_T *p_downsampler)
{
    ADE_API_RET_T ret_up=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_Pure,p_downsampler);

    #if (ADE_DOWNSAMPLER_IMP==ADE_DOWNSAMPLER_USE_BLAS)

         ret_up=ADE_Downsampler_doStep_blas(p_downsampler->p_blas_l1_memcpy);

    #elif (ADE_DOWNSAMPLER_IMP==ADE_DOWNSAMPLER_USE_CUSTOM)

        ret_up= ADE_Downsampler_doStep_custom (p_downsampler->in_buff_len,p_downsampler->p_out,p_downsampler->p_in,p_downsampler->up_fact);

    #else

        #error ADE_DOWNSAMPLER_IMP in ADE_Downsampler_Pure
    #endif

    ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doStep,ret_up);

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Downsampler_doStep_blas(ADE_blas_level1_T *p_b1)
{

ADE_API_RET_T ret=ADE_RET_ERROR;
ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doPure_blas,p_b1);

     ret=ADE_Blas_level1_copy(p_b1);
     ADE_CHECK_ADERETVAL(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doPure_blas,ret);

     return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Downsampler_doStep_custom (ADE_UINT32_T out_buff_len,ADE_FLOATING_T *p_out,ADE_FLOATING_T *p_in,ADE_UINT32_T downfact)

{

    ADE_UINT32_T i=0;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doPure_custom,p_out);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_DOWNSAMPLER,ADE_METHOD_doPure_custom,p_in);

        for (i=0;i<out_buff_len;i++)
        {
            p_out[i]=p_in[i*downfact];

        }

    return ADE_RET_SUCCESS;
}

