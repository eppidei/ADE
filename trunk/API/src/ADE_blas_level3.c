#include "headers/ADE_blas_level3.h"
#include "headers/ADE_blas_wrapper.h"
#include "headers/ADE_defines.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_cblas.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
static ADE_API_RET_T ADE_Blas_level3_launch_type1 (ADE_blas_level3_T *p_Blas_l3);
#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_VOID_T ADE_Blas_level3_sgemm (ADE_blas_level3_T *p_Blas_l3);
static ADE_VOID_T ADE_Blas_level3_cgemm (ADE_blas_level3_T *p_Blas_l3);

#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

static ADE_VOID_T ADE_Blas_level3_dgemm (ADE_blas_level3_T *p_Blas_l3);
static ADE_VOID_T ADE_Blas_level3_zgemm (ADE_blas_level3_T *p_Blas_l3);

#else
ADE_DEFINE_ERROR(ADE_FP_PRECISION);
#endif

ADE_API_RET_T ADE_Blas_level3_Init(ADE_blas_level3_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type )
{
    ADE_blas_level3_T* p_this = calloc(1,sizeof(ADE_blas_level3_T));

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
            ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Blas_level3_Init);
            return ADE_E42;
        }

        if (p_this->p_ALPHA==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_ALPHA,"%p",ADE_Blas_level3_Init);
            return ADE_E41;
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
            ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Blas_level3_Init);
            return ADE_E42;
        }

        if (p_this->p_BETA==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_BETA,"%p",ADE_Blas_level3_Init);
            return ADE_E41;
        }


        *dp_this=p_this;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_MEM,p_this,"%p",ADE_Blas_level3_Init);
        return ADE_E41;
    }



    return ADE_DEFAULT_RET;
}


ADE_VOID_T ADE_Blas_level3_Release (ADE_blas_level3_T* p_Blas_l3)
{
     ADE_CHECKNFREE(p_Blas_l3->p_ALPHA);
     ADE_CHECKNFREE(p_Blas_l3->p_BETA);
    ADE_CHECKNFREE(p_Blas_l3);
}

