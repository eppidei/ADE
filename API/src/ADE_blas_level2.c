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

/***************** Private methods prototypes *********************/

static ADE_API_RET_T ADE_Blas_level2_launch_type1 (ADE_blas_level2_T *p_Blas_l2);
#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_API_RET_T ADE_Blas_level2_sger (ADE_blas_level2_T *p_Blas_l2);
static ADE_API_RET_T ADE_Blas_level2_ssbmv (ADE_blas_level2_T *p_Blas_l2);
#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
static ADE_API_RET_T ADE_Blas_level2_dger (ADE_blas_level2_T *p_Blas_l2);
static ADE_API_RET_T ADE_Blas_level2_dsbmv (ADE_blas_level2_T *p_Blas_l2);
#else
#error(ADE_FP_PRECISION);
#endif
/***************** Init methods  *********************/
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


ADE_VOID_T ADE_Blas_level2_Release (ADE_blas_level2_T* p_Blas_l2)
{
     ADE_CHECKNFREE(p_Blas_l2->p_ALPHA);
     ADE_CHECKNFREE(p_Blas_l2->p_BETA);
    ADE_CHECKNFREE(p_Blas_l2);
}
/***************** Configuration methods  *********************/
ADE_API_RET_T ADE_Blas_Level2_SetUplo(ADE_blas_level2_T* p_Blas_l2, ADE_CHAR_T UPLO)
{
    if (UPLO!='U' && UPLO!='u' && UPLO!='L' && UPLO!='l')
    {


        ADE_PRINT_ERRORS(ADE_INCHECKS,UPLO,"%c",ADE_Blas_Level2_SetUplo);
        return ADE_E38;
    }
    else
    {
        #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)
        if (UPLO=='L' || UPLO=='l')
        {
            p_Blas_l2->UPLO='U';
        }
        else
        {
            p_Blas_l2->UPLO='L';
        }


         fprintf(stderr,"REMEMBER 2 ADD VERBOSITY LEVEL CTRL @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        fprintf(stderr,"*****WARNING UPLO value is being inverted for typical C row major for coherence with column major Fortran Interface **********\n");

        #else
        p_Blas_l2->UPLO=UPLO;
        #endif
        return ADE_DEFAULT_RET;
    }


}

ADE_API_RET_T ADE_Blas_Level2_SetTrans(ADE_blas_level2_T* p_Blas_l2, ADE_CHAR_T TRANS)
{
    if (TRANS!='N' && TRANS!='n' && TRANS!='T' && TRANS!='t' && TRANS!='C' && TRANS!='c')
    {

        ADE_PRINT_ERRORS(ADE_INCHECKS,TRANS,"%c",ADE_Blas_Level2_SetTrans);
        return ADE_E38;
    }
    else
    {
        p_Blas_l2->TRANS=TRANS;
        return ADE_DEFAULT_RET;
    }


}

ADE_API_RET_T ADE_Blas_Level2_SetDiag(ADE_blas_level2_T* p_Blas_l2, ADE_CHAR_T DIAG)
{
    if (DIAG!='U' && DIAG!='u' && DIAG!='N' && DIAG!='n')
    {

        ADE_PRINT_ERRORS(ADE_INCHECKS,DIAG,"%c",ADE_Blas_Level2_SetDiag);
        return ADE_E38;
    }
    else
    {
        p_Blas_l2->DIAG=DIAG;
        return ADE_DEFAULT_RET;
    }


}

ADE_VOID_T ADE_Blas_Level2_SetM(ADE_blas_level2_T* p_Blas_l2, ADE_UINT32_T M)
{

        p_Blas_l2->M=M;

}

ADE_VOID_T ADE_Blas_Level2_SetN(ADE_blas_level2_T* p_Blas_l2, ADE_UINT32_T N)
{

        p_Blas_l2->N=N;

}

ADE_VOID_T ADE_Blas_Level2_SetK(ADE_blas_level2_T* p_Blas_l2, ADE_UINT32_T K)
{

        p_Blas_l2->K=K;

}

ADE_VOID_T ADE_Blas_Level2_SetKl(ADE_blas_level2_T* p_Blas_l2, ADE_UINT32_T KL)
{

        p_Blas_l2->KL=KL;

}

ADE_VOID_T ADE_Blas_Level2_SetKu(ADE_blas_level2_T* p_Blas_l2, ADE_UINT32_T KU)
{

        p_Blas_l2->KU=KU;

}


ADE_VOID_T ADE_Blas_Level2_SetLda(ADE_blas_level2_T* p_Blas_l2, ADE_UINT32_T val)
{

        p_Blas_l2->LDA=val;

}


ADE_VOID_T ADE_Blas_Level2_SetIncx(ADE_blas_level2_T* p_Blas_l2, ADE_UINT32_T val)
{

        p_Blas_l2->INCX=val;

}


ADE_VOID_T ADE_Blas_Level2_SetIncy(ADE_blas_level2_T* p_Blas_l2, ADE_UINT32_T val)
{

        p_Blas_l2->INCY=val;

}

ADE_API_RET_T ADE_Blas_level2_SetAlpha(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

    if (p_Blas_l2->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_Blas_l2->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_Blas_l2->math_type,"%d",ADE_Blas_level2_SetAlpha);
        return ADE_E38;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_Blas_l2->p_ALPHA)[i]=p_val[i];
    }
     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_level2_SetBeta(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

    if (p_Blas_l2->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_Blas_l2->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_Blas_l2->math_type,"%d",ADE_Blas_level2_SetBeta);
        return ADE_E38;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_Blas_l2->p_BETA)[i]=p_val[i];
    }
     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_level2_SetA(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_buff)
{
    if (p_buff==NULL)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_buff,"%p",ADE_Blas_level2_SetA);
        return ADE_E38;
    }
    else
    {
        p_Blas_l2->p_A=p_buff;
        return ADE_DEFAULT_RET;
    }

}

