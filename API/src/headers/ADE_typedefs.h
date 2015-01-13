#ifndef _ADE_TYPEDEFS_H
#define _ADE_TYPEDEFS_H
#include <stdint.h>
#include "headers/ADE_defines.h"
#include <complex.h>
#include "fftw3.h"


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
typedef double complex ADE_CPLX_DP_T;
typedef float complex ADE_CPLX_SP_T;
#if (ADE_FP_PRECISION==ADE_USE_DOUBLE_PREC)
typedef ADE_FLOATING_DP_T ADE_FLOATING_T;
typedef ADE_CPLX_DP_T ADE_CPLX_T;
    #if (ADE_FFT_IMP==ADE_USE_FFTW)
    typedef fftw_complex ADE_FFTCPLX_T;
    #else
    typedef ADE_CPLX_T ADE_FFTCPLX_T;
    #endif
#elif (ADE_FP_PRECISION==ADE_USE_SINGLE_PREC)
typedef ADE_FLOATING_SP_T ADE_FLOATING_T;
typedef ADE_CPLX_SP_T ADE_CPLX_T;
    #if (ADE_FFT_IMP==ADE_USE_FFTW)
    typedef fftwf_complex ADE_FFTCPLX_T;
    #else
    typedef ADE_CPLX_T ADE_FFTCPLX_T;
    #endif
#endif

typedef ADE_INT32_T ADE_API_RET_T ;
typedef void ADE_VOID_T;
typedef size_t ADE_SIZE_T;

/*********************************** ADE ENVIRONMENT ***********************************/
typedef struct ADE_S ADE_T;
typedef struct ADE_S* ADE_HANDLE;

/**************************** BLAS *********************/
/*Level1*/
typedef struct ADE_blas_level1_S ADE_blas_level1_T;
typedef ADE_API_RET_T (*ADE_BLAS_LEVEL1_FCN_TYPE1_T)(ADE_blas_level1_T*);
typedef ADE_FLOATING_T* (*ADE_BLAS_LEVEL1_FCN_TYPE2_T)(ADE_blas_level1_T*);
/*Level2*/
typedef struct ADE_blas_level2_S ADE_blas_level2_T;
typedef ADE_API_RET_T (*ADE_BLAS_LEVEL2_FCN_TYPE1_T)(ADE_blas_level2_T*);
/*Level3*/
typedef struct ADE_blas_level3_S ADE_blas_level3_T;
typedef ADE_API_RET_T (*ADE_BLAS_LEVEL3_FCN_TYPE1_T)(ADE_blas_level3_T*);

#define CBLAS_INDEX size_t  /* this may vary between platforms */

typedef enum CBLAS_ORDER {CblasRowMajor=101, CblasColMajor=102} ADE_CBLAS_ORDER_T;
typedef enum CBLAS_TRANSPOSE {CblasNoTrans=111, CblasTrans=112, CblasConjTrans=113} ADE_CBLAS_TRANSPOSE_T;
typedef enum CBLAS_UPLO {CblasUpper=121, CblasLower=122} ADE_CBLAS_UPLO_T;
typedef enum CBLAS_DIAG {CblasNonUnit=131, CblasUnit=132} ADE_CBLAS_DIAG_T;
typedef enum CBLAS_SIDE {CblasLeft=141, CblasRight=142} ADE_CBLAS_SIDE_T;

/******************************* IIR **************************/
typedef struct ADE_IIR_S ADE_IIR_T;
typedef ADE_API_RET_T (*ADE_FILTER_IMPLEMENTATION_T)(ADE_IIR_T*);
typedef enum  {trasp_II,trasp_II_blas} ADE_FILTER_IMP_CHOICE_T;
/******************************* FIR **************************/
typedef struct ADE_FIR_S ADE_FIR_T;
typedef ADE_API_RET_T (*ADE_FIR_FILTER_IMPLEMENTATION_T)(ADE_FIR_T*);

/**************************** POLYFIT ****************************/
typedef struct ADE_POLYFIT_S ADE_POLYFIT_T;

/******************************* BLOW ***************************/
typedef struct ADE_BLOW_S ADE_BLOW_T;
typedef ADE_API_RET_T (*ADE_BLOW_FUNCS_T)(ADE_BLOW_T*);
typedef enum  {waiting,evaluation,tracking} BLOW_SM_STATES_T;

/******************************** MATLAB ***************************/
typedef struct ADE_MATLAB_S ADE_MATLAB_T;
typedef enum {ADE_CPLX,ADE_REAL} ADE_MATH_ATTRIBUTE_T;
/************************************* FFT ************************/
typedef struct ADE_FFT_S ADE_FFT_T;
typedef enum {ADE_FFT_C2C,ADE_FFT_R2C,ADE_FFT_C2R,ADE_FFT_R2R} ADE_FFT_TYPE_T;
typedef enum {ADE_FFT_FORWARD,ADE_FFT_BACKWARD} ADE_FFT_DIRECTION_T;
/**************************** UTILS *************************/
typedef enum {ADE_UTILS_FIRST_PRINT_ROW,ADE_UTILS_NOTFIRST_PRINT_ROW} ADE_UTILS_ROW_INFO_T;
/******************************* BLOW ***************************/
typedef struct ADE_SNAP_S ADE_SNAP_T;


#endif //_ADE_TYPEDEFS_H
