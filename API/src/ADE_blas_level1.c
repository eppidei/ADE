#include "headers/ADE_blas_level1.h"
#include "headers/ADE_blas_wrapper.h"
#include "headers/ADE_defines.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_cblas.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/**********Private methods Interfaces ******************/

static ADE_API_RET_T ADE_Blas_level1_launch_type1 (ADE_blas_level1_T *p_blas_l1);
#if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
static ADE_API_RET_T ADE_Blas_level1_saxpy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_scopy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_caxpy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_ccopy (ADE_blas_level1_T *p_blas_l1);
#elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
static ADE_API_RET_T ADE_Blas_level1_daxpy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_dcopy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_zaxpy (ADE_blas_level1_T *p_blas_l1);
static ADE_API_RET_T ADE_Blas_level1_zcopy (ADE_blas_level1_T *p_blas_l1);
#else
ADE_DEFINE_ERROR(ADE_FP_PRECISION);
#endif

/************ Init Methods ***********/
ADE_API_RET_T ADE_Blas_level1_Init(ADE_blas_level1_T** dp_this,ADE_MATH_ATTRIBUTE_T math_type )
{
    ADE_blas_level1_T* p_this = calloc(1,sizeof(ADE_blas_level1_T));

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

/****************Set Methods *****************/

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

//ADE_API_RET_T ADE_Blas_level1_set_pOut(ADE_blas_level1_T* p_blas_l1,ADE_FLOATING_T *p_buf)
//{
//    p_blas_l1->p_out=p_buf;
//     return ADE_DEFAULT_RET;
//
//}



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

/**************** Operative Methods ****************************/

ADE_API_RET_T ADE_Blas_level1_axpy(ADE_blas_level1_T* p_blas_l1)
{
    ADE_API_RET_T ret = ADE_DEFAULT_RET;

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
    if(p_blas_l1->math_type==ADE_REAL)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_saxpy;
    }
    else if (p_blas_l1->math_type==ADE_CPLX)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_caxpy;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l1->math_type,"%d",ADE_Blas_axpy);
        return ADE_E36;
    }

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
    if(p_blas_l1->math_type==ADE_REAL)
    {
     p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_daxpy;
     }
    else if (p_blas_l1->math_type==ADE_CPLX)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_zaxpy;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l1->math_type,"%d",ADE_Blas_axpy);
        return ADE_E36;
    }
     #else
        ADE_DEFINE_ERROR(ADE_FP_PRECISION);
    #endif

     ret = ADE_Blas_level1_launch_type1(p_blas_l1);

     #if (ADE_CHECK_RETURNS==1)
    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_BLAS_L1,ret,"%d",ADE_Blas_axpy);
    }
    #endif

    return ret;

}

ADE_API_RET_T ADE_Blas_level1_copy(ADE_blas_level1_T* p_blas_l1)
{
    ADE_API_RET_T ret = ADE_DEFAULT_RET;

    #if (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
    if(p_blas_l1->math_type==ADE_REAL)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_scopy;
    }
    else if (p_blas_l1->math_type==ADE_CPLX)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_ccopy;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l1->math_type,"%d",ADE_Blas_copy);
        return ADE_E36;
    }

    #elif (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
    if(p_blas_l1->math_type==ADE_REAL)
    {
     p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_dcopy;
     }
    else if (p_blas_l1->math_type==ADE_CPLX)
    {
        p_blas_l1->blas_level1_fcn_type1=ADE_Blas_level1_zcopy;
    }
    else
    {
        ADE_PRINT_ERRORS(ADE_INCHECKS,p_blas_l1->math_type,"%d",ADE_Blas_copy);
        return ADE_E36;
    }
     #else
        ADE_DEFINE_ERROR(ADE_FP_PRECISION);
    #endif

     ret = ADE_Blas_level1_launch_type1(p_blas_l1);

     #if (ADE_CHECK_RETURNS==1)
    if (ret<0)
    {
        ADE_PRINT_ERRORS(ADE_BLAS_L1,ret,"%d",ADE_Blas_copy);
    }
    #endif

    return ret;

}

