#include "headers/ADE_blas_level2.h"
#include "headers/ADE_defines.h"
#if ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB
#include "headers/ADE_blas_wrapper.h"
#elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB || ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)
#include "headers/ADE_cblas.h"
#else
#error(ADE_BLAS_IMPLEMENTATION)
#endif
#include "headers/ADE_errors.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/ADE_Error_Handler.h"
#include <string.h>
#ifdef ADE_MEX_PRINT
#include "mex.h"
#endif

/***************** Private methods prototypes *********************/

static ADE_API_RET_T ADE_Blas_level2_launch_type1 (ADE_blas_level2_T *p_Blas_l2);
static ADE_API_RET_T ADE_Blas_level2_doElewiseCustom(ADE_FLOATING_T* p_Y, ADE_FLOATING_T* p_X,ADE_FLOATING_T* p_A,ADE_UINT32_T N);
#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_API_RET_T ADE_Blas_level2_sger (ADE_blas_level2_T *p_Blas_l2);
static ADE_API_RET_T ADE_Blas_level2_ssbmv (ADE_blas_level2_T *p_Blas_l2);
/* custom impls */
static ADE_API_RET_T sger_custom_gp(ADE_INT32_T M,ADE_INT32_T N,ADE_FLOATING_SP_T ALPHA,ADE_FLOATING_SP_T* X,ADE_INT32_T INCX,ADE_FLOATING_SP_T *Y,ADE_INT32_T INCY,ADE_FLOATING_SP_T *A,ADE_INT32_T LDA);
#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
static ADE_API_RET_T ADE_Blas_level2_dger (ADE_blas_level2_T *p_Blas_l2);
static ADE_API_RET_T ADE_Blas_level2_dsbmv (ADE_blas_level2_T *p_Blas_l2);
#else
#error(ADE_FP_PRECISION);
#endif
/***************** ADE_METHOD_Init methods  *********************/
ADE_API_RET_T ADE_Blas_level2_Init(ADE_blas_level2_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type )
{
    ADE_blas_level2_T* p_this = calloc(1,sizeof(ADE_blas_level2_T));
     ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_Init,p_this);

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
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }


         ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_Init,p_this->p_ALPHA);




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
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

         ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_Init,p_this->p_BETA);


        *dp_this=p_this;


    return ADE_RET_SUCCESS;
}


ADE_VOID_T ADE_Blas_level2_Release (ADE_blas_level2_T* p_Blas_l2)
{
     ADE_CHECKNFREE(p_Blas_l2->p_ALPHA);
     ADE_CHECKNFREE(p_Blas_l2->p_BETA);
    ADE_CHECKNFREE(p_Blas_l2);
}
/***************** Set methods  *********************/
ADE_API_RET_T ADE_Blas_Level2_SetUplo(ADE_blas_level2_T* p_Blas_l2, ADE_CHAR_T UPLO)
{

//FILE * p_stream=ADE_STDERR_STREAM;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetUplo,p_Blas_l2);

    if (UPLO!='U' && UPLO!='u' && UPLO!='L' && UPLO!='l')
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetUplo,UPLO,"%c",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    else
    {
        #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)
        /* UPLO value is being inverted for typical C row major for coherence with column major Fortran Interface */
        if (UPLO=='L' || UPLO=='l')
        {
            p_Blas_l2->UPLO='U';
        }
        else
        {
            p_Blas_l2->UPLO='L';
        }


     //    ADE_LOG(p_stream,"REMEMBER 2 ADD VERBOSITY LEVEL CTRL @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
      //  ADE_LOG(p_stream,"*****WARNING UPLO value is being inverted for typical C row major for coherence with column major Fortran Interface **********\n");

        #else
        p_Blas_l2->UPLO=UPLO;
        #endif
        return ADE_RET_SUCCESS;
    }


}

ADE_API_RET_T ADE_Blas_Level2_SetTrans(ADE_blas_level2_T* p_Blas_l2, ADE_CHAR_T TRANS)
{

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetTrans,p_Blas_l2);

    if (TRANS!='N' && TRANS!='n' && TRANS!='T' && TRANS!='t' && TRANS!='C' && TRANS!='c')
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetTrans,TRANS,"%c",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    else
    {
        p_Blas_l2->TRANS=TRANS;
        return ADE_RET_SUCCESS;
    }
}