ADE_API_RET_T ADE_Blas_level2_SetY(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_buff)
{
    if (p_buff==NULL)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_buff,"%p",ADE_Blas_level2_SetY);
        return ADE_E38;
    }
    else
    {
        p_Blas_l2->p_Y=p_buff;
        return ADE_DEFAULT_RET;
    }

}

ADE_API_RET_T ADE_Blas_level2_SetX(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_buff)
{
    if (p_buff==NULL)
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_buff,"%p",ADE_Blas_level2_SetX);
        return ADE_E38;
    }
    else
    {
        p_Blas_l2->p_X=p_buff;
        return ADE_DEFAULT_RET;
    }

}

/***************** Operative methods  *********************/

ADE_API_RET_T ADE_Blas_level2_Elewise_Config(ADE_blas_level2_T* p_Blas_l2,ADE_FLOATING_T *p_buff1,ADE_FLOATING_T *p_buff2 ,ADE_FLOATING_T *p_out,ADE_FLOATING_T alpha_i,ADE_FLOATING_T beta_i,ADE_UINT32_T buff_len)
{

/** Configure p_Blas_l2 to work as element wise multiplier between two buffers**/

    ADE_INT32_T k=0;
    ADE_FLOATING_T alpha=alpha_i,beta=beta_i;
    ADE_API_RET_T ret = ADE_DEFAULT_RET;

    /* light input checks*/
    if (p_buff1==NULL )
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,p_buff1,"%p",ADE_Blas_level2_Elewise_Config)
        return ADE_E38;
    }
    if (p_buff2==NULL )
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,p_buff2,"%p",ADE_Blas_level2_Elewise_Config);
        return ADE_E38;
    }
    if (p_out==NULL )
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,p_out,"%p",ADE_Blas_level2_Elewise_Config);
        return ADE_E38;
    }

    ret = ADE_Blas_Level2_SetUplo(p_Blas_l2,'L');
    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blas_level2_Elewise_Config);
        return ADE_E40;
    }
    ADE_Blas_Level2_SetN(p_Blas_l2,buff_len);
    ADE_Blas_Level2_SetK(p_Blas_l2,k);
    ret=ADE_Blas_level2_SetAlpha(p_Blas_l2,&alpha);/*it's safe to pass a local pointer because the pointed value is copied, pointer is needed only for complex case*/
    if (ret<0)
    {
      ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blas_level2_Elewise_Config);
        return ADE_E40;
    }
    ret=ADE_Blas_level2_SetA(p_Blas_l2,p_buff1);
    if (ret<0)
    {
       ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blas_level2_Elewise_Config);
        return ADE_E40;
    }
    ADE_Blas_Level2_SetLda(p_Blas_l2,k+1);
    ret=ADE_Blas_level2_SetX(p_Blas_l2,p_buff2);
    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blas_level2_Elewise_Config);
        return ADE_E40;
    }
    ADE_Blas_Level2_SetIncx(p_Blas_l2,1);
    ret=ADE_Blas_level2_SetBeta(p_Blas_l2,&beta);
    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blas_level2_Elewise_Config);
        return ADE_E40;
    }
    ret=ADE_Blas_level2_SetY(p_Blas_l2,p_out);
    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blas_level2_Elewise_Config);
        return ADE_E40;
    }
    ADE_Blas_Level2_SetIncy(p_Blas_l2,1);
    //ADE_Blas_level2_Print(p_Blas_l2);

    return ADE_DEFAULT_RET;
}

