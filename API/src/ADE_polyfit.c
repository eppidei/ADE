#include "headers/ADE_polyfit.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "headers/ADE_Utils.h"
#include "headers/ADE_Error_Handler.h"




ADE_API_RET_T ADE_Polyfit_Init (ADE_POLYFIT_T **dp_poly)
{
    ADE_POLYFIT_T *p_this=NULL;
    ADE_UINT32_T poly_pieces = 0;
    ADE_UINT32_T max_poly_order = ADE_POLYFIT_MAX_POLY_ORDER;
    ADE_UINT32_T max_n_breaks = ADE_POLYFIT_MAX_N_BREAKS;

    p_this=calloc(1,sizeof(ADE_POLYFIT_T));
     ADE_CHECK_MEMALLOC(ADE_CLASS_POLYFIT,ADE_METHOD_Init, p_this);


        p_this->n_breaks=0;
         p_this->max_n_breaks=max_n_breaks;
        p_this->p_breaks=calloc(max_n_breaks,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_POLYFIT,ADE_METHOD_Init,  p_this->p_breaks);

        poly_pieces = p_this->max_n_breaks-1;
        if (poly_pieces<=0)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_POLYFIT,ADE_METHOD_Init,poly_pieces,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;//22;
        }

        p_this->poly_order=0;
         p_this->max_poly_order=max_poly_order;
        p_this->p_poly_coeffs=calloc(poly_pieces*(max_poly_order+1),sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_POLYFIT,ADE_METHOD_Init, p_this->p_poly_coeffs);

        *dp_poly=p_this;

    return ADE_RET_SUCCESS;

}

ADE_VOID_T ADE_Polyfit_Release(ADE_POLYFIT_T *p_poly)
{
    ADE_CHECKNFREE(p_poly->p_breaks);
    ADE_CHECKNFREE(p_poly->p_poly_coeffs);
    ADE_CHECKNFREE(p_poly);

}

ADE_API_RET_T ADE_Polyfit_Configure_params(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_breaks_i,ADE_UINT32_T n_breaks,ADE_FLOATING_T *p_coeffs_i,ADE_UINT32_T n_coeffs)
{
    ADE_API_RET_T ret_breaks = ADE_RET_ERROR;
    ADE_API_RET_T ret_coeffs = ADE_RET_ERROR;
    ADE_INT32_T break_min_val=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,p_breaks_i);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,p_poly);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,p_coeffs_i);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,n_breaks,"%d",break_min_val);

    ret_breaks = ADE_Polyfit_SetBreaks(p_poly,p_breaks_i,n_breaks);
    ADE_CHECK_ADERETVAL(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,ret_breaks);

    ret_coeffs = ADE_Polyfit_SetCoeffs(p_poly,p_coeffs_i,n_coeffs);
     ADE_CHECK_ADERETVAL(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,ret_coeffs);


    return ADE_RET_SUCCESS;


}

ADE_API_RET_T ADE_Polyfit_Configure_inout(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_out,ADE_INT32_T buff_len)
{
    ADE_API_RET_T ret_in=ADE_RET_ERROR;
    ADE_API_RET_T ret_out=ADE_RET_ERROR;
     ADE_API_RET_T ret_len=ADE_RET_ERROR;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Configure_inout,p_in);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Configure_inout,p_poly);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Configure_inout,p_out);

     ret_in=ADE_Polyfit_SetInBuff(p_poly,p_in);
     ADE_CHECK_ADERETVAL(ADE_CLASS_POLYFIT,ADE_METHOD_Configure_inout,ret_in);

     ret_out=ADE_Polyfit_SetOutBuff(p_poly,p_out);
     ADE_CHECK_ADERETVAL(ADE_CLASS_POLYFIT,ADE_METHOD_Configure_inout,ret_in);

     ret_len=ADE_Polyfit_SetBuffLen(p_poly,buff_len);
     ADE_CHECK_ADERETVAL(ADE_CLASS_POLYFIT,ADE_METHOD_Configure_inout,ret_len);

      return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Polyfit_Configure(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_breaks_i,ADE_UINT32_T n_breaks,ADE_FLOATING_T *p_coeffs_i,ADE_UINT32_T n_coeffs,ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_out,ADE_INT32_T buff_len)
{

    ADE_API_RET_T ret_par=ADE_RET_ERROR;
     ADE_API_RET_T ret_inout=ADE_RET_ERROR;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,p_in);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,p_poly);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,p_out);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,p_breaks_i);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,p_coeffs_i);


    ret_par=ADE_Polyfit_Configure_params(p_poly,p_breaks_i,n_breaks,p_coeffs_i,n_coeffs);
    ADE_CHECK_ADERETVAL(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,ret_par);

    ret_inout=ADE_Polyfit_Configure_inout(p_poly,p_in,p_out,buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_POLYFIT,ADE_METHOD_Configure,ret_inout);

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Polyfit_SetInBuff(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_in)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetInBuff,p_in);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetInBuff,p_poly);

     p_poly->p_in=p_in;

     return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Polyfit_SetOutBuff(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_out)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetOutBuff,p_out);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetOutBuff,p_poly);

     p_poly->p_out=p_out;

     return ADE_RET_SUCCESS;

}


