#ifndef _ADE_ERROR_HANDLER_H
#define _ADE_ERROR_HANDLER_H

#include "headers/ADE_typedefs.h"
#include "headers/ADE_errors.h"
#include <stdio.h>


static ADE_Error_Handler_T ade_error_handler;

#define ADE_PRINT_ERRORS(sev,type,class,met,var,format,stream)  ADE_Error_Handler_SetError(sev,type,class,met,format,&(var),#var,(FILE*)stream)
#define ADE_CHECK_MEMALLOC(class,met,var) if(ADE_Error_Handler_CheckMemAlloc(class,met,"%p",&(var),#var)==ADE_RET_ERROR) return ADE_RET_ERROR
#define ADE_CHECK_INPUTPOINTER(class,met,var) if(ADE_Error_Handler_CheckInputPointer(class,met,"%p",var,#var)==ADE_RET_ERROR) return ADE_RET_ERROR
#define ADE_CHECK_INPUTPOINTER_NORET(class,met,var) ADE_Error_Handler_CheckInputPointer(class,met,"%p",var,#var)
#define ADE_CHECK_ADERETVAL(class,met,var) if(ADE_Error_Handler_CheckReturn(class,met,"%d",&(var),#var)==ADE_RET_ERROR) return ADE_RET_ERROR
#define ADE_CHECK_VALUE_EQUAL(class,met,var,format,val) if(ADE_Error_Handler_CheckValue(class,met,format,&(var),&(val),ADE_ERROR_HANDLER_CHECKVALUE_EQUAL,#var)==ADE_RET_ERROR) return ADE_RET_ERROR
#define ADE_CHECK_VALUE_NOTEQUAL(class,met,var,format,val) if(ADE_Error_Handler_CheckValue(class,met,format,&(var),&(val),ADE_ERROR_HANDLER_CHECKVALUE_NOTEQUAL,#var)==ADE_RET_ERROR) return ADE_RET_ERROR
#define ADE_CHECK_VALUE_MAJOR(class,met,var,format,val) if(ADE_Error_Handler_CheckValue(class,met,format,&(var),&(val),ADE_ERROR_HANDLER_CHECKVALUE_MAJOR,#var)==ADE_RET_ERROR) return ADE_RET_ERROR
#define ADE_CHECK_VALUE_MAJOREQUAL(class,met,var,format,val) if(ADE_Error_Handler_CheckValue(class,met,format,&(var),&(val),ADE_ERROR_HANDLER_CHECKVALUE_MAJOREQUAL,#var)==ADE_RET_ERROR) return ADE_RET_ERROR
#define ADE_CHECK_INTERVAL_L_MIN_G_MAX(class,met,var,format,val1,val2) if(ADE_Error_Handler_CheckInterval(class,met,format,&(var),&(val1),&(val2),ADE_ERROR_HANDLER_CHECKVALUE_L_MIN_G_MAX,#var)==ADE_RET_ERROR) return ADE_RET_ERROR
#define ADE_CHECK_INTERVAL_LE_MIN_GE_MAX(class,met,var,format,val1,val2) if(ADE_Error_Handler_CheckInterval(class,met,format,&(var),&(val1),&(val2),ADE_ERROR_HANDLER_CHECKVALUE_LE_MIN_GE_MAX,#var)==ADE_RET_ERROR) return ADE_RET_ERROR
#define ADE_CHECK_INTERVAL_G_MIN_L_MAX(class,met,var,format,val1,val2) if(ADE_Error_Handler_CheckInterval(class,met,format,&(var),&(val1),&(val2),ADE_ERROR_HANDLER_CHECKVALUE_G_MIN_L_MAX,#var)==ADE_RET_ERROR) return ADE_RET_ERROR
#define ADE_CHECK_INTERVAL_GE_MIN_LE_MAX(class,met,var,format,val1,val2) if(ADE_Error_Handler_CheckInterval(class,met,format,&(var),&(val1),&(val2),ADE_ERROR_HANDLER_CHECKVALUE_GE_MIN_LE_MAX,#var)==ADE_RET_ERROR) return ADE_RET_ERROR
#define ADE_CHECK_INTERVAL_G_MIN_LE_MAX(class,met,var,format,val1,val2) if(ADE_Error_Handler_CheckInterval(class,met,format,&(var),&(val1),&(val2),ADE_ERROR_HANDLER_CHECKVALUE_G_MIN_LE_MAX,#var)==ADE_RET_ERROR) return ADE_RET_ERROR
#define ADE_CHECK_VALUE_LIST(class,met,var,format,p_list,n_ele) if(ADE_Error_Handler_CheckList(class,met,format,&(var),p_list,n_ele,#var)==ADE_RET_ERROR) return ADE_RET_ERROR


#ifdef __cplusplus
    extern "C" {
#endif

ADE_VOID_T ADE_Error_Handler_SetError(ADE_ERRSEVERITY_T severity,ADE_ERRTYPE_T type , ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_var,ADE_CHAR_T *var_name_str, FILE* p_stream);
ADE_VOID_T ADE_Error_Handler_Decoder(ADE_UINT32_T err_code, ADE_CHAR_T* p_decod_string);
ADE_API_RET_T ADE_Error_Handler_CheckMemAlloc(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_var, ADE_CHAR_T *var_name_str);
ADE_API_RET_T ADE_Error_Handler_CheckInputPointer(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_var, ADE_CHAR_T *var_name_str);
ADE_API_RET_T ADE_Error_Handler_CheckReturn(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_API_RET_T *p_ret_val, ADE_CHAR_T *var_name_str);
ADE_API_RET_T ADE_Error_Handler_CheckValue(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_value, ADE_VOID_T *p_limit,ADE_ERROR_HANDLER_CHECKVALUE_T type,ADE_CHAR_T *var_name_str);
ADE_API_RET_T ADE_Error_Handler_CheckInterval(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_value, ADE_VOID_T *p_limit,ADE_VOID_T *p_limit2,ADE_ERROR_HANDLER_CHECKVALUE_T type,ADE_CHAR_T *var_name_str);
ADE_API_RET_T ADE_Error_Handler_CheckList(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_value,ADE_VOID_T *p_list, ADE_UINT32_T n_list_elements,ADE_CHAR_T *var_name_str);
#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif //_ADE_RET_ERROR;//RROR_HANDLER_H