ADE_API_RET_T ADE_Blas_Level2_SetDiag(ADE_blas_level2_T* p_Blas_l2, ADE_CHAR_T DIAG)
{

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetDiag,p_Blas_l2);

    if (DIAG!='U' && DIAG!='u' && DIAG!='N' && DIAG!='n')
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetDiag,DIAG,"%c",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    else
    {
        p_Blas_l2->DIAG=DIAG;
        return ADE_RET_SUCCESS;
    }

}

ADE_API_RET_T ADE_Blas_Level2_SetM(ADE_blas_level2_T* p_Blas_l2, ADE_INT32_T M)
{
    ADE_INT32_T val0=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetM,p_Blas_l2);
    ADE_CHECK_VALUE_MAJOREQUAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetM,M,"%d",val0);

    p_Blas_l2->M=M;

     return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_Level2_SetN(ADE_blas_level2_T* p_Blas_l2, ADE_INT32_T N)
{
    ADE_INT32_T val0=0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetN,p_Blas_l2);
    ADE_CHECK_VALUE_MAJOREQUAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetN,N,"%d",val0);

        p_Blas_l2->N=N;
     return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_Level2_SetK(ADE_blas_level2_T* p_Blas_l2, ADE_INT32_T K)
{
    ADE_INT32_T val0=0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetK,p_Blas_l2);
     ADE_CHECK_VALUE_MAJOREQUAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetK,K,"%d",val0);

    p_Blas_l2->K=K;

     return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_Level2_SetKl(ADE_blas_level2_T* p_Blas_l2, ADE_INT32_T KL)
{
    ADE_INT32_T val0=0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetKl,p_Blas_l2);
     ADE_CHECK_VALUE_MAJOREQUAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetKl,KL,"%d",val0);

     p_Blas_l2->KL=KL;

     return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_Level2_SetKu(ADE_blas_level2_T* p_Blas_l2, ADE_INT32_T KU)
{
     ADE_INT32_T val0=0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetKu,p_Blas_l2);
     ADE_CHECK_VALUE_MAJOREQUAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetKu,KU,"%d",val0);

        p_Blas_l2->KU=KU;

     return ADE_RET_SUCCESS;
}


ADE_API_RET_T ADE_Blas_Level2_SetLda(ADE_blas_level2_T* p_Blas_l2, ADE_INT32_T val)
{
    ADE_INT32_T min_val=0;
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetLda,p_Blas_l2);
   min_val = 1 + p_Blas_l2->KL + p_Blas_l2->KU;
    ADE_CHECK_VALUE_MAJOREQUAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetLda,val,"%d",min_val);

        p_Blas_l2->LDA=val;
     return ADE_RET_SUCCESS;
}


ADE_API_RET_T ADE_Blas_Level2_SetINCX(ADE_blas_level2_T* p_Blas_l2, ADE_INT32_T val)
{
     ADE_INT32_T val0=0;

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetINCX,p_Blas_l2);
   ADE_CHECK_VALUE_NOTEQUAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetINCX,val,"%d",val0);

        p_Blas_l2->INCX=val;
     return ADE_RET_SUCCESS;
}


ADE_API_RET_T ADE_Blas_Level2_SetINCY(ADE_blas_level2_T* p_Blas_l2, ADE_INT32_T val)
{
    ADE_INT32_T val0=0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetINCY,p_Blas_l2);
     ADE_CHECK_VALUE_NOTEQUAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetINCY,val,"%d",val0);
        p_Blas_l2->INCY=val;
     return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_level2_SetALPHA(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;
     ADE_MATH_ATTRIBUTE_T math_type=ADE_MATH_REAL;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetALPHA,p_Blas_l2);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetALPHA,p_val);

    math_type=p_Blas_l2->math_type;

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
        (p_Blas_l2->p_ALPHA)[i]=p_val[i];
    }
     return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level2_SetBeta(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;
     ADE_MATH_ATTRIBUTE_T math_type=ADE_MATH_REAL;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetBeta,p_Blas_l2);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetBeta,p_val);

    math_type=p_Blas_l2->math_type;

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
        (p_Blas_l2->p_BETA)[i]=p_val[i];
    }
     return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level2_SetA(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_buff)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetA,p_Blas_l2);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetA,p_buff);
        p_Blas_l2->p_A=p_buff;
        return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level2_SetY(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_buff)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetY,p_Blas_l2);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetY,p_buff);

        p_Blas_l2->p_Y=p_buff;
        return ADE_RET_SUCCESS;


}

