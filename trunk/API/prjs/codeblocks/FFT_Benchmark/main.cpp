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
#define MAXCHAR (256)
#define MAXVAR (32)
#include <time.h>
#include <errno.h>



int main()
{

    ADE_FFT_T *p_fft;
    ADE_MATLAB_T *p_mat;
    ADE_UINT32_T buff_len;
    ADE_SIZE_T size_fft_in,size_fft_out;
    Engine *p_eng;
    ADE_FFT_TYPE_T fft_type=ADE_FFT_C2C;
    ADE_VOID_T *p_in;
    ADE_VOID_T *p_out;
    double *p_matdata;
    ADE_CPLX_T test_cplx[2];
    ADE_UINT32_T i=0,buff_len_idx=0;
    ADE_UINT32_T buff_lenghts[10]= {16,32,64,128,256,512,1024,2048,4096,8192};
    ADE_INT32_T lin,col;
    ADE_API_RET_T ret=ADE_DEFAULT_RET;
    double *p_test;
    float *p_test2;
     ADE_FLOATING_T tolerance=1e-4;
    ADE_VOID_T *p_fft_custom=NULL;
     struct timespec start_fft, stop_fft,start_cust, stop_cust,res;
    int ret_time=0;
    int err_code=0;
    FILE *p_fid=stdout;
#if ( (ADE_TARGET==ADE_PC_DEBUG_MATLAB) || (ADE_TARGET==ADE_PC_DEBUG_NORMAL) || (ADE_TARGET==ADE_PC_RELEASE) || (ADE_TARGET==ADE_ANDROID) )
    clockid_t clock_id=CLOCK_MONOTONIC;//CLOCK_REALTIME,CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID
#endif
   bench_times_T bench_times_int;

    bench_times_int.p_start_1=&start_fft;
    bench_times_int.p_stop_1=&stop_fft;
    bench_times_int.p_start_2=&start_cust;
    bench_times_int.p_stop_2=&stop_cust;
    bench_times_int.p_res=&res;

#if ( (ADE_TARGET==ADE_PC_DEBUG_MATLAB) || (ADE_TARGET==ADE_PC_DEBUG_NORMAL) || (ADE_TARGET==ADE_PC_RELEASE) || (ADE_TARGET==ADE_ANDROID) )

    ret_time=clock_getres(clock_id,&res);
    if (ret_time==-1)
    {
        err_code=errno;
        fprintf(p_fid,"Failed setting clock %d : error %d\n",clock_id,err_code);
        return -1;
    }
#endif

    for (buff_len_idx=0; buff_len_idx<1; buff_len_idx++)
    {

        buff_len=buff_lenghts[buff_len_idx];

        /*********************** IN-OUT BUFF MEM ALLOC ********************************/


        if (fft_type==ADE_FFT_C2C)
        {
            size_fft_in=buff_len*sizeof(ADE_FFTCPLX_T);
            p_in=fftw_malloc(size_fft_in);
            if (p_in==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,p_in,"%p",ADE_FFT_TEST_Procedure);
                return ADE_E31;
            }
            size_fft_out=buff_len*sizeof(ADE_FFTCPLX_T);
            p_out=fftw_malloc(size_fft_out);
            if (p_out==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,p_out,"%p",ADE_FFT_TEST_Procedure);
                return ADE_E31;
            }
            p_fft_custom=malloc(size_fft_in);
            if (p_fft_custom==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,p_fft_custom,"%p",ADE_FFT_TEST_Procedure);
                return ADE_E31;
            }
        }
        else if (fft_type==ADE_FFT_R2C)
        {
            size_fft_in=buff_len*sizeof(ADE_FLOATING_T);
            p_in=fftw_malloc(size_fft_in);
            if (p_in==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,p_in,"%p",ADE_Fft_Init);
                return ADE_E31;
            }
            size_fft_out=(buff_len/2+1)*sizeof(ADE_FFTCPLX_T);
            p_out=fftw_malloc(size_fft_out);
            if (p_out==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,p_out,"%p",ADE_Fft_Init);
                return ADE_E31;
            }
            /*2check*/
            p_fft_custom=malloc(size_fft_in);
            if (p_fft_custom==NULL)
            {
                ADE_PRINT_ERRORS(ADE_MEM,p_fft_custom,"%p",ADE_FFT_TEST_Procedure);
                return ADE_E31;
            }
            /**/

        }

        /******************* FFT SETUP ****************************/

        ret=ADE_Fft_Init(&p_fft, buff_len);
        ret=ADE_Fft_Configure(p_fft,fft_type, ADE_FFT_FORWARD,p_in,p_out);

        /******************* INPUT FILLING ********************************/

        if (fft_type==ADE_FFT_C2C)
        {


            for(i=0; i<buff_len; i++)
            {

                ((ADE_CPLX_T*)p_in)[i]=ADE_cset((rand()%10),(rand()%10));
            }

        }
        else if (fft_type==ADE_FFT_R2C)
        {
           ((ADE_FLOATING_T*)p_in)[i]=(ADE_FLOATING_T)(rand()%10);
        }

        memcpy(p_fft_custom,p_in,size_fft_in);




        /******************** FFT CALCULATION ******************************/
#if ( (ADE_TARGET==ADE_PC_DEBUG_MATLAB) || (ADE_TARGET==ADE_PC_DEBUG_NORMAL) || (ADE_TARGET==ADE_PC_RELEASE) || (ADE_TARGET==ADE_ANDROID) )
                         clock_gettime(clock_id,&start_fft);
//#elif (ADE_TARGET==ADE_IOS)
//                            TICK;
#endif
         ret=ADE_Fft_Step(p_fft);
         #if ( (ADE_TARGET==ADE_PC_DEBUG_MATLAB) || (ADE_TARGET==ADE_PC_DEBUG_NORMAL) || (ADE_TARGET==ADE_PC_RELEASE) || (ADE_TARGET==ADE_ANDROID) )
                          clock_gettime(clock_id,&stop_fft);

                          clock_gettime(clock_id,&start_cust);
//#elif (ADE_TARGET==ADE_IOS)
//                            TOCK;
//                            TICK;
#endif
         custom_FFT((ADE_FLOATING_T*)p_fft_custom-1,buff_len, ADE_CUSTOM_FFT_FORWARD);
#if ( (ADE_TARGET==ADE_PC_DEBUG_MATLAB) || (ADE_TARGET==ADE_PC_DEBUG_NORMAL) || (ADE_TARGET==ADE_PC_RELEASE) || (ADE_TARGET==ADE_ANDROID) )
                        clock_gettime(clock_id,&stop_cust);
//#elif (ADE_TARGET==ADE_IOS)
//                            TOCK;
#endif

       fft_checker(p_fft,p_fft_custom,tolerance,1,fft_type,&bench_times_int,p_fid);

        /************ FFT RELEASE***************/
        fftw_free(p_in);
        fftw_free(p_out);
        free(p_fft_custom);
        ADE_Fft_Release(p_fft);

    }

    return ret;
}
