#include "headers/ADE_blas_level1.h"
#include "headers/ADE_defines.h"
#if ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB
#include "headers/ADE_blas_wrapper.h"
#elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB || ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)
#include "headers/ADE_cblas.h"
#else
#error(ADE_BLAS_IMPLEMENTATION)
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/ADE_Error_Handler.h"
#include "headers/ADE_defines.h"

/**********Private methods Interfaces ******************/

static ADE_API_RET_T ADE_Blas_level1_launch_type1 (ADE_blas_level1_T *p_blas_l1);
static ADE_FLOATING_T ADE_Blas_level1_launch_type2 (ADE_blas_level1_T *p_blas_l1);
static ADE_CPLX_T ADE_Blas_level1_launch_type3 (ADE_blas_level1_T *p_blas_l1);
#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
static ADE_API_RET_T ADE_Blas_level1_Saxpy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_Scopy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_caxpy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_ccopy (ADE_blas_level1_T *p_blas_l1);
static ADE_FLOATING_T ADE_Blas_level1_Sdot (ADE_blas_level1_T *p_blas_l1);
static ADE_CPLX_T ADE_Blas_level1_cdotu (ADE_blas_level1_T *p_blas_l1);
static ADE_CPLX_T ADE_Blas_level1_cdotc (ADE_blas_level1_T *p_blas_l1);
static ADE_FLOATING_T ADE_Blas_level1_Snrm2 (ADE_blas_level1_T *p_blas_l1);
#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
static ADE_API_RET_T ADE_Blas_level1_daxpy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_dcopy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_zaxpy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_zcopy (ADE_blas_level1_T *p_blas_l1);
static ADE_FLOATING_T ADE_Blas_level1_ddot (ADE_blas_level1_T *p_blas_l1);
static ADE_CPLX_T ADE_Blas_level1_zdotu (ADE_blas_level1_T *p_blas_l1);
static ADE_CPLX_T ADE_Blas_level1_zdotc (ADE_blas_level1_T *p_blas_l1);
static ADE_FLOATING_T ADE_Blas_level1_dnrm2 (ADE_blas_level1_T *p_blas_l1);
#else
#error(ADE_FP_PRECISION);
#endif

/************ ADE_METHOD_Init Methods ***********/
ADE_API_RET_T ADE_Blas_level1_Init(ADE_blas_level1_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type )
{
    ADE_blas_level1_T* p_this = calloc(1,sizeof(ADE_blas_level1_T));

    ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,p_this);


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

           ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
           return ADE_RET_ERROR;
        }

        ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,p_this->p_ALPHA);


        /************** D1 ALLOC *************************/

        if (math_type==ADE_REAL)
        {
             p_this->p_D1=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_CPLX)
        {
            p_this->p_D1=calloc(1,sizeof(ADE_CPLX_T));
        }
        else
        {
          ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

         ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,p_this->p_D1);
         /************** D2 ALLOC *************************/

        if (math_type==ADE_REAL)
        {
             p_this->p_D2=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_CPLX)
        {
            p_this->p_D2=calloc(1,sizeof(ADE_CPLX_T));
        }
        else
        {
            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

        ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,p_this->p_D2);
         /************** A ALLOC *************************/

        if (math_type==ADE_REAL)
        {
             p_this->p_A=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_CPLX)
        {
            p_this->p_A=calloc(1,sizeof(ADE_CPLX_T));
        }
        else
        {

           ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

       ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,p_this->p_A);

         /************** B ALLOC *************************/

        if (math_type==ADE_REAL)
        {
             p_this->p_B=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_CPLX)
        {
            p_this->p_B=calloc(1,sizeof(ADE_CPLX_T));
        }
        else
        {

            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

        ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,p_this->p_B);

        /************** C ALLOC *************************/

        if (math_type==ADE_REAL)
        {
             p_this->p_C=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_CPLX)
        {
            p_this->p_C=calloc(1,sizeof(ADE_CPLX_T));
        }
        else
        {

            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

         ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,p_this->p_C);

        /************** S ALLOC *************************/

        if (math_type==ADE_REAL)
        {
             p_this->p_S=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_CPLX)
        {
            p_this->p_S=calloc(1,sizeof(ADE_CPLX_T));
        }
        else
        {

             ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

        ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,p_this->p_S);

        /************** PARAM ALLOC *************************/

        if (math_type==ADE_REAL)
        {
             p_this->p_PARAM=calloc(5,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_CPLX)
        {
            p_this->p_PARAM=calloc(5,sizeof(ADE_CPLX_T));
        }
        else
        {

            ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
            return ADE_RET_ERROR;
        }

         ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,p_this->p_PARAM);

        *dp_this=p_this;

    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_Blas_level1_Release (ADE_blas_level1_T* p_blas_l1)
{
     ADE_CHECKNFREE(p_blas_l1->p_ALPHA);
     ADE_CHECKNFREE(p_blas_l1->p_D1);
     ADE_CHECKNFREE(p_blas_l1->p_D2);
     ADE_CHECKNFREE(p_blas_l1->p_A);
     ADE_CHECKNFREE(p_blas_l1->p_B);
     ADE_CHECKNFREE(p_blas_l1->p_C);
     ADE_CHECKNFREE(p_blas_l1->p_S);
     ADE_CHECKNFREE(p_blas_l1->p_PARAM);
     ADE_CHECKNFREE(p_blas_l1);
}

