#include "headers/ADE_blas_level3.h"
#include "headers/ADE_defines.h"
#if ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB
#include "headers/ADE_blas_wrapper.h"
#elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB || ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)
#include "headers/ADE_cblas.h"
#elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

#elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_NEON)
#include <arm_neon.h>
#else
#error(ADE_BLAS_IMPLEMENTATION)
#endif
#include "headers/ADE_complex.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/ADE_Error_Handler.h"
#ifdef ADE_MEX_PRINT
#include "mex.h"
#endif

/*******************Private methods interface*******************************/

static ADE_API_RET_T ADE_Blas_level3_launch_type1 (ADE_blas_level3_T *p_Blas_l3);
#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_API_RET_T ADE_Blas_level3_sgemm (ADE_blas_level3_T *p_Blas_l3);
static ADE_API_RET_T ADE_Blas_level3_cgemm (ADE_blas_level3_T *p_Blas_l3);

static ADE_API_RET_T sgemm_custom_gp(ADE_CHAR_T TRANSA,ADE_CHAR_T TRANSB,ADE_INT32_T M,ADE_INT32_T N, ADE_INT32_T K,ADE_FLOATING_SP_T ALPHA,ADE_FLOATING_SP_T *A,
ADE_INT32_T LDA,ADE_FLOATING_SP_T *B,ADE_INT32_T LDB,ADE_FLOATING_SP_T BETA,ADE_FLOATING_SP_T* C,ADE_INT32_T LDC);

#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

static ADE_API_RET_T ADE_Blas_level3_dgemm (ADE_blas_level3_T *p_Blas_l3);
static ADE_API_RET_T ADE_Blas_level3_zgemm (ADE_blas_level3_T *p_Blas_l3);

#else
#error(ADE_FP_PRECISION);
#endif

/**********************ADE_METHOD_Init Methods************************************/

ADE_API_RET_T ADE_Blas_level3_Init(ADE_blas_level3_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type )
{
    ADE_blas_level3_T* p_this = calloc(1,sizeof(ADE_blas_level3_T));
      ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_Init,p_this);

    if (p_this!=NULL)
    {
        p_this->math_type=math_type;
        /**************ALPHA ALLOC ******************/
        if (math_type==ADE_MATH_REAL)
        {
             p_this->p_ALPHA=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_MATH_CPLX)
        {
            p_this->p_ALPHA=calloc(1,sizeof(ADE_CPLX_T));
        }
        else
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

         ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_Init,p_this->p_ALPHA);




        /********************ALLOC BETA**********************/

        if (math_type==ADE_MATH_REAL)
        {
             p_this->p_BETA=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_MATH_CPLX)
        {
            p_this->p_BETA=calloc(1,sizeof(ADE_CPLX_T));
        }
        else
        {
             ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

       ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_Init,p_this->p_BETA);


        *dp_this=p_this;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_Init,p_this,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }



    return ADE_RET_SUCCESS;
}


ADE_VOID_T ADE_Blas_level3_Release (ADE_blas_level3_T* p_Blas_l3)
{
     ADE_CHECKNFREE(p_Blas_l3->p_ALPHA);
     ADE_CHECKNFREE(p_Blas_l3->p_BETA);
    ADE_CHECKNFREE(p_Blas_l3);
}

/********************** Set Methods **************************************/

