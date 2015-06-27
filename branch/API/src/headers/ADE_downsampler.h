#ifndef _ADE_DOWNSAMPLER_H
#define _ADE_DOWNSAMPLER_H
#include "headers/ADE_typedefs.h"

struct ADE_DOWNSAMPLER_S
{
    ADE_FLOATING_T* p_in;
    ADE_FLOATING_T* p_out;
 //   ADE_FLOATING_T* p_temp_buff; //allocato dentro;
    ADE_UINT32_T in_buff_len;
    ADE_UINT32_T down_fact;
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
ADE_API_RET_T ADE_Downsampler_Init(ADE_DOWNSAMPLER_T **dp_downsampler,ADE_UINT32_T in_buff_len,ADE_UINT32_T upfact);
ADE_VOID_T ADE_Downsampler_Release(ADE_DOWNSAMPLER_T *p_downsampler);

/************** Set Methods **********************/

/***************** Config Methods ***************************/
ADE_API_RET_T ADE_Downsampler_Configure(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff, ADE_SIZE_T out_buff_size);
/************** Processing Methods ***************/

ADE_API_RET_T ADE_Downsampler_Step(ADE_DOWNSAMPLER_T *p_downsampler);
/************** Utils ******************************/

ADE_API_RET_T ADE_Downsampler_Print(ADE_DOWNSAMPLER_T* p_downsampler, ADE_FILE_T *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj);
#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif //_ADE_DOWNSAMPLER_H
