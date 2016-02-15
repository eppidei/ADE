#ifndef _ADE_SOURCES_H
#define _ADE_SOURCES_H
#include "headers/ADE_defines.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_SCDF_Interface.h"


#ifdef __cplusplus
    extern "C" {
#endif
/*************** Init Methods ****************/
ADE_API_RET_T ADE_Sources_Init( ADE_SOURCES_T ** dp_sources);
ADE_VOID_T ADE_Sources_Release(ADE_SOURCES_T * p_sources);
/*************** Get Methods ****************/
ADE_API_RET_T Ade_Sources_GetData(ADE_SOURCES_T * p_sources,ADE_FLOATING_T **dp_data);
ADE_API_RET_T Ade_Sources_GetNoCols(ADE_SOURCES_T * p_sources,ADE_INT32_T *p_NumCols);
ADE_API_RET_T Ade_Sources_GetNoRows(ADE_SOURCES_T * p_sources,ADE_INT32_T *p_NumRows);
ADE_API_RET_T Ade_Sources_GetOffsetRadians(ADE_SOURCES_T * p_sources,ADE_FLOATING_T *p_off);
ADE_API_RET_T Ade_Sources_GetLastPhase(ADE_SOURCES_T * p_sources,ADE_FLOATING_T  *p_phase);
ADE_API_RET_T Ade_Sources_GetFc(ADE_SOURCES_T * p_sources,ADE_FLOATING_T  *p_Fc);
ADE_API_RET_T Ade_Sources_GetFs(ADE_SOURCES_T * p_sources,ADE_FLOATING_T  *p_Fs);
ADE_API_RET_T Ade_Sources_GetGain(ADE_SOURCES_T * p_sources,ADE_FLOATING_T  *p_gain);
ADE_API_RET_T Ade_Sources_GetSrcType(ADE_SOURCES_T * p_sources,ADE_SOURCES_TYPE_T  *p_type);
ADE_API_RET_T Ade_Sources_GetMt(ADE_SOURCES_T * p_sources,unsigned long **dp_mt);

/******************** SET Methods ***********************************/
ADE_API_RET_T ADE_Sources_SetMatlab(ADE_SOURCES_T * p_sources, ADE_MATLAB_T *p_mat);
ADE_API_RET_T ADE_Sources_SetConfigFromMatlab(ADE_SOURCES_T * p_sources,ADE_MATLAB_T *p_mat,ADE_SOURCES_TYPE_T src_type,ADE_CHAR_T* n_cols_varname,ADE_CHAR_T* fs_varname,ADE_CHAR_T* gain_varname, ... );
ADE_API_RET_T Ade_Sources_SetLastPhase(ADE_SOURCES_T * p_sources,ADE_FLOATING_T phase);
ADE_API_RET_T ADE_Sources_SetMatlabInBuffName(ADE_SOURCES_T * p_sources,ADE_CHAR_T* varname);
ADE_API_RET_T ADE_Sources_SetMatlabInLenVarname(ADE_SOURCES_T * p_sources,ADE_CHAR_T* varname);
ADE_API_RET_T ADE_Sources_SetMatlabFsamplingVarName(ADE_SOURCES_T * p_sources,ADE_CHAR_T* varname);
/*************** Configure Methods ****************/
ADE_API_RET_T ADE_Sources_ConfigureParams(ADE_SOURCES_T * p_sources,ADE_SOURCES_TYPE_T src_type,ADE_INT32_T n_cols,ADE_FLOATING_T fs,ADE_FLOATING_DP_T gain, ... );
ADE_API_RET_T ADE_Sources_ConfigureFromMatlab(ADE_SOURCES_T * p_sources,ADE_MATLAB_T *p_mat,ADE_SOURCES_TYPE_T src_type,ADE_CHAR_T* InBuffVarname,ADE_CHAR_T* n_cols_varname,ADE_CHAR_T* fs_varname,ADE_CHAR_T* gain_varname);
/*************** Processing Methods ****************/
ADE_API_RET_T Ade_Sources_FillSensorData(ADE_SOURCES_T * p_sources,ADE_SCDF_Input_Int_T *p_SensorData);
ADE_API_RET_T Ade_Sources_Step(ADE_SOURCES_T * p_sources);
#ifdef __cplusplus
    }   /* extern "C" */
#endif
#endif //_ADE_SOURCES_H