/****************Set Methods *****************/

ADE_API_RET_T ADE_Blas_level1_SetA(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetA,p_blas_l1);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetA,p_val);

    if (p_blas_l1->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetA,p_blas_l1->math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }


    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_A)[i]=p_val[i];
    }

    return ADE_RET_SUCCESS;

}
ADE_API_RET_T ADE_Blas_level1_SetB(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
   ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetB,p_blas_l1);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetB,p_val);

    if (p_blas_l1->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetB,p_blas_l1->math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_B)[i]=p_val[i];
    }
     return ADE_RET_SUCCESS;
}
ADE_API_RET_T ADE_Blas_level1_SetC(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

      ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetC,p_blas_l1);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetC,p_val);

    if (p_blas_l1->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetC,p_blas_l1->math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_C)[i]=p_val[i];
    }
     return ADE_RET_SUCCESS;
}
ADE_API_RET_T ADE_Blas_level1_SetS(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

      ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetS,p_blas_l1);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetS,p_val);

    if (p_blas_l1->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetS,p_blas_l1->math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_S)[i]=p_val[i];
    }
     return ADE_RET_SUCCESS;

}
ADE_API_RET_T ADE_Blas_level1_SetD1(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

      ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetD1,p_blas_l1);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetD1,p_val);

    if (p_blas_l1->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetD1,p_blas_l1->math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_D1)[i]=p_val[i];
    }
     return ADE_RET_SUCCESS;

}
ADE_API_RET_T ADE_Blas_level1_SetD2(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
   ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

      ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetD2,p_blas_l1);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetD2,p_val);

    if (p_blas_l1->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetD2,p_blas_l1->math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_D2)[i]=p_val[i];
    }
     return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level1_SetALPHA(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

      ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetALPHA,p_blas_l1);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetALPHA,p_val);

    if (p_blas_l1->math_type==ADE_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_CPLX)
    {
         n_elements=2;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetALPHA,p_blas_l1->math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_ALPHA)[i]=p_val[i];
    }
     return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level1_SetPARAM(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetPARAM,p_blas_l1);
     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetPARAM,p_val);

    if (p_blas_l1->math_type==ADE_REAL)
    {
        n_elements=5;
    }
    else if (p_blas_l1->math_type==ADE_CPLX)
    {
         n_elements=10;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetPARAM,p_blas_l1->math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_PARAM)[i]=p_val[i];
    }

     return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level1_SetN(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T val)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetN,p_blas_l1);

    p_blas_l1->N=val;

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level1_SetINCX(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T val)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetINCX,p_blas_l1);

    p_blas_l1->INCX=val;

     return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level1_SetINCY(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T val)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetINCY,p_blas_l1);

    p_blas_l1->INCY=val;
     return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level1_SetX(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_buf)
{
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetX,p_blas_l1);
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetX,p_buf);

    p_blas_l1->p_X=p_buf;

     return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level1_SetY(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_buf)
{
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetY,p_blas_l1);
   ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_SetY,p_buf);

    p_blas_l1->p_Y=p_buf;
     return ADE_RET_SUCCESS;

}

