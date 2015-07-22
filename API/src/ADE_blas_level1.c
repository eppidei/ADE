#include "headers/ADE_blas_level1.h"
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
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/ADE_Error_Handler.h"
#include "headers/ADE_defines.h"
#include <string.h>
#ifdef ADE_MEX_PRINT
#include "mex.h"
#endif

/**********Private methods Interfaces ******************/

static ADE_API_RET_T ADE_Blas_level1_launch_type1 (ADE_blas_level1_T *p_blas_l1);
static ADE_FLOATING_T ADE_Blas_level1_launch_type2 (ADE_blas_level1_T *p_blas_l1);
static ADE_CPLX_T ADE_Blas_level1_launch_type3 (ADE_blas_level1_T *p_blas_l1);
#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
static ADE_API_RET_T ADE_Blas_level1_saxpy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_scopy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_caxpy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_ccopy (ADE_blas_level1_T *p_blas_l1);
static ADE_FLOATING_T ADE_Blas_level1_sdot (ADE_blas_level1_T *p_blas_l1);
static ADE_CPLX_T ADE_Blas_level1_cdotu (ADE_blas_level1_T *p_blas_l1);
static ADE_CPLX_T ADE_Blas_level1_cdotc (ADE_blas_level1_T *p_blas_l1);
static ADE_FLOATING_T ADE_Blas_level1_snrm2 (ADE_blas_level1_T *p_blas_l1);
/* custom impls*/
static ADE_FLOATING_SP_T sdot_custom_gp_par5(ADE_INT32_T N,ADE_FLOATING_SP_T *SX,ADE_INT32_T INCX,ADE_FLOATING_SP_T* SY,ADE_INT32_T INCY);
static ADE_FLOATING_SP_T sdot_custom_gp_par4(ADE_INT32_T N,ADE_FLOATING_SP_T *SX,ADE_INT32_T INCX,ADE_FLOATING_SP_T* SY,ADE_INT32_T INCY);
static ADE_FLOATING_SP_T sdot_custom_gp_par1(ADE_INT32_T N,ADE_FLOATING_SP_T *SX,ADE_INT32_T INCX,ADE_FLOATING_SP_T* SY,ADE_INT32_T INCY);
static ADE_FLOATING_SP_T sdot_NEON(ADE_INT32_T N,ADE_FLOATING_SP_T * __restrict SX,ADE_INT32_T INCX,ADE_FLOATING_SP_T * __restrict SY,ADE_INT32_T INCY);
static ADE_API_RET_T saxpy_custom_gp_par4(ADE_INT32_T N,ADE_FLOATING_SP_T SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T INCY);
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

