#include "headers/ADE_blas_level2.h"
#include "headers/ADE_blas_wrapper.h"
#include "headers/ADE_defines.h"
#include "headers/ADE_errors.h"
#include "headers/cblas.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

ADE_API_RET_T ADE_Blas_level2_Init(ADE_blas_level2_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type )
{
    ADE_blas_level2_T* p_this = calloc(1,sizeof(ADE_blas_level2_T));

    if (p_this!=NULL)
    {
        p_this->math_type=math_type;
        /**************ALPHA ALLOC ******************/
        if (math_type==ADE_REAL)
        {
             p_this->p_ALPHA=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_CPLX)
        {
            p_this->p_ALPHA=calloc(1,sizeof(ADE_CPLX_T));
        }
        else
        {
            ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Blas_level2_Init);
            return ADE_E38;
        }

        if (p_this->p_ALPHA==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_ALPHA,"%p",ADE_Blas_level2_Init);
            return ADE_E37;
        }




        /********************ALLOC BETA**********************/

        if (math_type==ADE_REAL)
        {
             p_this->p_BETA=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_CPLX)
        {
            p_this->p_BETA=calloc(1,sizeof(ADE_CPLX_T));
        }
        else
        {
            ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Blas_level2_Init);
            return ADE_E38;
        }

        if (p_this->p_BETA==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_BETA,"%p",ADE_Blas_level2_Init);
            return ADE_E37;
        }


        *dp_this=p_this;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_MEM,p_this,"%p",ADE_Blas_level2_Init);
        return ADE_E37;
    }



    return ADE_DEFAULT_RET;
}


ADE_VOID_T ADE_Blas_level2_Release (ADE_blas_level2_T* p_blas_l2)
{
     ADE_CHECKNFREE(p_blas_l2->p_ALPHA);
     ADE_CHECKNFREE(p_blas_l2->p_BETA);
    ADE_CHECKNFREE(p_blas_l2);
}

ADE_API_RET_T ADE_Blas_Level2_SetUplo(ADE_blas_level2_T* p_blas_l2, ADE_CHAR_T UPLO)
{
    if (UPLO!='U' && UPLO!='u' && UPLO!='L' && UPLO!='l')
    {

        ADE_PRINT_ERRORS(ADE_INCHECKS,UPLO,"%c",ADE_Blas_Level2_SetUplo);
        return ADE_E38;
    }
    else
    {
        p_blas_l2->UPLO=UPLO;
        return ADE_DEFAULT_RET;
    }


}

ADE_API_RET_T ADE_Blas_Level2_SetTrans(ADE_blas_level2_T* p_blas_l2, ADE_CHAR_T TRANS)
{
    if (TRANS!='N' && TRANS!='n' && TRANS!='T' && TRANS!='t' && TRANS!='C' && TRANS!='c')
    {

        ADE_PRINT_ERRORS(ADE_INCHECKS,TRANS,"%c",ADE_Blas_Level2_SetTrans);
        return ADE_E38;
    }
    else
    {
        p_blas_l2->TRANS=TRANS;
        return ADE_DEFAULT_RET;
    }


}

ADE_API_RET_T ADE_Blas_Level2_SetDiag(ADE_blas_level2_T* p_blas_l2, ADE_CHAR_T DIAG)
{
    if (DIAG!='U' && DIAG!='u' && DIAG!='N' && DIAG!='n')
    {

        ADE_PRINT_ERRORS(ADE_INCHECKS,DIAG,"%c",ADE_Blas_Level2_SetDiag);
        return ADE_E38;
    }
    else
    {
        p_blas_l2->DIAG=DIAG;
        return ADE_DEFAULT_RET;
    }


}

ADE_VOID_T ADE_Blas_Level2_SetM(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T M)
{

        p_blas_l2->M=M;

}

ADE_VOID_T ADE_Blas_Level2_SetN(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T N)
{

        p_blas_l2->N=N;

}

ADE_VOID_T ADE_Blas_Level2_SetK(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T K)
{

        p_blas_l2->K=K;

}

ADE_VOID_T ADE_Blas_Level2_SetKl(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T KL)
{

        p_blas_l2->KL=KL;

}

