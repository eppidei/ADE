#ifndef _ADE_POLYFIT_H
#define _ADE_POLYFIT_H
#include "headers/ADE_typedefs.h"
struct ADE_POLYFIT_S
{
    ADE_FLOATING_T* p_in;
    ADE_FLOATING_T* p_out;
    ADE_UINT32_T buff_len;
    ADE_UINT32_T n_breaks;
    ADE_UINT32_T max_n_breaks;
    ADE_FLOATING_T *p_breaks;
    ADE_UINT32_T poly_order;
    ADE_UINT32_T max_poly_order;
    ADE_FLOATING_T *p_poly_coeffs;
};

#ifdef __cplusplus
    extern "C" {
#endif
/********************** Init Methods ******************************/
ADE_API_RET_T ADE_Polyfit_Init (ADE_POLYFIT_T **dp_poly);
ADE_VOID_T ADE_Polyfit_Release(ADE_POLYFIT_T *p_poly);
/************************* Set Methods ****************************/
ADE_API_RET_T ADE_Polyfit_SetBreaks(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_breaks_i,ADE_INT32_T n_breaks);
ADE_API_RET_T ADE_Polyfit_SetCoeffs(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_coeffs_i,ADE_INT32_T n_coeffs);
ADE_API_RET_T ADE_Polyfit_SetInBuff(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_in);
ADE_API_RET_T ADE_Polyfit_SetOutBuff(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_out);
ADE_API_RET_T ADE_Polyfit_SetBuffLen(ADE_POLYFIT_T *p_poly,ADE_INT32_T buff_len);
/******************* Configure Methods *****************************/
ADE_API_RET_T ADE_Polyfit_Configure(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_breaks_i,ADE_UINT32_T n_breaks,ADE_FLOATING_T *p_coeffs_i,ADE_UINT32_T n_coeffs,ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_out,ADE_INT32_T buff_len);
ADE_API_RET_T ADE_Polyfit_Configure_params(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_breaks_i,ADE_UINT32_T n_breaks,ADE_FLOATING_T *p_coeffs_i,ADE_UINT32_T n_coeffs);
ADE_API_RET_T ADE_Polyfit_Configure_inout(ADE_POLYFIT_T *p_poly,ADE_FLOATING_T *p_in,ADE_FLOATING_T *p_out,ADE_INT32_T buff_len);
/********************* Processing Methods *************************/
ADE_API_RET_T ADE_Polyfit_Step(ADE_POLYFIT_T* p_poly);


#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_POLYFIT_H