ADE_API_RET_T ADE_Blas_level2_ger(ADE_blas_level2_T* p_Blas_l2)
{



    ADE_API_RET_T ret = ADE_DEFAULT_RET;

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

        p_Blas_l2->blas_level2_fcn_type1=ADE_Blas_level2_sger;

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
     p_Blas_l2->blas_level2_fcn_type1=ADE_Blas_level2_dger;
     #else
        #error(ADE_FP_PRECISION);
    #endif

     ret = ADE_Blas_level2_launch_type1(p_Blas_l2);

     #if (ADE_CHECK_RETURNS==1)
    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blas_level2_ger);
    }
    #endif

    return ret;

}

ADE_API_RET_T ADE_Blas_level2_sbmv(ADE_blas_level2_T* p_Blas_l2)
{



    ADE_API_RET_T ret = ADE_DEFAULT_RET;

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

        p_Blas_l2->blas_level2_fcn_type1=ADE_Blas_level2_ssbmv;

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
     p_Blas_l2->blas_level2_fcn_type1=ADE_Blas_level2_dsbmv;
     #else
        #error(ADE_FP_PRECISION);
    #endif

     ret = ADE_Blas_level2_launch_type1(p_Blas_l2);

     #if (ADE_CHECK_RETURNS==1)
    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blas_sbmv);
    }
    #endif

    return ret;

}


ADE_API_RET_T ADE_Blas_level2_Elewise(ADE_blas_level2_T* p_Blas_l2)
{
ADE_API_RET_T ret = ADE_DEFAULT_RET;
ADE_UINT32_T i = 0;

 #if (ADE_LIN_ALG_IMP==ADE_USE_LIN_ALG_CUSTOM)

 for (i=0;i<p_Blas_l2->N;i++)
 {
     (p_Blas_l2->p_Y)[i]=(p_Blas_l2->p_A)[i]*(p_Blas_l2->p_X)[i];
 }

 #elif (ADE_LIN_ALG_IMP==ADE_USE_LIN_ALG_BLAS)

    ret = ADE_Blas_level2_sbmv(p_Blas_l2);
 #else
     #error(ADE_LIN_ALG_IMP)

     #endif
    return ret;
}