/************ Init Methods ***********/
ADE_API_RET_T ADE_Blas_level1_Init(ADE_blas_level1_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type )
{
    ADE_blas_level1_T* p_this = calloc(1,sizeof(ADE_blas_level1_T));

    ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,p_this);


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

           ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,math_type,"%d",(FILE*)ADE_STD_STREAM);
           return ADE_RET_ERROR;
        }

        ADE_CHECK_MEMALLOC(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Init,p_this->p_ALPHA);


        /************** D1 ALLOC *************************/

        if (math_type==ADE_MATH_REAL)
        {
             p_this->p_D1=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_MATH_CPLX)
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

        if (math_type==ADE_MATH_REAL)
        {
             p_this->p_D2=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_MATH_CPLX)
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

        if (math_type==ADE_MATH_REAL)
        {
             p_this->p_A=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_MATH_CPLX)
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

        if (math_type==ADE_MATH_REAL)
        {
             p_this->p_B=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_MATH_CPLX)
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

        if (math_type==ADE_MATH_REAL)
        {
             p_this->p_C=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_MATH_CPLX)
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

        if (math_type==ADE_MATH_REAL)
        {
             p_this->p_S=calloc(1,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_MATH_CPLX)
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

        if (math_type==ADE_MATH_REAL)
        {
             p_this->p_PARAM=calloc(5,sizeof(ADE_FLOATING_T));
        }
        else if (math_type==ADE_MATH_CPLX)
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

    if (p_blas_l1->math_type==ADE_MATH_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_MATH_CPLX)
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

    if (p_blas_l1->math_type==ADE_MATH_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_MATH_CPLX)
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

    if (p_blas_l1->math_type==ADE_MATH_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_MATH_CPLX)
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

    if (p_blas_l1->math_type==ADE_MATH_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_MATH_CPLX)
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

    if (p_blas_l1->math_type==ADE_MATH_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_MATH_CPLX)
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

    if (p_blas_l1->math_type==ADE_MATH_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_MATH_CPLX)
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

    if (p_blas_l1->math_type==ADE_MATH_REAL)
    {
        n_elements=1;
    }
    else if (p_blas_l1->math_type==ADE_MATH_CPLX)
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

    if (p_blas_l1->math_type==ADE_MATH_REAL)
    {
        n_elements=5;
    }
    else if (p_blas_l1->math_type==ADE_MATH_CPLX)
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

/***************** Configure Methods **************************/

ADE_API_RET_T ADE_Blas_level1_configure_axpy_params(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_ALPHA,ADE_INT32_T INCX,ADE_INT32_T INCY,ADE_INT32_T N)
{
    ADE_API_RET_T ret_alpha = ADE_RET_ERROR;
    ADE_API_RET_T ret_incx = ADE_RET_ERROR;
    ADE_API_RET_T ret_incy = ADE_RET_ERROR;
    ADE_API_RET_T ret_n = ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy_params,p_blas_l1);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy_params,p_ALPHA);


    ret_alpha = ADE_Blas_level1_SetALPHA(p_blas_l1,p_ALPHA);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy_params,ret_alpha);

    ret_incx  = ADE_Blas_level1_SetINCX(p_blas_l1,INCX);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy_params,ret_incx);

    ret_incy  = ADE_Blas_level1_SetINCY(p_blas_l1,INCY);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy_params,ret_incy);

    ret_n = ADE_Blas_level1_SetN(p_blas_l1,N);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy_params,ret_n);

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level1_configure_axpy_inout(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_X,ADE_FLOATING_T *p_Y)
{
    ADE_API_RET_T ret_setx = ADE_RET_ERROR;
    ADE_API_RET_T ret_sety = ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy_inout,p_blas_l1);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy_inout,p_X);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy_inout,p_Y);

    ret_setx = ADE_Blas_level1_SetX(p_blas_l1,p_X);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy_inout,ret_setx);

    ret_sety = ADE_Blas_level1_SetY(p_blas_l1,p_Y);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy_inout,ret_sety);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_level1_configure_axpy(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_ALPHA,ADE_INT32_T INCX,ADE_INT32_T INCY,ADE_INT32_T N,ADE_FLOATING_T *p_X,ADE_FLOATING_T *p_Y)
{
    ADE_API_RET_T ret_params = ADE_RET_ERROR;
    ADE_API_RET_T ret_inout = ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy,p_blas_l1);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy,p_ALPHA);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy,p_X);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy,p_Y);

    ret_params = ADE_Blas_level1_configure_axpy_params(p_blas_l1,p_ALPHA,INCX,INCY,N);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy,ret_params);

    ret_inout = ADE_Blas_level1_configure_axpy_inout(p_blas_l1,p_X,p_Y);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_axpy,ret_inout);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_level1_configure_dotc_params(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T INCX,ADE_INT32_T INCY,ADE_INT32_T N)
{

    ADE_API_RET_T ret_incx = ADE_RET_ERROR;
    ADE_API_RET_T ret_incy = ADE_RET_ERROR;
    ADE_API_RET_T ret_n = ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc_params,p_blas_l1);


    ret_incx  = ADE_Blas_level1_SetINCX(p_blas_l1,INCX);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc_params,ret_incx);

    ret_incy  = ADE_Blas_level1_SetINCY(p_blas_l1,INCY);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc_params,ret_incy);

    ret_n = ADE_Blas_level1_SetN(p_blas_l1,N);
     ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc_params,ret_n);

    return ADE_RET_SUCCESS;



}

