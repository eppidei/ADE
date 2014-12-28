#ifndef _ADE_ERRORS_H
#define _ADE_ERRORS_H

/*************************** ERROR DEFINES ***************************/
#define ADE_MISSING_IMPLEMENTATION(fcn_name) fprintf(stderr," MISSING IMPLEMENTATION IN fcn ->  " #fcn_name " \n");
#define ADE_DEFINE_ERROR(define_name) fprintf(stderr, #define_name " DEFINE ERROR\n");
#define ADE_PRINT_ERRORS(error_type,var_name,var_format,fcn_name) fprintf(stderr, error_type " -> ERROR In fcn " #fcn_name " "#var_name " = " var_format " \n", var_name);
#define ADE_PRINT_WARNINGS(error_type,var_name,var_format,fcn_name) fprintf(stdout, error_type " -> WARNING In fcn " #fcn_name " "#var_name " = " var_format " \n", var_name);

#define ADE_NBITS_SIGN (1)
#define ADE_NBITS_TYPES (10)
#define ADE_NBITS_FRAMEWORK (4)
#define ADE_NBITS_CLASS (8)
#define ADE_NBITS_METHOD (8)

//SHIFTS CALC

#define ADE_NBITS_SIGN_SHIFT (32-ADE_NBITS_SIGN)
#define ADE_NBITS_TYPES_SHIFT (ADE_NBITS_SIGN_SHIFT-ADE_NBITS_TYPES)
#define ADE_NBITS_FRAMEWORK_SHIFT (ADE_NBITS_TYPES_SHIFT-ADE_NBITS_FRAMEWORK)
#define ADE_NBITS_CLASS_SHIFT (ADE_NBITS_FRAMEWORK_SHIFT-ADE_NBITS_CLASS)
#define ADE_NBITS_METHOD_SHIFT (ADE_NBITS_CLASS_SHIFT-ADE_NBITS_METHOD)

/************************** SIGN ************************************/
#define ADE_ERROR_SIGN (1) //Numero negativo
#define ADE_WARNING_SIGN (0) //Numero positivo

/*************************** ERROR TYPES ***************************/

#define ADE_ERROR_GENERAL_TYPE (0)
#define ADE_ERROR_INCHECKS_TYPE (1)

/**************************** ERROR TYPES STRINGS************************/
#define ADE_GENERAL "ADE General Error"
#define ADE_INCHECKS "ADE Input Checks"
#define ADE_BLAS_L1 "ADE Blas level 1"
#define ADE_MEM "ADE Memory Allocation"
#define ADE_RETCHECKS "ADE Return Checks"

/******************************* ERROR FRAMEWORK ***********************/
#define ADE_FRAMEWORK_ADE (0)

/*******************************ERROR  CLASSES **************************/

#define ADE_CLASS_BLAS_LEVEL1 (0)
#define ADE_CLASS_IIR (1)
#define ADE_CLASS_FIR (2)
#define ADE_CLASS_BLOW (3)
#define ADE_CLASS_POLYFIT (4)
#define ADE_CLASS_ADE (5)
#define ADE_CLASS_MATLAB (6)
#define ADE_CLASS_FFT (7)
/****ERRORS****/

#define ADE_ERROR_SAXPY_N (0)
#define ADE_ERROR_SAXPY_SA (1)
#define ADE_ERROR_ALLOCATION (2)
#define ADE_FILT_IMPLEMENTATION_NOT_EXISTENT (3)
#define ADE_FILT_IMPLEMENTATION_MISSING (4)
#define ADE_MISSING_DENOMS (5)
#define ADE_MISSING_NUMS (6)
#define ADE_MISSING_GAINS (7)
#define ADE_MISSING_IN_BUFF (8)
#define ADE_MISSING_OUT_BUFF (9)
#define ADE_MISSING_STATES (10)
#define ADE_MEMBER_NOT_SET (11)
#define ADE_UNKNOWN_STATE (12)
#define ADE_INVALID_PARAM (13)
#define ADE_RET_ERROR (14)
#define ADE_IDX_NOT_FOUND (15)
#define ADE_CASE_NOT_HANDLED (16)