ADE_API_RET_T ADE_Blas_level1_Print(ADE_blas_level1_T *p_blas_l1)
{
    FILE *p_fid=stdout;


   // p_fid=fopen(p_name,"w");

    if (p_fid!=NULL)
    {


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

        return ADE_DEFAULT_RET;
    }

    else
    {

        return ADE_E39;
    }




}


/******************* private methods ***************************/

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

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_saxpy((p_blas_l1->N),*(p_blas_l1->p_ALPHA),p_blas_l1->p_X,(p_blas_l1->INCX),p_blas_l1->p_Y,p_blas_l1->INCY );

#else
ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);
#endif




  return ADE_DEFAULT_RET;

}

static ADE_API_RET_T ADE_Blas_level1_scopy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_scopy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        scopy(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_scopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

#endif




  return ADE_DEFAULT_RET;

}


static ADE_API_RET_T ADE_Blas_level1_caxpy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_caxpy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        caxpy(&(p_blas_l1->N),(ADE_CPLX_T*)p_blas_l1->p_ALPHA,(ADE_CPLX_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_CPLX_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_caxpy(p_blas_l1->N,(ADE_CPLX_T*)p_blas_l1->p_ALPHA,(ADE_CPLX_T*)p_blas_l1->p_X,p_blas_l1->INCX,(ADE_CPLX_T*)p_blas_l1->p_Y,p_blas_l1->INCY );
#else

ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

#endif




  return ADE_DEFAULT_RET;

}
static ADE_API_RET_T ADE_Blas_level1_ccopy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_ccopy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        ccopy(&(p_blas_l1->N),(ADE_CPLX_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_CPLX_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_ccopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
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
   #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_daxpy(p_blas_l1->N,*(p_blas_l1->p_ALPHA),p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

#endif




  return ADE_DEFAULT_RET;

}
static ADE_API_RET_T ADE_Blas_level1_dcopy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_dcopy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        dcopy(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_dcopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

#endif




  return ADE_DEFAULT_RET;

}
static ADE_API_RET_T ADE_Blas_level1_zaxpy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_daxpy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        zaxpy(&(p_blas_l1->N),(ADE_CPLX_T*)p_blas_l1->p_ALPHA,(ADE_CPLX_T*)p_blas_l1->p_X,&(p_blas_l1->INCX),(ADE_CPLX_T*)p_blas_l1->p_Y,&(p_blas_l1->INCY) );
  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_zaxpy(p_blas_l1->N,(ADE_CPLX_T*)p_blas_l1->p_ALPHA,(ADE_CPLX_T*)p_blas_l1->p_X,p_blas_l1->INCX,(ADE_CPLX_T*)p_blas_l1->p_Y,p_blas_l1->INCY );
#else

ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

#endif




  return ADE_DEFAULT_RET;

}
static ADE_API_RET_T ADE_Blas_level1_zcopy (ADE_blas_level1_T *p_blas_l1)// (ADE_INT32_T *N,ADE_FLOATING_SP_T *SA,ADE_FLOATING_SP_T *SX,ADE_INT32_T *INCX,ADE_FLOATING_SP_T *SY,ADE_INT32_T *INCY)
{

    #if (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_CUSTOM)

        ADE_MISSING_IMPLEMENTATION(ADE_Blas_level1_zcopy);

  #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_BLAS_LIB)

        zcopy(&(p_blas_l1->N),p_blas_l1->p_X,&(p_blas_l1->INCX),p_blas_l1->p_Y,&(p_blas_l1->INCY) );

        #elif (ADE_BLAS_IMPLEMENTATION==ADE_USE_CBLAS_LIB)

        cblas_zcopy(p_blas_l1->N,p_blas_l1->p_X,p_blas_l1->INCX,p_blas_l1->p_Y,p_blas_l1->INCY );
#else

ADE_DEFINE_ERROR(ADE_BLAS_IMPLEMENTATION);

#endif




  return ADE_DEFAULT_RET;

}
#else
ADE_DEFINE_ERROR(ADE_FP_PRECISION);
#endif
