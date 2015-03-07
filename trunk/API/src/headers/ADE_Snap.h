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
    ADE_UINT32_T n_pow_est_slots;/* slots to divide the frame for power estimation into smaller time*/
    ADE_FLOATING_T threshold;
    ADE_FLOATING_T frame_time_len;
   // ADE_UINT32_T frame_len;
    ADE_FLOATING_T freq_left;
    ADE_FLOATING_T freq_right;
    ADE_FLOATING_T spectral_threshold_schiocco ;
    ADE_FLOATING_T thresh_gain;
    ADE_FLOATING_T thresh_bias;
    ADE_FLOATING_T at;
    ADE_FLOATING_T rt;
    ADE_FLOATING_T time_left;
    ADE_FLOATING_T time_right;
    ADE_FLOATING_T samp_range_search_time ;
    ADE_FLOATING_T max_range[2];
    ADE_UINT32_T samp_range_search;
    ADE_UINT32_T search_step ;
    ADE_UINT32_T look_ahead_step;


    /*in out buffers*/ //allocati fuori
    ADE_FLOATING_T *p_in;
    ADE_FLOATING_T *p_out;
    /*Internal buffers*/ //allocati internamente
    ADE_FLOATING_T *p_pow_est;
    ADE_FLOATING_T *p_dot_vals;
    ADE_FLOATING_T *p_thresh;
    ADE_FLOATING_T *p_tgk;
    ADE_FLOATING_T *p_indexes;
  //  ADE_FLOATING_T *p_tgk_temp;
    /*Friend Classes*/
    ADE_IIR_T *p_iir;
    ADE_blas_level1_T **dp_blas_l1_pow_est;
    ADE_blas_level1_T *p_blas_l1_threshold;
    ADE_blas_level2_T *p_blas_l2_tgk1;
    ADE_blas_level2_T *p_blas_l2_tgk2;
    #ifdef ADE_CONFIGURATION_INTERACTIVE
    ADE_MATLAB_T *p_mat;
    #endif

};

/*********************** Init Methods************************/
ADE_API_RET_T ADE_Snap_Init(ADE_SNAP_T **p_snap,ADE_UINT32_T buff_len,ADE_UINT32_T Fs_i,ADE_UINT32_T n_pow_slots_i);
ADE_VOID_T ADE_Snap_Release(ADE_SNAP_T *p_snap);
/**************************** Set Methods ********************************/
/************************ Configuration Methods **************************************/
/************************** Operative Methods *******************************/
ADE_API_RET_T ADE_Snap_Step(ADE_SNAP_T *p_snap);

#endif //_ADE_SNAP_H

