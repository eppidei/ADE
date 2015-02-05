#ifndef _ADE_BENCH_UTILS_H
#define _ADE_BENCH_UTILS_H
#include "headers/ADE_typedefs.h"


void checker(ADE_VOID_T *p_mat1,ADE_VOID_T *p_mat2,ADE_UINT32_T n_row,ADE_UINT32_T n_col,ADE_FLOATING_T tol,ADE_UINT32_T test_id,ADE_BENCH_MAT_T m_type,FILE* p_fid);
void load_fmatrix(ADE_FLOATING_T *p_mat,ADE_UINT32_T n_row,ADE_UINT32_T n_col);
void load_cmatrix(ADE_CPLX_T *p_mat,ADE_UINT32_T n_row,ADE_UINT32_T n_col);
void custom_faABpbC (ADE_FLOATING_T *p_alpha,ADE_FLOATING_T *p_beta,ADE_FLOATING_T *p_A,ADE_FLOATING_T *p_B,ADE_FLOATING_T *p_C,ADE_UINT32_T n_row_A,ADE_UINT32_T n_col_A,ADE_UINT32_T n_col_B);
void custom_caABpbC (ADE_CPLX_T *p_alpha,ADE_CPLX_T *p_beta,ADE_CPLX_T *p_A,ADE_CPLX_T *p_B,ADE_CPLX_T *p_C,ADE_UINT32_T n_row_A,ADE_UINT32_T n_col_A,ADE_UINT32_T n_col_B);
int blas3_configuration(ADE_blas_level3_T* p_Blas_l3,ADE_CHAR_T TRANSA,ADE_CHAR_T TRANSB,ADE_INT32_T M,
                         ADE_INT32_T N,ADE_INT32_T K,ADE_INT32_T LDA,ADE_INT32_T LDB,ADE_INT32_T LDC,
                         ADE_FLOATING_T *p_alpha,ADE_FLOATING_T *p_beta,ADE_FLOATING_T *p_A,ADE_FLOATING_T *p_B,ADE_FLOATING_T *p_C);
ADE_INT32_T blas3_test_procedure(ADE_BENCH_T *test_cases,ADE_UINT32_T n_tests,ADE_BENCH_MAT_T *mat_type,ADE_UINT32_T n_types,ADE_INT32_T *n_rows_A,ADE_INT32_T *n_cols_A,ADE_INT32_T *n_cols_B,ADE_INT32_T n_dim_cases);



#endif //_ADE_BENCH_UTILS_H
