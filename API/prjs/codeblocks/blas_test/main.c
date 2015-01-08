#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_blas_level2.h"
#define DIM (5)


int main()
{
//    double h=1.1;
//    ADE_PRINT_ERRORS(Ciccio,h,"%f");
//    return 0;


int incx = 1;
int incy = 1;
double alpha= 1,beta=1.0;
static double x[DIM]={1,2,3,4,5};
static double y[DIM]={0,0,0,0,0};
static double A[DIM]={1,2,3,4,5};
static double out[DIM],out2[DIM];
int N=DIM;
int k=0;

ADE_blas_level1_T *p_Blas_L1;
ADE_blas_level2_T *p_Blas_L2;

char test_case[20]="sbmv";

    ADE_API_RET_T ret = ADE_DEFAULT_RET;

    ADE_UINT32_T i = 0;


if (!strcmp(test_case,"axpy"))
{


ADE_Blas_level1_Init(&p_Blas_L1,ADE_REAL);

ADE_Blas_level1_setALPHA(p_Blas_L1,&alpha);
ADE_Blas_level1_setN(p_Blas_L1,N);
ADE_Blas_level1_setINCX(p_Blas_L1,incx);
ADE_Blas_level1_setINCY(p_Blas_L1,incy);
ADE_Blas_level1_setX(p_Blas_L1,x);
ADE_Blas_level1_setY(p_Blas_L1,y);
ADE_Blas_level1_Print(p_Blas_L1);

ADE_Blas_level1_axpy(p_Blas_L1);

ADE_Blas_level1_Release(p_Blas_L1);


//ADE_blas_saxpy(&N,&a,x,&incx,y,&incy);


PRINT_ARRAY(y,i,DIM,"%f");
}
else if (!strcmp(test_case,"sbmv"))
{
    /*Init*/
    ADE_Blas_level2_Init(&p_Blas_L2,ADE_REAL);
    /*Config*/
    ADE_Blas_Level2_SetUplo(p_Blas_L2,'L');
    ADE_Blas_Level2_SetN(p_Blas_L2,DIM);
    ADE_Blas_Level2_SetK(p_Blas_L2,k);
    ADE_Blas_level2_SetAlpha(p_Blas_L2,&alpha);
    ADE_Blas_level2_SetA(p_Blas_L2,A);
    ADE_Blas_Level2_SetLda(p_Blas_L2,k+1);
    ADE_Blas_level2_SetX(p_Blas_L2,x);
    ADE_Blas_Level2_SetIncx(p_Blas_L2,1);
    ADE_Blas_level2_SetBeta(p_Blas_L2,&beta);
    ADE_Blas_level2_SetY(p_Blas_L2,y);
    ADE_Blas_Level2_SetIncy(p_Blas_L2,1);
    ADE_Blas_level2_Print(p_Blas_L2);

    /*Process*/
    ADE_Blas_level2_sbmv(p_Blas_L2);

    /*Release*/
    ADE_Blas_level2_Release(p_Blas_L2);

    PRINT_ARRAY(y,i,DIM,"%f");

}
else
{
    return -1;
}
return ret;

}
