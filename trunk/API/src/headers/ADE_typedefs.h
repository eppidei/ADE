#ifndef _ADE_TYPEDEFS_H
#define _ADE_TYPEDEFS_H
#include <stdint.h>
#include "headers/ADE_defines.h"
#include <stdio.h>
#if defined (ADE_FFT_IMP) && (ADE_FFT_IMP==ADE_USE_FFTW)
#include "fftw3.h"
#endif
#include <stdbool.h>



typedef struct fcomplex_S
{
    float realpart;
    float imagpart;
} ADE_CPLX_SP_T;
typedef struct dcomplex_S
{
    double realpart;
    double imagpart;
}ADE_CPLX_DP_T;
/************************** Accelerate Framework ****************************/
typedef unsigned long ADE_vDSP_Length;
typedef long ADE_vDSP_Stride;
typedef struct ADE_OpaqueFFTSetupD * ADE_FFTSetupD;
typedef struct ADE_OpaqueFFTSetup * ADE_FFTSetup;
typedef int ADE_FFTRadix;
typedef int ADE_FFTDirection;
//struct DSPComplex { float real; float imag; };
typedef ADE_CPLX_SP_T ADE_DSPComplex;
struct DSPSplitComplex { float *realp; float *imagp; };
typedef struct DSPSplitComplex ADE_DSPSplitComplex;
//  struct DSPDoubleComplex { double real; double imag; };
typedef ADE_CPLX_DP_T ADE_DSPDoubleComplex;
struct DSPDoubleSplitComplex { double *realp; double *imagp; };
typedef struct DSPDoubleSplitComplex ADE_DSPDoubleSplitComplex;
typedef enum { ADE_kFFTRadix2 = 0, ADE_kFFTRadix3 = 1, ADE_kFFTRadix5 = 2 } ADE_kFFTRadix_T;
typedef enum {ADE_vDSP_DFT_FORWARD = +1,ADE_vDSP_DFT_INVERSE = -1} ADE_vDSP_DFT_Direction_T;
//struct vDSP_DFT_Direction {init(_ value: Int32)var value: Int32}
typedef int64_t ADE_INT64_T ;
typedef int32_t ADE_INT32_T ;
typedef int16_t ADE_INT16_T ;
typedef int8_t ADE_INT8_T ;
typedef char ADE_CHAR_T;
typedef uint64_t ADE_UINT64_T ;
typedef uint32_t ADE_UINT32_T ;
typedef uint16_t ADE_UINT16_T ;
typedef uint8_t ADE_UINT8_T ;
typedef unsigned char  ADE_UCHAR_T ;
typedef double ADE_FLOATING_DP_T;
typedef float ADE_FLOATING_SP_T;
typedef long ADE_LONG_T;
typedef unsigned long ADE_ULONG_T;
typedef bool ADE_BOOL_T;
typedef FILE ADE_FILE_T;
#define ADE_TRUE true
#define ADE_FALSE false

#if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
typedef ADE_FLOATING_DP_T ADE_FLOATING_T;
typedef ADE_CPLX_DP_T ADE_CPLX_T;
typedef ADE_DSPDoubleSplitComplex ADE_SplitComplex_T;
typedef ADE_FFTSetupD ADE_FFTSetup_T;
    #if (ADE_FFT_IMP==ADE_USE_FFTW)
    typedef fftw_complex ADE_FFTCPLX_T;
    #else
    typedef ADE_CPLX_T ADE_FFTCPLX_T;
    #endif
#elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
typedef ADE_FLOATING_SP_T ADE_FLOATING_T;
typedef ADE_CPLX_SP_T ADE_CPLX_T;
typedef ADE_DSPSplitComplex ADE_SplitComplex_T;
typedef ADE_FFTSetup ADE_FFTSetup_T;
    #if defined(ADE_FFT_IMP) && (ADE_FFT_IMP==ADE_USE_FFTW)
    typedef fftwf_complex ADE_FFTCPLX_T;
    #else
    typedef ADE_CPLX_T ADE_FFTCPLX_T;
    #endif
#else
    #error (ADE_FP_PRECISION) in ADE_typedefs.h