ADE_API_RET_T ADE_Blas_Level3_SetTrans(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{
    if (val!='N' && val!='n' && val!='T' && val!='t' && val!='C' && val!='c')
    {

        ADE_PRINT_ERRORS(ADE_INCHECKS,val,"%c",ADE_Blas_Level3_SetTrans);
        return ADE_E42;
    }
    else
    {
        p_Blas_l3->TRANS=val;
        return ADE_DEFAULT_RET;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetTransA(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{
    if (val!='N' && val!='n' && val!='T' && val!='t' && val!='C' && val!='c')
    {

        ADE_PRINT_ERRORS(ADE_INCHECKS,val,"%c",ADE_Blas_Level3_SetTrans);
        return ADE_E42;
    }
    else
    {
        p_Blas_l3->TRANSA=val;
        return ADE_DEFAULT_RET;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetTransB(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{
    if (val!='N' && val!='n' && val!='T' && val!='t' && val!='C' && val!='c')
    {

        ADE_PRINT_ERRORS(ADE_INCHECKS,val,"%c",ADE_Blas_Level3_SetTrans);
        return ADE_E42;
    }
    else
    {
        p_Blas_l3->TRANSB=val;
        return ADE_DEFAULT_RET;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetDiag(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{
    if (val!='U' && val!='u' && val!='N' && val!='n')
    {

        ADE_PRINT_ERRORS(ADE_INCHECKS,val,"%c",ADE_Blas_Level3_SetDiag);
        return ADE_E42;
    }
    else
    {
        p_Blas_l3->DIAG=val;
        return ADE_DEFAULT_RET;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetUplo(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{
    if (val!='U' && val!='u' && val!='L' && val!='l')
    {


        ADE_PRINT_ERRORS(ADE_INCHECKS,val,"%c",ADE_Blas_Level3_SetUplo);
        return ADE_E42;
    }
    else
    {
        #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)
        if (val=='L' || val=='l')
        {
            p_Blas_l3->UPLO='U';
        }
        else
        {
            p_Blas_l3->UPLO='L';
        }


         fprintf(stderr,"REMEMBER 2 ADD VERBOSITY LEVEL CTRL @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        fprintf(stderr,"*****WARNING UPLO value is being inverted for typical C row major for coherence with column major Fortran Interface **********\n");

        #else
        p_Blas_l3->UPLO=val;
        #endif
        return ADE_DEFAULT_RET;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetSide(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{
    if (val!='R' && val!='r' && val!='L' && val!='l')
    {

        ADE_PRINT_ERRORS(ADE_INCHECKS,val,"%c",ADE_Blas_Level3_SetSide);
        return ADE_E42;
    }
    else
    {
        p_Blas_l3->SIDE=val;
        return ADE_DEFAULT_RET;
    }


}

ADE_VOID_T ADE_Blas_Level3_SetM(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val)
{

        p_Blas_l3->M=val;

}

ADE_VOID_T ADE_Blas_Level3_SetN(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val)
{

        p_Blas_l3->N=val;

}

ADE_VOID_T ADE_Blas_Level3_SetK(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val)
{

        p_Blas_l3->K=val;

}

ADE_VOID_T ADE_Blas_Level3_SetLda(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val)
{

        p_Blas_l3->LDA=val;

}

ADE_VOID_T ADE_Blas_Level3_SetLdb(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val)
{

        p_Blas_l3->LDB=val;

}


ADE_VOID_T ADE_Blas_Level3_SetLdc(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val)
{

        p_Blas_l3->LDC=val;

}

ADE_API_RET_T ADE_Blas_level3_gemm(ADE_blas_level3_T* p_Blas_l3)
{



    ADE_API_RET_T ret = ADE_DEFAULT_RET;

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
     if(p_Blas_l3->math_type==ADE_REAL)
     {
           p_Blas_l3->blas_level3_fcn_type1=ADE_Blas_level3_sgemm;
     }
     else
     {
         p_Blas_l3->blas_level3_fcn_type1=ADE_Blas_level3_cgemm;
     }



    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
    if(p_Blas_l3->math_type==ADE_REAL)
     {
     p_Blas_l3->blas_level3_fcn_type1=ADE_Blas_level3_dgemm;
     }
     else
     {
         p_Blas_l3->blas_level3_fcn_type1=ADE_Blas_level3_zgemm;
     }
     #else
        ADE_DEFINE_ERROR(ADE_FP_PRECISION);
    #endif

     ret = ADE_Blas_level3_launch_type1(p_Blas_l3);

     #if (ADE_CHECK_RETURNS==1)
    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blas_level3_gemm);
    }
    #endif

    return ret;

}


static ADE_API_RET_T ADE_Blas_level3_launch_type1 (ADE_blas_level3_T *p_Blas_l3)
{

    ADE_API_RET_T ret = ADE_DEFAULT_RET;
    static unsigned short flag= 0;

    #if (ADE_CHECK_INPUTS==1)

        if (flag==0)
        {
             printf("REMEMBER TO ADD CHECKS ADE_Blas_level3_launch_type1\n");
             flag++;
        }


    #endif

    ret = (p_Blas_l3->blas_level3_fcn_type1)(p_Blas_l3);

    #if (ADE_CHECK_RETURNS==1)

    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blas_level3_launch_type1)
    }
    #endif

    return ret;
}


#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_VOID_T ADE_Blas_level3_sgemm (ADE_blas_level3_T *p_Blas_l3)
{
    ADE_CBLAS_ORDER_T cblas_order=CblasRowMajor;
    ADE_CBLAS_TRANSPOSE_T cblas_transposea,cblas_transposeb;
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level3_sgemm);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    sgemm(&(p_Blas_l3->TRANSA),&(p_Blas_l3->TRANSB),&(p_Blas_l3->M),&(p_Blas_l3->N),&(p_Blas_l3->K),p_Blas_l3->p_Alpha,p_Blas_l3->p_A,&(p_Blas_l3->LDA),p_Blas_l3->p_B,&(p_Blas_l3->LDB),p_Blas_l3->p_Beta,p_Blas_l3->p_C,&(p_Blas_l3->LDC));


    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)
   if (p_Blas_l3->TRANSA=='T' || p_Blas_l3->TRANSA=='t' )
    {
        cblas_transposea=CblasTrans;
    }
    else if (p_Blas_l3->TRANSA=='N' || p_Blas_l3->TRANSA=='n' )
    {
        cblas_transposea=CblasNoTrans;
    }
    else if (p_Blas_l3->TRANSA=='C' || p_Blas_l3->TRANSA=='c' )
    {
        cblas_transposea=CblasConjTrans;
    }
    if (p_Blas_l3->TRANSB=='T' || p_Blas_l3->TRANSB=='t' )
    {
        cblas_transposeb=CblasTrans;
    }
    else if (p_Blas_l3->TRANSB=='N' || p_Blas_l3->TRANSB=='n' )
    {
        cblas_transposeb=CblasNoTrans;
    }
    else if (p_Blas_l3->TRANSB=='C' || p_Blas_l3->TRANSB=='c' )
    {
        cblas_transposeb=CblasConjTrans;
    }
    cblas_sgemm(cblas_order,cblas_transposea,cblas_transposeb,p_Blas_l3->M,p_Blas_l3->N,p_Blas_l3->K,*(p_Blas_l3->p_ALPHA),p_Blas_l3->p_A,p_Blas_l3->LDA,p_Blas_l3->p_B,p_Blas_l3->LDB,*(p_Blas_l3->p_BETA),p_Blas_l3->p_C,p_Blas_l3->LDC);

     #else

        ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

    #endif

}

static ADE_VOID_T ADE_Blas_level3_cgemm (ADE_blas_level3_T *p_Blas_l3)
{
    ADE_CBLAS_ORDER_T cblas_order=CblasRowMajor;
    ADE_CBLAS_TRANSPOSE_T cblas_transposea,cblas_transposeb;
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level3_cgemm);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    cgemm(&(p_Blas_l3->TRANSA),&(p_Blas_l3->TRANSB),&(p_Blas_l3->M),&(p_Blas_l3->N),&(p_Blas_l3->K),p_Blas_l3->p_ALPHA,p_Blas_l3->p_A,&(p_Blas_l3->LDA),p_Blas_l3->p_B,&(p_Blas_l3->LDB),p_Blas_l3->p_BETA,p_Blas_l3->p_C,&(p_Blas_l3->LDC));


    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)
    if (p_Blas_l3->TRANSA=='T' || p_Blas_l3->TRANSA=='t' )
    {
        cblas_transposea=CblasTrans;
    }
    else if (p_Blas_l3->TRANSA=='N' || p_Blas_l3->TRANSA=='n' )
    {
        cblas_transposea=CblasNoTrans;
    }
    else if (p_Blas_l3->TRANSA=='C' || p_Blas_l3->TRANSA=='c' )
    {
        cblas_transposea=CblasConjTrans;
    }
    if (p_Blas_l3->TRANSB=='T' || p_Blas_l3->TRANSB=='t' )
    {
        cblas_transposeb=CblasTrans;
    }
    else if (p_Blas_l3->TRANSB=='N' || p_Blas_l3->TRANSB=='n' )
    {
        cblas_transposeb=CblasNoTrans;
    }
    else if (p_Blas_l3->TRANSB=='C' || p_Blas_l3->TRANSB=='c' )
    {
        cblas_transposeb=CblasConjTrans;
    }


    cblas_cgemm(cblas_order,cblas_transposea,cblas_transposeb,p_Blas_l3->M,p_Blas_l3->N,p_Blas_l3->K,(p_Blas_l3->p_ALPHA),p_Blas_l3->p_A,p_Blas_l3->LDA,p_Blas_l3->p_B,p_Blas_l3->LDB,(p_Blas_l3->p_BETA),p_Blas_l3->p_C,p_Blas_l3->LDC);

     #else

        ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

    #endif

}

#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

static ADE_VOID_T ADE_Blas_level3_dgemm (ADE_blas_level3_T *p_Blas_l3)
{
    ADE_CBLAS_ORDER_T cblas_order=CblasRowMajor;
    ADE_CBLAS_TRANSPOSE_T cblas_transposea,cblas_transposeb;
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level3_dgemm);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    dgemm(&(p_Blas_l3->TRANSA),&(p_Blas_l3->TRANSB),&(p_Blas_l3->M),&(p_Blas_l3->N),&(p_Blas_l3->K),p_Blas_l3->p_ALPHA,p_Blas_l3->p_A,&(p_Blas_l3->LDA),p_Blas_l3->p_B,&(p_Blas_l3->LDB),p_Blas_l3->p_BETA,p_Blas_l3->p_C,&(p_Blas_l3->LDC));


    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)
if (p_Blas_l3->TRANSA=='T' || p_Blas_l3->TRANSA=='t' )
    {
        cblas_transposea=CblasTrans;
    }
    else if (p_Blas_l3->TRANSA=='N' || p_Blas_l3->TRANSA=='n' )
    {
        cblas_transposea=CblasNoTrans;
    }
    else if (p_Blas_l3->TRANSA=='C' || p_Blas_l3->TRANSA=='c' )
    {
        cblas_transposea=CblasConjTrans;
    }
    if (p_Blas_l3->TRANSB=='T' || p_Blas_l3->TRANSB=='t' )
    {
        cblas_transposeb=CblasTrans;
    }
    else if (p_Blas_l3->TRANSB=='N' || p_Blas_l3->TRANSB=='n' )
    {
        cblas_transposeb=CblasNoTrans;
    }
    else if (p_Blas_l3->TRANSB=='C' || p_Blas_l3->TRANSB=='c' )
    {
        cblas_transposeb=CblasConjTrans;
    }
    cblas_dgemm(cblas_order,cblas_transposea,cblas_transposeb,p_Blas_l3->M,p_Blas_l3->N,p_Blas_l3->K,*(p_Blas_l3->p_ALPHA),p_Blas_l3->p_A,p_Blas_l3->LDA,p_Blas_l3->p_B,p_Blas_l3->LDB,*(p_Blas_l3->p_BETA),p_Blas_l3->p_C,p_Blas_l3->LDC);

     #else

        ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

    #endif

}

static ADE_VOID_T ADE_Blas_level3_zgemm (ADE_blas_level3_T *p_Blas_l3)
{
    ADE_CBLAS_ORDER_T cblas_order=CblasRowMajor;
    ADE_CBLAS_TRANSPOSE_T cblas_transposea,cblas_transposeb;
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level3_zgemm);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    zgemm(&(p_Blas_l3->TRANSA),&(p_Blas_l3->TRANSB),&(p_Blas_l3->M),&(p_Blas_l3->N),&(p_Blas_l3->K),p_Blas_l3->p_Alpha,p_Blas_l3->p_A,&(p_Blas_l3->LDA),p_Blas_l3->p_B,&(p_Blas_l3->LDB),p_Blas_l3->p_Beta,p_Blas_l3->p_C,&(p_Blas_l3->LDC));


    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)
if (p_Blas_l3->TRANSA=='T' || p_Blas_l3->TRANSA=='t' )
    {
        cblas_transposea=CblasTrans;
    }
    else if (p_Blas_l3->TRANSA=='N' || p_Blas_l3->TRANSA=='n' )
    {
        cblas_transposea=CblasNoTrans;
    }
    else if (p_Blas_l3->TRANSA=='C' || p_Blas_l3->TRANSA=='c' )
    {
        cblas_transposea=CblasConjTrans;
    }
    if (p_Blas_l3->TRANSB=='T' || p_Blas_l3->TRANSB=='t' )
    {
        cblas_transposeb=CblasTrans;
    }
    else if (p_Blas_l3->TRANSB=='N' || p_Blas_l3->TRANSB=='n' )
    {
        cblas_transposeb=CblasNoTrans;
    }
    else if (p_Blas_l3->TRANSB=='C' || p_Blas_l3->TRANSB=='c' )
    {
        cblas_transposeb=CblasConjTrans;
    }
    cblas_zgemm(cblas_order,cblas_transposea,cblas_transposeb,p_Blas_l3->M,p_Blas_l3->N,p_Blas_l3->K,(p_Blas_l3->p_ALPHA),p_Blas_l3->p_A,p_Blas_l3->LDA,p_Blas_l3->p_B,p_Blas_l3->LDB,(p_Blas_l3->p_BETA),p_Blas_l3->p_C,p_Blas_l3->LDC);

     #else

        ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

    #endif

}
#else
ADE_DEFINE_ERROR(ADE_FP_PRECISION);
#endif
