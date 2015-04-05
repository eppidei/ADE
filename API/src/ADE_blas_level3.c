#include "headers/ADE_blas_level3.h"
#include "headers/ADE_defines.h"
#if ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB
#include "headers/ADE_blas_wrapper.h"
#elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB || ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)
#include "headers/ADE_cblas.h"
#else
#error(ADE_BLAS_IMPLEMENTATION)
#endif
#include "headers/ADE_complex.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/ADE_Error_Handler.h"

/*******************Private methods interface*******************************/

static ADE_API_RET_T ADE_Blas_level3_launch_type1 (ADE_blas_level3_T *p_Blas_l3);
#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_API_RET_T ADE_Blas_level3_sgemm (ADE_blas_level3_T *p_Blas_l3);
static ADE_API_RET_T ADE_Blas_level3_cgemm (ADE_blas_level3_T *p_Blas_l3);

#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

static ADE_API_RET_T ADE_Blas_level3_dgemm (ADE_blas_level3_T *p_Blas_l3);
static ADE_API_RET_T ADE_Blas_level3_zgemm (ADE_blas_level3_T *p_Blas_l3);

#else
#error(ADE_FP_PRECISION);
#endif

/**********************Init Methods************************************/

ADE_API_RET_T ADE_Blas_level3_Init(ADE_blas_level3_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type )
{
    ADE_blas_level3_T* p_this = calloc(1,sizeof(ADE_blas_level3_T));
      ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL3,Initp_this);

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
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

         ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL3,Init,p_this->p_ALPHA);




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
             ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

       ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL3,Init,p_this->p_BETA);


        *dp_this=p_this;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_BLAS_LEVEL3,Init,p_this,"%p",(FILE*)ADE_STD_STREAM);
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

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetTrans,p_Blas_l3);

    if (val!='N' && val!='n' && val!='T' && val!='t' && val!='C' && val!='c')
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,SetTrans,val,"%c",(FILE*)ADE_STD_STREAM);
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
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetTransA,p_Blas_l3);

    if (val!='N' && val!='n' && val!='T' && val!='t' && val!='C' && val!='c')
    {

         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,SetTransA,val,"%c",(FILE*)ADE_STD_STREAM);
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

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetTransB,p_Blas_l3);

    if (val!='N' && val!='n' && val!='T' && val!='t' && val!='C' && val!='c')
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,SetTransB,val,"%c",(FILE*)ADE_STD_STREAM);
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

   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetDiag,p_Blas_l3);

    if (val!='U' && val!='u' && val!='N' && val!='n')
    {

        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,SetDiag,val,"%c",(FILE*)ADE_STD_STREAM);
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

    FILE *p_stream=ADE_STDERR_STREAM;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetUplo,p_Blas_l3);

    if (val!='U' && val!='u' && val!='L' && val!='l')
    {


        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,SetUplo,val,"%c",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
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


         fprintf(p_stream,"REMEMBER 2 ADD VERBOSITY LEVEL CTRL @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        fprintf(p_stream,"*****WARNING UPLO value is being inverted for typical C row major for coherence with column major Fortran Interface **********\n");

        #else
        p_Blas_l3->UPLO=val;
        #endif
        return ADE_RET_SUCCESS;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetSide(ADE_blas_level3_T* p_Blas_l3, ADE_CHAR_T val)
{
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetSide,p_Blas_l3);

    if (val!='R' && val!='r' && val!='L' && val!='l')
    {

       ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,SetSide,val,"%c",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }
    else
    {
        p_Blas_l3->SIDE=val;
        return ADE_RET_SUCCESS;
    }


}

ADE_API_RET_T ADE_Blas_Level3_SetM(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val)
{
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetM,p_Blas_l3);

        p_Blas_l3->M=val;

        return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_Level3_SetN(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val)
{
  ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetN,p_Blas_l3);

        p_Blas_l3->N=val;

         return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_Level3_SetK(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val)
{
  ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetK,p_Blas_l3);

        p_Blas_l3->K=val;

         return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_Level3_SetLda(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val)
{
  ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetLda,p_Blas_l3);

        p_Blas_l3->LDA=val;

         return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_Level3_SetLdb(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val)
{
 ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetLdb,p_Blas_l3);

        p_Blas_l3->LDB=val;

        return ADE_RET_SUCCESS;

}