#endif



typedef void ADE_VOID_T;
typedef size_t ADE_SIZE_T;

/*********************************** ADE ENVIRONMENT ***********************************/
typedef struct ADE_S ADE_T;
typedef struct ADE_S* ADE_HANDLE;

typedef enum {ADE_RET_SUCCESS=0,ADE_RET_ERROR=-1,ADE_RET_WARNING=1} ADE_API_RET_T;
/**************************** BLAS *********************/
/*Level1*/

typedef struct ADE_blas_level1_S ADE_blas_level1_T;
typedef ADE_API_RET_T (*ADE_BLAS_LEVEL1_FCN_TYPE1_T)(ADE_blas_level1_T*);
typedef ADE_FLOATING_T (*ADE_BLAS_LEVEL1_FCN_TYPE2_T)(ADE_blas_level1_T*);
typedef ADE_CPLX_T (*ADE_BLAS_LEVEL1_FCN_TYPE3_T)(ADE_blas_level1_T*);
/*Level2*/
typedef struct ADE_blas_level2_S ADE_blas_level2_T;
typedef ADE_API_RET_T (*ADE_BLAS_LEVEL2_FCN_TYPE1_T)(ADE_blas_level2_T*);
/*Level3*/
typedef struct ADE_blas_level3_S ADE_blas_level3_T;
typedef ADE_API_RET_T (*ADE_BLAS_LEVEL3_FCN_TYPE1_T)(ADE_blas_level3_T*);



/******************************* IIR **************************/
typedef struct ADE_IIR_S ADE_IIR_T;
typedef ADE_API_RET_T (*ADE_FILTER_IMPLEMENTATION_T)(ADE_IIR_T*);
typedef enum  {ADE_IIR_TRASP_II_B} ADE_IIR_IMP_CHOICE_T;
/******************************* FIR **************************/
typedef struct ADE_FIR_S ADE_FIR_T;
typedef ADE_API_RET_T (*ADE_FIR_FILTER_IMPLEMENTATION_T)(ADE_FIR_T*);
typedef enum  {ADE_FIR_TRASP_II,ADE_FIR_OVSA,ADE_FIR_IMP_UNDEF=-1} ADE_FIR_IMP_CHOICE_T;
/**************************** POLYFIT ****************************/
typedef struct ADE_POLYFIT_S ADE_POLYFIT_T;

/******************************* BLOW ***************************/
typedef struct ADE_BLOW_S ADE_BLOW_T;
typedef ADE_API_RET_T (*ADE_BLOW_FUNCS_T)(ADE_BLOW_T*);
typedef enum  {ADE_BLOW_WAITING,ADE_BLOW_EVALUATION,ADE_BLOW_TRACKING} BLOW_SM_STATES_T;

/******************************** MATLAB ***************************/
typedef struct ADE_MATLAB_S ADE_MATLAB_T;
typedef enum {ADE_MATH_CPLX=0,ADE_MATH_REAL=1,ADE_MATH_UNDEF=-1} ADE_MATH_ATTRIBUTE_T;
/************************************* FFT ************************/
typedef struct ADE_FFT_S ADE_FFT_T;
typedef enum {ADE_FFT_C2C=1,ADE_FFT_R2C=2,ADE_FFT_C2R=3,ADE_FFT_R2R=4,ADE_FFT_INVALID_TYPE=-1} ADE_FFT_TYPE_T;
typedef enum {ADE_FFT_FORWARD=1,ADE_FFT_BACKWARD=-1,ADE_FFT_INVALID_DIR=0} ADE_FFT_DIRECTION_T;
/****************** FFTW *****************************/
#if defined(ADE_FFT_IMP) && (ADE_FFT_IMP==ADE_USE_FFTW)
typedef enum {ADE_FFTW_FORWARD=FFTW_FORWARD,ADE_FFTW_BACKWARD=FFTW_BACKWARD} ADE_FFTW_DIRECTION_T;
#endif
/**************************** UTILS *************************/
typedef enum {ADE_UTILS_FIRST_PRINT_ROW,ADE_UTILS_NOTFIRST_PRINT_ROW} ADE_UTILS_ROW_INFO_T;
typedef enum {ADE_UTILS_MAJOR,ADE_UTILS_MINOR,ADE_UTILS_EQUAL,ADE_UTILS_MAJEQUAL,ADE_UTILS_MINEQUAL} ADE_UTILS_CONDITION_T;
/******************************* BLOW ***************************/
typedef struct ADE_SNAP_S ADE_SNAP_T;
/***********************Bench Utils *****************************/
typedef enum {ADE_CUSTOM_FFT_FORWARD=-1,ADE_CUSTOM_FFT_BACKWARD=1} ADE_CUSTOM_FFT_DIRECTION_T;

