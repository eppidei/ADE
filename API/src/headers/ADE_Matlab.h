#ifndef _ADE_MATLAB_H
#define _ADE_MATLAB_H
#include "headers/ADE_defines.h"
#include "headers/ADE_typedefs.h"
#include "engine.h"

struct ADE_MATLAB_S
{
    unsigned int n_vars;
    Engine *p_eng;
    char **dp_var_list;
    double **dp_vardouble;
    unsigned int *n_row;
    unsigned int *n_col;
    size_t *data_size;

};

ADE_API_RET_T ADE_Matlab_Init(ADE_MATLAB_T** dp_this, char ** dp_var_list, ADE_UINT32_T n_vars,Engine *p_mateng,char* filename,char *p_matpath);
ADE_VOID_T ADE_Matlab_Release(ADE_MATLAB_T* p_mat);
ADE_UINT32_T ADE_Matlab_GetVarIndex(ADE_MATLAB_T* p_mat, char *varname);
ADE_UINT32_T ADE_Matlab_GetNRows(ADE_MATLAB_T* p_mat, char *varname);
ADE_UINT32_T ADE_Matlab_GetNCols(ADE_MATLAB_T* p_mat, char *varname);
ADE_UINT32_T ADE_Matlab_GetSize(ADE_MATLAB_T* p_mat, char *varname);
ADE_UINT32_T ADE_Matlab_GetNElements(ADE_MATLAB_T* p_mat, char *varname);
double* ADE_Matlab_GetDataPointer(ADE_MATLAB_T* p_mat, char *varname);
ADE_API_RET_T ADE_Matlab_PutVarintoWorkspace(ADE_MATLAB_T* p_mat, double *p_var, char *var_matname, ADE_UINT32_T var_rows, ADE_UINT32_T var_cols, ADE_UINT32_T comp_type);
ADE_API_RET_T ADE_Matlab_Evaluate_String(ADE_MATLAB_T* p_mat, char *matcode);
ADE_API_RET_T ADE_Matlab_Evaluate_StringnWait(ADE_MATLAB_T* p_mat, char *matcode);
ADE_API_RET_T ADE_Matlab_Configure_Iir_sos(ADE_MATLAB_T* p_mat,ADE_IIR_T *p_iir, char *sosmatrix_varname, char *scalevalues_varname);



#endif //_ADE_MATLAB_H