ADE_API_RET_T ADE_Blas_level1_configure_dotc_inout(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_X,ADE_FLOATING_T *p_Y)
{
    ADE_API_RET_T ret_setx = ADE_RET_ERROR;
    ADE_API_RET_T ret_sety = ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc_inout,p_blas_l1);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc_inout,p_X);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc_inout,p_Y);

    ret_setx = ADE_Blas_level1_SetX(p_blas_l1,p_X);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc_inout,ret_setx);

    ret_sety = ADE_Blas_level1_SetY(p_blas_l1,p_Y);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc_inout,ret_sety);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_level1_configure_dotc(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T INCX,ADE_INT32_T INCY,ADE_INT32_T N,ADE_FLOATING_T *p_X,ADE_FLOATING_T *p_Y)
{

    ADE_API_RET_T ret_params = ADE_RET_ERROR;
    ADE_API_RET_T ret_inout = ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc,p_blas_l1);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc,p_X);
    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc,p_Y);

    ret_params = ADE_Blas_level1_configure_dotc_params(p_blas_l1,INCX,INCY,N);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc,ret_params);

    ret_inout = ADE_Blas_level1_configure_dotc_inout(p_blas_l1,p_X,p_Y);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dotc,ret_inout);

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level1_configure_dot_params(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T INCX,ADE_INT32_T INCY,ADE_INT32_T N)
{
    ADE_API_RET_T ret_dot_conf=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dot_params,p_blas_l1);

    ret_dot_conf = ADE_Blas_level1_configure_dotc_params(p_blas_l1,INCX,INCY,N);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dot_params,ret_dot_conf);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_Blas_level1_configure_dot_inout(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_X,ADE_FLOATING_T *p_Y)
{
    ADE_API_RET_T ret_dot_inout=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dot_inout,p_blas_l1);

    ret_dot_inout = ADE_Blas_level1_configure_dotc_inout(p_blas_l1,p_X,p_Y);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dot_inout,ret_dot_inout);

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_Blas_level1_configure_dot(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T INCX,ADE_INT32_T INCY,ADE_INT32_T N,ADE_FLOATING_T *p_X,ADE_FLOATING_T *p_Y)
{
    ADE_API_RET_T ret_dot=ADE_RET_ERROR;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dot,p_blas_l1);

    ret_dot = ADE_Blas_level1_configure_dotc(p_blas_l1,INCX,INCY,N,p_X,p_Y);
    ADE_CHECK_ADERETVAL(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_configure_dot,ret_dot);

    return ADE_RET_SUCCESS;


}

/**************** Processing Methods ****************************/

ADE_API_RET_T ADE_Blas_level1_axpy(ADE_blas_level1_T* p_blas_l1)
{
    ADE_API_RET_T ret = ADE_RET_ERROR;
    ADE_MATH_ATTRIBUTE_T math_type = ADE_MATH_REAL;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_axpy,p_blas_l1);

    math_type=p_blas_l1->math_type;

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
    if(math_type==ADE_MATH_REAL)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_saxpy;
    }
    else if (math_type==ADE_MATH_CPLX)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_caxpy;
    }

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
    if(math_type==ADE_MATH_REAL)
    {
     p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_daxpy;
     }
    else if (math_type==ADE_MATH_CPLX)
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
    ADE_MATH_ATTRIBUTE_T math_type = ADE_MATH_REAL;

     ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_copy,p_blas_l1);

     math_type=p_blas_l1->math_type;

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
    if(math_type==ADE_MATH_REAL)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_scopy;
    }
    else if (math_type==ADE_MATH_CPLX)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_ccopy;
    }
    else
    {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_INCHECKS,ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_copy,p_blas_l1->math_type,"%d",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
    }

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
    if(math_type==ADE_MATH_REAL)
    {
     p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_dcopy;
     }
    else if (math_type==ADE_MATH_CPLX)
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

    p_blas_l1->blas_level1_fcn_type2=ADE_Blas_level1_sdot;

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

    p_blas_l1->blas_level1_fcn_type2=ADE_Blas_level1_snrm2;

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

    p_blas_l1->blas_level1_fcn_type2=ADE_Blas_level1_dnrm2;

    #else
        #error(ADE_FP_PRECISION);
    #endif

    ret = ADE_Blas_level1_launch_type2(p_blas_l1);



    return ret;

}

