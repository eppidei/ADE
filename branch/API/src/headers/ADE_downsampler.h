#ifndef _ADE_DOWNSAMPLER_H
#define _ADE_DOWNSAMPLER_H
#include "headers/ADE_typedefs.h"


#ifdef __cplusplus
    extern "C" {
#endif

/************** Init Methods ******************/
ADE_API_RET_T ADE_Downsampler_Init(ADE_DOWNSAMPLER_T **dp_downsampler);//,ADE_UINT32_T in_buff_len,ADE_UINT32_T upfact);
ADE_VOID_T ADE_Downsampler_Release(ADE_DOWNSAMPLER_T *p_downsampler);

/************** Set Methods **********************/

/***************** Config Methods ***************************/
ADE_API_RET_T ADE_Downsampler_Configure(ADE_DOWNSAMPLER_T *p_downsampler,ADE_INT32_T out_buff_len,
                                        ADE_INT32_T downfact,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff);
ADE_API_RET_T ADE_Downsampler_Configure_params(ADE_DOWNSAMPLER_T *p_downsampler, ADE_INT32_T downfact);
ADE_API_RET_T ADE_Downsampler_Configure_inout(ADE_DOWNSAMPLER_T *p_downsampler,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T *p_outbuff);
ADE_API_RET_T ADE_Downsampler_Configure_bufflength(ADE_DOWNSAMPLER_T *p_downsampler, ADE_INT32_T out_buff_len);
/************** Processing Methods ***************/
ADE_API_RET_T ADE_Downsampler_Step(ADE_DOWNSAMPLER_T *p_downsampler);
/************** Utils ******************************/
ADE_API_RET_T ADE_Downsampler_Print(ADE_DOWNSAMPLER_T* p_downsampler, ADE_FILE_T *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj);
#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif //_ADE_DOWNSAMPLER_H