ADE_API_RET_T ADE_Blas_Level3_SetTrans(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetTrans,p_Blas_l3);

    if (val!='N' && val!='n' && val!='T' && val!='t' && val!='C' && val!='c')
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetTrans,val,"%c",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    else
    {
        p_Blas_l3->TRANS=val;
        return ADE_RET_SUCCESS;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetTransA(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetTransA,p_Blas_l3);

    if (val!='N' && val!='n' && val!='T' && val!='t' && val!='C' && val!='c')
    {

         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetTransA,val,"%c",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    else
    {
        p_Blas_l3->TRANSA=val;
        return ADE_RET_SUCCESS;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetTransB(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetTransB,p_Blas_l3);

    if (val!='N' && val!='n' && val!='T' && val!='t' && val!='C' && val!='c')
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetTransB,val,"%c",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    else
    {
        p_Blas_l3->TRANSB=val;
        return ADE_RET_SUCCESS;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetDiag(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetDiag,p_Blas_l3);

    if (val!='U' && val!='u' && val!='N' && val!='n')
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetDiag,val,"%c",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    else
    {
        p_Blas_l3->DIAG=val;
        return ADE_RET_SUCCESS;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetUplo(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{

   // FILE *p_stream=ADE_STDERR_STREAM;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetUplo,p_Blas_l3);

    if (val!='U' && val!='u' && val!='L' && val!='l')
    {


        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetUplo,val,"%c",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    else
    {
        #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)
        /* UPLO value is being inverted for typical C row major for coherence with column major Fortran Interface*/
        if (val=='L' || val=='l')
        {
            p_Blas_l3->UPLO='U';
        }
        else
        {
            p_Blas_l3->UPLO='L';
        }


       // ADE_LOG(p_stream,"REMEMBER 2 ADD VERBOSITY LEVEL CTRL @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        //ADE_LOG(p_stream,"*****WARNING UPLO value is being inverted for typical C row major for coherence with column major Fortran Interface **********\n");

        #else
        p_Blas_l3->UPLO=val;
        #endif
        return ADE_RET_SUCCESS;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetSide(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetSide,p_Blas_l3);

    if (val!='R' && val!='r' && val!='L' && val!='l')
    {

       ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetSide,val,"%c",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    else
    {
        p_Blas_l3->SIDE=val;
        return ADE_RET_SUCCESS;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetM(ADE_blas_level3_T* p_Blas_l3, ADE_INT32_T val)
{
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetM,p_Blas_l3);

        p_Blas_l3->M=val;

        return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_Level3_SetN(ADE_blas_level3_T* p_Blas_l3, ADE_INT32_T val)
{
  ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetN,p_Blas_l3);

        p_Blas_l3->N=val;

         return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_Level3_SetK(ADE_blas_level3_T* p_Blas_l3, ADE_INT32_T val)
{
  ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetK,p_Blas_l3);

        p_Blas_l3->K=val;

         return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_Level3_SetLda(ADE_blas_level3_T* p_Blas_l3, ADE_INT32_T val)
{
  ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetLda,p_Blas_l3);

        p_Blas_l3->LDA=val;

         return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_Level3_SetLdb(ADE_blas_level3_T* p_Blas_l3, ADE_INT32_T val)
{
 ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetLdb,p_Blas_l3);

        p_Blas_l3->LDB=val;

        return ADE_RET_SUCCESS;

}


ADE_API_RET_T ADE_Blas_Level3_SetLdc(ADE_blas_level3_T* p_Blas_l3, ADE_INT32_T val)
{
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetLdc,p_Blas_l3);

        p_Blas_l3->LDC=val;

        return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_Level3_SetA(ADE_blas_level3_T* p_Blas_l3, ADE_FLOATING_T * p_data)
{
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetA,p_Blas_l3);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetA,p_data);

    p_Blas_l3->p_A=p_data;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_Level3_SetB(ADE_blas_level3_T* p_Blas_l3, ADE_FLOATING_T * p_data)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetB,p_Blas_l3);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetB,p_data);

    p_Blas_l3->p_B=p_data;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_Level3_SetC(ADE_blas_level3_T* p_Blas_l3, ADE_FLOATING_T * p_data)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetC,p_Blas_l3);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetC,p_data);

    p_Blas_l3->p_C=p_data;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_Level3_SetALPHA(ADE_blas_level3_T* p_Blas_l3, ADE_FLOATING_T *p_val)
{
      ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;
     ADE_MATH_ATTRIBUTE_T math_type=ADE_MATH_REAL;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetALPHA,p_Blas_l3);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetALPHA,p_val);

    math_type=p_Blas_l3->math_type;

    if (math_type==ADE_MATH_REAL)
    {
        n_elements=1;
    }
    else if (math_type==ADE_MATH_CPLX)
    {
         n_elements=2;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_Blas_l3->p_ALPHA)[i]=p_val[i];
    }
     return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_Level3_SetBeta(ADE_blas_level3_T* p_Blas_l3, ADE_FLOATING_T *p_val)
{
     ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;
      ADE_MATH_ATTRIBUTE_T math_type=ADE_MATH_REAL;

      ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetBeta,p_Blas_l3);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_SetBeta,p_val);

    math_type=p_Blas_l3->math_type;


    if (math_type==ADE_MATH_REAL)
    {
        n_elements=1;
    }
    else if (math_type==ADE_MATH_CPLX)
    {
         n_elements=2;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_Blas_l3->p_BETA)[i]=p_val[i];
    }
     return ADE_RET_SUCCESS;
}
/*****************Configure Methods **************************/
/*ADE_API_RET_T ADE_Blas_level3_configure_gemm_params(ADE_blas_level3_T* p_blas_l3,ADE_CHAR_T TRANSA,ADE_CHAR_T TRANSB,
                                                    ADE_INT32_T M,ADE_INT32_T N, ADE_INT32_T K,ADE_FLOATING_T *p_ALPHA,
                                                    ADE_INT32_T LDA,ADE_INT32_T LDB,ADE_FLOATING_T *p_BETA,ADE_INT32_T LDC)
{

    ADE_API_RET_T ret_transa=ADE_RET_ERROR;
    ADE_API_RET_T ret_transb=ADE_RET_ERROR;
    ADE_API_RET_T ret_m=ADE_RET_ERROR;
    ADE_API_RET_T ret_n=ADE_RET_ERROR;
    ADE_API_RET_T ret_k=ADE_RET_ERROR;
    ADE_API_RET_T ret_alpha=ADE_RET_ERROR;
    ADE_API_RET_T ret_beta=ADE_RET_ERROR;
    ADE_API_RET_T ret_lda=ADE_RET_ERROR;
    ADE_API_RET_T ret_ldb=ADE_RET_ERROR;
    ADE_API_RET_T ret_ldc=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,p_blas_l3);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,p_ALPHA);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,p_BETA);

    ret_transa=ADE_Blas_Level3_SetTransA(p_blas_l3,TRANSA);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,ret_transa);

    ret_transb=ADE_Blas_Level3_SetTransB(p_blas_l3,TRANSB);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,ret_transb);

    ret_m=ADE_Blas_Level3_SetM(p_blas_l3,M);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,ret_m);

     ret_n=ADE_Blas_Level3_SetN(p_blas_l3,N);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,ret_n);

     ret_k=ADE_Blas_Level3_SetK(p_blas_l3,K);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,ret_k);

    ret_lda=ADE_Blas_Level3_SetLda(p_blas_l3,LDA);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,ret_lda);

    ret_ldb=ADE_Blas_Level3_SetLdb(p_blas_l3,LDB);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,ret_ldb);

    ret_ldc=ADE_Blas_Level3_SetLdc(p_blas_l3,LDC);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,ret_ldc);

    ret_alpha=ADE_Blas_Level3_SetALPHA(p_blas_l3,p_ALPHA);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,ret_alpha);

    ret_beta=ADE_Blas_Level3_SetALPHA(p_blas_l3,p_ALPHA);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_params,ret_beta);

   return ADE_RET_SUCCESS;
}
ADE_API_RET_T ADE_Blas_level3_configure_gemm_inout(ADE_blas_level3_T* p_blas_l3,ADE_FLOATING_T *p_A,ADE_FLOATING_T *p_B, ADE_FLOATING_T *p_C)
{
    ADE_API_RET_T ret_a=ADE_RET_ERROR;
    ADE_API_RET_T ret_b=ADE_RET_ERROR;
    ADE_API_RET_T ret_c=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_inout,p_blas_l3);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_inout,p_A);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_inout,p_B);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_inout,p_C);

    ret_a=ADE_Blas_Level3_SetA(p_blas_l3,p_A);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_inout,ret_a);

    ret_b=ADE_Blas_Level3_SetB(p_blas_l3,p_B);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_inout,ret_b);

    ret_c=ADE_Blas_Level3_SetC(p_blas_l3,p_C);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_configure_gemm_inout,ret_c);


    return ADE_RET_SUCCESS;
}*/

