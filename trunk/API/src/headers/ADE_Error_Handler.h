#ifndef _ADE_ERROR_HANDLER_H
#define _ADE_ERROR_HANDLER_H

#include "headers/ADE_typedefs.h"
#include "headers/ADE_errors.h"
#include <stdio.h>


static ADE_Error_Handler_T ade_error_handler;

#define ADE_PRINT_ERRORS(sev,type,class,met,var,format,stream)  ADE_Error_Handler_SetError(sev,type,class,met,format,&(var),#var,(FILE*)stream)
#define ADE_CHECK_MEMALLOC(class,met,var) ADE_Error_Handler_CheckMemAlloc(class,met,"%p",&(var),#var);return ADE_RET_ERROR
#define ADE_CHECK_INPUTPOINTER(class,met,var) ADE_Error_Handler_CheckInputPointer(class,met,"%p",&(var),#var);return ADE_RET_ERROR
#define ADE_CHECK_ADERETVAL(class,met,var) ADE_Error_Handler_CheckReturn(class,met,"%p",&(var),#var);return ADE_RET_ERROR

#ifdef __cplusplus
    extern "C" {
#endif

ADE_VOID_T ADE_Error_Handler_SetError(ADE_ERRSEVERITY_T severity,ADE_ERRTYPE_T type , ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_var,ADE_CHAR_T *var_name_str, FILE* p_stream);
ADE_VOID_T ADE_Error_Handler_Decoder(ADE_UINT32_T err_code, ADE_CHAR_T* p_decod_string);
ADE_VOID_T ADE_Error_Handler_CheckMemAlloc(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_var, ADE_CHAR_T *var_name_str);
ADE_VOID_T ADE_Error_Handler_CheckInputPointer(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_var, ADE_CHAR_T *var_name_str);
ADE_VOID_T ADE_Error_Handler_CheckReturn(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_INT32_T *p_ret_val, ADE_CHAR_T *var_name_str);

#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif //_ADE_RET_ERROR;//RROR_HANDLER_H