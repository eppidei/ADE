#ifndef _ADE_PROXIMITY_H
#define _ADE_PROXIMITY_H
#include "headers/ADE_typedefs.h"

struct ADE_PROXIMITY_S
{
    ADE_FLOATING_T Fs_i;
    ADE_UINT32_T buff_len_i;
    ADE_FLOATING_T Fs_o;
    ADE_UINT32_T buff_len_o;
    ADE_FLOATING_T * p_in;
    ADE_BOOL_T state;
};

#ifdef __cplusplus
    extern "C" {
#endif

ADE_API_RET_T ADE_Proximity_Init(ADE_PROXIMITY_T **dp_proxy);
ADE_VOID_T ADE_Proximity_Release(ADE_PROXIMITY_T *p_proxy);
ADE_API_RET_T ADE_Proximity_Configure_params(ADE_PROXIMITY_T *p_proxy,ADE_FLOATING_T Fs_i);
ADE_API_RET_T ADE_Proximity_Configure_inout(ADE_PROXIMITY_T *p_proxy,ADE_FLOATING_T * p_in);
ADE_API_RET_T ADE_Proximity_Configure_bufflength(ADE_PROXIMITY_T *p_proxy,ADE_UINT32_T buff_len_i);
ADE_API_RET_T ADE_Proximity_Configure(ADE_PROXIMITY_T *p_proxy,ADE_FLOATING_T Fs_i,ADE_UINT32_T buff_len_i,ADE_FLOATING_T * p_in);
ADE_API_RET_T ADE_Proximity_Step(ADE_PROXIMITY_T *p_proxy);

#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif // _ADE_PROXIMITY_H
