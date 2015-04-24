#ifndef _ADE_BLAS_LEVEL1_H
#define _ADE_BLAS_LEVEL1_H
#include "headers/ADE_typedefs.h"

struct ADE_blas_level1_S
{
    //parameters
    ADE_MATH_ATTRIBUTE_T math_type;
    ADE_INT32_T N;
    ADE_FLOATING_T *p_ALPHA; //pointer for possible complex
    ADE_FLOATING_T *p_X;
    ADE_INT32_T INCX;
    ADE_FLOATING_T *p_Y;
    ADE_INT32_T INCY;
    ADE_FLOATING_T *p_D1;
    ADE_FLOATING_T *p_D2;
    ADE_FLOATING_T *p_A;
    ADE_FLOATING_T *p_B;
    ADE_FLOATING_T *p_C;
    ADE_FLOATING_T *p_S;
    ADE_FLOATING_T *p_PARAM;
    //ADE_FLOATING_T *p_out;
    ADE_BLAS_LEVEL1_FCN_TYPE1_T blas_level1_fcn_type1;
    ADE_BLAS_LEVEL1_FCN_TYPE2_T blas_level1_fcn_type2;
    ADE_BLAS_LEVEL1_FCN_TYPE3_T blas_level1_fcn_type3;
};

#ifdef __cplusplus
    extern "C" {
#endif
/************ Init Methods *****************/
ADE_API_RET_T ADE_Blas_level1_Init(ADE_blas_level1_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type );
ADE_VOID_T ADE_Blas_level1_Release (ADE_blas_level1_T* p_blas_l1);
/************ Set Methods *****************/
ADE_API_RET_T ADE_Blas_level1_SetA(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val);
ADE_API_RET_T ADE_Blas_level1_SetB(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val);
ADE_API_RET_T ADE_Blas_level1_SetC(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val);
ADE_API_RET_T ADE_Blas_level1_SetS(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val);
ADE_API_RET_T ADE_Blas_level1_SetD1(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val);
ADE_API_RET_T ADE_Blas_level1_SetD2(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val);
ADE_API_RET_T ADE_Blas_level1_SetALPHA(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val);
ADE_API_RET_T ADE_Blas_level1_SetN(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T val);
ADE_API_RET_T ADE_Blas_level1_SetINCX(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T val);
ADE_API_RET_T ADE_Blas_level1_SetINCY(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T val);
ADE_API_RET_T ADE_Blas_level1_SetX(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_buf);
ADE_API_RET_T ADE_Blas_level1_SetY(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_buf);
ADE_API_RET_T ADE_Blas_level1_SetPARAM(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val);
/***************** Configure Methods **************************/
ADE_API_RET_T ADE_Blas_level1_configure_axpy_params(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_ALPHA,ADE_INT32_T INCX,ADE_INT32_T INCY,ADE_INT32_T N);
ADE_API_RET_T ADE_Blas_level1_configure_axpy_inout(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_X,ADE_FLOATING_T *p_Y);
ADE_API_RET_T ADE_Blas_level1_configure_axpy(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_ALPHA,ADE_INT32_T INCX,ADE_INT32_T INCY,ADE_INT32_T N,ADE_FLOATING_T *p_X,ADE_FLOATING_T *p_Y);
ADE_API_RET_T ADE_Blas_level1_configure_dotc_params(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T INCX,ADE_INT32_T INCY,ADE_INT32_T N);
ADE_API_RET_T ADE_Blas_level1_configure_dotc_inout(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_X,ADE_FLOATING_T *p_Y);
ADE_API_RET_T ADE_Blas_level1_configure_dotc(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T INCX,ADE_INT32_T INCY,ADE_INT32_T N,ADE_FLOATING_T *p_X,ADE_FLOATING_T *p_Y);
ADE_API_RET_T ADE_Blas_level1_configure_dot_params(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T INCX,ADE_INT32_T INCY,ADE_INT32_T N);
ADE_API_RET_T ADE_Blas_level1_configure_dot_inout(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_X,ADE_FLOATING_T *p_Y);
ADE_API_RET_T ADE_Blas_level1_configure_dot(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T INCX,ADE_INT32_T INCY,ADE_INT32_T N,ADE_FLOATING_T *p_X,ADE_FLOATING_T *p_Y);
/************ Utils Methods *****************/
ADE_API_RET_T ADE_Blas_level1_Print(ADE_blas_level1_T *p_blas_l1,FILE *p_fid);
/************ Processing Methods *****************/
ADE_API_RET_T ADE_Blas_level1_axpy(ADE_blas_level1_T* p_blas_l1);
ADE_API_RET_T ADE_Blas_level1_copy(ADE_blas_level1_T* p_blas_l1);
ADE_FLOATING_T ADE_Blas_level1_dot(ADE_blas_level1_T* p_blas_l1);
ADE_CPLX_T ADE_Blas_level1_dotc(ADE_blas_level1_T* p_blas_l1);
ADE_CPLX_T ADE_Blas_level1_dotu(ADE_blas_level1_T* p_blas_l1);
ADE_FLOATING_T ADE_Blas_level1_nrm2(ADE_blas_level1_T* p_blas_l1);

#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_BLAS_LEVEL1_H
