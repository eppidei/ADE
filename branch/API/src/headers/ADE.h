#ifndef _ADE_H
#define _ADE_H
#include "headers/ADE_typedefs.h"
#include "headers/ADE_SCDF_Interface.h"


#ifdef __cplusplus
    extern "C" {
#endif
ADE_API_RET_T ADE_Init(ADE_T **dp_ADE_Handle, ADE_UINT32_T Sel_Flag_i);//,ADE_UINT32_T in_buff_len,ADE_FLOATING_T input_rate);
ADE_VOID_T ADE_Release(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i);
/***************** Configure Methods ************************/
ADE_API_RET_T ADE_Configure(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_SCDF_Input_Int_T *p_in_struct);
/****************** Get Methods **********************/
ADE_SCDF_Output_Int_T* ADE_GetOutBuff(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i);
ADE_API_RET_T ADE_GetActiveAlgos(ADE_T* p_ADE,ADE_UINT32_T *p_ActiveFlags);
ADE_API_RET_T ADE_GetBlow(ADE_T* p_ADE,ADE_BLOW_T **dp_blow);
ADE_API_RET_T ADE_GetSnap(ADE_T* p_ADE,ADE_SNAP_T **dp_snap);
ADE_API_RET_T ADE_GetProxy(ADE_T* p_ADE,ADE_PROXIMITY_T **dp_proxy);
/*********************UTIL Methods ********************/
ADE_BOOL_T ADE_IsBlow(ADE_UINT32_T active_algs_flag);
ADE_BOOL_T ADE_IsSnap(ADE_UINT32_T active_algs_flag);
/********************** Processing Methids ************/
ADE_API_RET_T ADE_Step(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_SCDF_Input_Int_T *p_in_struct);
#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif //_ADE_H
