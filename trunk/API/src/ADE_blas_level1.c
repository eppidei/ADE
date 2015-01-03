#include "headers/ADE_blas_level1.h"
#include "headers/ADE_blas_wrapper.h"
#include "headers/ADE_defines.h"
#include "headers/ADE_errors.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static ADE_API_RET_T ADE_Blas_level1_launch_type1 (ADE_blas_level1_T *p_blas_l1);
#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
static ADE_API_RET_T ADE_Blas_level1_saxpy (ADE_blas_level1_T *p_blas_l1);
#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
static ADE_API_RET_T ADE_Blas_level1_daxpy (ADE_blas_level1_T *p_blas_l1);
#else
ADE_DEFINE_ERROR(ADE_FP_PRECISION);
#endif


ADE_API_RET_T ADE_Blas_level1_Init(ADE_blas_level1_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type )
{
    ADE_blas_level1_T* p_this = calloc(1,sizeof(ADE_blas_level1_T));

    if (p_this!=NULL)
    {
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
            ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Blas_level1_Init);
            return ADE_E36;
        }

        if (p_this->p_ALPHA==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_ALPHA,"%p",ADE_Blas_level1_Init);
            return ADE_E2;
        }


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
            ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Blas_level1_Init);
            return ADE_E36;
        }

        if (p_this->p_D1==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_D1,"%p",ADE_Blas_level1_Init);
            return ADE_E2;
        }

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
            ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Blas_level1_Init);
            return ADE_E36;
        }

        if (p_this->p_D2==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_D2,"%p",ADE_Blas_level1_Init);
            return ADE_E2;
        }

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
            ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Blas_level1_Init);
            return ADE_E36;
        }

        if (p_this->p_A==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_A,"%p",ADE_Blas_level1_Init);
            return ADE_E2;
        }

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
            ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Blas_level1_Init);
            return ADE_E36;
        }

        if (p_this->p_B==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_B,"%p",ADE_Blas_level1_Init);
            return ADE_E2;
        }

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
            ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Blas_level1_Init);
            return ADE_E36;
        }

        if (p_this->p_C==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_C,"%p",ADE_Blas_level1_Init);
            return ADE_E2;
        }

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
            ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Blas_level1_Init);
            return ADE_E36;
        }

        if (p_this->p_S==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_S,"%p",ADE_Blas_level1_Init);
            return ADE_E2;
        }

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
            ADE_PRINT_ERRORS(ADE_INCHECKS,math_type,"%d",ADE_Blas_level1_Init);
            return ADE_E36;
        }

        if (p_this->p_PARAM==NULL)
        {
            ADE_PRINT_ERRORS(ADE_MEM,p_this->p_PARAM,"%p",ADE_Blas_level1_Init);
            return ADE_E2;
        }

        *dp_this=p_this;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_MEM,p_this,"%p",ADE_Blas_level1_Init);
        return ADE_E2;
    }



    return ADE_DEFAULT_RET;
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

ADE_API_RET_T ADE_Blas_level1_setA(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

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
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l1->math_type,"%d",ADE_Blas_level1_setA);
        return ADE_E36;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_A)[i]=p_val[i];
    }

    return ADE_DEFAULT_RET;

}
ADE_API_RET_T ADE_Blas_level1_setB(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
   ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

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
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l1->math_type,"%d",ADE_Blas_level1_setB);
        return ADE_E36;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_B)[i]=p_val[i];
    }
     return ADE_DEFAULT_RET;
}
ADE_API_RET_T ADE_Blas_level1_setC(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

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
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l1->math_type,"%d",ADE_Blas_level1_setC);
        return ADE_E36;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_C)[i]=p_val[i];
    }
     return ADE_DEFAULT_RET;
}
ADE_API_RET_T ADE_Blas_level1_setS(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

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
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l1->math_type,"%d",ADE_Blas_level1_setS);
        return ADE_E36;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_S)[i]=p_val[i];
    }
     return ADE_DEFAULT_RET;

}
ADE_API_RET_T ADE_Blas_level1_setD1(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

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
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l1->math_type,"%d",ADE_Blas_level1_setD1);
        return ADE_E36;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_D1)[i]=p_val[i];
    }
     return ADE_DEFAULT_RET;

}
ADE_API_RET_T ADE_Blas_level1_setD2(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
   ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

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
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l1->math_type,"%d",ADE_Blas_level1_setD2);
        return ADE_E36;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_D2)[i]=p_val[i];
    }
     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_level1_setALPHA(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

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
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l1->math_type,"%d",ADE_Blas_level1_setALPHA);
        return ADE_E36;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_ALPHA)[i]=p_val[i];
    }
     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_level1_setN(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T val)
{
    p_blas_l1->N=val;
     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_level1_setINCX(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T val)
{
    p_blas_l1->INCX=val;
     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_level1_setINCY(ADE_blas_level1_T* p_blas_l1,ADE_INT32_T val)
{
    p_blas_l1->INCY=val;
     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_level1_setX(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_buf)
{
    p_blas_l1->p_X=p_buf;
     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_level1_setY(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_buf)
{
    p_blas_l1->p_Y=p_buf;
     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_level1_set_pOut(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_buf)
{
    p_blas_l1->p_out=p_buf;
     return ADE_DEFAULT_RET;

}



ADE_API_RET_T ADE_Blas_level1_setPARAM(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val)
{
    ADE_UINT32_T n_elements = 0;
     ADE_UINT32_T i = 0;

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
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l1->math_type,"%d",ADE_Blas_level1_setPARAM);
        return ADE_E36;
    }

    for (i=0;i<n_elements;i++)
    {
        (p_blas_l1->p_PARAM)[i]=p_val[i];
    }

     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_real_axpy(ADE_blas_level1_T* p_blas_l1)
{
    ADE_API_RET_T ret = ADE_DEFAULT_RET;

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_saxpy;
    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
     p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_daxpy;
     #else
        ADE_DEFINE_ERROR(ADE_FP_PRECISION);
    #endif

     ret = ADE_Blas_level1_launch_type1(p_blas_l1);

     #if (ADE_CHECK_RETURNS==1)
    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_BLAS_L1,ret,"%d",ADE_Blas_real_axpy);
    }
    #endif

    return ret;

}


static ADE_API_RET_T ADE_Blas_level1_launch_type1 (ADE_blas_level1_T *p_blas_l1)
{
    ADE_API_RET_T ret = ADE_DEFAULT_RET;
    static unsigned short flag= 0;

    #if (ADE_CHECK_INPUTS==1)

        if (flag==0)
        {
             printf("REMEMBER TO ADD CHECKS ADE_Blas_level1_launch_type1\n");
             flag++;
        }


    #endif

    ret = (p_blas_l1->blas_level1_fcn_type1)(p_blas_l1);

    #if (ADE_CHECK_RETURNS==1)

    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_BLAS_L1,ret,"%d",ADE_Blas_level1_launch_type1);
    }
    #endif

    return ret;
}