ADE_API_RET_T ADE_Blas_level1_Print(ADE_blas_level1_T *p_blas_l1,ADE_FILE_T *p_fid, ADE_CHAR_T *obj_name, ADE_CHAR_T *calling_obj)
{
    //FILE *p_fid=ADE_STDOUT_STREAM;
    ADE_CHAR_T fixed_str[64];
    ADE_CHAR_T pri_str[128];
    ADE_SIZE_T len_str;

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Print,p_blas_l1);
ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_Print,p_fid);

memset(fixed_str,'\0',sizeof(fixed_str));
strcat(fixed_str,calling_obj);
strcat(fixed_str,"->");
strcat(fixed_str,obj_name);
strcat(fixed_str,"->");
len_str=strlen(fixed_str);

//memset(pri_str,'\0',sizeof(pri_str));

        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"math_type = %d\n"),p_blas_l1->math_type);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"N = %d\n"),p_blas_l1->N);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_ALPHA = %p(%lf)\n"),p_blas_l1->p_ALPHA,*(p_blas_l1->p_ALPHA));
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_X = %p(%lf)\n"),p_blas_l1->p_X,(p_blas_l1->p_X)[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"INCX = %d\n"),p_blas_l1->INCX);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_Y = %p(%lf)\n"),p_blas_l1->p_Y,(p_blas_l1->p_Y)[0]);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"INCY = %d\n"),p_blas_l1->INCY);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_D1 = %p(%lf)\n"),p_blas_l1->p_D1,*(p_blas_l1->p_D1));
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_D2 = %p(%lf)\n"),p_blas_l1->p_D2,*(p_blas_l1->p_D2));
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_A = %p(%lf)\n"),p_blas_l1->p_A,*(p_blas_l1->p_A));
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_B = %p(%lf)\n"),p_blas_l1->p_B,*(p_blas_l1->p_B));
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_C = %p(%lf)\n"),p_blas_l1->p_C,*(p_blas_l1->p_C));
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_S = %p(%lf)\n"),p_blas_l1->p_S,*(p_blas_l1->p_S));
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"p_PARAM = %p\n"),p_blas_l1->p_PARAM);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"blas_level1_fcn_type1 = %p\n"),p_blas_l1->blas_level1_fcn_type1);
        strcpy(pri_str,fixed_str);
        ADE_LOG(p_fid,strcat(pri_str,"blas_level1_fcn_type2 = %p\n"),p_blas_l1->blas_level1_fcn_type2);
        strcpy(pri_str,fixed_str);
       // ADE_LOG(p_fid,"\n");
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

    ADE_CHECK_INPUTPOINTER_NORET(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_launch_type2,p_blas_l1);

    ret = (p_blas_l1->blas_level1_fcn_type2)(p_blas_l1);

    return ret;
}

static ADE_CPLX_T ADE_Blas_level1_launch_type3 (ADE_blas_level1_T *p_blas_l1)
{
    ADE_CPLX_T ret ;
//    static unsigned short flag= 0;

    ADE_CHECK_INPUTPOINTER_NORET(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_launch_type3,p_blas_l1);

    ret = (p_blas_l1->blas_level1_fcn_type3)(p_blas_l1);

    return ret;
}

/*******************LIST of single precision blas*************************/

#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)

static ADE_API_RET_T ADE_Blas_level1_saxpy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

 ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_saxpy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        saxpy_custom_gp_par4(p_blas_l1->N,*p_blas_l1->p_ALPHA,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY);
#elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_NEON)

        #warning (MISSING IMPLEMENTATION ADE_Blas_level1_saxpy)

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB  )

        saxpy(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_ALPHA,(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        cblas_saxpy((p_blas_l1->N),*(p_blas_l1->p_ALPHA),p_blas_l1->p_X,(p_blas_l1->INCX),p_blas_l1->p_Y,p_blas_l1->INCY );

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)
         cblas_saxpy((p_blas_l1->N),*(p_blas_l1->p_ALPHA),p_blas_l1->p_X,(p_blas_l1->INCX),p_blas_l1->p_Y,p_blas_l1->INCY );
#else
#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_saxpy);
#endif




  return ADE_RET_SUCCESS;

}

static ADE_API_RET_T ADE_Blas_level1_scopy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

 ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_scopy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_Scopy);

         #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_NEON)

        #warning (MISSING IMPLEMENTATION ADE_Blas_level1_scopy)

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        scopy(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB  )

        cblas_scopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

        cblas_scopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_scopy);

#endif

  return ADE_RET_SUCCESS;

}


static ADE_API_RET_T ADE_Blas_level1_caxpy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_caxpy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_caxpy);

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_NEON)

        #warning (MISSING IMPLEMENTATION ADE_Blas_level1_caxpy)

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        caxpy(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_ALPHA,(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        cblas_caxpy(p_blas_l1->N,(ADE_VOID_T*)p_blas_l1->p_ALPHA,(ADE_VOID_T*)p_blas_l1->p_X,p_blas_l1->INCX,(ADE_VOID_T*)p_blas_l1->p_Y,p_blas_l1->INCY );

        #elif ( ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

        cblas_caxpy(p_blas_l1->N,p_blas_l1->p_ALPHA,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

    #error(ADE_BLAS_IMPLEMENTATION) in ADE_Blas_level1_caxpy   ;

#endif




  return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Blas_level1_ccopy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_ccopy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_ccopy);

         #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_NEON)

        #warning (MISSING IMPLEMENTATION ADE_Blas_level1_ccopy)

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ccopy(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        cblas_ccopy(p_blas_l1->N,(ADE_VOID_T*)p_blas_l1->p_X,p_blas_l1->INCX,(ADE_VOID_T*)p_blas_l1->p_Y,p_blas_l1->INCY );

        #elif ( ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

         cblas_ccopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_ccopy);

#endif




  return ADE_RET_SUCCESS;

}

static ADE_FLOATING_T ADE_Blas_level1_sdot (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_FLOATING_T ret=0;

ADE_CHECK_INPUTPOINTER_NORET(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_sdot,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ret = sdot_custom_gp_par4(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_X,p_blas_l1->INCY);

    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_NEON)

        ret = sdot_NEON(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_X,p_blas_l1->INCY);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ret = sdot(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        ret =cblas_sdot(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );

        #elif ( ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB )

        ret =cblas_sdot(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_sdot);

#endif




  return ret;

}

static ADE_CPLX_T ADE_Blas_level1_cdotu (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CPLX_T ret ;

ADE_CHECK_INPUTPOINTER_NORET(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_cdotu,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_cdotu);

#elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_NEON)

#warning ADE_MISSING_IMPLEMENTATION

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ret=cdotu(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        cblas_cdotu_sub(p_blas_l1->N,(ADE_VOID_T*)p_blas_l1->p_X,p_blas_l1->INCX,(ADE_VOID_T*)p_blas_l1->p_Y,p_blas_l1->INCY,(ADE_VOID_T*)&ret );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

         cblas_cdotu_sub(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY,(openblas_complex_float*)&ret );
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_cdotu);

#endif

  return ret;

}

static ADE_CPLX_T ADE_Blas_level1_cdotc (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CPLX_T ret ;

ADE_CHECK_INPUTPOINTER_NORET(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_cdotc,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_cdotu);
#elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_NEON)

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ret=cdotc(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        cblas_cdotc_sub(p_blas_l1->N,(ADE_VOID_T*)p_blas_l1->p_X,p_blas_l1->INCX,(ADE_VOID_T*)p_blas_l1->p_Y,p_blas_l1->INCY ,(ADE_VOID_T*)&ret);

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

         cblas_cdotc_sub(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY ,(openblas_complex_float*)&ret);
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_cdotc);

#endif




  return ret;

}



static ADE_FLOATING_T ADE_Blas_level1_snrm2 (ADE_blas_level1_T *p_blas_l1)
{

ADE_FLOATING_T ret=0;

ADE_CHECK_INPUTPOINTER_NORET(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_snrm2,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_Snrm2);
#elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_NEON)

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ret=snrm2(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX));

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        ret=cblas_snrm2(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX);

        #elif ( ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)
        ret=cblas_snrm2(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX);
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_snrm2);

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

        daxpy(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_ALPHA,(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );
   #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        cblas_daxpy(p_blas_l1->N,*(p_blas_l1->p_ALPHA),p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
    #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

        cblas_daxpy(p_blas_l1->N,*(p_blas_l1->p_ALPHA),p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_daxpy);

#endif

  return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Blas_level1_dcopy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_dcopy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_dcopy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        dcopy(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        cblas_dcopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );

        #elif ( ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB )

         cblas_dcopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_dcopy);

#endif




  return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Blas_level1_zaxpy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_zaxpy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_daxpy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        zaxpy(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_ALPHA,(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );
  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        cblas_zaxpy(p_blas_l1->N,(ADE_VOID_T*)p_blas_l1->p_ALPHA,(ADE_VOID_T*)p_blas_l1->p_X,p_blas_l1->INCX,(ADE_VOID_T*)p_blas_l1->p_Y,p_blas_l1->INCY );
    #elif ( ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)
    cblas_zaxpy(p_blas_l1->N,p_blas_l1->p_ALPHA,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_zaxpy);

#endif

  return ADE_RET_SUCCESS;

}
static ADE_API_RET_T ADE_Blas_level1_zcopy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

ADE_CHECK_INPUTPOINTER(ADE_CLASS_BLAS_LEVEL1,ADE_METHOD_zcopy,p_blas_l1);

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_zcopy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        zcopy(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        cblas_zcopy(p_blas_l1->N,(ADE_VOID_T*)p_blas_l1->p_X,p_blas_l1->INCX,(ADE_VOID_T*)p_blas_l1->p_Y,p_blas_l1->INCY );

        #elif ( ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

        cblas_zcopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_zcopy);

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

        ret=ddot(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        ret=cblas_ddot(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

        ret=cblas_ddot(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_ddot);

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

        ret=zdotu(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        cblas_zdotu_sub(p_blas_l1->N,(ADE_VOID_T*)p_blas_l1->p_X,p_blas_l1->INCX,(ADE_VOID_T*)p_blas_l1->p_Y,p_blas_l1->INCY ,(ADE_VOID_T*)&ret);

        #elif ( ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

        cblas_zdotu_sub(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY ,(openblas_complex_double*)&ret);
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_zdotu);

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

       ret=zdotc(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_VOID_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        cblas_zdotc_sub(p_blas_l1->N,(ADE_VOID_T*)p_blas_l1->p_X,p_blas_l1->INCX,(ADE_VOID_T*)p_blas_l1->p_Y,p_blas_l1->INCY,(ADE_VOID_T*)&ret );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

         cblas_zdotc_sub(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY,(openblas_complex_double*)&ret );
#else

#error(ADE_BLAS_IMPLEMENTATION in ADE_Blas_level1_zdotc);

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

        ret=dnrm2(&(p_blas_l1->N),(ADE_VOID_T*)p_blas_l1->p_X,&(p_blas_l1->INCX));

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB )

        ret=cblas_dnrm2(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX);

        #elif ( ADE_BLAS_IMPLEMENTATION==ADE_USE_OPENBLAS_LIB)

        ret=cblas_dnrm2(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX);
#else

#error(ADE_BLAS_IMPLEMENTATION);

#endif

  return ret;

}

#else
#error(ADE_FP_PRECISION);
#endif

/************************* CUSTOM GENERAL PURPOSE IMPLEMENTATIONS *************************/
/* a bare fortran translation from original sdot.f fortran source v 3.4.0 */
static ADE_FLOATING_SP_T sdot_custom_gp_par5(ADE_INT32_T N,ADE_FLOATING_SP_T *SX,ADE_INT32_T INCX,ADE_FLOATING_SP_T* SY,ADE_INT32_T INCY)
{
    ADE_FLOATING_SP_T stemp=0.0,sdot=0.0;
    ADE_INT32_T i,ix,iy,m,mp1;

    if (N<=0)
    {
        return;
    }
    if (INCX==1 && INCY==1)
    {
        m=N%5;
        if (m!=0)
        {
            for (i=0;i<m;i++)
            {
                stemp=stemp+SX[i]*SY[i];
            }
            if (N<5)
            {
                sdot=stemp;
                return sdot;
            }
        }
            mp1 = m;

            for (i=mp1;i<N;i+=5)
            {
                stemp = stemp + SX[i]*SY[i] + SX[i+1]*SY[i+1] + SX[i+2]*SY[i+2] + SX[i+3]*SY[i+3] + SX[i+4]*SY[i+4] ;
            }
    }
    else
    {
        ix=0;
        iy=0;
        if (INCX<0)
        {
            ix = (-N+1) * INCX;
        }
        if (INCY<0)
        {
            iy = (-N+1) * INCY;
        }

        for (i=0;i<N;i++)
        {
            stemp=stemp+SX[ix]*SY[iy];
            ix = ix + INCX;
            iy = iy + INCY;
        }
    }
    sdot=stemp;
    return sdot;
}
/* a custom general purpose implementation with loop unrolling for paralellism degree 4
hopefully  more suited for NEON UNIT*/
static ADE_FLOATING_SP_T sdot_custom_gp_par4(ADE_INT32_T N,ADE_FLOATING_SP_T *SX,ADE_INT32_T INCX,ADE_FLOATING_SP_T* SY,ADE_INT32_T INCY)
{
    ADE_FLOATING_SP_T stemp=0.0,sdot=0.0;
    ADE_INT32_T i,ix,iy,m,mp1;
    ADE_FLOATING_SP_T sum[4];

    if (N<=0)
    {
        return;
    }
    if (INCX==1 && INCY==1)
    {
        m=N%4;
        if (m!=0)
        {
            for (i=0;i<m;i++)
            {
                stemp=stemp+SX[i]*SY[i];
            }
            if (N<4)
            {
                sdot=stemp;
                return sdot;
            }
        }
            mp1 = m;

            for (i=mp1;i<N;i+=4)
            {
                sum[0]=SX[i]*SY[i];
                sum[1]=SX[i+1]*SY[i+1];
                sum[2]=SX[i+2]*SY[i+2];
                sum[3]=SX[i+3]*SY[i+3];
                stemp = stemp +  sum[0] +  sum[1] + sum[2]+ sum[3];
            }
    }
    else
    {
        ix=0;
        iy=0;
        if (INCX<0)
        {
            ix = (-N+1) * INCX;
        }
        if (INCY<0)
        {
            iy = (-N+1) * INCY;
        }

        for (i=0;i<N;i++)
        {
            stemp=stemp+SX[ix]*SY[iy];
            ix = ix + INCX;
            iy = iy + INCY;
        }
    }
    sdot=stemp;
    return sdot;
}

static ADE_FLOATING_SP_T sdot_custom_gp_par1(ADE_INT32_T N,ADE_FLOATING_SP_T *SX,ADE_INT32_T INCX,ADE_FLOATING_SP_T* SY,ADE_INT32_T INCY)
{
    ADE_FLOATING_SP_T stemp=0.0,sdot=0.0;
    ADE_INT32_T i,ix,iy,m,mp1;

    if (N<=0)
    {
        return;
    }
    if (INCX==1 && INCY==1)
    {
        m=0;

            mp1 = m;

            for (i=mp1;i<N;i+=1)
            {
                stemp = stemp + SX[i]*SY[i] ;
            }
    }
    else
    {
        ix=0;
        iy=0;
        if (INCX<0)
        {
            ix = (-N+1) * INCX;
        }
        if (INCY<0)
        {
            iy = (-N+1) * INCY;
        }

        for (i=0;i<N;i++)
        {
            stemp=stemp+SX[ix]*SY[iy];
            ix = ix + INCX;
            iy = iy + INCY;
        }
    }
    sdot=stemp;
    return sdot;
}

static ADE_API_RET_T saxpy_custom_gp_par4(ADE_INT32_T N,ADE_FLOATING_SP_T SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T INCY)
{

    ADE_INT32_T m,i,ix,iy,mp1;

    if (N<=0) return ADE_RET_ERROR;
    if (SA==0.0) return ADE_RET_WARNING; /* SY doesn't need to be updated*/

    if (INCX==1 || INCY==1)
    {
        m=N%4;

        if (m!=0)
        {
            for (i=0;i<m;i++)
            {
                SY[i] = SY[i] + SA*SX[i];
            }

        }
        if (N<4)
        {
            return ADE_RET_SUCCESS;
        }

        mp1 = m;

        for (i=mp1;i<N;i+=4)
        {
             SY[i] = SY[i] + SA*SX[i];
             SY[i+1] = SY[i+1] + SA*SX[i+1];
             SY[i+2] = SY[i+2] + SA*SX[i+2];
             SY[i+3] = SY[i+3] + SA*SX[i+3];

        }


    }
    else
    {
        ix = 1;
        iy = 1;

        if (INCX<0)
        {
            ix = (-N+1)*INCX;
        }
        if (INCY<0)
        {
            iy = (-N+1)*INCY;
        }

        for (i=0;i<N;i++)
        {
            SY[i] = SY[i] + SA*SX[i];
            ix = ix + INCX;
            iy = iy + INCY;
        }

    }

    return ADE_RET_SUCCESS;

}

#if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_NEON)
/************************* CUSTOM NEON UNIT IMPLEMENTATION ****************************************/
static ADE_FLOATING_SP_T sdot_NEON(ADE_INT32_T N,ADE_FLOATING_SP_T * __restrict SX,ADE_INT32_T INCX,ADE_FLOATING_SP_T * __restrict SY,ADE_INT32_T INCY)
{
	float32x4_t vec1_q, vec2_q;
	float32x4_t stemp_q={0.0, 0.0, 0.0, 0.0};
	float32x2_t tmp[2];
    ADE_FLOATING_SP_T sdot=0.0;
    ADE_INT32_T i,ix,iy,m,mp1;

    if (N<=0)
    {
        return;
    }
    if (INCX==1 && INCY==1)
    {
        m=N%4;
        if (m!=0)
        {
            for (i=0;i<m;i++)
            {
                sdot=sdot+SX[i]*SY[i];
            }
            if (N<4)
            {
                return sdot;
            }
        }
		mp1 = m;
		//Ntemp = N - m;

		for (i=mp1;i<N;i+=4)
		{
			vec1_q=vld1q_f32(&SX[i]);
			vec2_q=vld1q_f32(&SY[i]);

			stemp_q = vmlaq_f32(stemp_q, vec1_q, vec2_q );

		}

		tmp[0] = vget_high_f32(stemp_q);
		tmp[1] = vget_low_f32 (stemp_q);
		tmp[0] = vpadd_f32(tmp[0], tmp[1]);
		tmp[0] = vpadd_f32(tmp[0], tmp[0]);
		sdot = sdot + vget_lane_f32(tmp[0], 0);


		return sdot;
    }
    else
    {
        ix=0;
        iy=0;
        if (INCX<0)
        {
            ix = (-N+1) * INCX;
        }
        if (INCY<0)
        {
            iy = (-N+1) * INCY;
        }

        for (i=0;i<N;i++)
        {
            sdot=sdot+SX[ix]*SY[iy];
            ix = ix + INCX;
            iy = iy + INCY;
        }
    }
    //sdot=stemp;
    return sdot;
}
#endif