ADE_API_RET_T ADE_Polyfit_SetBreaks(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_breaks_i,ADE_INT32_T n_breaks)
{
    ADE_INT32_T n_breaks_limit=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetBreaks,p_breaks_i);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetBreaks,p_poly);
    ADE_CHECK_VALUE_MAJOR(ADE_CLASS_POLYFIT,ADE_METHOD_SetBreaks,n_breaks,"%d",n_breaks_limit);

    p_poly->n_breaks = n_breaks;
    memcpy(p_poly->p_breaks,p_breaks_i,p_poly->n_breaks*sizeof(ADE_FLOATING_T));

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Polyfit_SetCoeffs(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_coeffs_i,ADE_INT32_T n_coeffs)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetCoeffs,p_coeffs_i);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetCoeffs,p_poly);

    p_poly->poly_order = n_coeffs-1;
    n_coeffs=(p_poly->poly_order+1)*(p_poly->n_breaks-1);
    memcpy(p_poly->p_poly_coeffs,p_coeffs_i,n_coeffs*sizeof(ADE_FLOATING_T));

      return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Polyfit_SetBuffLen(ADE_POLYFIT_T *p_poly,ADE_INT32_T buff_len)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetCoeffs,p_poly);

    p_poly->buff_len=buff_len;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Polyfit_Step(ADE_POLYFIT_T* p_poly)
{

    ADE_UINT32_T i=0;
    ADE_API_RET_T ret = ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Step,p_poly);

    for (i=0;i<p_poly->buff_len;i++)
    {
        ret=ADE_Utils_memoryless_expander(p_poly,&(p_poly->p_in[i]),&(p_poly->p_out[i]));
        ADE_CHECK_ADERETVAL(ADE_CLASS_POLYFIT,ADE_METHOD_Step,ret);
    }


    return ADE_RET_SUCCESS;
}

/************** Utils methods *********************/

ADE_API_RET_T ADE_Polyfit_Print(ADE_POLYFIT_T* p_poly, ADE_FILE_T *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj)
{
   ADE_UINT32_T i=0;
    ADE_CHAR_T fixed_str[64];

    ADE_CHAR_T temp_str2[16];
    ADE_CHAR_T pri_str[128];
    ADE_SIZE_T len_str;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_Print,p_poly);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_FIR,ADE_METHOD_Print,p_fid);

            memset(fixed_str,'\0',sizeof(fixed_str));
strcat(fixed_str,calling_obj);
strcat(fixed_str,"->");
strcat(fixed_str,obj_name);
strcat(fixed_str,"->");
len_str=strlen(fixed_str);


    if (p_fid!=NULL)
    {
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"buff_len = %u\n"),p_poly->buff_len);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_in = %p(%f)\n"),p_poly->p_in,p_poly->p_in[0]);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_out = %p(%f)\n"),p_poly->p_out,p_poly->p_out[0]);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"n_breaks = %u\n"),p_poly->n_breaks);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"max_n_breaks = %u\n"),p_poly->max_n_breaks);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_breaks = %p(%f)\n"),p_poly->p_breaks,p_poly->p_breaks[0]);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"poly_order = %u\n"),p_poly->poly_order);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"max_poly_order = %u\n"),p_poly->max_poly_order);
        strcpy(pri_str,fixed_str);
        fprintf(p_fid,strcat(pri_str,"p_poly_coeffs = %p(%f)\n"),p_poly->p_poly_coeffs,p_poly->p_poly_coeffs[0]);


    }
    return ADE_RET_SUCCESS;

}

/***************** Static methods *************************/




