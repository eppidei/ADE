#ifndef _ADE_MATLAB_H
#define _ADE_MATLAB_H
#include "headers/ADE_defines.h"
#include "headers/ADE_typedefs.h"
#include "engine.h"
#include <stdio.h>


#ifdef __cplusplus
    extern "C" {
#endif
/*************** Init Methods *****************/
ADE_API_RET_T ADE_Matlab_Init(ADE_MATLAB_T** dp_this, char* filename, char* p_matfname,char *p_matpath);
ADE_VOID_T ADE_Matlab_Release(ADE_MATLAB_T* p_mat);
/***************** Get Methods ***********************/
ADE_API_RET_T ADE_Matlab_GetVarIndex(ADE_MATLAB_T* p_mat, char *varname,ADE_UINT32_T *p_idx);
ADE_API_RET_T ADE_Matlab_GetNRows(ADE_MATLAB_T* p_mat, char *varname,ADE_UINT32_T *p_nRow);
ADE_API_RET_T ADE_Matlab_GetNCols(ADE_MATLAB_T* p_mat, char *varname, ADE_UINT32_T *p_ncol);
ADE_API_RET_T ADE_Matlab_GetSize(ADE_MATLAB_T* p_mat, char *varname, ADE_UINT32_T *p_size);
ADE_API_RET_T ADE_Matlab_GetNElements(ADE_MATLAB_T* p_mat, char *varname,ADE_UINT32_T *p_NoElements);
ADE_API_RET_T ADE_Matlab_GetScalar(ADE_MATLAB_T* p_mat, char *varname,double *p_dat);
ADE_API_RET_T ADE_Matlab_GetLength(ADE_MATLAB_T* p_mat, char *varname,ADE_UINT32_T *p_len);
ADE_API_RET_T ADE_Matlab_GetDataPointer(ADE_MATLAB_T* p_mat, char *varname,double **dp_data);
/*****************************************Set Methods *************************/

/********************** Utils Methods ****************/
ADE_API_RET_T ADE_Matlab_PutVarintoWorkspace(ADE_MATLAB_T* p_mat, double *p_var, char *var_matname, ADE_UINT32_T var_rows, ADE_UINT32_T var_cols, ADE_MATH_ATTRIBUTE_T comp_type);
ADE_API_RET_T ADE_Matlab_Evaluate_String(ADE_MATLAB_T* p_mat, char *matcode);
ADE_API_RET_T ADE_Matlab_Evaluate_StringnWait(ADE_MATLAB_T* p_mat, char *matcode);
ADE_API_RET_T ADE_Matlab_Configure_Iir_sos(ADE_MATLAB_T* p_mat,ADE_IIR_T *p_iir, char *sosmatrix_varname, char *scalevalues_varname,ADE_INT32_T buff_len,ADE_IIR_IMP_CHOICE_T filt_imp_type);
ADE_API_RET_T ADE_Matlab_launch_script_segment(ADE_MATLAB_T *p_mat, char *p_stopword);
ADE_API_RET_T ADE_Matlab_Print(ADE_MATLAB_T *p_mat, FILE *p_fid);

#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_MATLAB_H

