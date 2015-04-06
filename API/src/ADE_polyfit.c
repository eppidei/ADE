#include "headers/ADE_polyfit.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "headers/ADE_Utils.h"
#include "headers/ADE_Error_Handler.h"




ADE_API_RET_T ADE_Polyfit_Init (ADE_POLYFIT_T **dp_poly,ADE_UINT32_T poly_order_i,ADE_UINT32_T n_breaks_i)
{
    ADE_POLYFIT_T *p_this=NULL;
    ADE_UINT32_T poly_pieces = 0;

    p_this=calloc(1,sizeof(ADE_POLYFIT_T));
     ADE_CHECK_MEMALLOC(ADE_CLASS_POLYFIT,ADE_METHOD_Init, p_this);


        p_this->n_breaks=n_breaks_i;
        p_this->p_breaks=calloc(n_breaks_i,sizeof(ADE_FLOATING_T));
        ADE_CHECK_MEMALLOC(ADE_CLASS_POLYFIT,ADE_METHOD_Init,  p_this->p_breaks);

        poly_pieces = p_this->n_breaks-1;
        if (poly_pieces<=0)
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_POLYFIT,ADE_METHOD_Init,poly_pieces,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;//22;
        }

        p_this->poly_order=poly_order_i;
        p_this->p_poly_coeffs=calloc(poly_pieces*(poly_order_i+1),sizeof(ADE_FLOATING_T));
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

ADE_API_RET_T ADE_Polyfit_SetBreaks(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_breaks_i)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetBreaks,p_breaks_i);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetBreaks,p_poly);

    memcpy(p_poly->p_breaks,p_breaks_i,p_poly->n_breaks*sizeof(ADE_FLOATING_T));

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Polyfit_SetCoeffs(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_coeffs_i)
{
    ADE_UINT32_T n_coeffs=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetCoeffs,p_coeffs_i);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_SetCoeffs,p_poly);

    n_coeffs=(p_poly->poly_order+1)*(p_poly->n_breaks-1);
    memcpy(p_poly->p_poly_coeffs,p_coeffs_i,n_coeffs*sizeof(ADE_FLOATING_T));

      return ADE_RET_SUCCESS;
}



ADE_API_RET_T ADE_Polyfit_Step(ADE_POLYFIT_T* p_poly,ADE_FLOATING_T* frame_i,ADE_FLOATING_T* y_o,ADE_UINT32_T frame_len_i)
{

    ADE_UINT32_T i=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Step,frame_i);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Step,p_poly);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_POLYFIT,ADE_METHOD_Step,y_o);

    for (i=0;i<frame_len_i;i++)
    {
        ADE_Utils_memoryless_expander(p_poly,&(frame_i[i]),&(y_o[i]));
    }


    return ADE_RET_SUCCESS;
}

/***************** Static methods *************************/