ADE_API_RET_T ADE_Blas_Level3_SetLdc(ADE_blas_level3_T* p_Blas_l3, ADE_UINT32_T val)
{
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetLdc,p_Blas_l3);

        p_Blas_l3->LDC=val;

        return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_Level3_SetA(ADE_blas_level3_T* p_Blas_l3, ADE_FLOATING_T * p_data)
{
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetA,p_Blas_l3);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetA,p_data);

    p_Blas_l3->p_A=p_data;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_Level3_SetB(ADE_blas_level3_T* p_Blas_l3, ADE_FLOATING_T * p_data)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetB,p_Blas_l3);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetB,p_data);

    p_Blas_l3->p_B=p_data;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_Level3_SetC(ADE_blas_level3_T* p_Blas_l3, ADE_FLOATING_T * p_data)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetC,p_Blas_l3);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetC,p_data);

    p_Blas_l3->p_C=p_data;

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_Level3_SetALPHA(ADE_blas_level3_T* p_Blas_l3, ADE_FLOATING_T *p_val)
{
      ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetALPHA,p_Blas_l3);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetALPHA,p_val);

    if (p_Blas_l3->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_Blas_l3->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,SetALPHA,p_Blas_l3->math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
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

      ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetBeta,p_Blas_l3);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,SetBeta,p_val);

    if (p_Blas_l3->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_Blas_l3->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,SetBeta,p_Blas_l3->math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_Blas_l3->p_BETA)[i]=p_val[i];
    }
     return ADE_RET_SUCCESS;
}
/**************** Operative methods *************************/

ADE_API_RET_T ADE_Blas_level3_gemm(ADE_blas_level3_T* p_Blas_l3)
{

    ADE_API_RET_T ret = ADE_RET_SUCCESS;
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,gemm,p_Blas_l3);

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
        #error(ADE_FP_PRECISION);
    #endif

     ret = ADE_Blas_level3_launch_type1(p_Blas_l3);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL3,gemm,ret);

    return ret;

}


