#include "headers/ADE_Error_Handler.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

extern  ADE_Error_Handler_T ade_error_handler;

ADE_VOID_T ADE_Error_Handler_SetError(ADE_ERRSEVERITY_T severity,ADE_ERRTYPE_T type , ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_var, ADE_CHAR_T *var_name_str,FILE* p_stream)
{
    ADE_UINT32_T ade_code=0;
    ADE_UINT32_T stack_idx=0;
    ADE_CHAR_T* p_decod_string=NULL;

//    ADE_UINT32_T temp;
//
//    stack_idx=ade_error_handler.error_count;
//temp=severity<<ADE_NBITS_SEVERITY_SHIFT;
//temp=type << ADE_NBITS_TYPE_SHIFT;
//temp=_class << ADE_NBITS_CLASS_SHIFT;
//temp=method << ADE_NBITS_METHOD_SHIFT;

    ade_code=(severity<<ADE_NBITS_SEVERITY_SHIFT | type << ADE_NBITS_TYPE_SHIFT | _class << ADE_NBITS_CLASS_SHIFT| method << ADE_NBITS_METHOD_SHIFT);
    ade_error_handler.p_error_code[stack_idx]=ade_code;

    if (ade_error_handler.error_count<(ADE_ERRSTACK_DEPTH-1))
    {
        ade_error_handler.error_count++;
    }
    else
    {

        fprintf(stderr,"ERR STACK OVERFLOW!!!!!!!!!\n");
    }

    #if (ADE_TARGET_MODE==ADE_DEBUG)

    p_decod_string = calloc(ADE_ERRSTRING_LEN,sizeof(ADE_CHAR_T));

    ADE_Error_Handler_Decoder(ade_code, p_decod_string);
    strcat(p_decod_string," in variable %s with value");
    strcat(p_decod_string,format);
     strcat(p_decod_string,"\n");
    if (!strcmp(format,"%p"))
    {
        fprintf(p_stream,p_decod_string,var_name_str,*((ADE_CHAR_T*)p_var));
    }
    else if (!strcmp(format,"%d"))
    {
        fprintf(p_stream,p_decod_string,var_name_str,*((ADE_INT32_T*)p_var));
    }
    else if (!strcmp(format,"%u"))
    {
        fprintf(p_stream,p_decod_string,var_name_str,*((ADE_UINT32_T*)p_var));
    }
    else if (!strcmp(format,"%f"))
    {
        fprintf(p_stream,p_decod_string,var_name_str,*((ADE_FLOATING_T*)p_var));
    }
    else if (!strcmp(format,"%s"))
    {
        fprintf(p_stream,p_decod_string,var_name_str,*((ADE_CHAR_T*)p_var));
    }
    else if (!strcmp(format,"%c"))
    {
        fprintf(p_stream,p_decod_string,var_name_str,*((ADE_CHAR_T*)p_var));
    }
    else
    {
        fprintf(stderr,"ERROR IN ADE_Error_Handler_SetError format not recognized\n");
    }

    free(p_decod_string);


    #endif

}

ADE_VOID_T ADE_Error_Handler_Decoder(ADE_UINT32_T err_code, ADE_CHAR_T* p_decod_string)
{

    ADE_UINT32_T severity_mask=((ADE_UINT32_T)pow(2,ADE_NBITS_SEVERITY)-1)<<ADE_NBITS_SEVERITY_SHIFT;
    ADE_UINT32_T type_mask=((ADE_UINT32_T)pow(2,ADE_NBITS_TYPE)-1)<<ADE_NBITS_TYPE_SHIFT;
    ADE_UINT32_T class_mask=((ADE_UINT32_T)pow(2,ADE_NBITS_CLASS)-1)<<ADE_NBITS_CLASS_SHIFT;
    ADE_UINT32_T method_mask=((ADE_UINT32_T)pow(2,ADE_NBITS_METHOD)-1)<<ADE_NBITS_METHOD_SHIFT;
    ADE_UINT32_T severity=0;
    ADE_UINT32_T type=0;
    ADE_UINT32_T _class=0;
    ADE_UINT32_T method=0;
    //ADE_CHAR_T* p_decod_string=NULL;
//     ADE_CHAR_T *severity_str=0;
//    ADE_CHAR_T *type_str=0;
//    ADE_CHAR_T *_class_str=0;
//    ADE_CHAR_T *method_str=0;


    severity=(severity_mask&err_code)>>ADE_NBITS_SEVERITY_SHIFT;
    type=(type_mask&err_code)>>ADE_NBITS_TYPE_SHIFT;
    _class=(class_mask&err_code)>>ADE_NBITS_CLASS_SHIFT;
    method=(method_mask&err_code)>>ADE_NBITS_METHOD_SHIFT;

    strcat(p_decod_string,ADE_ERRSEVERITYStrings[severity]);
    strcat(p_decod_string," ");
    strcat(p_decod_string,ADE_ERRTYPEStrings[type]);
    strcat(p_decod_string," in Class ->");
    strcat(p_decod_string,ADE_ERRCLASSStrings[_class]);
    strcat(p_decod_string," in Method ->");
    strcat(p_decod_string,ADE_ERRMETHODSStrings[method]);
    strcat(p_decod_string," ");



}

