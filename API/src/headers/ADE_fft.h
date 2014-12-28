#ifndef _ADE_FFT_H
#define _ADE_FFT_H
#include "headers/ADE_typedefs.h"

#if (ADE_FFT_IMP==ADE_USE_FFTW)
#include "fftw3.h"
#else
ADE_DEFINE_ERROR(ADE_FFT_IMP;)
#endif

struct ADE_FFT_S {
        ADE_UINT32_T buff_len;
        ADE_VOID_T *p_in;//allocati dentro con ffttw_malloc
        ADE_VOID_T *p_out;//allocati dentro con ffttw_malloc
        #if (ADE_FFT_IMP==ADE_USE_FFTW)
        fftw_plan plan;
        #else
        ADE_DEFINE_ERROR(ADE_FFT_IMP);
        #endif
        };


ADE_API_RET_T ADE_Fft_Init(ADE_FFT_T** dp_this,ADE_UINT32_T buff_len);
ADE_API_RET_T ADE_Fft_Configure(ADE_FFT_T* p_fft,ADE_FFT_TYPE_T fft_type, ADE_FFT_DIRECTION_T fft_dir,ADE_VOID_T *p_inbuff,ADE_VOID_T *p_outbuff);
ADE_VOID_T ADE_Fft_Release(ADE_FFT_T* p_fft);


#endif //p_blow->p_mat
