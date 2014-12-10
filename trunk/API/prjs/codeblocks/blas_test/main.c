#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/ADE_blas_level1.h"
#include "headers/ADE_iir.h"
static ADE_API_RET_T filter(ADE_FLOATING_T *in, ADE_FLOATING_T *out, ADE_FLOATING_T *a, ADE_UINT32_T order, ADE_FLOATING_T *b,ADE_FLOATING_T gain, ADE_FLOATING_T *state,ADE_UINT32_T len_frame)
{
    ADE_UINT32_T i=0,k=0;



    for (k=0;k<len_frame;k++)
    {


        out[k] = gain*b[0]*(in[k])+state[0];



        for (i=0;i<(order);i++)// lostate deve essere inizializzato a 0 e lungo pari a order+1 (es. biquad ordine 2)
        {
            state[i]=b[i+1]*(in[k])+state[i+1]-a[i+1]*(out[k]);
        }

    }

    return ADE_DEFAULT_RET;

}

int main()
{
//    float h=1.1;
//    ADE_PRINT_ERRORS(Ciccio,h,"%f");
//    return 0;


int incx = 1;
int incy = 1;
float alpha= 1.0;
static float x[5]={1.0,0.0,0.0,0.0,0.0};
static float y[5]={1.0,2.0,3.0,4.0,5.0};
static float out[5],out2[5],state[5],state2[5];
int N=5;
unsigned int i=0;
ADE_blas_level1_T *p_Blas_L1;
ADE_IIR_T *p_iir;
ADE_API_RET_T ret = 0;
float a[3]={1,-0.9,-0.97};
float b[3]={1,2,0.5};
float gain[2] = {0.000134,0.000134};
float *num[2] ={b,b};
float *denom[2] ={a,a};
char test_case[20]="iir";

if (!strcmp(test_case,"blas"))
{


ADE_Blas_level1_Init(&p_Blas_L1);

ADE_Blas_level1_setALPHA(p_Blas_L1,alpha);
ADE_Blas_level1_setN(p_Blas_L1,N);
ADE_Blas_level1_setINCX(p_Blas_L1,incx);
ADE_Blas_level1_setINCY(p_Blas_L1,incy);
ADE_Blas_level1_setX(p_Blas_L1,x);
ADE_Blas_level1_setY(p_Blas_L1,y);

ADE_Blas_real_axpy(p_Blas_L1);

ADE_Blas_level1_Release(p_Blas_L1);


//ADE_blas_saxpy(&N,&a,x,&incx,y,&incy);


PRINT_ARRAY(y,i,5,"%f");
}
else if  (!strcmp(test_case,"iir"))
{




//float *gains=&gain;



filter(x,out,a,2,b,gain[0],state,5);
filter(out,out2,a,2,b,gain[0],state2,5);

ret = ADE_Iir_Init(&p_iir, 2,5);
//CHECK_RET_MAIN(ret);


PRINT_ARRAY(out,i,5,"%f");

PRINT_ARRAY(out2,i,5,"%f");

ADE_Iir_setGains( p_iir, gain);
ADE_Iir_setNums( p_iir, &(*num));
ADE_Iir_setDenoms( p_iir, &(*denom));
ADE_Iir_setInbuff( p_iir, x);
ADE_Iir_setOutbuff( p_iir, out);
ADE_Iir_setFilt_Implementation(p_iir,trasp_II);
ADE_Iir_Step(p_iir);

//
//
//for (i=0;i<N;i++)
//{
//    fprintf(stdout,"p_iir->out[%d]=%f\n",i,(p_iir->p_out)[i]);
//}
//

PRINT_ARRAY(p_iir->p_out,i,5,"%f");

ADE_Iir_Release(p_iir);


}



return ret;

}
