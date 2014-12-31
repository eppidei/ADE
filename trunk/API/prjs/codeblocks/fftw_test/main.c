#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "matrix.h"
#include "headers/ADE_typedefs.h"
#include "headers/ADE_Matlab.h"
#include "headers/ADE_fft.h"
#include "headers/ADE_errors.h"
#define MAXCHAR (256)
#define MAXVAR (32)



int main()
{

ADE_FFT_T *p_fft;
ADE_MATLAB_T *p_mat;
ADE_UINT32_T buff_len;
Engine *p_eng;
ADE_FFT_TYPE_T fft_type=ADE_FFT_C2C;
ADE_CPLX_T *p_in;
ADE_CPLX_T *p_out;
ADE_CPLX_T test_cplx[2]={1+3*I,2+5*I};
ADE_UINT32_T i=0;
ADE_INT32_T lin,col;


ADE_Matlab_Init(&p_mat, p_eng,"/home/leonardo/Windows_home/WCPYS_win/ADE_wcpy2/Blow/Matlab/testbenches/fft/fft_test.m", "./fft_test_ws.mat","/home/leonardo/Ubuntu_home/leonardo/Programmi/MATLAB/R2013A/bin/matlab");
ADE_Matlab_Print(p_mat);

// if (fft_type==ADE_FFT_C2C)
//        {
//             p_in=fftw_malloc(buff_len*sizeof(ADE_FFTCPLX_T));
//             if (p_in==NULL)
//             {
//                ADE_PRINT_ERRORS(ADE_MEM,p_in,"%p",ADE_Fft_Init);
//                return ADE_E31;
//             }
//              p_out=fftw_malloc(buff_len*sizeof(ADE_FFTCPLX_T));
//              if (p_out==NULL)
//             {
//                ADE_PRINT_ERRORS(ADE_MEM,p_out,"%p",ADE_Fft_Init);
//                return ADE_E31;
//             }
//        }
////        else if (fft_type==ADE_FFT_R2C)
////        {
////            p_in=fftw_malloc(buff_len*sizeof(ADE_FLOATING_T));
////             if (p_in==NULL)
////             {
////                ADE_PRINT_ERRORS(ADE_MEM,p_in,"%p",ADE_Fft_Init);
////                return ADE_E31;
////             }
////              p_out=fftw_malloc((buff_len/2+1)*sizeof(ADE_FFTCPLX_T));
////              if (p_out==NULL)
////             {
////                ADE_PRINT_ERRORS(ADE_MEM,p_out,"%p",ADE_Fft_Init);
////                return ADE_E31;
////             }
////
////        }
////        else if (fft_type==ADE_FFT_C2R)
////        {
////            p_in=fftw_malloc(buff_len*sizeof(ADE_FFTCPLX_T));
////             if (p_in==NULL)
////             {
////                ADE_PRINT_ERRORS(ADE_MEM,p_in,"%p",ADE_Fft_Init);
////                return ADE_E31;
////             }
////              p_out=fftw_malloc(2*(buff_len/2+1)*sizeof(ADE_FLOATING_T));
////              if (p_out==NULL)
////             {
////                ADE_PRINT_ERRORS(ADE_MEM,p_out,"%p",ADE_Fft_Init);
////                return ADE_E31;
////             }
////
////        }
////        else if (fft_type==ADE_FFT_R2R)
////        {
////             p_in=fftw_malloc(buff_len*sizeof(ADE_FLOATING_T));
////             if (p_in==NULL)
////             {
////                ADE_PRINT_ERRORS(ADE_MEM,p_in,"%p",ADE_Fft_Init);
////                return ADE_E31;
////             }
////              p_out=fftw_malloc(buff_len*sizeof(ADE_FLOATING_T));
////              if (p_out==NULL)
////             {
////                ADE_PRINT_ERRORS(ADE_MEM,p_out,"%p",ADE_Fft_Init);
////                return ADE_E31;http://stackoverflow.com/questions/2672015/hiding-members-in-a-c-struct
////             }
////        }
////        else
////        {
////            ADE_PRINT_ERRORS(ADE_INCHECKS,fft_type,"%d",ADE_Fft_Init);
////            return ADE_E32;
////        }
//
//buff_len=ADE_Matlab_GetScalar(p_mat,"in_len");
//ADE_Fft_Init(&p_fft, buff_len);
//ADE_Fft_Configure(p_fft,ADE_FFT_C2C, ADE_FFT_FORWARD,p_in,p_out);
//ADE_Fft_Step(p_fft);
//ADE_Fft_Release(p_fft);
//for (i=0;i<2;i++)
//{
//    fprintf(stdout,"%f+i%f\n",creal(test_cplx[i]),cimag(test_cplx[i]));
//}
//ADE_Matlab_Release(p_mat);

ADE_Get_Terminal_size(&lin ,&col  );
//fprintf(stdout,"%lin=d col=%d\n",lin,col);

return EXIT_SUCCESS;
}
