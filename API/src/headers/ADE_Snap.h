#ifndef _ADE_SNAP_H
#define _ADE_SNAP_H
#include "headers/ADE_typedefs.h"
#ifdef ADE_CONFIGURATION_INTERACTIVE
#include "headers/ADE_Matlab.h"
#include "matrix.h"
#include "engine.h"
#endif

struct ADE_SNAP_S
{
    ADE_FLOATING_T Fs;
    ADE_UINT32_T buff_len;
    ADE_UINT32_T n_threshold_slots;
    /*in out buffers*/ //allocati fuori
    ADE_FLOATING_T *p_in;
    ADE_FLOATING_T *p_out;
    /*Internal buffers*/ //allocati internamente
    ADE_FLOATING_T *p_pow_est;
    ADE_FLOATING_T *p_tgk;
    ADE_FLOATING_T *p_indexes;
  //  ADE_FLOATING_T *p_tgk_temp;
    /*Friend Classes*/
    ADE_IIR_T *p_iir;
    ADE_blas_level2_T **p_blas_l2_threshold;
    ADE_blas_level2_T *p_blas_l2_tgk1;
    ADE_blas_level2_T *p_blas_l2_tgk2;
    #ifdef ADE_CONFIGURATION_INTERACTIVE
    ADE_MATLAB_T *p_mat;
    #endif

};

/*********************** Init Methods************************/
ADE_API_RET_T ADE_Snap_Init(ADE_SNAP_T **p_snap,ADE_UINT32_T buff_len);
ADE_VOID_T ADE_Snap_Release(ADE_SNAP_T *p_snap);
/**************************** Set Methods ********************************/
/************************ Configuration Methods **************************************/
/************************** Operative Methods *******************************/
ADE_API_RET_T ADE_Snap_Step(ADE_SNAP_T *p_snap);

#endif //_ADE_SNAP_H