ADE_VOID_T ADE_Blas_Level2_SetKu(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T KU)
{

        p_blas_l2->KU=KU;

}


ADE_VOID_T ADE_Blas_Level2_SetLda(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T val)
{

        p_blas_l2->LDA=val;

}


ADE_VOID_T ADE_Blas_Level2_SetIncx(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T val)
{

        p_blas_l2->INCX=val;

}


ADE_VOID_T ADE_Blas_Level2_SetIncy(ADE_blas_level2_T* p_blas_l2, ADE_UINT32_T val)
{

        p_blas_l2->INCY=val;

}

ADE_API_RET_T ADE_Blas_level2_SetAlpha(ADE_blas_level2_T* p_blas_l2,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

    if (p_blas_l2->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l2->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l2->math_type,"%d",ADE_Blas_level2_SetAlpha);
        return ADE_E38;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l2->p_ALPHA)[i]=p_val[i];
    }
     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_level2_SetBeta(ADE_blas_level2_T* p_blas_l2,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

    if (p_blas_l2->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l2->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l2->math_type,"%d",ADE_Blas_level2_SetBeta);
        return ADE_E38;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l2->p_BETA)[i]=p_val[i];
    }
     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_level2_SetA(ADE_blas_level2_T* p_blas_l2,ADE_FLOATING_T *p_buff)
{
    if (p_buff==NULL)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_buff,"%p",ADE_Blas_level2_SetA);
        return ADE_E38;
    }
    else
    {
        p_blas_l2->p_A=p_buff;
        return ADE_DEFAULT_RET;
    }

}

ADE_API_RET_T ADE_Blas_level2_SetY(ADE_blas_level2_T* p_blas_l2,ADE_FLOATING_T *p_buff)
{
    if (p_buff==NULL)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_buff,"%p",ADE_Blas_level2_SetY);
        return ADE_E38;
    }
    else
    {
        p_blas_l2->p_Y=p_buff;
        return ADE_DEFAULT_RET;
    }

}

ADE_API_RET_T ADE_Blas_level2_SetX(ADE_blas_level2_T* p_blas_l2,ADE_FLOATING_T *p_buff)
{
    if (p_buff==NULL)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_buff,"%p",ADE_Blas_level2_SetX);
        return ADE_E38;
    }
    else
    {
        p_blas_l2->p_X=p_buff;
        return ADE_DEFAULT_RET;
    }

}

#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_API_RET_T ADE_Blas_level2_sger (ADE_blas_level2_T *p_blas_l2)
{
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level2_sger);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    sger(&(p_blas_l2->M),&(p_blas_l2->N),p_blas_l2->p_ALPHA,p_blas_l2->p_X,&(p_blas_l2->INCX),p_blas_l2->p_Y,&(p_blas_l2->INCY),p_blas_l2->p_A,&(p_blas_l2->LDA));

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

    cblas_sger(CblasRowMajor,(p_blas_l2->M),(p_blas_l2->N),*(p_blas_l2->p_ALPHA),p_blas_l2->p_X,(p_blas_l2->INCX),p_blas_l2->p_Y,(p_blas_l2->INCY),p_blas_l2->p_A,(p_blas_l2->LDA));
     #else

        ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

    #endif



}

#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
static ADE_VOID_T ADE_Blas_level2_dger (ADE_blas_level2_T *p_blas_l2)
{
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level2_dger);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    dger(&(p_blas_l2->M),&(p_blas_l2->N),p_blas_l2->p_ALPHA,p_blas_l2->p_X,&(p_blas_l2->INCX),p_blas_l2->p_Y,&(p_blas_l2->INCY),p_blas_l2->p_A,&(p_blas_l2->LDA));

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

    cblas_dger(CblasRowMajor,(p_blas_l2->M),(p_blas_l2->N),*(p_blas_l2->p_ALPHA),p_blas_l2->p_X,(p_blas_l2->INCX),p_blas_l2->p_Y,(p_blas_l2->INCY),p_blas_l2->p_A,(p_blas_l2->LDA));
     #else

        ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

    #endif



}

#else
ADE_DEFINE_ERROR(ADE_FP_PRECISION);
#endif