/**************** Processing methods *************************/

ADE_API_RET_T ADE_Blas_level3_gemm(ADE_blas_level3_T* p_Blas_l3)
{

    ADE_API_RET_T ret = ADE_RET_SUCCESS;
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_gemm,p_Blas_l3);

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
     if(p_Blas_l3->math_type==ADE_MATH_REAL)
     {
           p_Blas_l3->blas_level3_fcn_type1=ADE_Blas_level3_sgemm;
     }
     else
     {
         p_Blas_l3->blas_level3_fcn_type1=ADE_Blas_level3_cgemm;
     }



    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
    if(p_Blas_l3->math_type==ADE_MATH_REAL)
     {
     p_Blas_l3->blas_level3_fcn_type1=ADE_Blas_level3_dgemm;
     }
     else
     {
         p_Blas_l3->blas_level3_fcn_type1=ADE_Blas_level3_zgemm;
     }
     #else
        #error(ADE_FP_PRECISION);
    #endif

     ret = ADE_Blas_level3_launch_type1(p_Blas_l3);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_gemm,ret);

    return ret;

}

/**************** Utils methods *************************/
ADE_API_RET_T ADE_Blas_level3_Print(ADE_blas_level3_T *p_Blas_l3,FILE *p_fid)
{


 ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_Print,p_Blas_l3);
  ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_Print,p_fid);


        ADE_LOG(p_fid,"p_Blas_l3->math_type = %d\n",p_Blas_l3->math_type);
        ADE_LOG(p_fid,"p_Blas_l3->SIDE = %c\n",p_Blas_l3->SIDE);
        ADE_LOG(p_fid,"p_Blas_l3->UPLO = %c\n",p_Blas_l3->UPLO);
        ADE_LOG(p_fid,"p_Blas_l3->TRANS = %c\n",p_Blas_l3->TRANS);
        ADE_LOG(p_fid,"p_Blas_l3->TRANSA = %c\n",p_Blas_l3->TRANSA);
        ADE_LOG(p_fid,"p_Blas_l3->TRANSB = %c\n",p_Blas_l3->TRANSB);
        ADE_LOG(p_fid,"p_Blas_l3->DIAG = %c\n",p_Blas_l3->DIAG);
        ADE_LOG(p_fid,"p_Blas_l3->M = %d\n",p_Blas_l3->M);
        ADE_LOG(p_fid,"p_Blas_l3->N = %d\n",p_Blas_l3->N);
        ADE_LOG(p_fid,"p_Blas_l3->K = %d\n",p_Blas_l3->K);
        if (p_Blas_l3->math_type==ADE_MATH_REAL)
        {
            ADE_LOG(p_fid,"p_Blas_l3->p_ALPHA = %p(%lf)\n",p_Blas_l3->p_ALPHA,*(p_Blas_l3->p_ALPHA));
            ADE_LOG(p_fid,"p_Blas_l3->p_BETA = %p(%lf)\n",p_Blas_l3->p_BETA,*(p_Blas_l3->p_BETA));
            ADE_LOG(p_fid,"p_Blas_l3->p_A = %p(%lf)\n",p_Blas_l3->p_A,(p_Blas_l3->p_A)[0]);
            ADE_LOG(p_fid,"p_Blas_l3->p_B = %p(%lf)\n",p_Blas_l3->p_B,(p_Blas_l3->p_B)[0]);
            ADE_LOG(p_fid,"p_Blas_l3->p_C = %p(%lf)\n",p_Blas_l3->p_C,(p_Blas_l3->p_C)[0]);
        }
        else if (p_Blas_l3->math_type==ADE_MATH_CPLX)
        {
            ADE_LOG(p_fid,"p_Blas_l3->p_ALPHA = %p(%lf+%lfi)\n",p_Blas_l3->p_ALPHA,ADE_creal(*((ADE_CPLX_T*)p_Blas_l3->p_ALPHA)),ADE_cimag(*((ADE_CPLX_T*)p_Blas_l3->p_ALPHA)));
            ADE_LOG(p_fid,"p_Blas_l3->p_BETA = %p(%lf+%lfi)\n",p_Blas_l3->p_BETA,ADE_creal(*((ADE_CPLX_T*)p_Blas_l3->p_BETA)),ADE_cimag(*((ADE_CPLX_T*)p_Blas_l3->p_BETA)));
            ADE_LOG(p_fid,"p_Blas_l3->p_A = %p(%lf+%lfi)\n",p_Blas_l3->p_A,ADE_creal(((ADE_CPLX_T*)p_Blas_l3->p_A)[0]),ADE_cimag(((ADE_CPLX_T*)p_Blas_l3->p_A)[0]));
            ADE_LOG(p_fid,"p_Blas_l3->p_B = %p(%lf+%lfi)\n",p_Blas_l3->p_B,ADE_creal(((ADE_CPLX_T*)p_Blas_l3->p_B)[0]),ADE_cimag(((ADE_CPLX_T*)p_Blas_l3->p_B)[0]));
            ADE_LOG(p_fid,"p_Blas_l3->p_C = %p(%lf+%lfi)\n",p_Blas_l3->p_C,ADE_creal(((ADE_CPLX_T*)p_Blas_l3->p_C)[0]),ADE_cimag(((ADE_CPLX_T*)p_Blas_l3->p_C)[0]));
        }
        else
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_Print,p_Blas_l3->math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

        ADE_LOG(p_fid,"p_Blas_l3->LDA = %d\n",p_Blas_l3->LDA);
        ADE_LOG(p_fid,"p_Blas_l3->LDB = %d\n",p_Blas_l3->LDB);
        ADE_LOG(p_fid,"p_Blas_l3->LDC = %d\n",p_Blas_l3->LDC);

        ADE_LOG(p_fid,"p_Blas_l3->blas_level3_fcn_type1 = %p\n",p_Blas_l3->blas_level3_fcn_type1);
        ADE_LOG(p_fid,"\n");
      //  fclose(p_fid);

        return ADE_RET_SUCCESS;


}

