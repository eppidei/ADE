#ifndef _ADE_INTERPOLATOR_H
#define _ADE_INTERPOLATOR_H
#include "headers/ADE_typedefs.h"

struct ADE_INTERPOLATOR_S
{
    ADE_FLOATING_T* p_in;
    ADE_FLOATING_T* p_out;
    ADE_UINT32_T in_buff_len;
    ADE_UINT32_T out_buff_len;
    ADE_UINT32_T up_fact;
    ADE_UINT32_T filter_order;
    ADE_INTERPOLATOR_FILTER_T filt_type;
    ADE_INTERPOLATOR_ARCH_T arch_type;
    ADE_UPSAMPLER_T *p_upsampler;
    ADE_FLOATING_T *p_temp_buffer;
    ADE_FLOATING_T *p_num;
    ADE_FIR_T **dp_fir;
    ADE_IIR_T **dp_iir;
};

/*************** Init Methods ****************/
ADE_API_RET_T ADE_Interpolator_Init(ADE_INTERPOLATOR_T **dp_interpolator,ADE_UINT32_T in_buff_len,ADE_UINT32_T upfact,ADE_UINT32_T filter_order,
ADE_INTERPOLATOR_FILTER_T filt_type, ADE_INTERPOLATOR_ARCH_T arch_type,ADE_UINT32_T n_iir_sections);
ADE_VOID_T ADE_Interpolator_Release(ADE_INTERPOLATOR_T *p_interpolator);
/*************** Set Methods ****************/
ADE_API_RET_T ADE_Interpolator_SetInBuff(ADE_INTERPOLATOR_T *p_interpolator,ADE_FLOATING_T *p_buff);
ADE_API_RET_T ADE_Interpolator_SetOutBuff(ADE_INTERPOLATOR_T *p_interpolator,ADE_FLOATING_T *p_buff, ADE_SIZE_T buff_size);
/***************** Config Methods ***************************/
ADE_API_RET_T ADE_Interpolator_Configure(ADE_INTERPOLATOR_T *p_interpolator,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff,
ADE_SIZE_T out_buff_size,ADE_FLOATING_T *p_num,ADE_FLOATING_T **dp_num,ADE_FLOATING_T **dp_denoms);
/****************** Processing Methods ************************/
ADE_API_RET_T ADE_Interpolator_Step(ADE_INTERPOLATOR_T *p_interpolator);
#endif //_ADE_INTERPOLATOR_H