ADE_API_RET_T ADE_Blas_level2_Print(ADE_blas_level2_T *p_Blas_l2)
{
    FILE *p_fid=stdout;


   // p_fid=fopen(p_name,"w");

    if (p_fid!=NULL)
    {

        fprintf(p_fid,"p_Blas_l2->math_type = %d\n",p_Blas_l2->math_type);
        fprintf(p_fid,"p_Blas_l2->UPLO = %c\n",p_Blas_l2->UPLO);
        fprintf(p_fid,"p_Blas_l2->TRANS = %c\n",p_Blas_l2->TRANS);
        fprintf(p_fid,"p_Blas_l2->DIAG = %c\n",p_Blas_l2->DIAG);
        fprintf(p_fid,"p_Blas_l2->M = %d\n",p_Blas_l2->M);
        fprintf(p_fid,"p_Blas_l2->N = %d\n",p_Blas_l2->N);
        fprintf(p_fid,"p_Blas_l2->K = %d\n",p_Blas_l2->K);
        fprintf(p_fid,"p_Blas_l2->KL = %d\n",p_Blas_l2->KL);
        fprintf(p_fid,"p_Blas_l2->KU = %d\n",p_Blas_l2->KU);
        fprintf(p_fid,"p_Blas_l2->p_ALPHA = %p(%lf)\n",p_Blas_l2->p_ALPHA,*(p_Blas_l2->p_ALPHA));
        fprintf(p_fid,"p_Blas_l2->p_A = %p(%lf)\n",p_Blas_l2->p_A,(p_Blas_l2->p_A)[0]);
        fprintf(p_fid,"p_Blas_l2->LDA = %d\n",p_Blas_l2->LDA);
        fprintf(p_fid,"p_Blas_l2->p_X = %p(%lf)\n",p_Blas_l2->p_X,(p_Blas_l2->p_X)[0]);
        fprintf(p_fid,"p_Blas_l2->INCX = %d\n",p_Blas_l2->INCX);
        fprintf(p_fid,"p_Blas_l2->p_BETA = %p(%lf)\n",p_Blas_l2->p_BETA,*(p_Blas_l2->p_BETA));
        fprintf(p_fid,"p_Blas_l2->p_Y = %p(%lf)\n",p_Blas_l2->p_Y,(p_Blas_l2->p_Y)[0]);
        fprintf(p_fid,"p_Blas_l2->INCY = %d\n",p_Blas_l2->INCY);
        fprintf(p_fid,"p_Blas_l2->blas_level2_fcn_type1 = %p\n",p_Blas_l2->blas_level2_fcn_type1);
        fprintf(p_fid,"\n");
      //  fclose(p_fid);

        return ADE_DEFAULT_RET;
    }

    else
    {

        return ADE_E40;
    }




}

/****************************** Private methods ********************************************/

static ADE_API_RET_T ADE_Blas_level2_launch_type1 (ADE_blas_level2_T *p_Blas_l2)
{
    ADE_API_RET_T ret = ADE_DEFAULT_RET;
    static unsigned short flag= 0;

    #if (ADE_CHECK_INPUTS==ADE_CHECK_INPUTS_TRUE)

        if (flag==0)
        {
             printf("REMEMBER TO ADD CHECKS ADE_Blas_level2_launch_type1\n");
             flag++;
        }


    #endif

    ret = (p_Blas_l2->blas_level2_fcn_type1)(p_Blas_l2);

    #if (ADE_CHECK_RETURNS==1)

    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_RETCHECKS,ret,"%d",ADE_Blas_level2_launch_type1)
    }
    #endif

    return ret;
}

#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_API_RET_T ADE_Blas_level2_sger (ADE_blas_level2_T *p_Blas_l2)
{
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level2_sger);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    sger(&(p_Blas_l2->M),&(p_Blas_l2->N),p_Blas_l2->p_ALPHA,p_Blas_l2->p_X,&(p_Blas_l2->INCX),p_Blas_l2->p_Y,&(p_Blas_l2->INCY),p_Blas_l2->p_A,&(p_Blas_l2->LDA));

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

    cblas_sger(CblasRowMajor,(p_Blas_l2->M),(p_Blas_l2->N),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_X,(p_Blas_l2->INCX),p_Blas_l2->p_Y,(p_Blas_l2->INCY),p_Blas_l2->p_A,(p_Blas_l2->LDA));
     #else

        #error(ADE_BLAS_IMPLEMENTATION);

    #endif

    return ADE_DEFAULT_RET;

}