/***************** Private methods implementations ********************/
static ADE_API_RET_T ADE_Blas_level3_launch_type1 (ADE_blas_level3_T *p_Blas_l3)
{

    ADE_API_RET_T ret = ADE_RET_SUCCESS;
//    static unsigned short flag= 0;

 ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_launch_type1,p_Blas_l3);


    (p_Blas_l3->blas_level3_fcn_type1)(p_Blas_l3);

    return ret;
}

#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_API_RET_T ADE_Blas_level3_sgemm (ADE_blas_level3_T *p_Blas_l3)
{

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_sgemm,p_Blas_l3);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)
/*** Att. blas have to invert inputs for Row major operations cinterface.pdf pgg.191****/
    sgemm_custom_gp(p_Blas_l3->TRANSA,p_Blas_l3->TRANSB,p_Blas_l3->N,p_Blas_l3->M,p_Blas_l3->K,*p_Blas_l3->p_ALPHA,p_Blas_l3->p_B,
p_Blas_l3->LDB,p_Blas_l3->p_A,p_Blas_l3->LDA,*p_Blas_l3->p_BETA,p_Blas_l3->p_C,p_Blas_l3->LDC);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)
   /*** Att. blas have to invert inputs for Row major operations cinterface.pdf pgg.191****/
    sgemm(&(p_Blas_l3->TRANSA),&(p_Blas_l3->TRANSB),&(p_Blas_l3->N),&(p_Blas_l3->M),&(p_Blas_l3->K),(ADE_VOID_T*)p_Blas_l3->p_ALPHA,(ADE_VOID_T*)p_Blas_l3->p_B,&(p_Blas_l3->LDB),(ADE_VOID_T*)p_Blas_l3->p_A,&(p_Blas_l3->LDA),(ADE_VOID_T*)p_Blas_l3->p_BETA,(ADE_VOID_T*)p_Blas_l3->p_C,&(p_Blas_l3->LDC));


    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )
    ADE_CBLAS_ORDER_T cblas_order=CblasRowMajor;
    ADE_CBLAS_TRANSPOSE_T cblas_transposea,cblas_transposeb;
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

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

    ADE_CBLAS_ORDER_T cblas_order=CblasRowMajor;
    ADE_CBLAS_TRANSPOSE_T cblas_transposea,cblas_transposeb;
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

        #error(ADE_BLAS_IMPLEMENTATION);

    #endif
