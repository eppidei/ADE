#include "headers/ADE_interpolator.h"
#include "headers/ADE_upsampler.h"
#include "headers/ADE_fir.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_Error_Handler.h"
#include <stdlib.h>


/*************** Init Methods ****************/

ADE_API_RET_T ADE_Interpolator_Init(ADE_INTERPOLATOR_T **dp_interpolator,ADE_UINT32_T in_buff_len,ADE_UINT32_T upfact,ADE_UINT32_T filter_order,
ADE_INTERPOLATOR_FILTER_T filt_type, ADE_INTERPOLATOR_ARCH_T arch_type,ADE_UINT32_T n_iir_sections)
{
    ADE_INTERPOLATOR_T *p_this;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir=ADE_RET_ERROR;
    ADE_API_RET_T ret_upsamp=ADE_RET_ERROR;


    p_this=calloc(1,sizeof(ADE_INTERPOLATOR_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Init,p_this);


    p_this->up_fact=upfact;
    p_this->filter_order=filter_order;
    p_this->in_buff_len=in_buff_len;
    p_this->out_buff_len=upfact*in_buff_len;
    p_this->filt_type=filt_type;
    p_this->arch_type=arch_type;

    if (arch_type==ADE_INTERPOLATOR_ARCH_STANDARD)
    {
        ret_upsamp=ADE_Upsampler_Init(&(p_this->p_upsampler),in_buff_len,upfact);
        ADE_CHECK_ADERETVAL(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Init,ret_upsamp);

        p_this->p_temp_buffer=calloc( p_this->out_buff_len,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Init,p_this->p_temp_buffer);

        if (filt_type==ADE_INTERPOLATOR_FILTER_FIR)
        {
            p_this->dp_fir=(ADE_FIR_T**)calloc(1,sizeof(ADE_FIR_T*));
            ADE_CHECK_MEMALLOC(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Init,p_this->dp_fir);

            ret_fir=ADE_Fir_Init(&(p_this->dp_fir[0]),filter_order, p_this->out_buff_len,ADE_FIR_TRASP_II);/*qui dovrebbe essere meglio OVSA*/
            ADE_CHECK_ADERETVAL(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Init,ret_fir);
        }
        else if (filt_type==ADE_INTERPOLATOR_FILTER_IIR)
        {
             p_this->dp_iir=(ADE_IIR_T**)calloc(1,sizeof(ADE_IIR_T*));
            ADE_CHECK_MEMALLOC(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Init,p_this->dp_iir);

            ret_iir=ADE_Iir_Init(&(p_this->dp_iir[0]),n_iir_sections, p_this->out_buff_len,ADE_IIR_TRASP_II_B);/*qui dovrebbe essere meglio OVSA*/
            ADE_CHECK_ADERETVAL(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Init,ret_iir);
        }
        else
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_INTERPOLATOR,ADE_METHOD_Init,filt_type,"%d",(FILE*)ADE_STD_STREAM);
            fprintf((FILE*)ADE_STD_STREAM,"Unrecognized filt type \n");
            return ADE_RET_ERROR;
        }

    }
    else if (arch_type==ADE_INTERPOLATOR_ARCH_POLYPHASE)
    {
        if (filt_type==ADE_INTERPOLATOR_FILTER_FIR)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_INTERPOLATOR,ADE_METHOD_Init,filt_type,"%d",(FILE*)ADE_STD_STREAM);
            fprintf((FILE*)ADE_STD_STREAM,"Missing Implementation filt_type = %d \n",filt_type);
            return ADE_RET_ERROR;
        }
        else if (filt_type==ADE_INTERPOLATOR_FILTER_IIR)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_INTERPOLATOR,ADE_METHOD_Init,filt_type,"%d",(FILE*)ADE_STD_STREAM);
            fprintf((FILE*)ADE_STD_STREAM,"Missing Implementation filt_type = %d \n",filt_type);
            return ADE_RET_ERROR;
        }
        else
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_INTERPOLATOR,ADE_METHOD_Init,filt_type,"%d",(FILE*)ADE_STD_STREAM);
            fprintf((FILE*)ADE_STD_STREAM,"Unrecognized filt type \n");
             return ADE_RET_ERROR;
        }
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_INTERPOLATOR,ADE_METHOD_Init,arch_type,"%d",(FILE*)ADE_STD_STREAM);
         fprintf((FILE*)ADE_STD_STREAM,"Unrecognized arch type = %d \n",arch_type);
          return ADE_RET_ERROR;
    }

    *dp_interpolator=p_this;

    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_Interpolator_Release(ADE_INTERPOLATOR_T *p_interpolator)
{
    if (p_interpolator->arch_type==ADE_INTERPOLATOR_ARCH_STANDARD)
    {
        ADE_Upsampler_Release(p_interpolator->p_upsampler);
        ADE_CHECKNFREE(p_interpolator->p_temp_buffer);
        if (p_interpolator->filt_type==ADE_INTERPOLATOR_FILTER_FIR)
        {
            ADE_Fir_Release(p_interpolator->dp_fir[0]);
            ADE_CHECKNFREE(p_interpolator->dp_fir);
        }
        else if (p_interpolator->filt_type==ADE_INTERPOLATOR_FILTER_IIR)
        {
            ADE_Iir_Release(p_interpolator->dp_iir[0]);
            ADE_CHECKNFREE(p_interpolator->dp_iir);
        }
    }
     ADE_CHECKNFREE(p_interpolator);
}
/*************** Set Methods ****************/
ADE_API_RET_T ADE_Interpolator_SetInBuff(ADE_INTERPOLATOR_T *p_interpolator,ADE_FLOATING_T *p_buff)
{

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_INTERPOLATOR,ADE_METHOD_SetInBuff,p_interpolator);
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_INTERPOLATOR,ADE_METHOD_SetInBuff,p_buff);
    /* Do */

    p_interpolator->p_in=p_buff;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Interpolator_SetOutBuff(ADE_INTERPOLATOR_T *p_interpolator,ADE_FLOATING_T *p_buff, ADE_SIZE_T buff_size)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_INTERPOLATOR,ADE_METHOD_SetOutBuff,p_interpolator);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_INTERPOLATOR,ADE_METHOD_SetOutBuff,p_buff);

    if (buff_size!=(p_interpolator->out_buff_len*sizeof(ADE_FLOATING_T)))
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_INTERPOLATOR,ADE_METHOD_SetOutBuff,buff_size,"%u",(FILE*)ADE_STD_STREAM);
        fprintf((FILE*)ADE_STD_STREAM,"Interpolator Out Buffer size wrong expexted %u , actual %u \n",p_interpolator->out_buff_len*sizeof(ADE_FLOATING_T),buff_size);
        return ADE_RET_ERROR;
    }

    p_interpolator->p_out=p_buff;


    return ADE_RET_SUCCESS;

}

