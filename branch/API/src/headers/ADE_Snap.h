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
    ADE_INT32_T buff_len;
    ADE_INT32_T n_pow_est_slots;/* slots to divide the frame for power estimation into smaller time*/
    ADE_INT32_T n_max_pow_est_slots;
  // ADE_FLOATING_T threshold;
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
    ADE_INT32_T samp_range_search;
    ADE_INT32_T search_step ;
    ADE_INT32_T look_ahead_step;
    ADE_INT32_T n_max_indexes;
    ADE_INT32_T n_actual_indexes;
    ADE_INT32_T n_found_indexes;
    ADE_INT32_T extract_len;
    ADE_INT32_T fft_len;
    ADE_INT32_T max_extract_len;
    ADE_INT32_T max_fft_len;
    /*in out buffers*/ //allocati fuori
    ADE_FLOATING_T *p_in;
    //ADE_FLOATING_T *p_out;
    /*Internal buffers*/ //allocati internamente
    ADE_FLOATING_T *p_pow_est;//lungo buff_len
    ADE_FLOATING_T *p_dot_vals;//lungo n_slots
    ADE_FLOATING_T *p_thresh;//lungo buff_len
    ADE_FLOATING_T *p_tgk;//lungo buff_len usato per tgk e cc
    ADE_INT32_T *p_indexes;//lungo n_max_indexes
    ADE_ULONG_T *p_sort_indexes;
    ADE_FLOATING_T *p_index_vals;//lungo n_max_indexes
    ADE_FLOATING_T **dp_segments;//lungo n_max_indexes*extract_len
    ADE_VOID_T **dp_spectrum;//lungo n_max_indexes*extract_len*2
    ADE_FLOATING_T *p_percent_pow;
    ADE_BOOL_T *p_snaps;
    ADE_BOOL_T state;
  //  ADE_FLOATING_T *p_tgk_temp;
    /*Friend Classes*/
    ADE_IIR_T *p_iir;
    ADE_blas_level1_T **dp_blas_l1_pow_est;
    ADE_blas_level1_T *p_blas_l1_threshold;
    ADE_blas_level2_T *p_blas_l2_tgk1;
    ADE_blas_level2_T *p_blas_l2_tgk2;
    ADE_blas_level1_T **dp_blas_l1_pow_spect_whole;
    ADE_blas_level1_T **dp_blas_l1_pow_spect_band;
    ADE_FFT_T** dp_fft;
    #ifdef ADE_CONFIGURATION_INTERACTIVE
    ADE_MATLAB_T *p_mat;
    #endif

};

#ifdef __cplusplus
    extern "C" {
#endif

/*********************** Init Methods************************/
ADE_API_RET_T ADE_Snap_Init(ADE_SNAP_T **p_snap);//,ADE_UINT32_T buff_len,ADE_UINT32_T Fs_i,ADE_UINT32_T n_pow_slots_i,ADE_UINT32_T n_max_indexes_i,ADE_FLOATING_T time_left_i,ADE_FLOATING_T time_right_i,ADE_UINT32_T fft_len_i);
ADE_VOID_T ADE_Snap_Release(ADE_SNAP_T *p_snap);
/************************ Configuration Methods **************************************/
ADE_API_RET_T ADE_Snap_Configure(ADE_SNAP_T *p_snap, ADE_FLOATING_T *p_buff,ADE_INT32_T buff_len,ADE_FLOATING_T Fs_i,ADE_INT32_T n_pow_slots_i,
                                ADE_INT32_T n_actual_indexes_i,ADE_FLOATING_T time_left_i,ADE_FLOATING_T time_right_i,ADE_INT32_T fft_len_i);
ADE_API_RET_T ADE_Snap_Configure_params(ADE_SNAP_T *p_snap,ADE_INT32_T buff_len,ADE_FLOATING_T Fs_i,ADE_INT32_T n_pow_slots_i,
                                        ADE_INT32_T n_actual_indexes_i,ADE_FLOATING_T time_left_i,ADE_FLOATING_T time_right_i,
                                        ADE_INT32_T fft_len_i);
ADE_API_RET_T ADE_Snap_Configure_inout(ADE_SNAP_T *p_snap, ADE_FLOATING_T *p_buff);
/************************** Processing Methods *******************************/
ADE_API_RET_T ADE_Snap_Step(ADE_SNAP_T *p_snap);
/************************** Utils Methods *******************************/
ADE_API_RET_T ADE_Snap_Print(ADE_SNAP_T* p_snap, ADE_FILE_T *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj);


#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_SNAP_H

