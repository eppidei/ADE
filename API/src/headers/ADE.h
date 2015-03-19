#ifndef _ADE_H
#define _ADE_H
#include "headers/ADE_typedefs.h"

struct ADE_S
{
    ADE_UINT32_T active_algs_flag;
    ADE_BLOW_T *p_blow;
    ADE_SNAP_T *p_snap;
};

ADE_API_RET_T ADE_Init(ADE_T **dp_ADE_Handle, ADE_UINT32_T Sel_Flag_i,ADE_UINT32_T buff_size_i,ADE_FLOATING_T Fs_i);
ADE_VOID_T ADE_Release(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i);
ADE_VOID_T ADE_SetInBuff(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_FLOATING_T *p_buff);
ADE_VOID_T ADE_SetOutBuff(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i,ADE_FLOATING_T *p_buff);
ADE_API_RET_T ADE_Step(ADE_T* p_ADE,ADE_UINT32_T Sel_Flag_i);


#endif //_ADE_H
