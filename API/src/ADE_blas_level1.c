#include "headers/ADE_blas_level1.h"
#include "headers/ADE_blas_wrapper.h"
#include "headers/ADE_defines.h"
#include "headers/ADE_errors.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static ADE_API_RET_T ADE_Blas_level1_launch_type1 (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_saxpy (ADE_blas_level1_T *p_blas_l1);



ADE_API_RET_T ADE_Blas_level1_Init(ADE_blas_level1_T** dp_this )
{
    ADE_blas_level1_T* pthis = calloc(1,sizeof(ADE_blas_level1_T));

    *dp_this=pthis;

    return ADE_DEFAULT_RET;
}

ADE_VOID_T ADE_Blas_level1_Release (ADE_blas_level1_T* p_blas_l1)
{
    ADE_CHECKNFREE(p_blas_l1);
}

ADE_API_RET_T ADE_Blas_level1_setA(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val)
{
    p_blas_l1->A=val;
    return ADE_DEFAULT_RET;

}
ADE_API_RET_T ADE_Blas_level1_setB(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val)
{
    p_blas_l1->B=val;
     return ADE_DEFAULT_RET;
}
ADE_API_RET_T ADE_Blas_level1_setC(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val)
{
    p_blas_l1->C=val;
     return ADE_DEFAULT_RET;
}
ADE_API_RET_T ADE_Blas_level1_setS(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val)
{
    p_blas_l1->S=val;
     return ADE_DEFAULT_RET;

}
ADE_API_RET_T ADE_Blas_level1_setD1(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val)
{
    p_blas_l1->D1=val;
     return ADE_DEFAULT_RET;

}
ADE_API_RET_T ADE_Blas_level1_setD2(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val)
{
    p_blas_l1->D2=val;
     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_level1_setALPHA(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T val)
{
    p_blas_l1->ALPHA=val;
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



ADE_API_RET_T ADE_Blas_level1_setPARAM(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_val,ADE_UINT32_T n_params)
{
    ADE_UINT32_T i=0;

    for (i=0;i<n_params;i++)
    {
        (p_blas_l1->p_PARAM)[i]=p_val[i];
    }

     return ADE_DEFAULT_RET;

}

ADE_API_RET_T ADE_Blas_real_axpy(ADE_blas_level1_T* p_blas_l1)
{
    ADE_API_RET_T ret = ADE_DEFAULT_RET;

    #if (ADE_FP_PRECISION==0)
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_saxpy;
    #elif (ADE_FP_PRECISION==1)
     p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_daxpy;
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

    #if (ADE_CHECK_INPUTS==1)


        printf("REMEMBER TO ADD CHECKS ADE_Blas_level1_launch_type1\n");

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


static ADE_API_RET_T ADE_Blas_level1_saxpy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

    #if (ADE_BLAS_IMPLEMENTATION==0)

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



  #elif (ADE_BLAS_IMPLEMENTATION==1)

        saxpy(&(p_blas_l1->N),&(p_blas_l1->ALPHA),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

#endif




  return ADE_DEFAULT_RET;

}