/**************** Operative Methods ****************************/

ADE_API_RET_T ADE_Blas_level1_axpy(ADE_blas_level1_T* p_blas_l1)
{
    ADE_API_RET_T ret = ADE_RET_ERROR;
    ADE_MATH_ATTRIBUTE_T math_type = ADE_REAL;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_axpy,p_blas_l1);

    math_type=p_blas_l1->math_type;

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
    if(math_type==ADE_REAL)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_Saxpy;
    }
    else if (math_type==ADE_CPLX)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_caxpy;
    }

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
    if(math_type==ADE_REAL)
    {
     p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_daxpy;
     }
    else if (math_type==ADE_CPLX)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_zaxpy;
    }

     #else
        #error(ADE_FP_PRECISION);
    #endif

     ret = ADE_Blas_level1_launch_type1(p_blas_l1);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_axpy,ret);

    return ret;

}

ADE_API_RET_T ADE_Blas_level1_copy(ADE_blas_level1_T* p_blas_l1)
{
    ADE_API_RET_T ret = ADE_RET_SUCCESS;
    ADE_MATH_ATTRIBUTE_T math_type = ADE_REAL;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_copy,p_blas_l1);

     math_type=p_blas_l1->math_type;

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
    if(math_type==ADE_REAL)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_Scopy;
    }
    else if (math_type==ADE_CPLX)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_ccopy;
    }
    else
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,copy,p_blas_l1->math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
    if(math_type==ADE_REAL)
    {
     p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_dcopy;
     }
    else if (math_type==ADE_CPLX)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_zcopy;
    }
     #else
        #error(ADE_FP_PRECISION);
    #endif

     ret = ADE_Blas_level1_launch_type1(p_blas_l1);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_copy,ret);

    return ret;

}

ADE_FLOATING_T ADE_Blas_level1_dot(ADE_blas_level1_T* p_blas_l1
)
{
 ADE_FLOATING_T ret = 0;

  ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_dot,p_blas_l1);

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

    p_blas_l1->blas_level1_fcn_type2=ADE_Blas_level1_Sdot;

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

    p_blas_l1->blas_level1_fcn_type2=ADE_Blas_level1_ddot;

    #else
        #error(ADE_FP_PRECISION);
    #endif

    ret = ADE_Blas_level1_launch_type2(p_blas_l1);

    return ret;

}

ADE_CPLX_T ADE_Blas_level1_dotc(ADE_blas_level1_T* p_blas_l1)
{
 ADE_CPLX_T ret ;

  // ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_dotc,p_blas_l1);

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

    p_blas_l1->blas_level1_fcn_type3=ADE_Blas_level1_cdotc;

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

    p_blas_l1->blas_level1_fcn_type3=ADE_Blas_level1_zdotc;

    #else
        #error(ADE_FP_PRECISION);
    #endif

    ret = ADE_Blas_level1_launch_type3(p_blas_l1);


    return ret;

}

ADE_CPLX_T ADE_Blas_level1_dotu(ADE_blas_level1_T* p_blas_l1)
{
 ADE_CPLX_T ret ;

 //ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_dotu,p_blas_l1);

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

    p_blas_l1->blas_level1_fcn_type3=ADE_Blas_level1_cdotu;

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

    p_blas_l1->blas_level1_fcn_type3=ADE_Blas_level1_zdotu;

    #else
        #error(ADE_FP_PRECISION);
    #endif

    ret = ADE_Blas_level1_launch_type3(p_blas_l1);



    return ret;

}


