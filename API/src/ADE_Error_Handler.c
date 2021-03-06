#include "headers/ADE_Error_Handler.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef ADE_MEX_PRINT
#include "mex.h"
#endif

//extern  ADE_Error_Handler_T ade_error_handler;

static ADE_API_RET_T ADE_Error_Handler_CheckCondition(ADE_CHAR_T *format,ADE_VOID_T *p_value, ADE_VOID_T *p_limit,ADE_VOID_T *p_limit2,
                                        ADE_ERROR_HANDLER_CHECKVALUE_T type,ADE_BOOL_T * condition);

ADE_VOID_T ADE_Error_Handler_SetError(ADE_ERRSEVERITY_T severity,ADE_ERRTYPE_T type , ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_var, ADE_CHAR_T *var_name_str,FILE* p_stream)
{
    ADE_UINT32_T ade_code=0;
    ADE_UINT32_T stack_idx=0;
    ADE_CHAR_T* p_decod_string=NULL;

    ade_code=(severity<<ADE_NBITS_SEVERITY_SHIFT | type << ADE_NBITS_TYPE_SHIFT | _class << ADE_NBITS_CLASS_SHIFT| method << ADE_NBITS_METHOD_SHIFT);
    ade_error_handler.p_error_code[stack_idx]=ade_code;

    if (ade_error_handler.error_count<(ADE_ERRSTACK_DEPTH-1))
    {
        ade_error_handler.error_count++;
    }
    else
    {

        ADE_LOG(stderr,"WARNING : HANDLER STACK OVERFLOW in ADE_Error_Handler_SetError!!!!!!!!!\n");
    }

    #if (ADE_TARGET_MODE==ADE_DEBUG)

    p_decod_string = calloc(ADE_ERRSTRING_LEN,sizeof(ADE_CHAR_T));

    ADE_Error_Handler_Decoder(ade_code, p_decod_string);
    strcat(p_decod_string," in variable %s with value ");
    strcat(p_decod_string,format);
     strcat(p_decod_string,"\n");
    if (!strcmp(format,"%p"))
    {
        ADE_LOG(p_stream,p_decod_string,var_name_str,p_var);
    }
    else if (!strcmp(format,"%d"))
    {
        ADE_LOG(p_stream,p_decod_string,var_name_str,*((ADE_INT32_T*)p_var));
    }
    else if (!strcmp(format,"%u"))
    {
        ADE_LOG(p_stream,p_decod_string,var_name_str,*((ADE_UINT32_T*)p_var));
    }
    else if (!strcmp(format,"%f"))
    {
        ADE_LOG(p_stream,p_decod_string,var_name_str,*((ADE_FLOATING_T*)p_var));
    }
    else if (!strcmp(format,"%s"))
    {
        ADE_LOG(p_stream,p_decod_string,var_name_str,*((ADE_CHAR_T*)p_var));
    }
    else if (!strcmp(format,"%c"))
    {
        ADE_LOG(p_stream,p_decod_string,var_name_str,*((ADE_CHAR_T*)p_var));
    }
    else
    {
        ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_SetError format not recognized\n");
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

ADE_API_RET_T ADE_Error_Handler_CheckMemAlloc(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_var, ADE_CHAR_T *var_name_str)
{

    #if (ADE_CHECK_MEM==ADE_CHECK_MEM_TRUE)

    FILE *p_stream=ADE_STDOUT_STREAM;

    if (p_var==NULL)
    {

        ADE_Error_Handler_SetError(ADE_ERROR,ADE_MEM,_class,method,format,p_var, var_name_str,p_stream);
        return ADE_RET_ERROR;
    }

    #elif (ADE_CHECK_MEM==ADE_CHECK_MEM_FALSE)
    ;
    #else
        #error ADE_CHECK_MEM in ADE_Error_Handler_CheckMemAlloc
    #endif

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Error_Handler_CheckInputPointer(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_var, ADE_CHAR_T *var_name_str)
{
    #if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
    FILE *p_stream=ADE_STDOUT_STREAM;

    if (p_var==NULL)
    {

        ADE_Error_Handler_SetError(ADE_ERROR,ADE_INCHECKS,_class,method,format,p_var, var_name_str,p_stream);
        return ADE_RET_ERROR;
    }
    #elif (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_FALSE)
    ;
    #else

        #error ADE_CHECK_INPUTS in ADE_Error_Handler_CheckInputPointer
    #endif

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Error_Handler_CheckReturn(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_API_RET_T *p_ret_val, ADE_CHAR_T *var_name_str)
{
    #if (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_TRUE)
        FILE *p_stream=ADE_STDOUT_STREAM;

        if (*p_ret_val==ADE_RET_ERROR)
        {
            ADE_Error_Handler_SetError(ADE_ERROR,ADE_RETCHECKS,_class,method,format,p_ret_val, var_name_str,p_stream);
             return ADE_RET_ERROR;
        }
    #elif (ADE_CHECK_RETURNS==ADE_CHECK_RETURNS_FALSE)

        ;

    #else
            #error (ADE_CHECK_RETURNS) in ADE_Error_Handler_CheckReturn
    #endif

 return ADE_RET_SUCCESS;

}


ADE_API_RET_T ADE_Error_Handler_CheckValue(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_value, ADE_VOID_T *p_limit,ADE_ERROR_HANDLER_CHECKVALUE_T type,ADE_CHAR_T *var_name_str)
{
    #if  (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
        FILE *p_stream=ADE_STDOUT_STREAM;
        ADE_BOOL_T condition = ADE_TRUE;
        ADE_INT32_T limit2_dummy=0;
        ADE_API_RET_T ret=0;

         if (type==ADE_ERROR_HANDLER_CHECKVALUE_MAJOR || type==ADE_ERROR_HANDLER_CHECKVALUE_MAJOREQUAL ||
           type==ADE_ERROR_HANDLER_CHECKVALUE_MINOR || type==ADE_ERROR_HANDLER_CHECKVALUE_MINOREQUAL || type==ADE_ERROR_HANDLER_CHECKVALUE_NOTEQUAL )
           {
                ret=ADE_Error_Handler_CheckCondition(format,p_value, p_limit,&limit2_dummy,type,&condition);
                 ADE_CHECK_ADERETVAL(ADE_CLASS_ERROR_HANDLER,ADE_METHOD_CheckValue,ret);
           }
        else{
            ADE_LOG(stderr,"type not handled in ADE_Error_Handler_CheckValue\n");
        }

        if (condition)
        {
            ADE_Error_Handler_SetError(ADE_ERROR,ADE_RETCHECKS,_class,method,format,p_value, var_name_str,p_stream);

             return ADE_RET_ERROR;
        }
    #elif  (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_FALSE)

        ;

    #else
            #error (ADE_CHECK_RETURNS) in ADE_Error_Handler_CheckValue
    #endif

 return ADE_RET_SUCCESS;

}


ADE_API_RET_T ADE_Error_Handler_CheckList(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_value,ADE_VOID_T *p_list, ADE_UINT32_T n_list_elements,ADE_CHAR_T *var_name_str)
{

    ADE_INT32_T i=0;
    ADE_INT32_T int_value=0;
    ADE_INT32_T *p_int_list=NULL;
    FILE *p_stream=ADE_STDOUT_STREAM;


    #if  (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)

    if (!strcmp(format,"%d"))
    {
        int_value=*(ADE_INT32_T*)p_value;
        p_int_list=(ADE_INT32_T*)p_list;
        for(i=0;i<n_list_elements;i++)
        {
            if ( int_value==p_int_list[i])
            {

                break;
            }
            else if (int_value!=p_int_list[i] && (i==n_list_elements-1) )
            {

                 ADE_Error_Handler_SetError(ADE_ERROR,ADE_RETCHECKS,_class,method,format,p_value, var_name_str,p_stream);

                return ADE_RET_ERROR;
            }
        }
    }
    else
    {
        ADE_LOG(stderr,"format  not handled in ADE_Error_Handler_CheckList\n");
    }

     #elif  (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_FALSE)

        ;

    #else
            #error (ADE_CHECK_RETURNS) in ADE_Error_Handler_CheckList
    #endif

     return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Error_Handler_CheckInterval(ADE_ERRCLASS_T _class,ADE_ERRMETHODS_T method,ADE_CHAR_T *format,ADE_VOID_T *p_value, ADE_VOID_T *p_limit,ADE_VOID_T *p_limit2,ADE_ERROR_HANDLER_CHECKVALUE_T type,ADE_CHAR_T *var_name_str)
{
    #if  (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)
        FILE *p_stream=ADE_STDOUT_STREAM;
        ADE_BOOL_T condition = ADE_TRUE;
        //ADE_INT32_T limit2_dummy=0;
        ADE_API_RET_T ret=0;

         if (type==ADE_ERROR_HANDLER_CHECKVALUE_L_MIN_G_MAX || type==ADE_ERROR_HANDLER_CHECKVALUE_LE_MIN_GE_MAX ||
           type==ADE_ERROR_HANDLER_CHECKVALUE_G_MIN_L_MAX || type==ADE_ERROR_HANDLER_CHECKVALUE_GE_MIN_LE_MAX ||
           type==ADE_ERROR_HANDLER_CHECKVALUE_G_MIN_LE_MAX
           )
           {
                ret=ADE_Error_Handler_CheckCondition(format,p_value, p_limit,p_limit2,type,&condition);
                 ADE_CHECK_ADERETVAL(ADE_CLASS_ERROR_HANDLER,ADE_METHOD_CheckValue,ret);
           }
        else{
            ADE_LOG(stderr,"type not handled in ADE_Error_Handler_CheckValue\n");
        }

        if (condition)
        {
            ADE_Error_Handler_SetError(ADE_ERROR,ADE_RETCHECKS,_class,method,format,p_value, var_name_str,p_stream);

             return ADE_RET_ERROR;
        }
    #elif  (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_FALSE)

        ;

    #else
            #error (ADE_CHECK_RETURNS) in ADE_Error_Handler_CheckReturn
    #endif

 return ADE_RET_SUCCESS;

}

static ADE_BOOL_T ADE_Error_isminmaxordered(ADE_VOID_T* p_lim1,ADE_VOID_T* p_lim2,ADE_CHAR_T *format)
{

    ADE_FLOATING_T lim1_int=0.0,lim2_int=0.0;
    if (!strcmp(format,"%d"))
    {
        lim1_int=(ADE_FLOATING_T)(*(ADE_INT32_T*)p_lim1);
        lim2_int=(ADE_FLOATING_T)(*(ADE_INT32_T*)p_lim2);
    }
    else if (!strcmp(format,"%u"))
    {
        lim1_int=(ADE_FLOATING_T)(*(ADE_UINT32_T*)p_lim1);
        lim2_int=(ADE_FLOATING_T)(*(ADE_UINT32_T*)p_lim2);
    }
    else if (!strcmp(format,"%f"))
    {
        lim1_int=(*(ADE_FLOATING_T*)p_lim1);
        lim2_int=(*(ADE_FLOATING_T*)p_lim2);
    }
    else
    {
        fprintf(ADE_STDERR_STREAM,"ERROR IN ADE_Error_isminmaxordered format %s not handled\n",format);
        return ADE_FALSE;
    }

    return (lim1_int<=lim2_int);
}

static ADE_API_RET_T ADE_Error_Handler_CheckCondition(ADE_CHAR_T *format,ADE_VOID_T *p_value, ADE_VOID_T *p_limit,ADE_VOID_T *p_limit2,
                                        ADE_ERROR_HANDLER_CHECKVALUE_T type,ADE_BOOL_T * condition)
{

ADE_INT32_T value_int32=0;
ADE_INT32_T limit1_int32=0;
ADE_INT32_T limit2_int32=0;
ADE_UINT32_T value_uint32=0;
ADE_UINT32_T limit1_uint32=0;
ADE_UINT32_T limit2_uint32=0;
ADE_FLOATING_T value_float=0;
ADE_FLOATING_T limit1_float=0;
ADE_FLOATING_T limit2_float=0;

 if (!strcmp(format,"%p"))
        {
           ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition value cannot be a pointer\n");
            return ADE_RET_ERROR;
        }
        else if (!strcmp(format,"%d"))
        {
            value_int32=*(ADE_INT32_T*)p_value;
            limit1_int32=*(ADE_INT32_T*)p_limit;
            limit2_int32=*(ADE_INT32_T*)p_limit2;

            if (type==ADE_ERROR_HANDLER_CHECKVALUE_MAJOR)
            {
                *condition = ( value_int32<=limit1_int32 );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : value %d <= %d\n",value_int32,limit1_int32);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_MAJOREQUAL)
            {
                *condition = ( value_int32<limit1_int32 );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : value %d < %d\n",value_int32,limit1_int32);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_MINOR)
            {
                *condition = ( value_int32>=limit1_int32 );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : value %d >= %d\n",value_int32,limit1_int32);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_MINOREQUAL)
            {
                *condition = ( value_int32>limit1_int32 );
                 if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : value %d > %d\n",value_int32,limit1_int32);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_EQUAL)
            {
                *condition = ( value_int32!=limit1_int32 );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Value %d != %d\n",value_int32,limit1_int32);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_NOTEQUAL)
            {
                *condition = ( value_int32==limit1_int32 );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Value %d == %d\n",value_int32,limit1_int32);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_L_MIN_G_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_int32>=limit1_int32 && value_int32<=limit2_int32);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %d <= value %d <= %d\n",limit1_int32,value_int32,limit2_int32);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_G_MIN_L_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_int32<=limit1_int32 || value_int32>=limit2_int32);
                     if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %d >= value %d >= %d\n",limit1_int32,value_int32,limit2_int32);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_LE_MIN_GE_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_int32>limit1_int32 && value_int32<limit2_int32);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %d < value %d < %d\n",limit1_int32,value_int32,limit2_int32);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_GE_MIN_LE_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_int32<limit1_int32 || value_int32>limit2_int32);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %d > value %d > %d\n",limit1_int32,value_int32,limit2_int32);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_G_MIN_LE_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_int32<=limit1_int32 || value_int32>limit2_int32);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %d >= value %d > %d\n",limit1_int32,value_int32,limit2_int32);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else
            {

                ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition type not recognized\n");
                 return ADE_RET_ERROR;
            }
        }
        else if (!strcmp(format,"%u"))
        {
             value_uint32=*(ADE_UINT32_T*)p_value;
            limit1_uint32=*(ADE_UINT32_T*)p_limit;
            limit2_uint32=*(ADE_UINT32_T*)p_limit2;
             if (type==ADE_ERROR_HANDLER_CHECKVALUE_MAJOR)
            {
                *condition = ( value_uint32<=limit1_uint32 );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : value %u <= %u\n",value_uint32,limit1_uint32);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_MAJOREQUAL)
            {
                *condition = ( value_uint32<limit1_uint32 );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : value %u < %u\n",value_uint32,limit1_uint32);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_MINOR)
            {
                *condition = ( value_uint32>=limit1_uint32 );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : value %u >= %u\n",value_uint32,limit1_uint32);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_MINOREQUAL)
            {
                *condition = ( value_uint32>limit1_uint32 );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : value %u > %u\n",value_uint32,limit1_uint32);
            }
             else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_EQUAL)
            {
                *condition = ( value_uint32!=limit1_uint32 );
                 if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Value %u != %u\n",value_uint32,limit1_uint32);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_NOTEQUAL)
            {
                *condition = ( value_uint32==limit1_uint32 );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Value %u == %u\n",value_uint32,limit1_uint32);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_L_MIN_G_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_uint32>=limit1_uint32 && value_uint32<=limit2_uint32);
                     if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %u <= value %u <= %u\n",limit1_uint32,value_uint32,limit2_uint32);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_G_MIN_L_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_uint32<=limit1_uint32 || value_uint32>=limit2_uint32);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %u >= value %u >= %u\n",limit1_uint32,value_uint32,limit2_uint32);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_LE_MIN_GE_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_uint32>limit1_uint32 && value_uint32<limit2_uint32);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %u < value %u < %u\n",limit1_uint32,value_uint32,limit2_uint32);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_GE_MIN_LE_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_uint32<limit1_uint32 || value_uint32>limit2_uint32);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %u > value %u > %u\n",limit1_uint32,value_uint32,limit2_uint32);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_G_MIN_LE_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_uint32<=limit1_uint32 || value_uint32>limit2_uint32);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %u >= value %u > %u\n",limit1_uint32,value_uint32,limit2_uint32);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else
            {

                ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition type not recognized\n");
                 return ADE_RET_ERROR;
            }
        }
        else if (!strcmp(format,"%f"))
        {
            value_float=*(ADE_FLOATING_T*)p_value;
            limit1_float=*(ADE_FLOATING_T*)p_limit;
            limit2_float=*(ADE_FLOATING_T*)p_limit2;
             if (type==ADE_ERROR_HANDLER_CHECKVALUE_MAJOR)
            {
                *condition = ( value_float<=limit1_float );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : value %f <= %f\n",value_float,limit1_float);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_MAJOREQUAL)
            {
                *condition = ( value_float<limit1_float );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : value %f < %f\n",value_float,limit1_float);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_MINOR)
            {
                *condition = ( value_float>=limit1_float );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : value %f >= %f\n",value_float,limit1_float);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_MINOREQUAL)
            {
                *condition = ( value_float>limit1_float );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : value %f > %f\n",value_float,limit1_float);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_EQUAL)
            {
                *condition = ( value_float!=limit1_float );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Value %f != %f\n",value_float,limit1_float);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_NOTEQUAL)
            {
                *condition = ( value_float==limit1_float );
                if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Value %f == %f\n",value_float,limit1_float);
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_L_MIN_G_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_float>=limit1_float && value_float<=limit2_float);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %f <= value %f <= %f\n",limit1_float,value_float,limit2_float);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_G_MIN_L_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_float<=limit1_float || value_float>=limit2_float);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %f >= value %f >= %f\n",limit1_float,value_float,limit2_float);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_LE_MIN_GE_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_float>limit1_float && value_float<limit2_float);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %f < value %f < %f\n",limit1_float,value_float,limit2_float);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_GE_MIN_LE_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_float<limit1_float || value_float>limit2_float);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %f > value %f > %f\n",limit1_float,value_float,limit2_float);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else if  (type==ADE_ERROR_HANDLER_CHECKVALUE_G_MIN_LE_MAX)
            {
                if ( ADE_Error_isminmaxordered(p_limit,p_limit2,format))
                {
                    *condition = ( value_float<=limit1_float || value_float>limit2_float);
                    if (*condition) ADE_LOG(ADE_STDERR_STREAM,"**** Exceeded bounds : %f >= value %f > %f\n",limit1_float,value_float,limit2_float);
                }
                else
                {
                    ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition minmax not ordered\n");
                    return ADE_RET_ERROR;
                }
            }
            else
            {

                ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition type not recognized\n");
                 return ADE_RET_ERROR;
            }
        }
        else
        {
            ADE_LOG(stderr,"ERROR IN ADE_Error_Handler_CheckCondition format not recognized\n");
             return ADE_RET_ERROR;
        }

    return ADE_RET_SUCCESS;
}
