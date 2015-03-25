#ifndef _ADE_ERROR_HANDLER_H
#define _ADE_ERROR_HANDLER_H

#include "headers/ADE_typedefs.h"
#include "headers/ADE_errors.h"
#include <stdio.h>

ADE_VOID_T ADE_Error_Handler_SetError(ADE_ERRSEVERITY_T severity,ADE_ERRTYPE_T type , ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_var,ADE_CHAR_T *var_name_str, FILE* p_stream);
ADE_VOID_T ADE_Error_Handler_Decoder(ADE_UINT32_T err_code, ADE_CHAR_T* p_decod_string);
#endif //_ADE_RET_ERROR;//RROR_HANDLER_H
