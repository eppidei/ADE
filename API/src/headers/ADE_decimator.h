#ifndef _ADE_DECIMATOR_H
#define _ADE_DECIMATOR_H
#include "headers/ADE_typedefs.h"

struct ADE_DECIMATOR_S
{
    ADE_FLOATING_T* p_in;
    ADE_FLOATING_T* p_out;
    ADE_UINT32_T in_buff_len;
    ADE_UINT32_T out_buff_len;
    ADE_UINT32_T down_fact;
    ADE_UINT32_T filter_order;
    ADE_DECIMATOR_FILTER_T filt_type;
    ADE_DECIMATOR_ARCH_T arch_type;
    ADE_DOWNSAMPLER_T *p_downsampler;
    ADE_FLOATING_T *p_temp_buffer;
    ADE_FLOATING_T *p_num;
    ADE_FIR_T **dp_fir;
    ADE_IIR_T **dp_iir;
};
#ifdef __cplusplus
    extern "C" {
#endif
/*************** Init Methods ****************/
ADE_API_RET_T ADE_Decimator_Init(ADE_DECIMATOR_T **dp_decimator,ADE_UINT32_T in_buff_len,ADE_UINT32_T upfact,ADE_UINT32_T filter_order,
ADE_DECIMATOR_FILTER_T filt_type, ADE_DECIMATOR_ARCH_T arch_type,ADE_UINT32_T n_iir_sections);
ADE_VOID_T ADE_Decimator_Release(ADE_DECIMATOR_T *p_decimator);
/*************** Set Methods ****************/
ADE_API_RET_T ADE_Decimator_SetInBuff(ADE_DECIMATOR_T *p_decimator,ADE_FLOATING_T *p_buff);
ADE_API_RET_T ADE_Decimator_SetOutBuff(ADE_DECIMATOR_T *p_decimator,ADE_FLOATING_T *p_buff, ADE_SIZE_T buff_size);
/***************** Config Methods ***************************/
ADE_API_RET_T ADE_Decimator_Configure(ADE_DECIMATOR_T *p_decimator,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff,
ADE_SIZE_T out_buff_size,ADE_FLOATING_T *p_num,ADE_FLOATING_T **dp_num,ADE_FLOATING_T **dp_denoms);
/****************** Processing Methods ************************/
ADE_API_RET_T ADE_Decimator_Step(ADE_DECIMATOR_T *p_decimator);
#ifdef __cplusplus
    }   /* extern "C" */
#endif
#endif //_ADE_DECIMATOR_H

