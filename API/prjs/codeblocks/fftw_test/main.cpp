#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Matlab.h"
#include "headers/ADE_Utils.h"
#include "headers/ADE_fft.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_complex.h"
#include "headers/ADE_Bench_Utils.h"
#include "headers/ADE_Error_Handler.h"
#define MAXCHAR (256)
#define MAXVAR (32)



int main()
{

ADE_FFT_T *p_fft;
ADE_MATLAB_T *p_mat;
ADE_UINT32_T buff_len;
ADE_SIZE_T size_fft_in,size_fft_out;
Engine *p_eng;
ADE_FFT_TYPE_T fft_type=ADE_FFT_R2C;
ADE_VOID_T *p_in;
ADE_VOID_T *p_out;
double *p_matdata;
ADE_CPLX_T test_cplx[2];
ADE_UINT32_T i=0;
ADE_INT32_T lin,col;
ADE_API_RET_T ret=ADE_RET_ERROR;
double *p_test;
float *p_test2;

//printf("double pointer size %d pointed size %d\n",sizeof(p_test),sizeof(*p_test));
//printf("double pointer size %d pointed size %d\n",sizeof(p_test2),sizeof(*p_test2));
test_cplx[0]=ADE_cset(1,3);
test_cplx[1]=ADE_cset(2,5);

ADE_Matlab_Init(&p_mat, p_eng,"/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/testbenches/fft/fft_test.m", "./fft_test_ws.mat","/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/matlab");
//ADE_Matlab_launch_script_segment(p_mat,"Output");
ADE_Matlab_Print(p_mat);/*** Printa le variabili del workspace matlab****/

buff_len=ADE_Matlab_GetScalar(p_mat,"in_len");


 if (fft_type==ADE_FFT_C2C)
{
    size_fft_in=buff_len*sizeof(ADE_FFTCPLX_T);
     p_in=fftw_malloc(size_fft_in);
     if (p_in==NULL)
     {
         ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_MAIN,tb,p_in,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
     }
     size_fft_out=buff_len*sizeof(ADE_FFTCPLX_T);
      p_out=fftw_malloc(size_fft_out);
      if (p_out==NULL)
     {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_MAIN,tb,p_out,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
     }
}
else if (fft_type==ADE_FFT_R2C)
{
   size_fft_in=buff_len*sizeof(ADE_FLOATING_T);
     p_in=fftw_malloc(size_fft_in);
     if (p_in==NULL)
     {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_MAIN,tb,p_in,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
     }
     size_fft_out=(buff_len/2+1)*sizeof(ADE_FFTCPLX_T);
      p_out=fftw_malloc(size_fft_out);
      if (p_out==NULL)
     {
        ADE_PRINT_ERRORS(ADE_ERROR,ADE_MEM,ADE_CLASS_MAIN,tb,p_out,"%p",(FILE*)ADE_STD_STREAM);
        return ADE_RET_ERROR;
     }

}
////        else if (fft_type==ADE_FFT_C2R)
////        {
////            p_in=fftw_malloc(buff_len*sizeof(ADE_FFTCPLX_T));
////             if (p_in==NULL)
////             {
////                ADE_PRINT_ERRORS(ADE_MEM,p_in,"%p",ADE_Fft_Init);
////                return ADE_RET_ERROR;
////             }
////              p_out=fftw_malloc(2*(buff_len/2+1)*sizeof(ADE_FLOATING_T));
////              if (p_out==NULL)
////             {
////                ADE_PRINT_ERRORS(ADE_MEM,p_out,"%p",ADE_Fft_Init);
////                return ADE_RET_ERROR;
////             }
////
////        }
////        else if (fft_type==ADE_FFT_R2R)
////        {
////             p_in=fftw_malloc(buff_len*sizeof(ADE_FLOATING_T));
////             if (p_in==NULL)
////             {
////                ADE_PRINT_ERRORS(ADE_MEM,p_in,"%p",ADE_Fft_Init);
////                return ADE_RET_ERROR;
////             }
////              p_out=fftw_malloc(buff_len*sizeof(ADE_FLOATING_T));
////              if (p_out==NULL)
////             {
////                ADE_PRINT_ERRORS(save('fft_test_ws');ADE_MEM,p_out,"%p",ADE_Fft_Init);
////                return ADE_RET_ERROR;http://stackoverflow.com/questions/2672015/hiding-members-in-a-c-struct
////             }
////        }
////        else
////        {
////            ADE_PRINT_ERRORS(ADE_INCHECKS,fft_type,"%d",ADE_Fft_Init);
////            return ADE_E32;
////        }
//


ret=ADE_Fft_Init(&p_fft, buff_len);
ret=ADE_Fft_Configure(p_fft,fft_type, ADE_FFT_FORWARD,p_in,p_out);
if (fft_type==ADE_FFT_C2C)
{
    p_matdata=ADE_Matlab_GetDataPointer(p_mat,"real_input");

    for(i=0;i<buff_len;i++)
    {
        ((ADE_FLOATING_T*)p_in)[2*i]=p_matdata[i];
         ((ADE_FLOATING_T*)p_in)[2*i+1]=0.0;
    }

}
else if (fft_type==ADE_FFT_R2C)
{
    memcpy(p_in,ADE_Matlab_GetDataPointer(p_mat,"real_input"),size_fft_in);
}

ret=ADE_Fft_Step(p_fft);
//ADE_Fft_Release(p_fft);
if (fft_type==ADE_FFT_C2C)
{
ret=ADE_Utils_PrintArray(p_out,0,buff_len-1, 0,0,(ADE_CHAR_T*) "outC2C_FFTW", stdout,ADE_MATH_CPLX);
}
else if (fft_type==ADE_FFT_R2C)
{
    ret=ADE_Utils_PrintArray(p_out,0,(buff_len/2+1)-1, 0,0,(ADE_CHAR_T*) "outR2C_FFTW", stdout,ADE_MATH_CPLX);
}
//for (i=0;i<2;i++)
//{
//    fprintf(stdout,"%f+i%f\n",creal(test_cplx[i]),cimag(test_cplx[i]));
//}
ADE_Matlab_Release(p_mat);


custom_FFT((ADE_FLOATING_T*)p_in-1,buff_len, ADE_CUSTOM_FFT_FORWARD);
if (fft_type==ADE_FFT_C2C)
{
ret=ADE_Utils_PrintArray(p_in,0,buff_len-1, 0,0,(ADE_CHAR_T*) "outC2C_Custom", stdout,ADE_MATH_CPLX);
}

//ADE_Get_Terminal_size(&lin ,&col  );
//fprintf(stdout,"%lin=d col=%d\n",lin,col);
ADE_Fft_Release(p_fft);
return ret;
}
