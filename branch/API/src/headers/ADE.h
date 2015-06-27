#ifndef _ADE_H
#define _ADE_H
#include "headers/ADE_typedefs.h"
#include "headers/ADE_SCDF_Interface.h"

struct ADE_S
{
    ADE_UINT32_T active_algs_flag;
    /*Blow*/
    ADE_BLOW_T *p_blow;
    ADE_SCDF_Input_Int_T *p_blow_in_struct;
    ADE_SCDF_Output_Int_T *p_blow_out_struct;
   // ADE_DOWNSAMPLER_T *p_blow_downsampler;
    /*Snap*/
    ADE_SNAP_T *p_snap;
    ADE_SCDF_Input_Int_T *p_snap_in_struct;
    ADE_SCDF_Output_Int_T *p_snap_out_struct;
    /*Proxy*/
    ADE_PROXIMITY_T *p_proxy;
    ADE_SCDF_Input_Int_T *p_proxy_in_struct;
    ADE_SCDF_Output_Int_T *p_proxy_out_struct;
};
#ifdef __cplusplus
    extern "C" {
#endif
ADE_API_RET_T ADE_Init(ADE_T **dp_ADE_Handle, ADE_UINT32_T Sel_Flag_i,ADE_UINT32_T in_buff_len,ADE_FLOATING_T input_rate);
ADE_VOID_T ADE_Release(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i);
/***************** Configure Methods ************************/
ADE_API_RET_T ADE_Configure_params(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_FLOATING_T Fs_i);
/****************** Get Methods **********************/
ADE_SCDF_Output_Int_T* ADE_GetOutBuff(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i);
/********************** Processing Methids ************/
ADE_API_RET_T ADE_Step(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_SCDF_Input_Int_T *p_in_struct);
#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif //_ADE_H