ADE_API_RET_T ADE_Blas_level3_Print(ADE_blas_level3_T *p_Blas_l3,FILE *p_fid)
{


 ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,Print,p_Blas_l3);
  ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,Print,p_fid);


        fprintf(p_fid,"p_Blas_l3->math_type = %d\n",p_Blas_l3->math_type);
        fprintf(p_fid,"p_Blas_l3->SIDE = %c\n",p_Blas_l3->SIDE);
        fprintf(p_fid,"p_Blas_l3->UPLO = %c\n",p_Blas_l3->UPLO);
        fprintf(p_fid,"p_Blas_l3->TRANS = %c\n",p_Blas_l3->TRANS);
        fprintf(p_fid,"p_Blas_l3->TRANSA = %c\n",p_Blas_l3->TRANSA);
        fprintf(p_fid,"p_Blas_l3->TRANSB = %c\n",p_Blas_l3->TRANSB);
        fprintf(p_fid,"p_Blas_l3->DIAG = %c\n",p_Blas_l3->DIAG);
        fprintf(p_fid,"p_Blas_l3->M = %d\n",p_Blas_l3->M);
        fprintf(p_fid,"p_Blas_l3->N = %d\n",p_Blas_l3->N);
        fprintf(p_fid,"p_Blas_l3->K = %d\n",p_Blas_l3->K);
        if (p_Blas_l3->math_type==ADE_REAL)
        {
            fprintf(p_fid,"p_Blas_l3->p_ALPHA = %p(%lf)\n",p_Blas_l3->p_ALPHA,*(p_Blas_l3->p_ALPHA));
            fprintf(p_fid,"p_Blas_l3->p_BETA = %p(%lf)\n",p_Blas_l3->p_BETA,*(p_Blas_l3->p_BETA));
            fprintf(p_fid,"p_Blas_l3->p_A = %p(%lf)\n",p_Blas_l3->p_A,(p_Blas_l3->p_A)[0]);
            fprintf(p_fid,"p_Blas_l3->p_B = %p(%lf)\n",p_Blas_l3->p_B,(p_Blas_l3->p_B)[0]);
            fprintf(p_fid,"p_Blas_l3->p_C = %p(%lf)\n",p_Blas_l3->p_C,(p_Blas_l3->p_C)[0]);
        }
        else if (p_Blas_l3->math_type==ADE_CPLX)
        {
            fprintf(p_fid,"p_Blas_l3->p_ALPHA = %p(%lf+%lfi)\n",p_Blas_l3->p_ALPHA,ADE_creal(*((ADE_CPLX_T*)p_Blas_l3->p_ALPHA)),ADE_cimag(*((ADE_CPLX_T*)p_Blas_l3->p_ALPHA)));
            fprintf(p_fid,"p_Blas_l3->p_BETA = %p(%lf+%lfi)\n",p_Blas_l3->p_BETA,ADE_creal(*((ADE_CPLX_T*)p_Blas_l3->p_BETA)),ADE_cimag(*((ADE_CPLX_T*)p_Blas_l3->p_BETA)));
            fprintf(p_fid,"p_Blas_l3->p_A = %p(%lf+%lfi)\n",p_Blas_l3->p_A,ADE_creal(((ADE_CPLX_T*)p_Blas_l3->p_A)[0]),ADE_cimag(((ADE_CPLX_T*)p_Blas_l3->p_A)[0]));
            fprintf(p_fid,"p_Blas_l3->p_B = %p(%lf+%lfi)\n",p_Blas_l3->p_B,ADE_creal(((ADE_CPLX_T*)p_Blas_l3->p_B)[0]),ADE_cimag(((ADE_CPLX_T*)p_Blas_l3->p_B)[0]));
            fprintf(p_fid,"p_Blas_l3->p_C = %p(%lf+%lfi)\n",p_Blas_l3->p_C,ADE_creal(((ADE_CPLX_T*)p_Blas_l3->p_C)[0]),ADE_cimag(((ADE_CPLX_T*)p_Blas_l3->p_C)[0]));
        }
        else
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL3,Print,p_Blas_l3->math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

        fprintf(p_fid,"p_Blas_l3->LDA = %d\n",p_Blas_l3->LDA);
        fprintf(p_fid,"p_Blas_l3->LDB = %d\n",p_Blas_l3->LDB);
        fprintf(p_fid,"p_Blas_l3->LDC = %d\n",p_Blas_l3->LDC);

        fprintf(p_fid,"p_Blas_l3->blas_level3_fcn_type1 = %p\n",p_Blas_l3->blas_level3_fcn_type1);
        fprintf(p_fid,"\n");
      //  fclose(p_fid);

        return ADE_RET_SUCCESS;
    }

}

/***************** Private methods implementations ********************/
static ADE_API_RET_T ADE_Blas_level3_launch_type1 (ADE_blas_level3_T *p_Blas_l3)
{

    ADE_API_RET_T ret = ADE_RET_SUCCESS;
    static unsigned short flag= 0;

 ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,launch_type1,p_Blas_l3);


    (p_Blas_l3->blas_level3_fcn_type1)(p_Blas_l3);

    return ret;
}

#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_API_RET_T ADE_Blas_level3_sgemm (ADE_blas_level3_T *p_Blas_l3)
{

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,sgemm,p_Blas_l3);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level3_sgemm);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)
   /*** Att. blas have to invert inputs for Row major operations cinterface.pdf pgg.191****/
    sgemm(&(p_Blas_l3->TRANSA),&(p_Blas_l3->TRANSB),&(p_Blas_l3->N),&(p_Blas_l3->M),&(p_Blas_l3->K),p_Blas_l3->p_ALPHA,p_Blas_l3->p_B,&(p_Blas_l3->LDB),p_Blas_l3->p_A,&(p_Blas_l3->LDA),p_Blas_l3->p_BETA,p_Blas_l3->p_C,&(p_Blas_l3->LDC));


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
    cblas_sgemm(cblas_order,cblas_transposea,cblas_transposeb,p_Blas_l3->M,p_Blas_l3->N,p_Blas_l3->K,*(p_Blas_l3->p_ALPHA),p_Blas_l3->p_A,p_Blas_l3->LDA,p_Blas_l3->p_B,p_Blas_l3->LDB,*(p_Blas_l3->p_BETA),p_Blas_l3->p_C,p_Blas_l3->LDC);

     #else

        #error(ADE_BLAS_IMPLEMENTATION);

    #endif