ADE_FLOATING_T ADE_Blas_level1_nrm2(ADE_blas_level1_T* p_blas_l1)
{
 ADE_FLOATING_T ret = 0;

 //ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_nrm2,p_blas_l1);

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

    p_blas_l1->blas_level1_fcn_type2=ADE_Blas_level1_Snrm2;

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

    p_blas_l1->blas_level1_fcn_type2=ADE_Blas_level1_dnrm2;

    #else
        #error(ADE_FP_PRECISION);
    #endif

    ret = ADE_Blas_level1_launch_type2(p_blas_l1);



    return ret;

}

ADE_API_RET_T ADE_Blas_level1_Print(ADE_blas_level1_T *p_blas_l1,FILE *p_fid)
{
    //FILE *p_fid=ADE_STDOUT_STREAM;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Print,p_blas_l1);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Print,p_fid);

        fprintf(p_fid,"p_blas_l1->math_type = %d\n",p_blas_l1->math_type);
        fprintf(p_fid,"p_blas_l1->N = %d\n",p_blas_l1->N);
        fprintf(p_fid,"p_blas_l1->p_ALPHA = %p(%lf)\n",p_blas_l1->p_ALPHA,*(p_blas_l1->p_ALPHA));
        fprintf(p_fid,"p_blas_l1->p_X = %p(%lf)\n",p_blas_l1->p_X,(p_blas_l1->p_X)[0]);
        fprintf(p_fid,"p_blas_l1->INCX = %d\n",p_blas_l1->INCX);
        fprintf(p_fid,"p_blas_l1->p_Y = %p(%lf)\n",p_blas_l1->p_Y,(p_blas_l1->p_Y)[0]);
        fprintf(p_fid,"p_blas_l1->INCY = %d\n",p_blas_l1->INCY);
        fprintf(p_fid,"p_blas_l1->p_D1 = %p(%lf)\n",p_blas_l1->p_D1,*(p_blas_l1->p_D1));
        fprintf(p_fid,"p_blas_l1->p_D2 = %p(%lf)\n",p_blas_l1->p_D2,*(p_blas_l1->p_D2));
        fprintf(p_fid,"p_blas_l1->p_A = %p(%lf)\n",p_blas_l1->p_A,*(p_blas_l1->p_A));
        fprintf(p_fid,"p_blas_l1->p_B = %p(%lf)\n",p_blas_l1->p_B,*(p_blas_l1->p_B));
        fprintf(p_fid,"p_blas_l1->p_C = %p(%lf)\n",p_blas_l1->p_C,*(p_blas_l1->p_C));
        fprintf(p_fid,"p_blas_l1->p_S = %p(%lf)\n",p_blas_l1->p_S,*(p_blas_l1->p_S));
        fprintf(p_fid,"p_blas_l1->p_PARAM = %p\n",p_blas_l1->p_PARAM);
       //fprintf(p_fid,"p_blas_l1->p_out = %p(%lf)\n",p_blas_l1->p_out,(p_blas_l1->p_out)[0]);
        fprintf(p_fid,"p_blas_l1->blas_level1_fcn_type1 = %p\n",p_blas_l1->blas_level1_fcn_type1);
        fprintf(p_fid,"p_blas_l1->blas_level1_fcn_type2 = %p\n",p_blas_l1->blas_level1_fcn_type2);
        fprintf(p_fid,"\n");
      //  fclose(p_fid);

        return ADE_RET_SUCCESS;



}


/******************* private methods ***************************/

static ADE_API_RET_T ADE_Blas_level1_launch_type1 (ADE_blas_level1_T *p_blas_l1)
{
    ADE_API_RET_T ret = ADE_RET_ERROR;
    //static unsigned short flag= 0;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_launch_type1,p_blas_l1);

    ret = (p_blas_l1->blas_level1_fcn_type1)(p_blas_l1);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_launch_type1,ret);


    return ret;
}

static ADE_FLOATING_T ADE_Blas_level1_launch_type2 (ADE_blas_level1_T *p_blas_l1)
{
    ADE_FLOATING_T ret = 0;
   // static unsigned short flag= 0;

    //ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,launch_type2,p_blas_l1);

    ret = (p_blas_l1->blas_level1_fcn_type2)(p_blas_l1);

    return ret;
}

