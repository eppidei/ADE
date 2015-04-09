#ifndef _ADE_ERRORS_H
#define _ADE_ERRORS_H
#include "headers/ADE_typedefs.h"


/*************************** ERROR DEFINES ***************************/
#define ADE_MISSING_IMPLEMENTATION(fcn_name) fprintf(stderr," MISSING IMPLEMENTATION IN fcn ->  " #fcn_name " \n");

#define ADE_NBITS_SEVERITY (8)
#define ADE_NBITS_TYPE (8)
#define ADE_NBITS_CLASS (8)
#define ADE_NBITS_METHOD (8)

#define ADE_NBITS_METHOD_SHIFT (0)
#define ADE_NBITS_CLASS_SHIFT (ADE_NBITS_METHOD+ADE_NBITS_METHOD_SHIFT)
#define ADE_NBITS_TYPE_SHIFT (ADE_NBITS_CLASS+ADE_NBITS_CLASS_SHIFT)
#define ADE_NBITS_SEVERITY_SHIFT (ADE_NBITS_TYPE+ADE_NBITS_TYPE_SHIFT)

/************************** SEVERITY ************************************/
#define ERRSEVERITY \
X_ERRSEVERITY(ADE_ERROR) \
X_ERRSEVERITY(ADE_WARNING)

#define X_ERRSEVERITY(a) a,
typedef enum  { ERRSEVERITY } ADE_ERRSEVERITY_T;
#undef X_ERRSEVERITY

#define X_ERRSEVERITY(a) #a,
static char *ADE_ERRSEVERITYStrings[] = { ERRSEVERITY };
#undef X_ERRSEVERITY

/*************************** ERROR TYPES ***************************/

#define ERRTYPE \
X_ERRTYPE(ADE_INCHECKS) \
X_ERRTYPE(ADE_MEM) \
X_ERRTYPE(ADE_RETCHECKS)

#define X_ERRTYPE(a) a,
typedef enum  { ERRTYPE } ADE_ERRTYPE_T;
#undef X_ERRTYPE

#define X_ERRTYPE(a) #a,
static char *ADE_ERRTYPEStrings[] = { ERRTYPE };
#undef X_ERRTYPE


/*******************************ERROR  CLASSES **************************/
#define ERRCLASS \
X_ERRCLASS(ADE_CLASS_MAIN) \
X_ERRCLASS(ADE_CLASS_BLAS_LEVEL1) \
X_ERRCLASS(ADE_CLASS_IIR) \
X_ERRCLASS(ADE_CLASS_FIR) \
X_ERRCLASS(ADE_CLASS_BLOW) \
X_ERRCLASS(ADE_CLASS_POLYFIT) \
X_ERRCLASS(ADE_CLASS_ADE) \
X_ERRCLASS(ADE_CLASS_MATLAB) \
X_ERRCLASS(ADE_CLASS_FFT) \
X_ERRCLASS(ADE_CLASS_UTILS) \
X_ERRCLASS(ADE_CLASS_BENCH_UTILS) \
X_ERRCLASS(ADE_CLASS_BLAS_LEVEL2) \
X_ERRCLASS(ADE_CLASS_BLAS_LEVEL3) \
X_ERRCLASS(ADE_CLASS_SNAP) \
X_ERRCLASS(ADE_CLASS_UPSAMPLER) \
X_ERRCLASS(ADE_CLASS_DOWNSAMPLER) \
X_ERRCLASS(ADE_CLASS_COMPLEX) \
X_ERRCLASS(ADE_CLASS_INTERPOLATOR) \
X_ERRCLASS(ADE_CLASS_DECIMATOR)

#define X_ERRCLASS(a) a,
typedef enum  { ERRCLASS } ADE_ERRCLASS_T;
#undef X_ERRCLASS

#define X_ERRCLASS(a) #a,
static char *ADE_ERRCLASSStrings[] = { ERRCLASS };
#undef X_ERRCLASS

/******************ERROR METHODS********************/

