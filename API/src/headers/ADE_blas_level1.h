#ifndef _ADE_BLAS_LEVEL1_H
#define _ADE_BLAS_LEVEL1_H
#include "headers/ADE_typedefs.h"

struct ADE_blas_level1_S
{
    //parameters
    ADE_INT32_T N;
    ADE_FLOATING_T ALPHA;
    ADE_FLOATING_T *p_X;
    ADE_INT32_T INCX;
    ADE_FLOATING_T *p_Y;
    ADE_INT32_T INCY;
    ADE_FLOATING_T D1;
    ADE_FLOATING_T D2;
    ADE_FLOATING_T A;
    ADE_FLOATING_T B;
    ADE_FLOATING_T C;
    ADE_FLOATING_T S;
    ADE_FLOATING_T *p_PARAM;
    ADE_FLOATING_T *p_out;
    ADE_BLAS_LEVEL1_FCN_TYPE1_T blas_level1_fcn_type1;
    ADE_BLAS_LEVEL1_FCN_TYPE2_T blas_level1_fcn_type2;
};



ADE_API_RET_T ADE_Blas_level1_Init(ADE_blas_level1_T** dp_this);
ADE_VOID_T ADE_Blas_level1_Release (ADE_blas_level1_T* p_blas_l1);
ADE_API_RET_T ADE_Blas_level1_setA(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val);
ADE_API_RET_T ADE_Blas_level1_setB(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val);
ADE_API_RET_T ADE_Blas_level1_setC(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val);
ADE_API_RET_T ADE_Blas_level1_setS(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val);
ADE_API_RET_T ADE_Blas_level1_setD1(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val);
ADE_API_RET_T ADE_Blas_level1_setD2(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val);
ADE_API_RET_T ADE_Blas_level1_setALPHA(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val);
ADE_API_RET_T ADE_Blas_level1_setN(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T val);
ADE_API_RET_T ADE_Blas_level1_setINCX(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T val);
ADE_API_RET_T ADE_Blas_level1_setINCY(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T val);
ADE_API_RET_T ADE_Blas_level1_setX(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_buf);
ADE_API_RET_T ADE_Blas_level1_setY(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_buf);
ADE_API_RET_T ADE_Blas_level1_set_pOut(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_buf);
ADE_API_RET_T ADE_Blas_level1_setPARAM(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val,ADE_UINT32_T n_params);
ADE_API_RET_T ADE_Blas_real_axpy(ADE_blas_level1_T* p_blas_l1);

#endif //_ADE_BLAS_LEVEL1_H