/********************************* Benchmarks***************************/
typedef enum {ADE_BLAS_L1_BM,ADE_BLAS_L2_BM,ADE_BLAS_L3_BM,ADE_FFTW_BM} ADE_BENCH_T;
typedef enum {ADE_BENCH_REAL,ADE_BENCH_CPLX} ADE_BENCH_MAT_T;

 typedef struct bench_times {
        struct timespec* p_start_1;
        struct timespec* p_stop_1;
        struct timespec* p_start_2;
        struct timespec* p_stop_2;
        struct timespec* p_res;
    } bench_times_T;

/********** Upsampler *****************/
typedef struct ADE_UPSAMPLER_S ADE_UPSAMPLER_T;
//typedef enum  {ADE_UPSAMP_PURE,ADE_UPSAMP_FIR,ADE_UPSAMP_IIR} ADE_UPSAMP_CHOICE_T;

/********** Interpolator *****************/
typedef struct ADE_INTERPOLATOR_S ADE_INTERPOLATOR_T;
typedef enum {ADE_INTERPOLATOR_FILTER_FIR,ADE_INTERPOLATOR_FILTER_IIR} ADE_INTERPOLATOR_FILTER_T;
typedef enum {ADE_INTERPOLATOR_ARCH_STANDARD,ADE_INTERPOLATOR_ARCH_POLYPHASE} ADE_INTERPOLATOR_ARCH_T;


/********** Downsampler *****************/
typedef struct ADE_DOWNSAMPLER_S ADE_DOWNSAMPLER_T;
//typedef enum  {ADE_DOWNSAMP_PURE,ADE_DOWNSAMP_FIR,ADE_DOWNSAMP_IIR} ADE_DOWNSAMP_CHOICE_T;

/********** Decimator *****************/
typedef struct ADE_DECIMATOR_S ADE_DECIMATOR_T;
typedef enum {ADE_DECIMATOR_FILTER_FIR,ADE_DECIMATOR_FILTER_IIR} ADE_DECIMATOR_FILTER_T;
typedef enum {ADE_DECIMATOR_ARCH_STANDARD,ADE_DECIMATOR_ARCH_POLYPHASE} ADE_DECIMATOR_ARCH_T;

/**************** Error Handeler**************/
typedef struct ADE_Error_Handler_S
{
    ADE_UINT32_T p_error_code[ADE_ERRSTACK_DEPTH];
    ADE_UINT32_T error_count;
   // ADE_CHAR_T p_error_str[64];
} ADE_Error_Handler_T;

typedef enum {ADE_ERROR_HANDLER_CHECKVALUE_MAJOR,ADE_ERROR_HANDLER_CHECKVALUE_MAJOREQUAL,
              ADE_ERROR_HANDLER_CHECKVALUE_MINOR,ADE_ERROR_HANDLER_CHECKVALUE_MINOREQUAL,
               ADE_ERROR_HANDLER_CHECKVALUE_INRANGE,ADE_ERROR_HANDLER_CHECKVALUE_OUTRANGE,
               ADE_ERROR_HANDLER_CHECKVALUE_EQUAL,ADE_ERROR_HANDLER_CHECKVALUE_NOTEQUAL,
                ADE_ERROR_HANDLER_CHECKVALUE_UNDEF=-1} ADE_ERROR_HANDLER_CHECKVALUE_T;


/********** Decimator *****************/
typedef struct ADE_PROXIMITY_S ADE_PROXIMITY_T;
#endif //_ADE_TYPEDEFS_H
