#ifndef _ADE_BLAS_LEVEL3_H
#define _ADE_BLAS_LEVEL3_H
#include "headers/ADE_typedefs.h"

struct ADE_blas_level3_S
{
    ADE_MATH_ATTRIBUTE_T math_type;
    ADE_CHAR_T SIDE;
    ADE_CHAR_T UPLO;
    ADE_CHAR_T TRANS;
    ADE_CHAR_T TRANSA;
    ADE_CHAR_T TRANSB;
    ADE_CHAR_T DIAG;
    ADE_INT32_T M;
    ADE_INT32_T N;
    ADE_INT32_T K;
    ADE_FLOATING_T *p_ALPHA;
    ADE_FLOATING_T *p_A;
    ADE_INT32_T LDA;
    ADE_FLOATING_T *p_B;
    ADE_INT32_T LDB;
    ADE_FLOATING_T *p_BETA;
    ADE_FLOATING_T *p_C;
    ADE_INT32_T LDC;
    ADE_BLAS_LEVEL3_FCN_TYPE1_T blas_level3_fcn_type1;
};
/************************* Init Methods **********************************/
ADE_API_RET_T ADE_Blas_level3_Init(ADE_blas_level3_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type );
ADE_VOID_T ADE_Blas_level3_Release (ADE_blas_level3_T* p_Blas_l3);
/*************************** Set Methods ***************************************/
ADE_API_RET_T ADE_Blas_Level3_SetTrans(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val);
ADE_API_RET_T ADE_Blas_Level3_SetTransA(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val);
ADE_API_RET_T ADE_Blas_Level3_SetTransB(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val);
ADE_API_RET_T ADE_Blas_Level3_SetSide(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val);
ADE_API_RET_T ADE_Blas_Level3_SetUplo(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val);
ADE_VOID_T ADE_Blas_Level3_SetM(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val);
ADE_VOID_T ADE_Blas_Level3_SetN(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val);
ADE_VOID_T ADE_Blas_Level3_SetK(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val);
ADE_VOID_T ADE_Blas_Level3_SetLda(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val);
ADE_VOID_T ADE_Blas_Level3_SetLdb(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val);
ADE_VOID_T ADE_Blas_Level3_SetLdc(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val);

#endif //_ADE_BLAS_LEVEL3_H