ADE_API_RET_T ADE_Blas_level2_SetX(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_buff)
{
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetX,p_Blas_l2);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_SetX,p_buff);
        p_Blas_l2->p_X=p_buff;
        return ADE_RET_SUCCESS;


}

/************************** Configure Methods ***********************************/

ADE_API_RET_T ADE_Blas_level2_configure_elewise_params(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T alpha_i,ADE_FLOATING_T beta_i)
{

    ADE_INT32_T k=0;
    ADE_FLOATING_T alpha=alpha_i,beta=beta_i;
    ADE_API_RET_T ret_uplo = ADE_RET_ERROR;
    ADE_API_RET_T ret_K = ADE_RET_ERROR;
    ADE_API_RET_T ret_Alpha = ADE_RET_ERROR;
    ADE_API_RET_T ret_Beta = ADE_RET_ERROR;
    ADE_API_RET_T ret_Lda = ADE_RET_ERROR;
    ADE_API_RET_T ret_incx = ADE_RET_ERROR;
    ADE_API_RET_T ret_incy = ADE_RET_ERROR;


    ret_uplo = ADE_Blas_Level2_SetUplo(p_Blas_l2,'L');
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise_params,ret_uplo);


    ret_K=ADE_Blas_Level2_SetK(p_Blas_l2,k);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise_params,ret_K);

    ret_Alpha=ADE_Blas_level2_SetALPHA(p_Blas_l2,&alpha);/*it's safe to pass a local pointer because the pointed value is copied, pointer is needed only for complex case*/
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise_params,ret_Alpha);

    ret_Lda=ADE_Blas_Level2_SetLda(p_Blas_l2,k+1);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise_params,ret_Lda);

    ret_incx=ADE_Blas_Level2_SetINCX(p_Blas_l2,1);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise_params,ret_incx);

    ret_Beta=ADE_Blas_level2_SetBeta(p_Blas_l2,&beta);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise_params,ret_Beta);

    ret_incy=ADE_Blas_Level2_SetINCY(p_Blas_l2,1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise_params,ret_incy);

    return ADE_RET_SUCCESS;



}

ADE_API_RET_T ADE_Blas_level2_configure_elewise_inout(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_buff1,ADE_FLOATING_T *p_buff2 ,ADE_FLOATING_T *p_out,ADE_UINT32_T buff_len)
{

    ADE_API_RET_T ret_x = ADE_RET_ERROR;
    ADE_API_RET_T ret_y = ADE_RET_ERROR;
    ADE_API_RET_T ret_A = ADE_RET_ERROR;
     ADE_API_RET_T ret_N = ADE_RET_ERROR;


    ret_A=ADE_Blas_level2_SetA(p_Blas_l2,p_buff1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise_inout,ret_A);

    ret_x=ADE_Blas_level2_SetX(p_Blas_l2,p_buff2);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise_inout,ret_x);

     ret_N=ADE_Blas_Level2_SetN(p_Blas_l2,buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise_params,ret_N);

    ret_y=ADE_Blas_level2_SetY(p_Blas_l2,p_out);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise_inout,ret_y);

     return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_level2_configure_elewise(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_buff1,ADE_FLOATING_T *p_buff2 ,ADE_FLOATING_T *p_out,ADE_FLOATING_T alpha_i,ADE_FLOATING_T beta_i,ADE_UINT32_T buff_len)
{

/** Configure p_Blas_l2 to work as element wise multiplier between two buffers**/
    ADE_API_RET_T ret_params = ADE_RET_ERROR;
    ADE_API_RET_T ret_inout = ADE_RET_ERROR;

    ret_params = ADE_Blas_level2_configure_elewise_params(p_Blas_l2,alpha_i,beta_i);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise,ret_params);

    ret_inout = ADE_Blas_level2_configure_elewise_inout(p_Blas_l2,p_buff1,p_buff2 ,p_out,buff_len);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_configure_elewise,ret_inout);

    return ADE_RET_SUCCESS;
}

