#ifndef _ADE_UPSAMPLER_H
#define _ADE_UPSAMPLER_H
#include "headers/ADE_typedefs.h"

struct ADE_UPSAMPLER_S
{
    ADE_FLOATING_T* p_in;
    ADE_FLOATING_T* p_out;
 //   ADE_FLOATING_T* p_temp_buff; //allocato dentro;
    ADE_UINT32_T in_buff_len;
    ADE_UINT32_T up_fact;
    ADE_UINT32_T out_buff_len;
  //  ADE_UINT32_T filter_order;
//    ADE_FIR_T *p_fir;
//    ADE_FIR_IMP_CHOICE_T fir_type;
//    ADE_IIR_T *p_iir;
//    ADE_IIR_IMP_CHOICE_T iir_type;
  //  ADE_UINT32_T n_iir_sections;
  //  ADE_UPSAMP_CHOICE_T up_samp_type;
    ADE_blas_level1_T *p_blas_l1_memcpy;
};

#ifdef __cplusplus
    extern "C" {
#endif
/************** Init Methods ******************/
ADE_API_RET_T ADE_Upsampler_Init(ADE_UPSAMPLER_T **dp_upsampler,ADE_UINT32_T in_buff_len,ADE_UINT32_T upfact);
ADE_VOID_T ADE_Upsampler_Release(ADE_UPSAMPLER_T *p_upsampler);

/************** Set Methods **********************/

/***************** Config Methods ***************************/
ADE_API_RET_T ADE_Upsampler_Configure(ADE_UPSAMPLER_T *p_upsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff, ADE_SIZE_T out_buff_size);
/************** Processing Methods ***************/

ADE_API_RET_T ADE_Upsampler_Step(ADE_UPSAMPLER_T *p_upsampler);


#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif //_ADE_UPSAMPLER_H