/*******************LIST of single precision blas*************************/

#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)


static ADE_API_RET_T ADE_Blas_level1_saxpy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

ADE_INT32_T M = 0, MP1 = 0, IX = 0, IY=0, N_int = p_blas_l1->N;
ADE_UINT32_T i = 0;
ADE_FLOATING_SP_T SA_int = p_blas_l1->ALPHA;
ADE_INT32_T INCX_int = p_blas_l1->INCX , INCY_int = p_blas_l1->INCY;


    #ifdef ADE_CHECK_INPUTS

	if (N_int<=0)
	{
	  #if (ADE_VERBOSITY_LEVEL>=ADE_WARNING_LEVEL)
       ADE_PRINT_WARNINGS(ADE_INCHECKS,N_int,"%d",ADE_Blas_level1_saxpy);
	  #endif
	  return ADE_W0;

	}

	if (SA_int==0.0F)

    {
        #if (ADE_VERBOSITY_LEVEL>=ADE_WARNING_LEVEL)
        ADE_PRINT_WARNINGS(ADE_INCHECKS,SA_int,"%f",ADE_Blas_level1_saxpy);
	    #endif
	  return ADE_W1;
    }

    #endif



    if ( (INCX_int==1) && (INCY_int==1) )
    {
        M = (N_int)%4;
         if (M!=0)
         {
             for(i=0;i<M;i++)
             {
                  p_blas_l1->p_Y[i] = p_blas_l1->p_Y[i] + SA_int*p_blas_l1->p_X[i];
             }

         }

         if  (N_int<4)
         {
             return ADE_DEFAULT_RET;
         }
         for (i = M; i<N_int;i=i+4)
         {
              p_blas_l1->p_Y[i] = p_blas_l1->p_Y[i] + SA_int*p_blas_l1->p_X[i];
              p_blas_l1->p_Y[i+1] = p_blas_l1->p_Y[i+1]+ SA_int*p_blas_l1->p_X[i+1];
              p_blas_l1->p_Y[i+2] = p_blas_l1->p_Y[i+2] + SA_int*p_blas_l1->p_X[i+2];
              p_blas_l1->p_Y[i+3] = p_blas_l1->p_Y[i+3] + SA_int*p_blas_l1->p_X[i+3];
         }

    }
    else
    {
          //  IX = 1
         //IY = 1
         if  (INCX_int<0)
         {
             IX = (-N_int+1)*INCX_int;//IX = (-N+1)*INCX + 1; TO CHECK
         }
         if  (INCY_int<0)
         {
             IY = (-N_int+1)*INCY_int;//IY = (-N+1)*INCY + 1; TO CHECK
         }
         for  (i=0;i<N_int;i++)
         {
             p_blas_l1->p_Y[IY] = p_blas_l1->p_Y[IY] + SA_int*p_blas_l1->p_X[IX];
             IX = IX + INCX_int;
             IY = IY + INCY_int;
         }
    }



  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        saxpy(&(p_blas_l1->N),p_blas_l1->p_ALPHA,p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

#else
ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);
#endif




  return ADE_DEFAULT_RET;

}

/*******************LIST of double precision blas*************************/

#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)

static ADE_API_RET_T ADE_Blas_level1_daxpy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_daxpy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        daxpy(&(p_blas_l1->N),p_blas_l1->p_ALPHA,p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );
#else

ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

#endif




  return ADE_DEFAULT_RET;

}

#else
ADE_DEFINE_ERROR(ADE_FP_PRECISION);
#endif