/***************** Config Methods ***************************/

ADE_API_RET_T ADE_Interpolator_Configure(ADE_INTERPOLATOR_T *p_interpolator,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff,
ADE_SIZE_T out_buff_size,ADE_FLOATING_T *p_num,ADE_FLOATING_T **dp_num,ADE_FLOATING_T **dp_denoms)
{

    ADE_API_RET_T ret_up=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir=ADE_RET_ERROR;
    ADE_API_RET_T ret_in=ADE_RET_ERROR;
    ADE_API_RET_T ret_out=ADE_RET_ERROR;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Configure,p_interpolator);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Configure,p_inbuff);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Configure,p_outbuff);

    ret_in=ADE_Interpolator_SetInBuff(p_interpolator,p_inbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Configure,ret_in);

        if (p_interpolator->arch_type==ADE_INTERPOLATOR_ARCH_STANDARD)
        {
            ret_up=ADE_Upsampler_Configure(p_interpolator->p_upsampler,p_inbuff,p_interpolator->p_temp_buffer, out_buff_size);
            ADE_CHECK_ADERETVAL(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Configure,ret_up);

            if (p_interpolator->filt_type==ADE_INTERPOLATOR_FILTER_FIR)
            {
                ADE_CHECK_INPUTPOINTER(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Configure,p_num);

                ret_fir=ADE_Fir_Configure(p_interpolator->dp_fir[0],p_num,p_interpolator->p_temp_buffer,p_outbuff);
                ADE_CHECK_ADERETVAL(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Configure,ret_fir);
            }
            else if (p_interpolator->filt_type==ADE_INTERPOLATOR_FILTER_IIR)
            {
                ADE_CHECK_INPUTPOINTER(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Configure,dp_num);
                ADE_CHECK_INPUTPOINTER(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Configure,dp_denoms);

                ret_iir=ADE_Iir_Configure(p_interpolator->dp_iir[0],dp_num,dp_denoms,p_interpolator->p_temp_buffer,p_outbuff);
                ADE_CHECK_ADERETVAL(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Configure,ret_iir);
            }
        }

    ret_out=ADE_Interpolator_SetOutBuff(p_interpolator,p_outbuff, out_buff_size);
    ADE_CHECK_ADERETVAL(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Configure,ret_out);

        return ADE_RET_SUCCESS;
}

/****************** Processing Methods ************************/
ADE_API_RET_T ADE_Interpolator_Step(ADE_INTERPOLATOR_T *p_interpolator)
{
    ADE_API_RET_T ret_up=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

    if (p_interpolator->arch_type==ADE_INTERPOLATOR_ARCH_STANDARD)
    {
        if (p_interpolator->filt_type==ADE_INTERPOLATOR_FILTER_FIR)
        {
            ret_up=ADE_Upsampler_Step(p_interpolator->p_upsampler);
            ADE_CHECK_ADERETVAL(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Step,ret_up);

            ret_fir=ADE_Fir_Step(p_interpolator->dp_fir[0]);
            ADE_CHECK_ADERETVAL(ADE_CLASS_INTERPOLATOR,ADE_METHOD_Step,ret_fir);
        }
    }

    return ADE_RET_SUCCESS;
}


