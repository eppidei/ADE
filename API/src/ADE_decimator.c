#include "headers/ADE_decimator.h"
#include "headers/ADE_downsampler.h"
#include "headers/ADE_fir.h"
#include "headers/ADE_iir.h"
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_Error_Handler.h"
#include <stdlib.h>


/*************** Init Methods ****************/

ADE_API_RET_T ADE_Decimator_Init(ADE_DECIMATOR_T **dp_decimator,ADE_UINT32_T in_buff_len,ADE_UINT32_T downfact,ADE_UINT32_T filter_order,
ADE_DECIMATOR_FILTER_T filt_type, ADE_DECIMATOR_ARCH_T arch_type,ADE_UINT32_T n_iir_sections)
{
    ADE_DECIMATOR_T *p_this;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir=ADE_RET_ERROR;
    ADE_API_RET_T ret_upsamp=ADE_RET_ERROR;


    p_this=calloc(1,sizeof(ADE_DECIMATOR_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_DECIMATOR,ADE_METHOD_Init,p_this);


    p_this->down_fact=downfact;
    p_this->filter_order=filter_order;
    p_this->in_buff_len=in_buff_len;
    if ( (in_buff_len%downfact)!=0)
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DECIMATOR,ADE_METHOD_Init,downfact,"%d",(FILE*)ADE_STD_STREAM);
        fprintf((FILE*)ADE_STD_STREAM,"ERROR : downfact %u is not an integer multiple of in buff len %u \n",downfact, in_buff_len);
        return ADE_RET_ERROR;
    }
    p_this->out_buff_len=in_buff_len/downfact;
    p_this->filt_type=filt_type;
    p_this->arch_type=arch_type;

    if (arch_type==ADE_DECIMATOR_ARCH_STANDARD)
    {
        ret_upsamp=ADE_Downsampler_Init(&(p_this->p_downsampler),in_buff_len,downfact);
        ADE_CHECK_ADERETVAL(ADE_CLASS_DECIMATOR,ADE_METHOD_Init,ret_upsamp);

        p_this->p_temp_buffer=calloc( p_this->out_buff_len,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_DECIMATOR,ADE_METHOD_Init,p_this->p_temp_buffer);

        if (filt_type==ADE_DECIMATOR_FILTER_FIR)
        {
            p_this->dp_fir=(ADE_FIR_T**)calloc(1,sizeof(ADE_FIR_T*));
            ADE_CHECK_MEMALLOC(ADE_CLASS_DECIMATOR,ADE_METHOD_Init,p_this->dp_fir);

            ret_fir=ADE_Fir_Init(&(p_this->dp_fir[0]),filter_order, p_this->out_buff_len,ADE_FIR_TRASP_II);/*qui dovrebbe essere meglio OVSA*/
            ADE_CHECK_ADERETVAL(ADE_CLASS_DECIMATOR,ADE_METHOD_Init,ret_fir);
        }
        else if (filt_type==ADE_DECIMATOR_FILTER_IIR)
        {
             p_this->dp_iir=(ADE_IIR_T**)calloc(1,sizeof(ADE_IIR_T*));
            ADE_CHECK_MEMALLOC(ADE_CLASS_DECIMATOR,ADE_METHOD_Init,p_this->dp_iir);

            ret_iir=ADE_Iir_Init(&(p_this->dp_iir[0]),n_iir_sections, p_this->out_buff_len,ADE_IIR_TRASP_II_B);/*qui dovrebbe essere meglio OVSA*/
            ADE_CHECK_ADERETVAL(ADE_CLASS_DECIMATOR,ADE_METHOD_Init,ret_iir);
        }
        else
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DECIMATOR,ADE_METHOD_Init,filt_type,"%d",(FILE*)ADE_STD_STREAM);
            fprintf((FILE*)ADE_STD_STREAM,"Unrecognized filt type \n");
            return ADE_RET_ERROR;
        }

    }
    else if (arch_type==ADE_DECIMATOR_ARCH_POLYPHASE)
    {
        if (filt_type==ADE_DECIMATOR_FILTER_FIR)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DECIMATOR,ADE_METHOD_Init,filt_type,"%d",(FILE*)ADE_STD_STREAM);
            fprintf((FILE*)ADE_STD_STREAM,"Missing Implementation filt_type = %d \n",filt_type);
            return ADE_RET_ERROR;
        }
        else if (filt_type==ADE_DECIMATOR_FILTER_IIR)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DECIMATOR,ADE_METHOD_Init,filt_type,"%d",(FILE*)ADE_STD_STREAM);
            fprintf((FILE*)ADE_STD_STREAM,"Missing Implementation filt_type = %d \n",filt_type);
            return ADE_RET_ERROR;
        }
        else
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DECIMATOR,ADE_METHOD_Init,filt_type,"%d",(FILE*)ADE_STD_STREAM);
            fprintf((FILE*)ADE_STD_STREAM,"Unrecognized filt type \n");
             return ADE_RET_ERROR;
        }
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DECIMATOR,ADE_METHOD_Init,arch_type,"%d",(FILE*)ADE_STD_STREAM);
         fprintf((FILE*)ADE_STD_STREAM,"Unrecognized arch type = %d \n",arch_type);
          return ADE_RET_ERROR;
    }

    *dp_decimator=p_this;

    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_Decimator_Release(ADE_DECIMATOR_T *p_decimator)
{
    if (p_decimator->arch_type==ADE_DECIMATOR_ARCH_STANDARD)
    {
        ADE_Downsampler_Release(p_decimator->p_downsampler);
        ADE_CHECKNFREE(p_decimator->p_temp_buffer);
        if (p_decimator->filt_type==ADE_DECIMATOR_FILTER_FIR)
        {
            ADE_Fir_Release(p_decimator->dp_fir[0]);
            ADE_CHECKNFREE(p_decimator->dp_fir);
        }
        else if (p_decimator->filt_type==ADE_DECIMATOR_FILTER_IIR)
        {
            ADE_Iir_Release(p_decimator->dp_iir[0]);
            ADE_CHECKNFREE(p_decimator->dp_iir);
        }
    }
     ADE_CHECKNFREE(p_decimator);
}
/*************** Set Methods ****************/
ADE_API_RET_T ADE_Decimator_SetInBuff(ADE_DECIMATOR_T *p_decimator,ADE_FLOATING_T *p_buff)
{

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_DECIMATOR,ADE_METHOD_SetInBuff,p_decimator);
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_DECIMATOR,ADE_METHOD_SetInBuff,p_buff);
    /* Do */

    p_decimator->p_in=p_buff;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Decimator_SetOutBuff(ADE_DECIMATOR_T *p_decimator,ADE_FLOATING_T *p_buff, ADE_SIZE_T buff_size)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DECIMATOR,ADE_METHOD_SetOutBuff,p_decimator);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_DECIMATOR,ADE_METHOD_SetOutBuff,p_buff);

    if (buff_size!=(p_decimator->out_buff_len*sizeof(ADE_FLOATING_T)))
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_DECIMATOR,ADE_METHOD_SetOutBuff,buff_size,"%u",(FILE*)ADE_STD_STREAM);
        fprintf((FILE*)ADE_STD_STREAM,"Decimator Out Buffer size wrong expexted %u , actual %u \n",p_decimator->out_buff_len*sizeof(ADE_FLOATING_T),buff_size);
        return ADE_RET_ERROR;
    }

    p_decimator->p_out=p_buff;


    return ADE_RET_SUCCESS;

}

