#ifndef _ADE_POLYFIT_H
#define _ADE_POLYFIT_H
#include "headers/ADE_typedefs.h"

struct ADE_POLYFIT_S
{
    ADE_UINT32_T n_breaks;
    ADE_FLOATING_T *p_breaks;
    ADE_UINT32_T poly_order;
    ADE_FLOATING_T *p_poly_coeffs;
};

ADE_API_RET_T ADE_Polyfit_Init (ADE_POLYFIT_T **dp_poly,ADE_UINT32_T poly_order_i,ADE_UINT32_T n_breaks_i);
ADE_VOID_T ADE_Polyfit_Release(ADE_POLYFIT_T *p_poly);
ADE_API_RET_T ADE_Polyfit_SetBreaks(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_breaks_i);
ADE_API_RET_T ADE_Polyfit_SetCoeffs(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_coeffs_i);
ADE_API_RET_T ADE_memoryless_blow_expander(ADE_POLYFIT_T* p_poly,ADE_FLOATING_T* frame_i,ADE_FLOATING_T* y_o);
#endif //_ADE_POLYFIT_H
