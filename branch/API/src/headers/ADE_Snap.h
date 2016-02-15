#ifndef _ADE_SNAP_H
#define _ADE_SNAP_H
#include "headers/ADE_typedefs.h"
#ifdef ADE_CONFIGURATION_INTERACTIVE
#include "headers/ADE_Matlab.h"
#include "matrix.h"
#include "engine.h"
#endif




#ifdef __cplusplus
    extern "C" {
#endif

/*********************** Init Methods************************/
ADE_API_RET_T ADE_Snap_Init(ADE_SNAP_T **p_snap);//,ADE_UINT32_T buff_len,ADE_UINT32_T Fs_i,ADE_UINT32_T n_pow_slots_i,ADE_UINT32_T n_max_indexes_i,ADE_FLOATING_T time_left_i,ADE_FLOATING_T time_right_i,ADE_UINT32_T fft_len_i);
ADE_VOID_T ADE_Snap_Release(ADE_SNAP_T *p_snap);
/*************************** GET METHODS ****************************/
ADE_API_RET_T ADE_Snap_GetIndexes(ADE_SNAP_T *p_snap,ADE_INT32_T **dp_idx);
ADE_API_RET_T ADE_Snap_GetBuffLength(ADE_SNAP_T *p_snap,ADE_INT32_T *p_blen);
ADE_API_RET_T ADE_Snap_GetNoFoundIndexes(ADE_SNAP_T *p_snap,ADE_INT32_T *p_NoFoundIdx);
ADE_API_RET_T ADE_Snap_GetSnaps(ADE_SNAP_T *p_snap,ADE_BOOL_T **dp_snaps);
ADE_API_RET_T ADE_Snap_GetMatlab(ADE_SNAP_T* p_snap, ADE_MATLAB_T **dp_mat);
ADE_API_RET_T ADE_Snap_GetState(ADE_SNAP_T *p_snap,ADE_BOOL_T *p_state);
ADE_API_RET_T ADE_Snap_GetFs(ADE_SNAP_T *p_snap,ADE_FLOATING_T *p_Fs);
/************************** Set Methods ******************************/
ADE_API_RET_T ADE_Snap_SetMatlab(ADE_SNAP_T* p_snap, ADE_MATLAB_T *p_mat);
/************************ Configuration Methods **************************************/
ADE_API_RET_T ADE_Snap_Configure(ADE_SNAP_T *p_snap, ADE_FLOATING_T *p_buff,ADE_INT32_T buff_len,ADE_FLOATING_T Fs_i,ADE_INT32_T n_pow_slots_i,
                                ADE_INT32_T n_actual_indexes_i,ADE_FLOATING_T time_left_i,ADE_FLOATING_T time_right_i,ADE_INT32_T fft_len_i);

ADE_API_RET_T ADE_Snap_Configure_inout(ADE_SNAP_T *p_snap, ADE_FLOATING_T *p_buff);
/************************** Processing Methods *******************************/
ADE_API_RET_T ADE_Snap_Step(ADE_SNAP_T *p_snap);
/************************** Utils Methods *******************************/
ADE_API_RET_T ADE_Snap_Print(ADE_SNAP_T* p_snap, ADE_FILE_T *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj);


#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_SNAP_H