#define ERRMETHODS \
X_ERRMETHODS(ADE_METHOD_Init) \
X_ERRMETHODS(ADE_METHOD_Release) \
X_ERRMETHODS(ADE_METHOD_Step) \
X_ERRMETHODS(ADE_METHOD_SetA) \
X_ERRMETHODS(ADE_METHOD_SetB) \
X_ERRMETHODS(ADE_METHOD_SetC) \
X_ERRMETHODS(ADE_METHOD_SetS) \
X_ERRMETHODS(ADE_METHOD_SetD1) \
X_ERRMETHODS(ADE_METHOD_SetD2) \
X_ERRMETHODS(ADE_METHOD_SetALPHA) \
X_ERRMETHODS(ADE_METHOD_SetN) \
X_ERRMETHODS(ADE_METHOD_SetINCX) \
X_ERRMETHODS(ADE_METHOD_SetINCY) \
X_ERRMETHODS(ADE_METHOD_SetX) \
X_ERRMETHODS(ADE_METHOD_SetY) \
X_ERRMETHODS(ADE_METHOD_SetPARAM) \
X_ERRMETHODS(ADE_METHOD_Print) \
X_ERRMETHODS(ADE_METHOD_copy) \
X_ERRMETHODS(ADE_METHOD_csum) \
X_ERRMETHODS(ADE_METHOD_cset) \
X_ERRMETHODS(ADE_METHOD_cdiff) \
X_ERRMETHODS(ADE_METHOD_cmult) \
X_ERRMETHODS(ADE_METHOD_creal) \
X_ERRMETHODS(ADE_METHOD_cimag) \
X_ERRMETHODS(ADE_METHOD_cabs) \
X_ERRMETHODS(ADE_METHOD_setNum) \
X_ERRMETHODS(ADE_METHOD_ResetState) \
X_ERRMETHODS(ADE_METHOD_SetInBuff) \
X_ERRMETHODS(ADE_METHOD_SetOutBuff) \
X_ERRMETHODS(ADE_METHOD_setNums) \
X_ERRMETHODS(ADE_METHOD_setGains) \
X_ERRMETHODS(ADE_METHOD_setDenoms) \
X_ERRMETHODS(ADE_METHOD_GetVarIndex) \
X_ERRMETHODS(ADE_METHOD_GetNRows) \
X_ERRMETHODS(ADE_METHOD_GetNCols) \
X_ERRMETHODS(ADE_METHOD_GetSize) \
X_ERRMETHODS(ADE_METHOD_GetNElements) \
X_ERRMETHODS(ADE_METHOD_GetDataPointer) \
X_ERRMETHODS(ADE_METHOD_PutVarintoWorkspace) \
X_ERRMETHODS(ADE_METHOD_Evaluate_String) \
X_ERRMETHODS(ADE_METHOD_C2Mat_copy) \
X_ERRMETHODS(ADE_METHOD_Evaluate_StringnWait) \
X_ERRMETHODS(ADE_METHOD_Configure_Iir_sos) \
X_ERRMETHODS(ADE_METHOD_GetScalar) \
X_ERRMETHODS(ADE_METHOD_launch_script_segment) \
X_ERRMETHODS(ADE_METHOD_GetLength) \
X_ERRMETHODS(ADE_METHOD_SetBreaks) \
X_ERRMETHODS(ADE_METHOD_memoryless_expander) \
X_ERRMETHODS(ADE_METHOD_SetCoeffs) \
X_ERRMETHODS(ADE_METHOD_Get_Terminal_size) \
X_ERRMETHODS(ADE_METHOD_PrintArray) \
X_ERRMETHODS(ADE_METHOD_FindIndexes) \
X_ERRMETHODS(ADE_METHOD_Complex2Split) \
X_ERRMETHODS(ADE_METHOD_Split2Complex) \
X_ERRMETHODS(ADE_METHOD_SetSplit) \
X_ERRMETHODS(ADE_METHOD_FillSplitReal) \
X_ERRMETHODS(ADE_METHOD_FillSplitImag) \
X_ERRMETHODS(ADE_METHOD_FillSplitCplx) \
X_ERRMETHODS(ADE_METHOD_memset_float) \
X_ERRMETHODS(ADE_METHOD_memset_cplx) \
X_ERRMETHODS(ADE_METHOD_memcpy_float) \
X_ERRMETHODS(ADE_METHOD_memcpy_cplx) \
X_ERRMETHODS(ADE_METHOD_indexx) \
X_ERRMETHODS(ADE_METHOD_indexx_descend) \
X_ERRMETHODS(ADE_METHOD_setFilt_Implementation) \
X_ERRMETHODS(ADE_METHOD_memset_blasconfig) \
X_ERRMETHODS(ADE_METHOD_memcpy_blasconfig) \
X_ERRMETHODS(ADE_METHOD_PrintRowArrayReal) \
X_ERRMETHODS(ADE_METHOD_PrintRowArrayCplx) \
X_ERRMETHODS(ADE_METHOD_PrintColArrayReal) \
X_ERRMETHODS(ADE_METHOD_PrintColArrayCplax) \
X_ERRMETHODS(ADE_METHOD_PrintRowArray) \
X_ERRMETHODS(ADE_METHOD_PrintColArray) \
X_ERRMETHODS(ADE_METHOD_PrintMatrix) \
X_ERRMETHODS(ADE_METHOD_Upsampler_Pure) \
X_ERRMETHODS(ADE_METHOD_Upsampler_doPure_blas) \
X_ERRMETHODS(ADE_METHOD_Upsampler_doPure_custom) \
X_ERRMETHODS(ADE_METHOD_SetUplo) \
X_ERRMETHODS(ADE_METHOD_SetTrans) \
X_ERRMETHODS(ADE_METHOD_SetDiag) \
X_ERRMETHODS(ADE_METHOD_SetM) \
X_ERRMETHODS(ADE_METHOD_SetK) \
X_ERRMETHODS(ADE_METHOD_SetKl) \
X_ERRMETHODS(ADE_METHOD_SetKu) \
X_ERRMETHODS(ADE_METHOD_SetLda) \
X_ERRMETHODS(ADE_METHOD_SetBeta) \
X_ERRMETHODS(ADE_METHOD_Elewise_Config) \
X_ERRMETHODS(ADE_METHOD_Elewise) \
X_ERRMETHODS(ADE_METHOD_launch_type1) \
X_ERRMETHODS(ADE_METHOD_launch_type2) \
X_ERRMETHODS(ADE_METHOD_launch_type3) \
X_ERRMETHODS(ADE_METHOD_Saturation_detector_sm) \
X_ERRMETHODS(ADE_METHOD_doSaturation_detector_sm) \
X_ERRMETHODS(ADE_METHOD_Fir_Config) \
X_ERRMETHODS(ADE_METHOD_Iir_Config) \
X_ERRMETHODS(ADE_METHOD_Iir2_Config) \
X_ERRMETHODS(ADE_METHOD_Configure) \
X_ERRMETHODS(ADE_METHOD_SetDirection) \
X_ERRMETHODS(ADE_METHOD_SetType) \
X_ERRMETHODS(ADE_METHOD_SetPlan) \
X_ERRMETHODS(ADE_METHOD_SetTransA) \
X_ERRMETHODS(ADE_METHOD_SetTransB) \
X_ERRMETHODS(ADE_METHOD_SetSide) \
X_ERRMETHODS(ADE_METHOD_SetLdb) \
X_ERRMETHODS(ADE_METHOD_SetLdc) \
X_ERRMETHODS(ADE_METHOD_gemm) \
X_ERRMETHODS(ADE_METHOD_TeagerKaiser) \
X_ERRMETHODS(ADE_METHOD_ThresholdDetection) \
X_ERRMETHODS(ADE_METHOD_extract_events) \
X_ERRMETHODS(ADE_METHOD_tb) \
X_ERRMETHODS(ADE_METHOD_Config_blas_memcpy) \
X_ERRMETHODS(ADE_METHOD_Config_fir) \
X_ERRMETHODS(ADE_METHOD_Pure) \
X_ERRMETHODS(ADE_METHOD_doPure_blas) \
X_ERRMETHODS(ADE_METHOD_doPure_custom) \
X_ERRMETHODS(ADE_METHOD_Fir) \
X_ERRMETHODS(ADE_METHOD_GetOutBuff) \
X_ERRMETHODS(ADE_METHOD_axpy) \
X_ERRMETHODS(ADE_METHOD_dot) \
X_ERRMETHODS(ADE_METHOD_sdot) \
X_ERRMETHODS(ADE_METHOD_cdotu) \
X_ERRMETHODS(ADE_METHOD_cdotc) \
X_ERRMETHODS(ADE_METHOD_daxpy) \
X_ERRMETHODS(ADE_METHOD_zaxpy) \
X_ERRMETHODS(ADE_METHOD_dcopy) \
X_ERRMETHODS(ADE_METHOD_zcopy) \
X_ERRMETHODS(ADE_METHOD_ger) \
X_ERRMETHODS(ADE_METHOD_sbmv) \
X_ERRMETHODS(ADE_METHOD_doElewiseCustom) \
X_ERRMETHODS(ADE_METHOD_dger) \
X_ERRMETHODS(ADE_METHOD_dsbmv) \
X_ERRMETHODS(ADE_METHOD_dgemm) \
X_ERRMETHODS(ADE_METHOD_zgemm) \
X_ERRMETHODS(ADE_METHOD_Static_Params) \
X_ERRMETHODS(ADE_METHOD_Configuration) \
X_ERRMETHODS(ADE_METHOD_Set_SatThresh) \
X_ERRMETHODS(ADE_METHOD_Set_EvalTimesample) \
X_ERRMETHODS(ADE_METHOD_Set_Expander_static_params) \
X_ERRMETHODS(ADE_METHOD_Set_NSatThresh) \
X_ERRMETHODS(ADE_METHOD_Set_NPowThreshHigh) \
X_ERRMETHODS(ADE_METHOD_Set_NPowThreshLow) \
X_ERRMETHODS(ADE_METHOD_Set_RunPowWinFast) \
X_ERRMETHODS(ADE_METHOD_Set_RunPowWinSlow) \
X_ERRMETHODS(ADE_METHOD_Set_NPowThreshAttack) \
X_ERRMETHODS(ADE_METHOD_Set_NPowThreshRelease) \
X_ERRMETHODS(ADE_METHOD_Expander_Config) \
X_ERRMETHODS(ADE_METHOD_filter_DII_T) \
X_ERRMETHODS(ADE_METHOD_dofilter_DII_T_blas) \
X_ERRMETHODS(ADE_METHOD_dofilter_DII_T_custom) \
X_ERRMETHODS(ADE_METHOD_filter_DII_T_b) \
X_ERRMETHODS(ADE_METHOD_dofilter_DII_T_b_blas) \
X_ERRMETHODS(ADE_METHOD_dofilter_DII_T_b_custom) \
X_ERRMETHODS(ADE_METHOD_Mat2C_copy) \
X_ERRMETHODS(ADE_METHOD_Xrms2) \
X_ERRMETHODS(ADE_METHOD_find_local_max) \
X_ERRMETHODS(ADE_METHOD_snap_recognition) \
X_ERRMETHODS(ADE_METHOD_snap_detector) \
X_ERRMETHODS(ADE_METHOD_PrintColArrayCplx) \
X_ERRMETHODS(ADE_METHOD_PrintMatrixReal) \
X_ERRMETHODS(ADE_METHOD_PrintMatrixCplx) \
X_ERRMETHODS(ADE_METHOD_saxpy) \
X_ERRMETHODS(ADE_METHOD_caxpy) \
X_ERRMETHODS(ADE_METHOD_scopy) \
X_ERRMETHODS(ADE_METHOD_ccopy) \
X_ERRMETHODS(ADE_METHOD_snrm2) \
X_ERRMETHODS(ADE_METHOD_sger) \
X_ERRMETHODS(ADE_METHOD_ssbmv) \
X_ERRMETHODS(ADE_METHOD_doStep) \
X_ERRMETHODS(ADE_METHOD_doStep_blas) \
X_ERRMETHODS(ADE_METHOD_doStep_custom)





