#ifndef _ADE_UTILS_H
#define _ADE_UTILS_H
#include "headers/ADE_typedefs.h"


#ifdef __cplusplus
    extern "C" {
#endif


ADE_VOID_T ADE_Utils_Get_Terminal_size(ADE_INT32_T *lines ,ADE_INT32_T *columns  );
ADE_API_RET_T ADE_Utils_PrintArray(ADE_VOID_T *p_var,ADE_UINT32_T start_0based_row_idx,ADE_UINT32_T stop_0based_row_idx, ADE_UINT32_T start_0based_col_idx,ADE_UINT32_T stop_0based_col_idx, ADE_CHAR_T *p_varname, FILE *p_stream,ADE_MATH_ATTRIBUTE_T math_type);
ADE_API_RET_T ADE_Utils_FindIndexes(ADE_FLOATING_T *frame_i,ADE_UINT32_T frame_len, ADE_FLOATING_T *indexes_o, ADE_FLOATING_T *n_indexes_o,ADE_FLOATING_T threshold,ADE_UTILS_CONDITION_T condition);
ADE_API_RET_T ADE_Utils_Complex2Split(ADE_CPLX_T *p_in,ADE_UINT32_T Stride_in,ADE_SplitComplex_T *p_out,ADE_UINT32_T Stride_out,ADE_UINT32_T cplx_len);
ADE_API_RET_T ADE_Utils_Split2Complex( ADE_SplitComplex_T *p_in,ADE_UINT32_T Stride_in,ADE_CPLX_T *p_out,ADE_UINT32_T Stride_out,ADE_UINT32_T split_len);
ADE_API_RET_T ADE_Utils_SetSplit(ADE_VOID_T *p_buff,ADE_UINT32_T buff_len,ADE_SplitComplex_T *p_split);
ADE_API_RET_T ADE_Utils_FillSplitReal(ADE_FLOATING_T real,ADE_UINT32_T idx,ADE_SplitComplex_T *p_split);
ADE_API_RET_T ADE_Utils_FillSplitImag(ADE_FLOATING_T imag,ADE_UINT32_T idx,ADE_SplitComplex_T *p_split);
ADE_API_RET_T ADE_Utils_FillSplitCplx(ADE_FLOATING_T real,ADE_FLOATING_T imag,ADE_UINT32_T idx,ADE_SplitComplex_T *p_split);
ADE_API_RET_T ADE_Utils_SetSplit(ADE_VOID_T *p_buff,ADE_UINT32_T buff_len,ADE_SplitComplex_T *p_split);
#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_UTILS_H
