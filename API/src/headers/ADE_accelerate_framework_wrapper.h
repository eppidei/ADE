#ifndef _ADE_ACCELERATE_FRAMEWORK_WRAPPER_H
#define _ADE_ACCELERATE_FRAMEWORK_WRAPPER_H
#include "headers/ADE_typedefs.h"


#ifdef __cplusplus
    extern "C" {
#endif
/****************************** SETUP ******************************/
extern ADE_FFTSetup vDSP_create_fftsetup ( ADE_vDSP_Length __vDSP_Log2n, ADE_FFTRadix __vDSP_Radix );
extern ADE_FFTSetupD vDSP_create_fftsetupD ( ADE_vDSP_Length __vDSP_Log2n, ADE_FFTRadix __vDSP_Radix );
extern void vDSP_destroy_fftsetup ( ADE_FFTSetup __vDSP_setup );
extern void vDSP_destroy_fftsetupD ( ADE_FFTSetup __vDSP_setup );

/******************************** CONVERSION *****************************************/
extern void vDSP_ctoz (const ADE_DSPComplex *__vDSP_C,ADE_vDSP_Stride __vDSP_IC,const ADE_DSPSplitComplex *__vDSP_Z,ADE_vDSP_Stride __vDSP_IZ,ADE_vDSP_Length __vDSP_N);
extern void vDSP_ctozD (const ADE_DSPDoubleComplex *__vDSP_C,ADE_vDSP_Stride __vDSP_IC,const ADE_DSPDoubleSplitComplex *__vDSP_Z,ADE_vDSP_Stride __vDSP_IZ,ADE_vDSP_Length __vDSP_N);
extern void vDSP_ztoc (const ADE_DSPSplitComplex *__vDSP_Z,ADE_vDSP_Stride __vDSP_IZ,ADE_DSPComplex *__vDSP_C,ADE_vDSP_Stride __vDSP_IC,ADE_vDSP_Length __vDSP_N);
extern void vDSP_ztocD (const ADE_DSPDoubleSplitComplex *__vDSP_Z,ADE_vDSP_Stride __vDSP_IZ,ADE_DSPDoubleComplex *__vDSP_C,ADE_vDSP_Stride __vDSP_IC,ADE_vDSP_Length __vDSP_N);

/********************************************* REAL FFT*************************************************/

/****  REAL FFTs 1-D SINGLE PECISION In Place*******/
extern void vDSP_fft_zrip ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
 /* Multiple */
extern void vDSP_fftm_zrip ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IM, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );
/* temporary*/
extern  void vDSP_fft_zript ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, const ADE_DSPSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
/* Multiple temporary*/
extern void vDSP_fftm_zript ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IM, const ADE_DSPSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );

/****  REAL FFT 1-D SINGLE PECISION Out of Place*******/
extern void vDSP_fft_zrop ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
 /* Multiple */
extern void vDSP_fftm_zrop ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, ADE_vDSP_Stride __vDSP_IMA, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IMC, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );
/* temporary*/
extern void vDSP_fft_zropt ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, const ADE_DSPSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
/* Multiple temporary*/
extern void vDSP_fftm_zropt ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, ADE_vDSP_Stride __vDSP_IMA, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IMC, const ADE_DSPSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );

/****  REAL FFT 1-D DOUBLE PECISION In Place*******/
extern void vDSP_fft_zripD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
 /* Multiple */
extern void vDSP_fftm_zripD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IM, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );
/* temporary*/
extern void vDSP_fft_zriptD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, const ADE_DSPDoubleSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
/* Multiple temporary*/
extern void vDSP_fftm_zriptD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IM, const ADE_DSPDoubleSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );

/****  REAL FFT 1-D DOUBLE PECISION Out Of Place*******/
extern void vDSP_fft_zropD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
 /* Multiple */
extern void vDSP_fftm_zropD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, ADE_vDSP_Stride __vDSP_IMA, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IMC, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );
/* temporary*/
extern void vDSP_fft_zroptD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, const ADE_DSPDoubleSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
/* Multiple temporary*/
extern void vDSP_fftm_zroptD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, ADE_vDSP_Stride __vDSP_IMA, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IMC, const ADE_DSPDoubleSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );


/************************************************************** COMPLEX FFT *********************************************************/

/****  COMPLEX FFTs 1-D SINGLE PECISION In Place*******/
extern void vDSP_fft_zip ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
 /* Multiple */
extern void vDSP_fftm_zip ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IM, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );
/* temporary*/
extern  void vDSP_fft_zipt ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, const ADE_DSPSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
/* Multiple temporary*/
extern void vDSP_fftm_zipt ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IM, const ADE_DSPSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );

/****  COMPLEX FFT 1-D SINGLE PECISION Out of Place*******/
extern void vDSP_fft_zop ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
 /* Multiple */
extern void vDSP_fftm_zop ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, ADE_vDSP_Stride __vDSP_IMA, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IMC, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );
/* temporary*/
extern void vDSP_fft_zopt ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, const ADE_DSPSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
/* Multiple temporary*/
extern void vDSP_fftm_zopt ( ADE_FFTSetup __vDSP_Setup, const ADE_DSPSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, ADE_vDSP_Stride __vDSP_IMA, const ADE_DSPSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IMC, const ADE_DSPSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );

/****  COMPLEX FFT 1-D DOUBLE PECISION In Place*******/
extern void vDSP_fft_zipD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
 /* Multiple */
extern void vDSP_fftm_zipD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IM, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );
/* temporary*/
extern void vDSP_fft_ziptD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, const ADE_DSPDoubleSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
/* Multiple temporary*/
extern void vDSP_fftm_ziptD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IM, const ADE_DSPDoubleSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );

/****  COMPLEX FFT 1-D DOUBLE PECISION Out Of Place*******/
extern void vDSP_fft_zopD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
 /* Multiple */
extern void vDSP_fftm_zopD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, ADE_vDSP_Stride __vDSP_IMA, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IMC, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );
/* temporary*/
extern void vDSP_fft_zoptD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, const ADE_DSPDoubleSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_FFTDirection __vDSP_Direction );
/* Multiple temporary*/
extern void vDSP_fftm_zoptD ( ADE_FFTSetupD __vDSP_Setup, const ADE_DSPDoubleSplitComplex *__vDSP_A, ADE_vDSP_Stride __vDSP_IA, ADE_vDSP_Stride __vDSP_IMA, const ADE_DSPDoubleSplitComplex *__vDSP_C, ADE_vDSP_Stride __vDSP_IC, ADE_vDSP_Stride __vDSP_IMC, const ADE_DSPDoubleSplitComplex *__vDSP_Buffer, ADE_vDSP_Length __vDSP_Log2N, ADE_vDSP_Length __vDSP_M, ADE_FFTDirection __vDSP_Direction );



#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif //_ADE_ACCELERATE_FRAMEWORK_WRAPPER_H
