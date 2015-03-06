#include "headers/ADE_polyfit.h"
#include <stddef.h>
#include "headers/ADE_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>





static ADE_VOID_T ADE_memoryless_blow_expander(ADE_POLYFIT_T* p_poly,ADE_FLOATING_T* frame_i,ADE_FLOATING_T* y_o);

ADE_API_RET_T ADE_Polyfit_Init (ADE_POLYFIT_T **dp_poly,ADE_UINT32_T poly_order_i,ADE_UINT32_T n_breaks_i)
{
    ADE_POLYFIT_T *p_this=NULL;
    ADE_UINT32_T poly_pieces = 0;

    p_this=calloc(1,sizeof(ADE_POLYFIT_T));

    if (p_this!=NULL)
    {
        p_this->n_breaks=n_breaks_i;
        p_this->p_breaks=calloc(n_breaks_i,sizeof(ADE_FLOATING_T));
        if (p_this->p_breaks==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_breaks,"%p",ADE_Polyfit_Init);
            return ADE_E21;
        }
        poly_pieces = p_this->n_breaks-1;
        if (poly_pieces<=0)
        {
            ADE_PRINT_ERRORS(ADE_INCHECKS,poly_pieces,"%d",ADE_Polyfit_Init);
            return ADE_E22;
        }

        p_this->poly_order=poly_order_i;
        p_this->p_poly_coeffs=calloc(poly_pieces*(poly_order_i+1),sizeof(ADE_FLOATING_T));
        if (p_this->p_poly_coeffs==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_poly_coeffs,"%p",ADE_Polyfit_Init);
            return ADE_E21;
        }

        *dp_poly=p_this;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_MEM,p_this,"%p",ADE_Polyfit_Init);
        return ADE_E21;
    }


    return ADE_DEFAULT_RET;

}

ADE_VOID_T ADE_Polyfit_Release(ADE_POLYFIT_T *p_poly)
{
    ADE_CHECKNFREE(p_poly->p_breaks);
    ADE_CHECKNFREE(p_poly->p_poly_coeffs);
    ADE_CHECKNFREE(p_poly);

}

ADE_API_RET_T ADE_Polyfit_SetBreaks(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_breaks_i)
{
    memcpy(p_poly->p_breaks,p_breaks_i,p_poly->n_breaks*sizeof(ADE_FLOATING_T));

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Polyfit_SetCoeffs(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_coeffs_i)
{
    ADE_UINT32_T n_coeffs=(p_poly->poly_order+1)*(p_poly->n_breaks-1);

    memcpy(p_poly->p_poly_coeffs,p_coeffs_i,n_coeffs*sizeof(ADE_FLOATING_T));

      return ADE_DEFAULT_RET;
}

static ADE_VOID_T ADE_memoryless_blow_expander(ADE_POLYFIT_T* p_poly,ADE_FLOATING_T* frame_i,ADE_FLOATING_T* y_o)
{

  //  ADE_UINT32_T frame_size = p_blow->buff_size;
    ADE_UINT32_T i=0, k=0,zz=0;
    ADE_FLOATING_T data=0.0;
    ADE_FLOATING_T* coeffs=p_poly->p_poly_coeffs;
    ADE_UINT32_T n_pieces = p_poly->n_breaks-1;
    ADE_FLOATING_T *x_breaks = p_poly->p_breaks;
    ADE_UINT32_T coeffs_row_idx=0, coeffs_col_idx=0;
    ADE_UINT32_T n_coeff_per_piece = p_poly->poly_order+1;



//for (i=0;i<frame_size;i++)
//{
    *y_o=0;
    data = *frame_i;

    if ( data>=0 )
    {
        for (k=0;k<n_pieces;k++)
        {
             if (k==0)
             {
                  if ( (data>=x_breaks[k]) && (data<x_breaks[k+1]) )
                  {
                      zz=k;
                    break;
                  }

             }

        else if (k==(n_pieces-1))
        {
             if ( (data>=x_breaks[k]) && (data<=x_breaks[k+1]) )
             {
                 zz=k;
                 break;
             }
        }
        else
        {
             if ( (data>=x_breaks[k]) && (data<x_breaks[k+1]) )
             {
                   zz=k;
                    break;
             }

        }



        }


    }


    else
    {
        zz=0;
        data = 0;
    }

    coeffs_row_idx=zz*n_coeff_per_piece;

    for(i=0;i<n_coeff_per_piece;i++)
    {
        coeffs_col_idx = n_coeff_per_piece-1-i;
       *y_o +=coeffs[coeffs_row_idx+coeffs_col_idx]*pow((data-x_breaks[zz]),i);
    }

}


ADE_API_RET_T ADE_Polyfit_Step(ADE_POLYFIT_T* p_poly,ADE_FLOATING_T* frame_i,ADE_FLOATING_T* y_o,ADE_UINT32_T frame_len_i)
{

    ADE_UINT32_T i=0;

      #if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)

        if (p_poly->p_poly_coeffs==NULL)
        {
           ADE_PRINT_ERRORS(ADE_INCHECKS,p_poly->p_poly_coeffs,"%p",ADE_Polyfit_Step);
           return ADE_E22;
        }

        if (p_poly->p_breaks==NULL)
        {
           ADE_PRINT_ERRORS(ADE_INCHECKS,p_poly->p_breaks,"%p",ADE_Polyfit_Step);
           return ADE_E22;
        }

    #endif

    for (i=0;i<frame_len_i;i++)
    {
        ADE_memoryless_blow_expander(p_poly,&(frame_i[i]),&(y_o[i]));
    }


    return ADE_DEFAULT_RET;
}