/********************************* ERROR CODES *****************************************/

/*****************************ERROR BLAS  CODES **********************/
#define ADE_W0 (ADE_WARNING_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL1<<ADE_NBITS_CLASS_SHIFT | ADE_ERROR_SAXPY_N<<ADE_NBITS_METHOD_SHIFT)
#define ADE_W1 (ADE_WARNING_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLAS_LEVEL1<<ADE_NBITS_CLASS_SHIFT | ADE_ERROR_SAXPY_SA<<ADE_NBITS_METHOD_SHIFT)
/******************************* ERRORS IIR **************************************/
#define ADE_E3 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_ERROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E4 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_FILT_IMPLEMENTATION_NOT_EXISTENT<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E5 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_FILT_IMPLEMENTATION_MISSING<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E6 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_DENOMS<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E7 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_NUMS<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E8 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_GAINS<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E15 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_STATES<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E10 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_IN_BUFF<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E11 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_IIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_OUT_BUFF<<ADE_NBITS_METHOD_SHIFT)
/******************************* ERRORS FIR **************************************/
#define ADE_E9 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FIR<<ADE_NBITS_CLASS_SHIFT | ADE_FILT_IMPLEMENTATION_NOT_EXISTENT<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E12 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_IN_BUFF<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E13 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_OUT_BUFF<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E14 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FIR<<ADE_NBITS_CLASS_SHIFT | ADE_MISSING_STATES<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E17 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FIR<<ADE_NBITS_CLASS_SHIFT | ADE_ERROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
/********************************* ERRORS BLOW *************************************/
#define ADE_E16 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLOW<<ADE_NBITS_CLASS_SHIFT | ADE_ERROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E18 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLOW<<ADE_NBITS_CLASS_SHIFT | ADE_MEMBER_NOT_SET<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E19 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLOW<<ADE_NBITS_CLASS_SHIFT | ADE_UNKNOWN_STATE<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E20 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLOW<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E25 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_BLOW<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
/*********************************** ERRORS POLYFIT ************************************/
#define ADE_E21 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_POLYFIT<<ADE_NBITS_CLASS_SHIFT | ADE_ERROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E22 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_POLYFIT<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
/*********************************** ERRORS ADE ************************************/
#define ADE_E23 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_ADE<<ADE_NBITS_CLASS_SHIFT | ADE_ERROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E24 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_ADE<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
/*********************************** ERRORS MATLAB ************************************/
#define ADE_E26 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_MATLAB<<ADE_NBITS_CLASS_SHIFT | ADE_ERROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E27 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_MATLAB<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E28 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_MATLAB<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E29 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_MATLAB<<ADE_NBITS_CLASS_SHIFT | ADE_IDX_NOT_FOUND<<ADE_NBITS_METHOD_SHIFT)
#define ADE_W30 (ADE_WARNING_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_MATLAB<<ADE_NBITS_CLASS_SHIFT | ADE_CASE_NOT_HANDLED<<ADE_NBITS_METHOD_SHIFT)
/**************************************** ERRORS FFT *************************************************************/
#define ADE_E31 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FFT<<ADE_NBITS_CLASS_SHIFT | ADE_ERROR_ALLOCATION<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E32 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FFT<<ADE_NBITS_CLASS_SHIFT | ADE_INVALID_PARAM<<ADE_NBITS_METHOD_SHIFT)
#define ADE_E33 (ADE_ERROR_SIGN<<ADE_NBITS_SIGN_SHIFT | ADE_FRAMEWORK_ADE<<ADE_NBITS_FRAMEWORK_SHIFT | ADE_CLASS_FFT<<ADE_NBITS_CLASS_SHIFT | ADE_RET_ERROR<<ADE_NBITS_METHOD_SHIFT)
#endif