return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blas_level3_cgemm (ADE_blas_level3_T *p_Blas_l3)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,cgemm,p_Blas_l3);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level3_cgemm);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    cgemm(&(p_Blas_l3->TRANSA),&(p_Blas_l3->TRANSB),&(p_Blas_l3->N),&(p_Blas_l3->M),&(p_Blas_l3->K),(ADE_CPLX_T*)p_Blas_l3->p_ALPHA,(ADE_CPLX_T*)p_Blas_l3->p_B,&(p_Blas_l3->LDB),(ADE_CPLX_T*)p_Blas_l3->p_A,&(p_Blas_l3->LDA),(ADE_CPLX_T*)p_Blas_l3->p_BETA,(ADE_CPLX_T*)p_Blas_l3->p_C,&(p_Blas_l3->LDC));


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


    cblas_cgemm(cblas_order,cblas_transposea,cblas_transposeb,p_Blas_l3->M,p_Blas_l3->N,p_Blas_l3->K,(p_Blas_l3->p_ALPHA),p_Blas_l3->p_A,p_Blas_l3->LDA,p_Blas_l3->p_B,p_Blas_l3->LDB,(p_Blas_l3->p_BETA),p_Blas_l3->p_C,p_Blas_l3->LDC);

     #else

        #error(ADE_BLAS_IMPLEMENTATION);

    #endif
return ADE_RET_SUCCESS;
}

#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

static ADE_API_RET_T ADE_Blas_level3_dgemm (ADE_blas_level3_T *p_Blas_l3)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,dgemm,p_Blas_l3);
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level3_dgemm);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    dgemm(&(p_Blas_l3->TRANSA),&(p_Blas_l3->TRANSB),&(p_Blas_l3->N),&(p_Blas_l3->M),&(p_Blas_l3->K),p_Blas_l3->p_ALPHA,p_Blas_l3->p_B,&(p_Blas_l3->LDB),p_Blas_l3->p_A,&(p_Blas_l3->LDA),p_Blas_l3->p_BETA,p_Blas_l3->p_C,&(p_Blas_l3->LDC));


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

     #else

        #error(ADE_BLAS_IMPLEMENTATION);

    #endif
return ADE_RET_SUCCESS;
}

static ADE_API_RET_T ADE_Blas_level3_zgemm (ADE_blas_level3_T *p_Blas_l3)
{
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL3,zgemm,p_Blas_l3);
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

    ADE_MISSING_IMPLEMENTATION(ADE_Blas_level3_zgemm);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

    zgemm(&(p_Blas_l3->TRANSA),&(p_Blas_l3->TRANSB),&(p_Blas_l3->N),&(p_Blas_l3->M),&(p_Blas_l3->K),(ADE_CPLX_T*)p_Blas_l3->p_ALPHA,(ADE_CPLX_T*)p_Blas_l3->p_B,&(p_Blas_l3->LDB),(ADE_CPLX_T*)p_Blas_l3->p_A,&(p_Blas_l3->LDA),(ADE_CPLX_T*)p_Blas_l3->p_BETA,(ADE_CPLX_T*)p_Blas_l3->p_C,&(p_Blas_l3->LDC));


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
    cblas_zgemm(cblas_order,cblas_transposea,cblas_transposeb,p_Blas_l3->M,p_Blas_l3->N,p_Blas_l3->K,(p_Blas_l3->p_ALPHA),p_Blas_l3->p_A,p_Blas_l3->LDA,p_Blas_l3->p_B,p_Blas_l3->LDB,(p_Blas_l3->p_BETA),p_Blas_l3->p_C,p_Blas_l3->LDC);

     #else

        #error(ADE_BLAS_IMPLEMENTATION);

    #endif
return ADE_RET_SUCCESS;
}
#else
#error(ADE_FP_PRECISION);
#endif