return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blas_level3_cgemm (ADE_blas_level3_T *p_Blas_l3)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_cgemm,p_Blas_l3);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level3_cgemm);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    cgemm(&(p_Blas_l3->TRANSA),&(p_Blas_l3->TRANSB),&(p_Blas_l3->N),&(p_Blas_l3->M),&(p_Blas_l3->K),(ADE_VOID_T*)p_Blas_l3->p_ALPHA,(ADE_VOID_T*)p_Blas_l3->p_B,&(p_Blas_l3->LDB),(ADE_VOID_T*)p_Blas_l3->p_A,&(p_Blas_l3->LDA),(ADE_VOID_T*)p_Blas_l3->p_BETA,(ADE_VOID_T*)p_Blas_l3->p_C,&(p_Blas_l3->LDC));


    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )
     ADE_CBLAS_ORDER_T cblas_order=CblasRowMajor;
    ADE_CBLAS_TRANSPOSE_T cblas_transposea,cblas_transposeb;
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


    cblas_cgemm(cblas_order,cblas_transposea,cblas_transposeb,p_Blas_l3->M,p_Blas_l3->N,p_Blas_l3->K,(ADE_VOID_T*)(p_Blas_l3->p_ALPHA),(ADE_VOID_T*)p_Blas_l3->p_A,p_Blas_l3->LDA,(ADE_VOID_T*)p_Blas_l3->p_B,p_Blas_l3->LDB,(ADE_VOID_T*)(p_Blas_l3->p_BETA),(ADE_VOID_T*)p_Blas_l3->p_C,p_Blas_l3->LDC);
    #elif ( ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

     ADE_CBLAS_ORDER_T cblas_order=CblasRowMajor;
    ADE_CBLAS_TRANSPOSE_T cblas_transposea,cblas_transposeb;
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

        #error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level3_cgemm);

    #endif