#define X_ERRMETHODS(a) a,
typedef enum { ERRMETHODS } ADE_ERRMETHODS_T;
#undef X_ERRMETHODS

#define X_ERRMETHODS(a) #a,
static char *ADE_ERRMETHODSStrings[] = { ERRMETHODS };
#undef X_ERRMETHODS


///*****************************ERROR BLAS  CODES **********************/
///*Level1*/
//#define ADE_W0 (ADE_WARNING_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL1<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_SAXPY_N<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_W1 (ADE_WARNING_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL1<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_SAXPY_SA<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//2 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL1<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//36 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL1<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//39 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL1<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
///*Level2*/
//#define ADE_RET_ERROR;//37 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL2<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//38 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL2<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//40 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL2<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
///*Level3*/
//#define ADE_RET_ERROR;//41 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL3<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//42 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL3<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//43 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL3<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
///******************************* ERRORS IIR **************************************/
//#define ADE_RET_ERROR;//3 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//4 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//5 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
////#define ADE_RET_ERROR;//6 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_DENOMS<<ADE_NBITS_METHOD_SHIFT)
////#define ADE_RET_ERROR;//7 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_NUMS<<ADE_NBITS_METHOD_SHIFT)
////#define ADE_RET_ERROR;//8 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_GAINS<<ADE_NBITS_METHOD_SHIFT)
////#define ADE_RET_ERROR;//15 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_STATES<<ADE_NBITS_METHOD_SHIFT)
////#define ADE_RET_ERROR;//10 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_IN_BUFF<<ADE_NBITS_METHOD_SHIFT)
////#define ADE_RET_ERROR;//11 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_OUT_BUFF<<ADE_NBITS_METHOD_SHIFT)
///******************************* ERRORS FIR **************************************/
////#define ADE_RET_ERROR;//9 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FIR<<ADE_NBITS_CLASS_SHIFT | ADE_FILT_IMPLEMENTATION_NOT_EXISTENT<<ADE_NBITS_METHOD_SHIFT)
////#define ADE_RET_ERROR;//12 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_IN_BUFF<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//13 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FIR<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//14 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FIR<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//17 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FIR<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
///********************************* ERRORS BLOW *************************************/
//#define ADE_RET_ERROR;//16 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLOW<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//18 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLOW<<ADE_NBITS_CLASS_SHIFT | ADE_MEMBER_NOT_SET<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//19 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLOW<<ADE_NBITS_CLASS_SHIFT | ADE_UNKNOWN_STATE<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//20 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLOW<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//25 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLOW<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
///*********************************** ERRORS POLYFIT ************************************/
//#define ADE_RET_ERROR;//21 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_POLYFIT<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//22 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_POLYFIT<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
///*********************************** ERRORS ADE ************************************/
//#define ADE_RET_ERROR;//23 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_ADE<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//24 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_ADE<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
///*********************************** ERRORS MATLAB ************************************/
//#define ADE_RET_ERROR;//26 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_MATLAB<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//27 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_MATLAB<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//28 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_MATLAB<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//29 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_MATLAB<<ADE_NBITS_CLASS_SHIFT | ADE_IDX_NOT_FOUND<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_W30 (ADE_WARNING_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_MATLAB<<ADE_NBITS_CLASS_SHIFT | ADE_CASE_NOT_HANDLED<<ADE_NBITS_METHOD_SHIFT)
///**************************************** ERRORS FFT *************************************************************/
//#define ADE_RET_ERROR;//31 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FFT<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//32 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FFT<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//33 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FFT<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
///****************************************** ERRORS UTILS **************************************************************/
//#define ADE_RET_ERROR;//34 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_UTILS<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//35 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_UTILS<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
///********************************************** SNAP ************************************************/
//#define ADE_RET_ERROR;//44 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_SNAP<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//45 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_SNAP<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//46 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_SNAP<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
///********************************************** UPSAMPLER ************************************************/
//#define ADE_RET_ERROR;//47 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_UPSAMPLER<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR;//RROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//48 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_UPSAMPLER<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
//#define ADE_RET_ERROR;//49 (ADE_RET_ERROR;//RROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_UPSAMPLER<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)

#endif