static ADE_API_RET_T ADE_Blas_level2_ssbmv (ADE_blas_level2_T *p_Blas_l2)
{
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level2_ssbmv);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    ssbmv(&(p_Blas_l2->UPLO),&(p_Blas_l2->N),&(p_Blas_l2->K),p_Blas_l2->p_ALPHA,p_Blas_l2->p_A,&(p_Blas_l2->LDA),p_Blas_l2->p_X,&(p_Blas_l2->INCX),p_Blas_l2->p_BETA,p_Blas_l2->p_Y,&(p_Blas_l2->INCY));

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)



     if (p_Blas_l2->UPLO=='L' || p_Blas_l2->UPLO=='l')
     {
          cblas_ssbmv(CblasRowMajor,CblasLower,(p_Blas_l2->N),(p_Blas_l2->K),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_A,(p_Blas_l2->LDA),p_Blas_l2->p_X,(p_Blas_l2->INCX),*(p_Blas_l2->p_BETA),p_Blas_l2->p_Y,(p_Blas_l2->INCY));
     }
     else
     {
         cblas_ssbmv(CblasRowMajor,CblasUpper,(p_Blas_l2->N),(p_Blas_l2->K),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_A,(p_Blas_l2->LDA),p_Blas_l2->p_X,(p_Blas_l2->INCX),*(p_Blas_l2->p_BETA),p_Blas_l2->p_Y,(p_Blas_l2->INCY));
     }



     #else

        #error(ADE_BLAS_IMPLEMENTATION);

    #endif

return ADE_DEFAULT_RET;

}

#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
static ADE_API_RET_T ADE_Blas_level2_dger (ADE_blas_level2_T *p_Blas_l2)
{
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level2_dger);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    dger(&(p_Blas_l2->M),&(p_Blas_l2->N),p_Blas_l2->p_ALPHA,p_Blas_l2->p_X,&(p_Blas_l2->INCX),p_Blas_l2->p_Y,&(p_Blas_l2->INCY),p_Blas_l2->p_A,&(p_Blas_l2->LDA));

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

    cblas_dger(CblasRowMajor,(p_Blas_l2->M),(p_Blas_l2->N),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_X,(p_Blas_l2->INCX),p_Blas_l2->p_Y,(p_Blas_l2->INCY),p_Blas_l2->p_A,(p_Blas_l2->LDA));
     #else

        #error(ADE_BLAS_IMPLEMENTATION);

    #endif

return ADE_DEFAULT_RET;

}

static ADE_API_RET_T ADE_Blas_level2_dsbmv (ADE_blas_level2_T *p_Blas_l2)
{
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level2_dsbmv);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    dsbmv(&(p_Blas_l2->UPLO),&(p_Blas_l2->N),&(p_Blas_l2->K),p_Blas_l2->p_ALPHA,p_Blas_l2->p_A,&(p_Blas_l2->LDA),p_Blas_l2->p_X,&(p_Blas_l2->INCX),p_Blas_l2->p_BETA,p_Blas_l2->p_Y,&(p_Blas_l2->INCY));

     #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)



     if (p_Blas_l2->UPLO=='L' || p_Blas_l2->UPLO=='l')
     {
          cblas_dsbmv(CblasRowMajor,CblasLower,(p_Blas_l2->N),(p_Blas_l2->K),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_A,(p_Blas_l2->LDA),p_Blas_l2->p_X,(p_Blas_l2->INCX),*(p_Blas_l2->p_BETA),p_Blas_l2->p_Y,(p_Blas_l2->INCY));
     }
     else
     {
         cblas_dsbmv(CblasRowMajor,CblasUpper,(p_Blas_l2->N),(p_Blas_l2->K),*(p_Blas_l2->p_ALPHA),p_Blas_l2->p_A,(p_Blas_l2->LDA),p_Blas_l2->p_X,(p_Blas_l2->INCX),*(p_Blas_l2->p_BETA),p_Blas_l2->p_Y,(p_Blas_l2->INCY));
     }

     #else

        #error(ADE_BLAS_IMPLEMENTATION);

    #endif

return ADE_DEFAULT_RET;

}

#else
#error(ADE_FP_PRECISION);
#endif