/***************** Config Methods ***************************/

ADE_API_RET_T ADE_Decimator_Configure(ADE_DECIMATOR_T *p_decimator,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff,
ADE_SIZE_T out_buff_size,ADE_FLOATING_T *p_num,ADE_FLOATING_T **dp_num,ADE_FLOATING_T **dp_denoms)
{

    ADE_API_RET_T ret_up=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;
    ADE_API_RET_T ret_iir=ADE_RET_ERROR;
    ADE_API_RET_T ret_in=ADE_RET_ERROR;
    ADE_API_RET_T ret_out=ADE_RET_ERROR;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_DECIMATOR,ADE_METHOD_Configure,p_decimator);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_DECIMATOR,ADE_METHOD_Configure,p_inbuff);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_DECIMATOR,ADE_METHOD_Configure,p_outbuff);

    ret_in=ADE_Decimator_SetInBuff(p_decimator,p_inbuff);
    ADE_CHECK_ADERETVAL(ADE_CLASS_DECIMATOR,ADE_METHOD_Configure,ret_in);

        if (p_decimator->arch_type==ADE_DECIMATOR_ARCH_STANDARD)
        {
            ret_up=ADE_Downsampler_Configure(p_decimator->p_downsampler,p_decimator->p_temp_buffer,p_outbuff, out_buff_size);
            ADE_CHECK_ADERETVAL(ADE_CLASS_DECIMATOR,ADE_METHOD_Configure,ret_up);

            if (p_decimator->filt_type==ADE_DECIMATOR_FILTER_FIR)
            {
                ADE_CHECK_INPUTPOINTER(ADE_CLASS_DECIMATOR,ADE_METHOD_Configure,p_num);

                ret_fir=ADE_Fir_Configure(p_decimator->dp_fir[0],p_num,p_inbuff,p_decimator->p_temp_buffer);
                ADE_CHECK_ADERETVAL(ADE_CLASS_DECIMATOR,ADE_METHOD_Configure,ret_fir);
            }
            else if (p_decimator->filt_type==ADE_DECIMATOR_FILTER_IIR)
            {
                ADE_CHECK_INPUTPOINTER(ADE_CLASS_DECIMATOR,ADE_METHOD_Configure,dp_num);
                ADE_CHECK_INPUTPOINTER(ADE_CLASS_DECIMATOR,ADE_METHOD_Configure,dp_denoms);

                ret_iir=ADE_Iir_Configure(p_decimator->dp_iir[0],dp_num,dp_denoms,p_inbuff,p_decimator->p_temp_buffer);
                ADE_CHECK_ADERETVAL(ADE_CLASS_DECIMATOR,ADE_METHOD_Configure,ret_iir);
            }
        }

    ret_out=ADE_Decimator_SetOutBuff(p_decimator,p_outbuff, out_buff_size);
    ADE_CHECK_ADERETVAL(ADE_CLASS_DECIMATOR,ADE_METHOD_Configure,ret_out);

        return ADE_RET_SUCCESS;
}

/****************** Processing Methods ************************/
ADE_API_RET_T ADE_Decimator_Step(ADE_DECIMATOR_T *p_decimator)
{
    ADE_API_RET_T ret_up=ADE_RET_ERROR;
    ADE_API_RET_T ret_fir=ADE_RET_ERROR;

    if (p_decimator->arch_type==ADE_DECIMATOR_ARCH_STANDARD)
    {
        if (p_decimator->filt_type==ADE_DECIMATOR_FILTER_FIR)
        {
            ret_up=ADE_Downsampler_Step(p_decimator->p_downsampler);
            ADE_CHECK_ADERETVAL(ADE_CLASS_DECIMATOR,ADE_METHOD_Step,ret_up);

            ret_fir=ADE_Fir_Step(p_decimator->dp_fir[0]);
            ADE_CHECK_ADERETVAL(ADE_CLASS_DECIMATOR,ADE_METHOD_Step,ret_fir);
        }
    }

    return ADE_RET_SUCCESS;
}



