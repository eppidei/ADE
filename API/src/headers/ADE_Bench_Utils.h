#ifndef _ADE_BENCH_UTILS_H
#define _ADE_BENCH_UTILS_H
#include "headers/ADE_typedefs.h"

#ifdef __cplusplus
    extern "C" {
#endif

ADE_VOID_T gemm_checker(ADE_blas_level3_T *p_blas_l3,ADE_VOID_T *p_C_custom,ADE_FLOATING_T tol,ADE_UINT32_T test_id,ADE_BENCH_MAT_T m_type,bench_times_T *bench_struct,FILE* p_fid);
ADE_VOID_T load_fmatrix(ADE_FLOATING_T *p_mat,ADE_UINT32_T n_row,ADE_UINT32_T n_col);
ADE_VOID_T load_cmatrix(ADE_CPLX_T *p_mat,ADE_UINT32_T n_row,ADE_UINT32_T n_col);
ADE_VOID_T custom_faABpbC (ADE_blas_level3_T *p_blas_l3,ADE_FLOATING_T *p_C);
ADE_VOID_T custom_caABpbC (ADE_blas_level3_T *p_blas_l3,ADE_CPLX_T *p_C);
ADE_INT32_T blas3_configuration(ADE_blas_level3_T* p_Blas_l3,ADE_CHAR_T TRANSA,ADE_CHAR_T TRANSB,ADE_INT32_T M,
                         ADE_INT32_T N,ADE_INT32_T K,ADE_INT32_T LDA,ADE_INT32_T LDB,ADE_INT32_T LDC,
                         ADE_FLOATING_T *p_alpha,ADE_FLOATING_T *p_beta,ADE_FLOATING_T *p_A,ADE_FLOATING_T *p_B,ADE_FLOATING_T *p_C);


ADE_INT32_T blas3_test_procedure(ADE_BENCH_T *test_cases,ADE_UINT32_T n_tests,ADE_BENCH_MAT_T *mat_type,ADE_UINT32_T n_types,ADE_INT32_T *n_rows_A,ADE_INT32_T *n_cols_A,ADE_INT32_T *n_cols_B,ADE_INT32_T n_dim_cases,FILE* p_fid);
ADE_VOID_T custom_FFT(ADE_FLOATING_T data[], unsigned long nn, ADE_CUSTOM_FFT_DIRECTION_T isign);
ADE_VOID_T fft_checker(ADE_FFT_T *p_fft,ADE_VOID_T *p_fft_custom,ADE_FLOATING_T tol,ADE_UINT32_T test_id,ADE_FFT_TYPE_T m_type,bench_times_T *bench_struct,FILE* p_fid);
ADE_INT32_T fft_test_procedure(ADE_FFT_TYPE_T fft_type,ADE_UINT32_T *p_dim,ADE_INT32_T n_dim_cases,FILE* p_fid);
        ADE_INT32_T fft_iosreal_test_procedure(ADE_UINT32_T *p_dim,ADE_INT32_T n_dim_cases,FILE* p_fid);

#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif //_ADE_BENCH_UTILS_H