/***************** Processing methods  *********************/

ADE_API_RET_T ADE_Blas_level2_ger(ADE_blas_level2_T* p_Blas_l2)
{
    ADE_API_RET_T ret = ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_ger,p_Blas_l2);

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

        p_Blas_l2->blas_level2_fcn_type1=ADE_Blas_level2_sger;

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
     p_Blas_l2->blas_level2_fcn_type1=ADE_Blas_level2_dger;
     #else
        #error(ADE_FP_PRECISION) in ADE_Blas_level2_ger
    #endif

     ret = ADE_Blas_level2_launch_type1(p_Blas_l2);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_ger,ret);

    return ret;

}

ADE_API_RET_T ADE_Blas_level2_sbmv(ADE_blas_level2_T* p_Blas_l2)
{

    ADE_API_RET_T ret = ADE_RET_ERROR;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_sbmv,p_Blas_l2);

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

        p_Blas_l2->blas_level2_fcn_type1=ADE_Blas_level2_ssbmv;

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
     p_Blas_l2->blas_level2_fcn_type1=ADE_Blas_level2_dsbmv;
     #else
        #error(ADE_FP_PRECISION) in ADE_Blas_level2_sbmv
    #endif

    ret = ADE_Blas_level2_launch_type1(p_Blas_l2);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_sbmv,ret);

    return ret;

}


ADE_API_RET_T ADE_Blas_level2_Elewise(ADE_blas_level2_T* p_Blas_l2)
{
ADE_API_RET_T ret = ADE_RET_ERROR;
ADE_UINT32_T i = 0;
ADE_FLOATING_T* p_Y=NULL;
ADE_FLOATING_T* p_X=NULL;
ADE_FLOATING_T* p_A=NULL;
ADE_UINT32_T N=0;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_Elewise,p_Blas_l2);


 #if (ADE_LIN_ALG_IMP==ADE_USE_LIN_ALG_CUSTOM)
p_Y=p_Blas_l2->p_Y;
p_X=p_Blas_l2->p_X;
p_A=p_Blas_l2->p_A;
N=p_Blas_l2->N;
 ret=ADE_Blas_level2_doElewiseCustom(p_Y,p_X,p_A,N);

 #elif (ADE_LIN_ALG_IMP==ADE_USE_LIN_ALG_BLAS)

    ret = ADE_Blas_level2_sbmv(p_Blas_l2);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_Elewise,ret);

 #else
     #error(ADE_LIN_ALG_IMP)

     #endif
    return ret;
}



ADE_API_RET_T ADE_Blas_level2_Print(ADE_blas_level2_T *p_Blas_l2,FILE *p_fid,ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj)
{
    //FILE *p_fid=ADE_STDOUT_STREAM;
   ADE_CHAR_T fixed_str[64];
    ADE_CHAR_T pri_str[128];
    ADE_SIZE_T len_str;

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_Print,p_Blas_l2);

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_Print,p_fid);

    memset(fixed_str,'\0',sizeof(fixed_str));
