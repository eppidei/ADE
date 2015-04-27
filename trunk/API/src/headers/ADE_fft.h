#ifndef _ADE_FFT_H
#define _ADE_FFT_H
#include "headers/ADE_typedefs.h"

#if (ADE_FFT_IMP==ADE_USE_FFTW)
    #include "fftw3.h"
#elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
    #include "ADE_accelerate_framework_wrapper.h"
#else
#error (ADE_FFT_IMP);
#endif

struct ADE_FFT_S {
        ADE_UINT32_T buff_len;
         ADE_FFT_TYPE_T type;
        #if (ADE_FFT_IMP==ADE_USE_FFTW)
        ADE_VOID_T *p_in;//allocati fuori con ffttw_malloc
        ADE_VOID_T *p_out;//allocati fuori con ffttw_malloc
        ADE_FFTW_DIRECTION_T direction;
        #if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
        fftw_plan plan;
        #elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
        fftwf_plan plan;
        #else
         #error (ADE_FP_PRECISION in ADE_FFT_S)
        #endif
        #elif (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
        ADE_SplitComplex_T split_in;
        ADE_SplitComplex_T split_out;
        ADE_vDSP_DFT_Direction_T direction;
        ADE_FFTSetup_T p_setup;

        #else
        #error (ADE_FFT_IMP in ADE_FFT_S)
        #endif
        };

#ifdef __cplusplus
    extern "C" {
#endif
/******** Init Methods ************/
ADE_API_RET_T ADE_Fft_Init(ADE_FFT_T** dp_this,ADE_UINT32_T buff_len);
ADE_VOID_T ADE_Fft_Release(ADE_FFT_T* p_fft);
/************* Configure Methods ****************/
ADE_API_RET_T ADE_Fft_Configure(ADE_FFT_T* p_fft,ADE_FFT_TYPE_T fft_type, ADE_FFT_DIRECTION_T fft_dir,ADE_VOID_T *p_inbuff,ADE_VOID_T *p_outbuff);
#if (ADE_FFT_IMP==ADE_USE_ACCEL_FMW_FFT)
ADE_API_RET_T ADE_Fft_FillSplitIn(ADE_FFT_T* p_fft,ADE_FLOATING_T real,ADE_FLOATING_T imag,ADE_UINT32_T idx);
ADE_API_RET_T ADE_Fft_FillSplitOut(ADE_FFT_T* p_fft,ADE_FLOATING_T real,ADE_FLOATING_T imag,ADE_UINT32_T idx);
#endif
/********** Processing Methods *****************/
ADE_API_RET_T ADE_Fft_Step(ADE_FFT_T* p_fft);

/************** Utils methods *********************/

ADE_API_RET_T ADE_Fir_Print(ADE_FIR_T* p_fir, ADE_FILE_T *p_fid);

#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //p_blow->p_mat