return ADE_RET_SUCCESS;
}

#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

static ADE_API_RET_T ADE_Blas_level3_dgemm (ADE_blas_level3_T *p_Blas_l3)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_dgemm,p_Blas_l3);
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level3_dgemm);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    dgemm(&(p_Blas_l3->TRANSA),&(p_Blas_l3->TRANSB),&(p_Blas_l3->N),&(p_Blas_l3->M),&(p_Blas_l3->K),(ADE_VOID_T*)p_Blas_l3->p_ALPHA,(ADE_VOID_T*)p_Blas_l3->p_B,&(p_Blas_l3->LDB),(ADE_VOID_T*)p_Blas_l3->p_A,&(p_Blas_l3->LDA),(ADE_VOID_T*)p_Blas_l3->p_BETA,(ADE_VOID_T*)p_Blas_l3->p_C,&(p_Blas_l3->LDC));


    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)
    ADE_CBLAS_ORDER_T cblas_order=CblasRowMajor;
    ADE_CBLAS_TRANSPOSE_T cblas_transposea,cblas_transposeb;
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

    #elif ( ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)
        ADE_CBLAS_ORDER_T cblas_order=CblasRowMajor;
    ADE_CBLAS_TRANSPOSE_T cblas_transposea,cblas_transposeb;
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

        #error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level3_dgemm);

    #endif
return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blas_level3_zgemm (ADE_blas_level3_T *p_Blas_l3)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,ADE_METHOD_zgemm,p_Blas_l3);
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level3_zgemm);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    zgemm(&(p_Blas_l3->TRANSA),&(p_Blas_l3->TRANSB),&(p_Blas_l3->N),&(p_Blas_l3->M),&(p_Blas_l3->K),(ADE_VOID_T*)p_Blas_l3->p_ALPHA,(ADE_VOID_T*)p_Blas_l3->p_B,&(p_Blas_l3->LDB),(ADE_VOID_T*)p_Blas_l3->p_A,&(p_Blas_l3->LDA),(ADE_VOID_T*)p_Blas_l3->p_BETA,(ADE_VOID_T*)p_Blas_l3->p_C,&(p_Blas_l3->LDC));


    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

    ADE_CBLAS_ORDER_T cblas_order=CblasRowMajor;
    ADE_CBLAS_TRANSPOSE_T cblas_transposea,cblas_transposeb;

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
    cblas_zgemm(cblas_order,cblas_transposea,cblas_transposeb,p_Blas_l3->M,p_Blas_l3->N,p_Blas_l3->K,(ADE_VOID_T*)(p_Blas_l3->p_ALPHA),(ADE_VOID_T*)p_Blas_l3->p_A,p_Blas_l3->LDA,(ADE_VOID_T*)p_Blas_l3->p_B,p_Blas_l3->LDB,(ADE_VOID_T*)(p_Blas_l3->p_BETA),(ADE_VOID_T*)p_Blas_l3->p_C,p_Blas_l3->LDC);

    #elif ( ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)
    ADE_CBLAS_ORDER_T cblas_order=CblasRowMajor;
    ADE_CBLAS_TRANSPOSE_T cblas_transposea,cblas_transposeb;

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

        #error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level3_zgemm);

    #endif
return ADE_RET_SUCCESS;
}
#else
#error(ADE_FP_PRECISION);
#endif