strcat(fixed_str,calling_obj);
strcat(fixed_str,"->");
strcat(fixed_str,obj_name);
strcat(fixed_str,"->");
len_str=strlen(fixed_str);

    if (p_fid!=NULL)
    {
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"math_type = %d\n"),p_Blas_l2->math_type);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"UPLO = %c\n"),p_Blas_l2->UPLO);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"TRANS = %c\n"),p_Blas_l2->TRANS);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"DIAG = %c\n"),p_Blas_l2->DIAG);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"M = %d\n"),p_Blas_l2->M);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"N = %d\n"),p_Blas_l2->N);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"K = %d\n"),p_Blas_l2->K);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"KL = %d\n"),p_Blas_l2->KL);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"KU = %d\n"),p_Blas_l2->KU);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_ALPHA = %p(%lf)\n"),p_Blas_l2->p_ALPHA,*(p_Blas_l2->p_ALPHA));
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_A = %p(%lf)\n"),p_Blas_l2->p_A,(p_Blas_l2->p_A)[0]);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"LDA = %d\n"),p_Blas_l2->LDA);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_X = %p(%lf)\n"),p_Blas_l2->p_X,(p_Blas_l2->p_X)[0]);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"INCX = %d\n"),p_Blas_l2->INCX);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_BETA = %p(%lf)\n"),p_Blas_l2->p_BETA,*(p_Blas_l2->p_BETA));
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_Y = %p(%lf)\n"),p_Blas_l2->p_Y,(p_Blas_l2->p_Y)[0]);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"INCY = %d\n"),p_Blas_l2->INCY);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"blas_level2_fcn_type1 = %p\n"),p_Blas_l2->blas_level2_fcn_type1);
         strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,"\n");
      //  fclose(p_fid);

        return ADE_RET_SUCCESS;
    }

    else
    {

        return ADE_RET_ERROR;
    }




}

/****************************** Private methods ********************************************/
static ADE_API_RET_T ADE_Blas_level2_doElewiseCustom(ADE_FLOATING_T* p_Y, ADE_FLOATING_T* p_X,ADE_FLOATING_T* p_A,ADE_UINT32_T N)
{

ADE_UINT32_T i = 0;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_doElewiseCustom,p_Y);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_doElewiseCustom,p_X);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_doElewiseCustom,p_A);

 for (i=0;i<N;i++)
 {
     (p_Y)[i]=(p_A)[i]*(p_X)[i];
 }

 return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Blas_level2_launch_type1 (ADE_blas_level2_T *p_Blas_l2)
{
    ADE_API_RET_T ret = ADE_RET_ERROR;
    //static unsigned short flag= 0;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_launch_type1,p_Blas_l2);

    ret = (p_Blas_l2->blas_level2_fcn_type1)(p_Blas_l2);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_launch_type1,ret);

    return ret;
}

