#ifndef _ADE_BLAS_LEVEL2_H
#define _ADE_BLAS_LEVEL2_H
#include "headers/ADE_typedefs.h"


#ifdef __cplusplus
    extern "C" {
#endif
/************************* Init Methods **********************************/
ADE_API_RET_T ADE_Blas_level2_Init(ADE_blas_level2_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type );
ADE_VOID_T ADE_Blas_level2_Release (ADE_blas_level2_T* p_blas_l2);

/******************* Configure Methods **********************************/
ADE_API_RET_T ADE_Blas_level2_configure_elewise_bufflength(ADE_blas_level2_T* p_Blas_l2,ADE_INT32_T buff_len);
ADE_API_RET_T ADE_Blas_level2_configure_elewise_params(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T alpha_i,ADE_FLOATING_T beta_i);
ADE_API_RET_T ADE_Blas_level2_configure_elewise_inout(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_buff1,ADE_FLOATING_T *p_buff2 ,ADE_FLOATING_T *p_out);
ADE_API_RET_T ADE_Blas_level2_configure_elewise(ADE_blas_level2_T* p_blas_l2,ADE_FLOATING_T *p_buff1,ADE_FLOATING_T *p_buff2 ,ADE_FLOATING_T *p_out,ADE_FLOATING_T alpha_i,ADE_FLOATING_T beta_i,ADE_UINT32_T buff_len);

/******************* Processing Methods ******************************/
ADE_API_RET_T ADE_Blas_level2_sbmv(ADE_blas_level2_T* p_blas_l2);
ADE_API_RET_T ADE_Blas_level2_ger(ADE_blas_level2_T* p_blas_l2);
ADE_API_RET_T ADE_Blas_level2_Elewise(ADE_blas_level2_T* p_Blas_l2);

/******************* Utils Methods ******************************/
ADE_API_RET_T ADE_Blas_level2_Print(ADE_blas_level2_T *p_blas_l2,FILE *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj);
#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif
