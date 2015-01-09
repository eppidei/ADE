#ifndef _ADE_BLAS_LEVEL2_H
#define _ADE_BLAS_LEVEL2_H
#include "headers/ADE_typedefs.h"

struct ADE_blas_level2_S
{
    ADE_MATH_ATTRIBUTE_T math_type;
    ADE_CHAR_T UPLO;
    ADE_CHAR_T TRANS;
    ADE_CHAR_T DIAG;
    ADE_INT32_T M;
    ADE_INT32_T N;
    ADE_INT32_T K;
    ADE_INT32_T KL;
    ADE_INT32_T KU;
    ADE_FLOATING_T *p_ALPHA;
    ADE_FLOATING_T *p_A;
    ADE_INT32_T LDA;
    ADE_FLOATING_T *p_X;
    ADE_INT32_T INCX;
    ADE_FLOATING_T *p_BETA;
    ADE_FLOATING_T *p_Y;
    ADE_INT32_T INCY;
    ADE_BLAS_LEVEL2_FCN_TYPE1_T blas_level2_fcn_type1;
};
/************************* Init Methods **********************************/
ADE_API_RET_T ADE_Blas_level2_Init(ADE_blas_level2_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type );
ADE_VOID_T ADE_Blas_level2_Release (ADE_blas_level2_T* p_blas_l2);
/*************************** Set Methods ***************************************/
ADE_API_RET_T ADE_Blas_Level2_SetUplo(ADE_blas_level2_T* p_blas_l2, ADE_CHAR_T UPLO);
ADE_API_RET_T ADE_Blas_Level2_SetTrans(ADE_blas_level2_T* p_blas_l2, ADE_CHAR_T TRANS);
ADE_API_RET_T ADE_Blas_Level2_SetDiag(ADE_blas_level2_T* p_blas_l2, ADE_CHAR_T DIAG);
ADE_VOID_T ADE_Blas_Level2_SetM(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T M);
ADE_VOID_T ADE_Blas_Level2_SetN(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T N);
ADE_VOID_T ADE_Blas_Level2_SetK(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T K);
ADE_VOID_T ADE_Blas_Level2_SetKl(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T KL);
ADE_VOID_T ADE_Blas_Level2_SetKu(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T KU);
ADE_VOID_T ADE_Blas_Level2_SetLda(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T val);
ADE_VOID_T ADE_Blas_Level2_SetIncx(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T val);
ADE_VOID_T ADE_Blas_Level2_SetIncy(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T val);
ADE_API_RET_T ADE_Blas_level2_SetAlpha(ADE_blas_level2_T* p_blas_l2,ADE_FLOATING_T *p_val);
ADE_API_RET_T ADE_Blas_level2_SetBeta(ADE_blas_level2_T* p_blas_l2,ADE_FLOATING_T *p_val);
ADE_API_RET_T ADE_Blas_level2_SetA(ADE_blas_level2_T* p_blas_l2,ADE_FLOATING_T *p_buff);
ADE_API_RET_T ADE_Blas_level2_SetY(ADE_blas_level2_T* p_blas_l2,ADE_FLOATING_T *p_buff);
ADE_API_RET_T ADE_Blas_level2_SetX(ADE_blas_level2_T* p_blas_l2,ADE_FLOATING_T *p_buff);
/******************* Operative Methods ******************************/
ADE_API_RET_T ADE_Blas_level2_Print(ADE_blas_level2_T *p_blas_l2);
ADE_API_RET_T ADE_Blas_level2_sbmv(ADE_blas_level2_T* p_blas_l2);
ADE_API_RET_T ADE_Blas_level2_ger(ADE_blas_level2_T* p_blas_l2);
ADE_API_RET_T ADE_Blas_level2_Elewise(ADE_blas_level2_T* p_Blas_l2);
//ADE_API_RET_T ADE_Blas_level2_Elewise_Config(ADE_blas_level2_T* p_blas_l2,ADE_FLOATING_T *p_buff1,ADE_FLOATING_T *p_buff2 ,ADE_FLOATING_T *p_out,ADE_UINT32_T buff_len);
#endif