#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_API_RET_T ADE_Blas_level2_sger (ADE_blas_level2_T *p_Blas_l2)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_sger,p_Blas_l2);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    sger_custom_gp(p_Blas_l2->M,p_Blas_l2->N,p_Blas_l2->p_ALPHA,p_Blas_l2->p_X,p_Blas_l2->INCX,_Blas_l2->p_Y,p_Blas_l2->INCY,p_Blas_l2->p_A,p_Blas_l2->LDA);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    sger(&(p_Blas_l2->M),&(p_Blas_l2->N),(ADE_VOID_T*)p_Blas_l2->p_ALPHA,(ADE_VOID_T*)p_Blas_l2->p_X,&(p_Blas_l2->INCX),(ADE_VOID_T*)p_Blas_l2->p_Y,&(p_Blas_l2->INCY),(ADE_VOID_T*)p_Blas_l2->p_A,&(p_Blas_l2->LDA));

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

    cblas_sger(CblasRowMajor,(p_Blas_l2->M),(p_Blas_l2->N),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_X,(p_Blas_l2->INCX),p_Blas_l2->p_Y,(p_Blas_l2->INCY),p_Blas_l2->p_A,(p_Blas_l2->LDA));

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

      cblas_sger(CblasRowMajor,(p_Blas_l2->M),(p_Blas_l2->N),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_X,(p_Blas_l2->INCX),p_Blas_l2->p_Y,(p_Blas_l2->INCY),p_Blas_l2->p_A,(p_Blas_l2->LDA));

     #else

        #error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level2_sger);

    #endif

    return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Blas_level2_ssbmv (ADE_blas_level2_T *p_Blas_l2)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_ssbmv,p_Blas_l2);
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level2_ssbmv);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    ssbmv(&(p_Blas_l2->UPLO),&(p_Blas_l2->N),&(p_Blas_l2->K),(ADE_VOID_T*)p_Blas_l2->p_ALPHA,(ADE_VOID_T*)p_Blas_l2->p_A,&(p_Blas_l2->LDA),(ADE_VOID_T*)p_Blas_l2->p_X,&(p_Blas_l2->INCX),(ADE_VOID_T*)p_Blas_l2->p_BETA,(ADE_VOID_T*)p_Blas_l2->p_Y,&(p_Blas_l2->INCY));

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )



     if (p_Blas_l2->UPLO=='L' || p_Blas_l2->UPLO=='l')
     {
          cblas_ssbmv(CblasRowMajor,CblasLower,(p_Blas_l2->N),(p_Blas_l2->K),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_A,(p_Blas_l2->LDA),p_Blas_l2->p_X,(p_Blas_l2->INCX),*(p_Blas_l2->p_BETA),p_Blas_l2->p_Y,(p_Blas_l2->INCY));
     }
     else
     {
         cblas_ssbmv(CblasRowMajor,CblasUpper,(p_Blas_l2->N),(p_Blas_l2->K),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_A,(p_Blas_l2->LDA),p_Blas_l2->p_X,(p_Blas_l2->INCX),*(p_Blas_l2->p_BETA),p_Blas_l2->p_Y,(p_Blas_l2->INCY));
     }

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

     if (p_Blas_l2->UPLO=='L' || p_Blas_l2->UPLO=='l')
     {
          cblas_ssbmv(CblasRowMajor,CblasLower,(p_Blas_l2->N),(p_Blas_l2->K),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_A,(p_Blas_l2->LDA),p_Blas_l2->p_X,(p_Blas_l2->INCX),*(p_Blas_l2->p_BETA),p_Blas_l2->p_Y,(p_Blas_l2->INCY));
     }
     else
     {
         cblas_ssbmv(CblasRowMajor,CblasUpper,(p_Blas_l2->N),(p_Blas_l2->K),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_A,(p_Blas_l2->LDA),p_Blas_l2->p_X,(p_Blas_l2->INCX),*(p_Blas_l2->p_BETA),p_Blas_l2->p_Y,(p_Blas_l2->INCY));
     }

     #else

        #error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level2_ssbmv);

    #endif

return ADE_RET_SUCCESS;

}

#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
static ADE_API_RET_T ADE_Blas_level2_dger (ADE_blas_level2_T *p_Blas_l2)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_dger,p_Blas_l2);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level2_dger);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    dger(&(p_Blas_l2->M),&(p_Blas_l2->N),(ADE_VOID_T*)p_Blas_l2->p_ALPHA,(ADE_VOID_T*)p_Blas_l2->p_X,&(p_Blas_l2->INCX),(ADE_VOID_T*)p_Blas_l2->p_Y,&(p_Blas_l2->INCY),(ADE_VOID_T*)p_Blas_l2->p_A,&(p_Blas_l2->LDA));

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

    cblas_dger(CblasRowMajor,(p_Blas_l2->M),(p_Blas_l2->N),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_X,(p_Blas_l2->INCX),p_Blas_l2->p_Y,(p_Blas_l2->INCY),p_Blas_l2->p_A,(p_Blas_l2->LDA));

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

     cblas_dger(CblasRowMajor,(p_Blas_l2->M),(p_Blas_l2->N),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_X,(p_Blas_l2->INCX),p_Blas_l2->p_Y,(p_Blas_l2->INCY),p_Blas_l2->p_A,(p_Blas_l2->LDA));

     #else

        #error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level2_dger);

    #endif

return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Blas_level2_dsbmv (ADE_blas_level2_T *p_Blas_l2)
{

        ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL2,ADE_METHOD_dsbmv,p_Blas_l2);
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level2_dsbmv);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    dsbmv(&(p_Blas_l2->UPLO),&(p_Blas_l2->N),&(p_Blas_l2->K),(ADE_VOID_T*)p_Blas_l2->p_ALPHA,(ADE_VOID_T*)p_Blas_l2->p_A,&(p_Blas_l2->LDA),(ADE_VOID_T*)p_Blas_l2->p_X,&(p_Blas_l2->INCX),(ADE_VOID_T*)p_Blas_l2->p_BETA,(ADE_VOID_T*)p_Blas_l2->p_Y,&(p_Blas_l2->INCY));

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

     if (p_Blas_l2->UPLO=='L' || p_Blas_l2->UPLO=='l')
     {
          cblas_dsbmv(CblasRowMajor,CblasLower,(p_Blas_l2->N),(p_Blas_l2->K),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_A,(p_Blas_l2->LDA),p_Blas_l2->p_X,(p_Blas_l2->INCX),*(p_Blas_l2->p_BETA),p_Blas_l2->p_Y,(p_Blas_l2->INCY));
     }
     else
     {
         cblas_dsbmv(CblasRowMajor,CblasUpper,(p_Blas_l2->N),(p_Blas_l2->K),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_A,(p_Blas_l2->LDA),p_Blas_l2->p_X,(p_Blas_l2->INCX),*(p_Blas_l2->p_BETA),p_Blas_l2->p_Y,(p_Blas_l2->INCY));
     }

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

     if (p_Blas_l2->UPLO=='L' || p_Blas_l2->UPLO=='l')
     {
          cblas_dsbmv(CblasRowMajor,CblasLower,(p_Blas_l2->N),(p_Blas_l2->K),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_A,(p_Blas_l2->LDA),p_Blas_l2->p_X,(p_Blas_l2->INCX),*(p_Blas_l2->p_BETA),p_Blas_l2->p_Y,(p_Blas_l2->INCY));
     }
     else
     {
         cblas_dsbmv(CblasRowMajor,CblasUpper,(p_Blas_l2->N),(p_Blas_l2->K),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_A,(p_Blas_l2->LDA),p_Blas_l2->p_X,(p_Blas_l2->INCX),*(p_Blas_l2->p_BETA),p_Blas_l2->p_Y,(p_Blas_l2->INCY));
     }
     #else

        #error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level2_dsbmv);

    #endif

return ADE_RET_SUCCESS;

}

#else
#error(ADE_FP_PRECISION);
#endif

/************************* CUSTOM GENERAL PURPOSE IMPLEMENTATIONS *************************/
/* a bare fortran translation from original sger.f fortran source v 3.4.0 */

static ADE_API_RET_T sger_custom_gp(ADE_INT32_T M,ADE_INT32_T N,ADE_FLOATING_SP_T ALPHA,ADE_FLOATING_SP_T* X,ADE_INT32_T INCX,ADE_FLOATING_SP_T *Y,ADE_INT32_T INCY,ADE_FLOATING_SP_T *A,ADE_INT32_T LDA)
{

    ADE_INT32_T j,i,lin_idx,kx,jy,ix;
    ADE_FLOATING_SP_T temp;

    if ((M==0) || (N==0) || (ALPHA==0.0)) return ADE_RET_ERROR;

    if (INCY>0)
    {
        jy = 0;
    }
    else
    {
        jy = 0 - (N-1)*INCY;
    }

    if (INCX==1)
    {
        for (j=0;j<N;j++)
        {
            if (Y[jy]!=0.0)
            {
                temp=ALPHA*Y[jy];
                for (i=0;i<M;i++)
                {
                    lin_idx = i*N+j;
                    A[lin_idx]=A[lin_idx]+X[i]*temp;
                }
            }
            jy=jy+INCY;
        }
    }
    else
    {
        if (INCX>0)
        {
            kx=0;
        }
        else
        {
            kx = 0 - (M-1)*INCX;
        }
        for(j=0;j<N;j++)
        {
            if(Y[jy]!=0)
            {
                temp=ALPHA*Y[jy];
                ix=kx;
                for (i=0;i<M;i++)
                {
                     lin_idx = i*N+j;
                     A[lin_idx]=A[lin_idx] + X[ix]*temp;
                     ix = ix + INCX;
                }

            }
            jy=jy+INCY;

        }

    }


    return ADE_RET_SUCCESS;


}