static ADE_CPLX_T ADE_Blas_level1_launch_type3 (ADE_blas_level1_T *p_blas_l1)
{
    ADE_CPLX_T ret ;
//    static unsigned short flag= 0;

    //ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,launch_type3,p_blas_l1);

    ret = (p_blas_l1->blas_level1_fcn_type3)(p_blas_l1);

    return ret;
}

/*******************LIST of single precision blas*************************/

#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_API_RET_T ADE_Blas_level1_Saxpy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

 ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,Saxpy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

//ADE_INT32_T M = 0, MP1 = 0, IX = 0, IY=0, N_int = p_blas_l1->N;
//ADE_UINT32_T i = 0;
//ADE_FLOATING_SP_T SA_int = p_blas_l1->ALPHA;
//ADE_INT32_T INCX_int = p_blas_l1->INCX , INCY_int = p_blas_l1->INCY;
//
//
//    #ifdef ADE_CHECK_INPUTS
//
//	if (N_int<=0)
//	{
//	  #if (ADE_VERBOSITY_LEVEL>=ADE_WARNING_LEVEL)
//       ADE_PRINT_WARNINGS(ADE_INCHECKS,N_int,"%d",ADE_Blas_level1_Saxpy);
//	  #endif
//	  return ADE_W0;
//
//	}
//
//	if (SA_int==0.0F)
//
//    {
//        #if (ADE_VERBOSITY_LEVEL>=ADE_WARNING_LEVEL)
//        ADE_PRINT_WARNINGS(ADE_INCHECKS,SA_int,"%f",ADE_Blas_level1_Saxpy);
//	    #endif
//	  return ADE_W1;
//    }
//
//    #endif
//
//
//
//    if ( (INCX_int==1) && (INCY_int==1) )
//    {
//        M = (N_int)%4;
//         if (M!=0)
//         {
//             for(i=0;i<M;i++)
//             {
//                  p_blas_l1->p_Y[i] = p_blas_l1->p_Y[i] + SA_int*p_blas_l1->p_X[i];
//             }
//
//         }
//
//         if  (N_int<4)
//         {
//             return ADE_RET_SUCCESS;
//         }
//         for (i = M; i<N_int;i=i+4)
//         {
//              p_blas_l1->p_Y[i] = p_blas_l1->p_Y[i] + SA_int*p_blas_l1->p_X[i];
//              p_blas_l1->p_Y[i+1] = p_blas_l1->p_Y[i+1]+ SA_int*p_blas_l1->p_X[i+1];
//              p_blas_l1->p_Y[i+2] = p_blas_l1->p_Y[i+2] + SA_int*p_blas_l1->p_X[i+2];
//              p_blas_l1->p_Y[i+3] = p_blas_l1->p_Y[i+3] + SA_int*p_blas_l1->p_X[i+3];
//         }
//
//    }
//    else
//    {
//          //  IX = 1
//         //IY = 1
//         if  (INCX_int<0)
//         {
//             IX = (-N_int+1)*INCX_int;//IX = (-N+1)*INCX + 1; TO CHECK
//         }
//         if  (INCY_int<0)
//         {
//             IY = (-N_int+1)*INCY_int;//IY = (-N+1)*INCY + 1; TO CHECK
//         }
//         for  (i=0;i<N_int;i++)
//         {
//             p_blas_l1->p_Y[IY] = p_blas_l1->p_Y[IY] + SA_int*p_blas_l1->p_X[IX];
//             IX = IX + INCX_int;
//             IY = IY + INCY_int;
//         }
//    }



  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        saxpy(&(p_blas_l1->N),p_blas_l1->p_ALPHA,p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_saxpy((p_blas_l1->N),*(p_blas_l1->p_ALPHA),p_blas_l1->p_X,(p_blas_l1->INCX),p_blas_l1->p_Y,p_blas_l1->INCY );

#else
#error(ADE_BLAS_IMPLEMENTATION);
#endif




  return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Blas_level1_Scopy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

 ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,Scopy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_Scopy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        scopy(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_scopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif

  return ADE_RET_SUCCESS;

}


static ADE_API_RET_T ADE_Blas_level1_caxpy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,caxpy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_caxpy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        caxpy(&(p_blas_l1->N),(ADE_CPLX_T*)p_blas_l1->p_ALPHA,(ADE_CPLX_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_CPLX_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_caxpy(p_blas_l1->N,(ADE_CPLX_T*)p_blas_l1->p_ALPHA,(ADE_CPLX_T*)p_blas_l1->p_X,p_blas_l1->INCX,(ADE_CPLX_T*)p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif




  return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Blas_level1_ccopy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ccopy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_ccopy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ccopy(&(p_blas_l1->N),(ADE_CPLX_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_CPLX_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_ccopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif




  return ADE_RET_SUCCESS;

}

static ADE_FLOATING_T ADE_Blas_level1_Sdot (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_FLOATING_T ret=0;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,Sdot,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_Sdot);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ret = sdot(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        ret =cblas_sdot(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif




  return ret;

}

static ADE_CPLX_T ADE_Blas_level1_cdotu (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CPLX_T ret ;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,cdotu,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_cdotu);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ret=cdotu(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_cdotu_sub(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY,(void*)&ret );
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif

  return ret;

}

static ADE_CPLX_T ADE_Blas_level1_cdotc (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CPLX_T ret ;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,cdotc,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_cdotu);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ret=cdotc(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_cdotc_sub(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY ,(void*)&ret);
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif




  return ret;

}



static ADE_FLOATING_T ADE_Blas_level1_Snrm2 (ADE_blas_level1_T *p_blas_l1)
{

ADE_FLOATING_T ret=0;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,Snrm2,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_Snrm2);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ret=snrm2(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX));

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        ret=cblas_snrm2(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX);
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif




  return ret;

}

/*******************LIST of double precision blas*************************/


#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
static ADE_API_RET_T ADE_Blas_level1_daxpy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_daxpy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_daxpy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        daxpy(&(p_blas_l1->N),p_blas_l1->p_ALPHA,p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );
   #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_daxpy(p_blas_l1->N,*(p_blas_l1->p_ALPHA),p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif




  return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Blas_level1_dcopy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_dcopy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_dcopy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        dcopy(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_dcopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif




  return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Blas_level1_zaxpy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_zaxpy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_daxpy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        zaxpy(&(p_blas_l1->N),(ADE_CPLX_T*)p_blas_l1->p_ALPHA,(ADE_CPLX_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_CPLX_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );
  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_zaxpy(p_blas_l1->N,(ADE_CPLX_T*)p_blas_l1->p_ALPHA,(ADE_CPLX_T*)p_blas_l1->p_X,p_blas_l1->INCX,(ADE_CPLX_T*)p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif

  return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Blas_level1_zcopy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_zcopy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_zcopy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        zcopy(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_zcopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif

  return ADE_RET_SUCCESS;

}
static ADE_FLOATING_T ADE_Blas_level1_ddot (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_FLOATING_T ret=0;

//ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_ddot,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_ddot);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ret=ddot(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        ret=cblas_ddot(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif

  return ret;

}

static ADE_CPLX_T ADE_Blas_level1_zdotu (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CPLX_T ret;

//ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,zdotu,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_zdotu);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ret=zdotu(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_zdotu_sub(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY ,(void*)&ret);
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif

  return ret;

}

static ADE_CPLX_T ADE_Blas_level1_zdotc (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CPLX_T ret;

//ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,zdotc,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_zdotc);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

       ret=zdotc(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_zdotc_sub(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY,(void*)&ret );
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif

  return ret;

}

static ADE_FLOATING_T ADE_Blas_level1_dnrm2 (ADE_blas_level1_T *p_blas_l1)
{


ADE_FLOATING_T ret=0;

//ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,dnrm2,p_blas_l1);
    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_Snrm2);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ret=dnrm2(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX));

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        ret=cblas_dnrm2(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX);
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif

  return ret;

}

#else
#error(ADE_FP_PRECISION);
#endif

