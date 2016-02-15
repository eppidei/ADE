#ifndef _ADE_BLOW_H
#define _ADE_BLOW_H
#include "headers/ADE_typedefs.h"
#ifdef ADE_CONFIGURATION_INTERACTIVE
#include "headers/ADE_Matlab.h"
#include "matrix.h"
#include "engine.h"
#endif



#ifdef __cplusplus
    extern "C" {
#endif

/***************** Init Methods *************************/
ADE_API_RET_T ADE_Blow_Init(ADE_BLOW_T** dp_this);//,ADE_UINT32_T buff_size,ADE_FLOATING_T Fs_i,ADE_FLOATING_T Fs_o_i);
ADE_VOID_T ADE_Blow_Release(ADE_BLOW_T* p_blow);
/************* Set Methods *************************/
ADE_API_RET_T ADE_Blow_SetMatlab(ADE_BLOW_T* p_blow, ADE_MATLAB_T *p_mat);
/************* Get Methods *************************/
ADE_API_RET_T ADE_Blow_GetFs(ADE_BLOW_T* p_blow, ADE_FLOATING_T *p_Fs);
ADE_API_RET_T ADE_Blow_GetFsOut(ADE_BLOW_T* p_blow, ADE_FLOATING_T *p_Fs);
ADE_API_RET_T ADE_Blow_GetOutBuff(ADE_BLOW_T* p_blow, ADE_FLOATING_T **dp_buff);
ADE_API_RET_T ADE_Blow_GetBuffLen(ADE_BLOW_T* p_blow, ADE_INT32_T *p_BuffLen);
ADE_API_RET_T ADE_Blow_GetMatlab(ADE_BLOW_T* p_blow, ADE_MATLAB_T **dp_mat);
ADE_API_RET_T ADE_Blow_GetState(ADE_BLOW_T* p_blow, ADE_BOOL_T *p_state);
ADE_API_RET_T ADE_Blow_GetOut(ADE_BLOW_T* p_blow, ADE_FLOATING_T **dp_Out);
/******************* Configure Methods ************************/
ADE_API_RET_T ADE_Blow_Configure_bufflength(ADE_BLOW_T* p_blow,ADE_INT32_T in_buff_len);
ADE_API_RET_T ADE_Blow_Configure_params(ADE_BLOW_T* p_blow,ADE_FLOATING_T Fs_i,ADE_FLOATING_T Fs_o);
ADE_API_RET_T ADE_Blow_Configure_inout(ADE_BLOW_T* p_blow,ADE_FLOATING_T *p_inbuff);
ADE_API_RET_T ADE_Blow_Configure(ADE_BLOW_T* p_blow,ADE_FLOATING_T *p_inbuff,ADE_FLOATING_T Fs_i,ADE_FLOATING_T Fs_o,ADE_INT32_T in_buff_len);
/************ Processing methods ***********************/
ADE_API_RET_T ADE_Blow_Step(ADE_BLOW_T* p_blow);
/************ Utils methods ***********************/
ADE_API_RET_T ADE_Blow_Print(ADE_BLOW_T* p_blow, ADE_FILE_T *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj);

#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_BLOW_H