/************************* CUSTOM GENERAL PURPOSE IMPLEMENTATIONS *************************/
/* a bare fortran translation from original sdot.f fortran source v 3.4.0
only 1 based indexing is adapted, column major order is left as it is and handled by changing matrix order and/or transpose parameter
the same way as cblas interface */
static ADE_API_RET_T sgemm_custom_gp(ADE_CHAR_T TRANSA,ADE_CHAR_T TRANSB,ADE_INT32_T M,ADE_INT32_T N, ADE_INT32_T K,ADE_FLOATING_SP_T ALPHA,ADE_FLOATING_SP_T *A,
ADE_INT32_T LDA,ADE_FLOATING_SP_T *B,ADE_INT32_T LDB,ADE_FLOATING_SP_T BETA,ADE_FLOATING_SP_T* C,ADE_INT32_T LDC)
{

ADE_INT32_T i,info,j,l,ncola,nrowa,nrowb,lin_idx,lin_idx2;
ADE_BOOL_T  nota,notb;
ADE_FLOATING_SP_T temp;

/* Set NOTA and NOTB as true if A and B respectively are not
    transposed and set NROWA, NCOLA and NROWB as the number of rows
    and columns of A and the number of rows of B respectively.*/
    nota = ADE_FALSE;
    if (TRANSA=='N' || TRANSA=='n')
    {
        nota=ADE_TRUE;
    }

    notb = ADE_FALSE;
    if (TRANSB=='N' || TRANSB=='n')
    {
        notb=ADE_TRUE;
    }


    if (nota)
    {
        nrowa = M;
        ncola = K;
    }
    else
    {
        nrowa = K;
        ncola = M;
    }


    if (notb)
    {
         nrowb = K;
    }
    else
    {
        nrowb = N;
    }




  if ( (M==0) || (N==0) || ( ( (ALPHA==0.0) || (K==0)) && (BETA==1.0))) return ADE_RET_WARNING;

    if (ALPHA==0.0)
    {
        if (BETA==0.0)
        {
              for (j=0 ; j<N;j++)
              {
                    for (i=0;i<M;i++)
                    {
                        lin_idx = i*N+j;
                        C[lin_idx] = 0.0;
                    }
              }
        }
        else
        {
            for (j=0 ; j<N;j++)
              {
                    for (i=0;i<M;i++)
                    {
                        lin_idx = i*N+j;
                        C[lin_idx] = BETA * C[lin_idx];
                    }
              }
        }
        return ADE_RET_SUCCESS;
    }

  if (notb)
  {
                   if(nota)
                   {

                    /* Form C := alpha*A*B + beta*C. */

                       for (j=0;j<N;j++)
                       {
                            if (BETA==0.0)
                            {
                                for (i=0;i<M;i++)
                                {
                                    lin_idx = i*N+j;
                                    C[lin_idx] = 0.0;
                                }
                            }
                            else if(BETA!=1.0)
                            {
                               for (i=0;i<M;i++)
                                {
                                    lin_idx = i*N+j;
                                    C[lin_idx] = BETA * C[lin_idx];
                                }
                            }
                            for (l=0;l<K;l++)
                            {
                                lin_idx = l*N+j;
                                if (B[lin_idx]!=0.0)
                                {
                                    temp = ALPHA*B[lin_idx];
                                    for (i=0;i<M;i++)
                                    {
                                        lin_idx=i*N+j;
                                        lin_idx2=i*K+l;
                                        C[lin_idx] =  C[lin_idx] + temp* A[lin_idx2];
                                    }
                                }
                            }
                        }
                    }
                    else
                    {

                    /*Form C := alpha*A**T*B + beta*C */

                        for (j=0;j<N;j++)
                        {
                            for (i=0;i<M;i++)
                            {
                                temp = 0.0;
                                for (l=0;l<K;l++)
                                {
                                    lin_idx=l*M+i;
                                    lin_idx2=l*N+j;
                                    temp = temp + A[lin_idx]*B[lin_idx2];
                                }
                                 lin_idx=i*N+j;
                                if (BETA==0.0)
                                {
                                  C[lin_idx] = ALPHA*temp;
                                }
                                else
                                {
                                    C[lin_idx] = ALPHA*temp + BETA*C[lin_idx];
                                }
                            }
                        }
                    }
    }
  else
    {
    if(nota)
    {

/*Form C := alpha*A*B**T + beta*C*/

            for (j=0;j<N;j++)
            {
                if(BETA==0.0)
                {
                    for (i=0;i<M;i++)
                    {
                        lin_idx = i*N+j;
                        C[lin_idx] = 0.0;
                    }
                }
                else if (BETA!=1.0)
                {
                    for (i=0;i<M;i++)
                    {
                        lin_idx = i*N+j;
                        C[lin_idx] = BETA * C[lin_idx];
                    }
                }
                for (l=0;l<K;l++)
                {
                    lin_idx = j*K+l;
                    if (B[lin_idx]!=0.0)
                    {
                       temp = ALPHA*B[lin_idx];
                        for (i=0;i<M;i++)
                        {
                            lin_idx=i*N+j;
                            lin_idx2=i*K+l;
                            C[lin_idx] = C[lin_idx] + temp*A[lin_idx2];
                        }
                   }
                }
            }
    }
    else
    {

    /* Form C := alpha*A**T*B**T + beta*C */

            for (j=0;j<N;j++)
            {
                for (i=0;i<M;i++)
                {
                    temp = 0.0;
                    for (l=0;l<K;l++)
                    {
                        lin_idx=l*M+i;
                        lin_idx2=j*K+l;
                        temp = temp + A[lin_idx]*B[lin_idx2];
                    }
                    lin_idx=i*N+j;
                      if (BETA==0.0)
                      {
                        C[lin_idx] = ALPHA*temp;
                      }
                      else
                      {
                        C[lin_idx] = ALPHA*temp+ BETA*C[lin_idx];
                      }
                }
            }
    }
}


return ADE_RET_SUCCESS;




}
